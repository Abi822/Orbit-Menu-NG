#include "Input/KeyBinds.h"

namespace Orbit::Input
{
	std::optional<std::uint32_t> ToKeyBind(const RE::ButtonEvent &a_event)
	{
		const std::uint32_t id = a_event.GetIDCode();
		switch (a_event.GetDevice())
		{
		case RE::INPUT_DEVICE::kKeyboard:
			return IsKeyboardBind(id) ? std::optional{id} : std::nullopt;
		case RE::INPUT_DEVICE::kMouse:
			return IsMouseBind(kMouseLeft + id) ? std::optional{kMouseLeft + id} : std::nullopt;
		case RE::INPUT_DEVICE::kGamepad:
		{
			const std::uint32_t code = InputMap::GamepadMaskToKeycode(id);
			return IsGamepadBind(code) ? std::optional{code} : std::nullopt;
		}
		default:
			return std::nullopt;
		}
	}

	std::string KeyName(std::uint32_t a_keybind)
	{
		if (a_keybind == kKeyBindNone)
		{
			return "Not bound.";
		}
		auto name = InputMap::GetKeyName(a_keybind);
		return name.empty() ? std::format("Key {}", a_keybind) : name;
	}
}
