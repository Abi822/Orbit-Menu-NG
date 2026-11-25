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
		static InputManager &Get();
		void Enable();
		[[nodiscard]] bool ProcessEvent(RE::InputEvent *a_event);
		[[nodiscard]] bool ProcessCaptureEvent(RE::InputEvent *a_event);
		void ForceClose();
		bool IsOrbitMenuOpen() const { return _orbitMenuOpen; }
		bool IsAssignMode() const { return _assignMode; }
		OrbitSnapshot GetSnapshot() const;
		void BeginKeyCapture(KeyBinding a_binding);
		void CancelKeyCapture();
		std::optional<KeyBinding> CapturingBinding() const;
		[[nodiscard]] std::optional<CapturedKey> TakeCapturedKey();

	private:
		using Clock = std::chrono::steady_clock;

		struct Capture
		{
			bool active = false;
			KeyBinding binding;
			std::optional<CapturedKey> result;
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

		struct Result
		{
			bool block = false;
			std::optional<Commit> committed;
			bool swapHands = false;
			int pageStep = 0;
		};

		InputManager() = default;
		const Config::Settings &CurrentSettingsLocked();
		void HandleButton(RE::ButtonEvent &a_button, const Config::Settings &a_settings, Result &a_result);
		bool HandleMouseMove(const RE::MouseMoveEvent &a_move, float a_sensitivity, float a_deadzone);
		void HandleThumbstick(RE::ThumbstickEvent &a_stick, bool a_rightStick, float a_deadzone);
		bool HandleCapture(RE::InputEvent &a_event);
		bool IsBlockedLocked(bool a_assign) const;
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
