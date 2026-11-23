#pragma once

#include "Config/Settings.h"
#include "Core/OrbitState.h"
#include "Input/KeyBinds.h"

namespace Orbit::Input
{
	enum class Hand
	{
		kDefault,
		kLeft,
		kRight,
		kBoth
	};

	struct OrbitSnapshot
	{
		bool open = false;
		int hoveredSlot = -1;
		std::uint32_t openCount = 0;
		std::uint32_t closeCount = 0;
		bool lastCloseChoseSlot = false;
		bool gamepad = false;
		bool assign = false;
	};

	class InputManager
	{
	public:
		using CommitHandler = std::function<void(int a_slot, Hand a_hand)>;
		using AssignHandler = std::function<bool(int a_slot)>;
		using SwapHandler = std::function<void()>;
		using PageHandler = std::function<void(int a_step)>;
		using BusyPredicate = std::function<bool()>;

		[[nodiscard]] static InputManager &Get() noexcept;

		void SetCommitHandler(CommitHandler a_handler);

		void SetAssignHandler(AssignHandler a_handler);

		void SetSwapHandler(SwapHandler a_handler);

		void SetPageHandler(PageHandler a_handler);

		void SetUiBusyPredicate(BusyPredicate a_predicate);

		void Enable();

		[[nodiscard]] bool ProcessEvent(RE::InputEvent *a_event);

		[[nodiscard]] bool ProcessCaptureEvent(RE::InputEvent *a_event);

		void ForceClose();

		[[nodiscard]] bool IsOrbitMenuOpen() const noexcept { return _orbitMenuOpen.load(std::memory_order_acquire); }
		[[nodiscard]] bool IsAssignMode() const noexcept { return _assignMode.load(std::memory_order_acquire); }
		[[nodiscard]] OrbitSnapshot GetSnapshot() const;

		void BeginKeyCapture(KeyBinding a_binding);
		void CancelKeyCapture();
		[[nodiscard]] std::optional<KeyBinding> CapturingBinding() const;
		[[nodiscard]] std::optional<CapturedKey> TakeCapturedKey();
		[[nodiscard]] std::string CaptureMessage() const;

	private:
		using Clock = std::chrono::steady_clock;

		struct Capture
		{
			bool active = false;
			KeyBinding binding;
			std::optional<CapturedKey> result;
			std::string message;
		};

		struct CapturedPress
		{
			std::uint32_t code;
			Clock::time_point at;
		};

		struct Commit
		{
			int slot;
			Hand hand;
			bool assign;
		};

		struct Outcome
		{
			bool block = false;
			std::optional<Commit> committed;
			bool swapHands = false;
			int pageStep = 0;
		};

		InputManager() = default;

		[[nodiscard]] const Config::Settings &CurrentSettingsLocked();
		void HandleButton(RE::ButtonEvent &a_button, const Config::Settings &a_settings, Outcome &a_outcome);
		bool HandleMouseMove(const RE::MouseMoveEvent &a_move, float a_sensitivity, float a_deadzone);
		void HandleThumbstick(RE::ThumbstickEvent &a_stick, bool a_rightStick, float a_deadzone);
		bool HandleCapture(RE::InputEvent &a_event);

		[[nodiscard]] bool IsBlockedLocked(bool a_assign) const;
		void OpenLocked(bool a_gamepad, bool a_assign);
		std::optional<int> CloseLocked(bool a_commit);
		std::optional<Commit> ReleaseLocked(const Config::Settings &a_settings);
		bool NotifyCommit(const Commit &a_commit) const;

		mutable std::mutex _lock;
		Config::Settings _settings;
		std::optional<std::uint32_t> _settingsVersion;
		Core::OrbitState _state;
		Capture _capture;
		bool _enabled = false;
		CommitHandler _onCommit;
		AssignHandler _onAssign;
		SwapHandler _onSwap;
		PageHandler _onPage;
		BusyPredicate _uiBusy;

		std::array<bool, InputMap::kMaxMacros> _swallow{};

		std::uint8_t _clicks = 0;

		ImVec2 _stick{};
		bool _stickCentred = false;

		std::optional<CapturedPress> _capturedPress;

		std::uint32_t _closeCount = 0;
		bool _lastCloseChoseSlot = false;
		bool _openedWithGamepad = false;
		std::atomic<bool> _orbitMenuOpen{false};
		std::atomic<bool> _assignMode{false};
	};
}
