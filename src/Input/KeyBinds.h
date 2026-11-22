#pragma once

namespace RMS::Input
{
	namespace InputMap = SKSE::InputMap;

	inline constexpr std::uint32_t kKeyBindNone = 0;
	inline constexpr std::uint32_t kMouseLeft = InputMap::kMacro_MouseButtonOffset;
	inline constexpr std::uint32_t kMouseWheelUp = InputMap::kMacro_MouseWheelOffset;
	inline constexpr std::uint32_t kMouseWheelDown = kMouseWheelUp + 1;

	[[nodiscard]] constexpr bool IsKeyboardBind(std::uint32_t a_keybind) noexcept
	{
		return a_keybind > kKeyBindNone && a_keybind < kMouseLeft;
	}

	[[nodiscard]] constexpr bool IsMouseBind(std::uint32_t a_keybind) noexcept
	{
		return a_keybind >= kMouseLeft && a_keybind <= kMouseWheelDown;
	}

	[[nodiscard]] constexpr bool IsGamepadBind(std::uint32_t a_keybind) noexcept
	{
		return a_keybind >= InputMap::kMacro_GamepadOffset && a_keybind < InputMap::kMaxMacros;
	}

	[[nodiscard]] std::optional<std::uint32_t> ToKeyBind(const RE::ButtonEvent &a_event);

	[[nodiscard]] bool IsBindable(std::uint32_t a_keybind, std::string *a_reason = nullptr);

	[[nodiscard]] std::string KeyName(std::uint32_t a_keybind);

	inline constexpr int kBindingActivation = -1;
	inline constexpr int kBindingLeftHand = -2;
	inline constexpr int kBindingRightHand = -3;
	inline constexpr int kBindingSwapHands = -4;
	inline constexpr int kBindingNextPage = -5;
	inline constexpr int kBindingPreviousPage = -6;

	struct KeyBinding
	{
		int id = kBindingActivation;
		bool gamepad = false;

		bool operator==(const KeyBinding &) const = default;
	};

	struct CapturedKey
	{
		int binding;
		std::uint32_t code;
	};
}
