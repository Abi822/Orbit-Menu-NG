#include "Input/InputManager.h"
#include "Game/GameState.h"
#include "Integration/MenuFrameworkBridge.h"
#include "Items/Assignment.h"
#include "Items/EquipController.h"
#include "Items/Pages.h"

namespace Orbit::Input
{
	namespace
	{
		constexpr auto kCapturedPressWindow = 250ms;

		constexpr std::uint8_t kClickLeft = 1 << 0;
		constexpr std::uint8_t kClickRight = 1 << 1;

		constexpr float kSwitchDistance = 130.0f;

		std::optional<int> SlotOfKey(const Config::Settings &a_settings, std::uint32_t a_keybind)
		{
			const auto key = std::ranges::find(a_settings.slotKeys, a_keybind);
			if (key == a_settings.slotKeys.end())
			{
				return std::nullopt;
			}
			return static_cast<int>(key - a_settings.slotKeys.begin());
		}

		int PageStep(const Config::Settings &a_settings, std::uint32_t a_keybind)
		{
			if (!a_settings.enablePages)
			{
				return 0;
			}
			if (a_keybind == a_settings.nextPageKey || a_keybind == a_settings.gamepadNextPageKey)
			{
				return 1;
			}
			if (a_keybind == a_settings.previousPageKey || a_keybind == a_settings.gamepadPreviousPageKey)
			{
				return -1;
			}
			return 0;
		}

		Core::PointerLimits MouseLimits(float a_deadzone, float a_sensitivity)
		{
			Core::PointerLimits limits;
			limits.deadzone = a_deadzone;
			if (a_sensitivity > 0.0f)
			{
				limits.maxDistance = a_deadzone + kSwitchDistance / a_sensitivity;
			}
			return limits;
		}
	}

	InputManager &InputManager::Get()
	{
		static InputManager singleton;
		return singleton;
	}

	void InputManager::Enable()
	{
		std::scoped_lock lock{_lock};
		_enabled = true;
	}

	bool InputManager::ProcessEvent(RE::InputEvent *a_event)
	{
		if (!a_event)
		{
			return false;
		}

		Result result;
		{
			std::scoped_lock lock{_lock};
			if (!_enabled)
			{
				return false;
			}
			const auto &settings = CurrentSettingsLocked();

			if (_state.IsOpen() && IsBlockedLocked(_assignMode))
			{
				CloseLocked(false);
			}

			switch (a_event->GetEventType())
			{
			case RE::INPUT_EVENT_TYPE::kButton:
				if (auto *button = a_event->AsButtonEvent())
				{
					HandleButton(*button, settings, result);
				}
				break;
			case RE::INPUT_EVENT_TYPE::kMouseMove:
				if (const auto *move = a_event->AsMouseMoveEvent())
				{
					result.block = HandleMouseMove(*move, settings.mouseSensitivity, settings.SelectionDeadzone());
				}
				break;
			case RE::INPUT_EVENT_TYPE::kThumbstick:
				if (auto *stick = a_event->AsThumbstickEvent())
				{
					HandleThumbstick(*stick, settings.useRightStick, Config::Settings::kControllerDeadzone);
				}
				break;
			default:
				break;
			}
		}

		if (result.committed && NotifyCommit(*result.committed))
		{
			if (const auto *button = a_event->AsButtonEvent(); button && button->IsDown())
			{
				if (const auto code = ToKeyBind(*button))
				{
					std::scoped_lock lock{_lock};
					_swallow[*code] = true;
				}
			}
			result.block = true;
		}
		if (result.swapHands)
		{
			Items::EquipController::Get().SwapHands();
		}
		if (result.pageStep != 0)
		{
			Items::Pages::Get().Turn(result.pageStep);
		}
		return result.block;
	}

	const Config::Settings &InputManager::CurrentSettingsLocked()
	{
		auto &store = Config::SettingsStore::Get();
		if (const auto version = store.ChangeCount(); version != _settingsVersion)
		{
			_settingsVersion = version;
			_settings = store.Snapshot();
		}
		return _settings;
	}

	bool InputManager::ProcessCaptureEvent(RE::InputEvent *a_event)
	{
		if (!a_event)
		{
			return false;
		}
		std::scoped_lock lock{_lock};
		return _capture.active && HandleCapture(*a_event);
	}

	void InputManager::HandleButton(RE::ButtonEvent &a_button, const Config::Settings &a_settings, Result &a_result)
	{
		const auto code = ToKeyBind(a_button);
		if (!code)
		{
			return;
		}

		const bool down = a_button.IsDown();
		const bool up = a_button.IsUp();
		auto &swallowed = _swallow[*code];
		if (down)
		{
			if (_capturedPress && _capturedPress->code == *code && Clock::now() - _capturedPress->at < kCapturedPressWindow)
			{
				_capturedPress.reset();
				swallowed = true;
				a_result.block = true;
				return;
			}
			swallowed = false;
		}

		const bool gamepad = IsGamepadBind(*code);
		if (*code == a_settings.activationKey || *code == a_settings.gamepadActivationKey)
		{
			if (down)
			{
				if (!_state.IsOpen())
				{
					if (const bool assign = Game::IsInItemMenu(); !IsBlockedLocked(assign))
					{
						OpenLocked(gamepad, assign);
						swallowed = true;
						a_result.block = true;
						return;
					}
				}
				else
				{
					if (a_settings.toggleOrbitMenu)
					{
						a_result.committed = ReleaseLocked(a_settings);
					}
					swallowed = true;
					a_result.block = true;
					return;
				}
			}
			else if (up)
			{
				const bool wasSwallowed = std::exchange(swallowed, false);
				if (_state.IsOpen() && !a_settings.toggleOrbitMenu)
				{
					a_result.committed = ReleaseLocked(a_settings);
					a_result.block = true;
				}
				else
				{
					a_result.block = wasSwallowed || _state.IsOpen();
				}
				return;
			}
			else
			{
				a_result.block = swallowed || _state.IsOpen();
				return;
			}
		}

		const bool leftHandKey = *code == a_settings.leftHandKey || *code == a_settings.gamepadLeftHandKey;
		if (_state.IsOpen() && (leftHandKey || *code == a_settings.rightHandKey || *code == a_settings.gamepadRightHandKey))
		{
			const std::uint8_t button = leftHandKey ? kClickLeft : kClickRight;
			if (down)
			{
				_clicks |= button;
				swallowed = true;
				a_result.block = true;
				return;
			}
			if (up && (_clicks & button) != 0)
			{
				const Hand hand = _clicks == (kClickLeft | kClickRight) ? Hand::kBoth : (button == kClickLeft ? Hand::kRight : Hand::kLeft);
				_clicks = 0;
				swallowed = false;
				a_result.block = true;
				if (const auto slot = _state.Choose())
				{
					a_result.committed = Commit{*slot, hand, _assignMode};
				}
				return;
			}
		}

		if (swallowed && !down)
		{
			if (up)
			{
				swallowed = false;
			}
			a_result.block = true;
			return;
		}

		const auto keySlot = SlotOfKey(a_settings, *code);
		if (!_state.IsOpen())
		{
			if (down && !_capture.active && keySlot)
			{
				a_result.committed = Commit{*keySlot, Hand::kDefault, true};
			}
			return;
		}

		if (keySlot)
		{
			if (down)
			{
				_state.Select(*keySlot);
				swallowed = true;
				a_result.block = true;
			}
			return;
		}

		if (const int step = PageStep(a_settings, *code); step != 0)
		{
			if (down)
			{
				swallowed = true;
				a_result.block = true;
				a_result.pageStep = step;
			}
			return;
		}

		if (!_assignMode && (*code == a_settings.swapHandsKey || *code == a_settings.gamepadSwapHandsKey) && down)
		{
			swallowed = true;
			a_result.block = true;
			a_result.swapHands = true;
			return;
		}

		// Stop every other button in inventory so that nothing gets equipped or unequipped by accident.
		if (_assignMode || IsMouseBind(*code))
		{
			if (down)
			{
				swallowed = true;
				a_result.block = true;
			}
			return;
		}
	}

	bool InputManager::HandleMouseMove(const RE::MouseMoveEvent &a_move, float a_sensitivity, float a_deadzone)
	{
		if (!_state.IsOpen())
		{
			return false;
		}

		const ImVec2 delta{static_cast<float>(a_move.mouseInputX), static_cast<float>(a_move.mouseInputY)};
		_state.AddPointerDelta(delta, MouseLimits(a_deadzone, a_sensitivity));
		return true;
	}

	void InputManager::HandleThumbstick(RE::ThumbstickEvent &a_stick, bool a_rightStick, float a_deadzone)
	{
		const bool selects = a_rightStick ? a_stick.IsRight() : a_stick.IsLeft();
		if (selects)
		{
			_stick = {a_stick.xValue, -a_stick.yValue};
		}
		if (!_state.IsOpen())
		{
			return;
		}
		if (selects)
		{
			_stickCentred = _stickCentred || std::hypot(_stick.x, _stick.y) <= a_deadzone;
			if (_stickCentred)
			{
				_state.SetPointer(_stick, {.deadzone = a_deadzone});
			}
		}
		a_stick.xValue = 0.0f;
		a_stick.yValue = 0.0f;
	}

	bool InputManager::HandleCapture(RE::InputEvent &a_event)
	{
		auto *button = a_event.AsButtonEvent();
		if (!button || !button->IsDown())
		{
			return false;
		}

		const auto code = ToKeyBind(*button);
		if (!code)
		{
			return false;
		}

		if (IsGamepadBind(*code) != _capture.binding.gamepad)
		{
			return false;
		}

		_capturedPress = CapturedPress{*code, Clock::now()};
		_swallow[*code] = true;
		_capture.active = false;
		_capture.result = CapturedKey{_capture.binding.id, *code};
		logger::info("InputManager: Key captured: {} ({})", *code, KeyName(*code));
		return true;
	}

	void InputManager::ForceClose()
	{
		std::scoped_lock lock{_lock};
		CloseLocked(false);
	}

	OrbitSnapshot InputManager::GetSnapshot() const
	{
		std::scoped_lock lock{_lock};
		return {.open = _state.IsOpen(),
				.hoveredSlot = _state.HoveredSlot(),
				.openCount = _state.OpenCount(),
				.closeCount = _closeCount,
				.lastCloseChoseSlot = _lastCloseChoseSlot,
				.gamepad = _openedWithGamepad,
				.assign = _assignMode};
	}

	void InputManager::BeginKeyCapture(KeyBinding a_binding)
	{
		std::scoped_lock lock{_lock};
		_capture = {};
		_capture.active = true;
		_capture.binding = a_binding;
	}

	void InputManager::CancelKeyCapture()
	{
		std::scoped_lock lock{_lock};
		if (_capture.active)
		{
			_capture = {};
		}
	}

	std::optional<KeyBinding> InputManager::CapturingBinding() const
	{
		std::scoped_lock lock{_lock};
		return _capture.active ? std::optional{_capture.binding} : std::nullopt;
	}

	std::optional<CapturedKey> InputManager::TakeCapturedKey()
	{
		std::scoped_lock lock{_lock};
		return std::exchange(_capture.result, std::nullopt);
	}

	bool InputManager::IsBlockedLocked(bool a_assign) const
	{
		return !(a_assign ? Game::IsInItemMenu() : Game::IsInGameplay()) || Integration::MenuFramework::IsBlockingWindowOpen();
	}

	void InputManager::OpenLocked(bool a_gamepad, bool a_assign)
	{
		_state.Open();
		_stickCentred = std::hypot(_stick.x, _stick.y) <= Config::Settings::kControllerDeadzone;
		_clicks = 0;
		_openedWithGamepad = a_gamepad;
		_assignMode = a_assign;
		_orbitMenuOpen = true;
	}

	std::optional<int> InputManager::CloseLocked(bool a_commit)
	{
		if (!_state.IsOpen())
		{
			return std::nullopt;
		}
		_clicks = 0;
		const auto slot = _state.Close(a_commit);
		++_closeCount;
		_lastCloseChoseSlot = slot.has_value();
		_orbitMenuOpen = false;
		return slot;
	}

	std::optional<InputManager::Commit> InputManager::ReleaseLocked(const Config::Settings &a_settings)
	{
		if (const auto slot = CloseLocked(!_assignMode && a_settings.releaseToEquip))
		{
			return Commit{*slot, Hand::kDefault, false};
		}
		return std::nullopt;
	}

	bool InputManager::NotifyCommit(const Commit &a_commit) const
	{
		if (a_commit.assign)
		{
			return Items::Assignment::AssignHighlighted(a_commit.slot);
		}
		Items::EquipController::Get().EquipSlot(a_commit.slot, a_commit.hand);
		return true;
	}
}
