#include "Config/SettingsMenu/MenuFrameworkBase.h"
#include <SKSEMenuFramework.h>
#include "Config/Translation.h"
#include <map>

namespace Orbit::Config::SettingsMenu
{
	// Returns where a binding key is stored in settings
	std::uint32_t *MenuFrameworkBase::BoundKey(Settings &a_settings, Input::KeyBinding a_binding)
	{
		const bool pad = a_binding.gamepad;
		switch (a_binding.id)
		{
		case Input::kBindingActivation:
			return pad ? &a_settings.gamepadActivationKey : &a_settings.activationKey;
		case Input::kBindingLeftHand:
			return pad ? &a_settings.gamepadLeftHandKey : &a_settings.leftHandKey;
		case Input::kBindingRightHand:
			return pad ? &a_settings.gamepadRightHandKey : &a_settings.rightHandKey;
		case Input::kBindingSwapHands:
			return pad ? &a_settings.gamepadSwapHandsKey : &a_settings.swapHandsKey;
		case Input::kBindingNextPage:
			return pad ? &a_settings.gamepadNextPageKey : &a_settings.nextPageKey;
		case Input::kBindingPreviousPage:
			return pad ? &a_settings.gamepadPreviousPageKey : &a_settings.previousPageKey;
		default:
			return pad ? nullptr : &a_settings.slotKeys[static_cast<std::size_t>(a_binding.id)];
		}
	}

	const char *MenuFrameworkBase::Label(const char *a_label)
	{
		const std::string_view label{a_label};
		const auto id = label.find("##");
		const auto english = label.substr(0, id);
		const auto text = Translate(english);
		if (text == english)
		{
			return a_label;
		}
		static std::map<std::string, std::string, std::less<>> labels;
		auto found = labels.find(label);
		if (found == labels.end())
		{
			found = labels.emplace(label, std::string{text} + std::string{id == std::string_view::npos ? std::string_view{} : label.substr(id)}).first;
		}
		return found->second.c_str();
	}

	void MenuFrameworkBase::Tooltip(const char *a_text)
	{
		if (a_text && ImGuiMCP::IsItemHovered())
		{
			ImGuiMCP::SetTooltip("%s", Translate(a_text));
		}
	}

	void MenuFrameworkBase::Slider(const char *a_label, float &a_value, float a_min, float a_max, const char *a_format, const char *a_tooltip)
	{
		ImGuiMCP::SliderFloat(Label(a_label), &a_value, a_min, a_max, a_format, ImGuiMCP::ImGuiSliderFlags_AlwaysClamp);
		_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
		Tooltip(a_tooltip);
	}

	void MenuFrameworkBase::Slider(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, const char *a_tooltip)
	{
		ImGuiMCP::SliderInt(Label(a_label), &a_value, a_min, a_max, "%d", ImGuiMCP::ImGuiSliderFlags_AlwaysClamp);
		_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
		Tooltip(a_tooltip);
	}

	void MenuFrameworkBase::Number(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, std::int32_t a_step, const char *a_tooltip)
	{
		if (ImGuiMCP::InputInt(Label(a_label), &a_value, a_step, a_step * 4))
		{
			a_value = std::clamp(a_value, a_min, a_max);
		}
		_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
		Tooltip(a_tooltip);
	}

	void MenuFrameworkBase::Check(const char *a_label, bool &a_value, const char *a_tooltip)
	{
		_save |= ImGuiMCP::Checkbox(Label(a_label), &a_value);
		Tooltip(a_tooltip);
	}

	// SKSE Menu Framework for the alpha channel still have convert to use ImGui's ColorEdit4 for ARGB to work.
	void MenuFrameworkBase::Color(const char *a_label, std::uint32_t &a_argb)
	{
		constexpr std::array kShifts{16, 8, 0, 24};
		float rgba[4]{};
		for (std::size_t i = 0; i < kShifts.size(); ++i)
		{
			rgba[i] = static_cast<float>((a_argb >> kShifts[i]) & 0xFF) / 255.0f;
		}
		if (ImGuiMCP::ColorEdit4(Label(a_label), rgba, ImGuiMCP::ImGuiColorEditFlags_AlphaBar | ImGuiMCP::ImGuiColorEditFlags_AlphaPreviewHalf))
		{
			a_argb = 0;
			for (std::size_t i = 0; i < kShifts.size(); ++i)
			{
				a_argb |= static_cast<std::uint32_t>(std::lround(std::clamp(rgba[i], 0.0f, 1.0f) * 255.0f)) << kShifts[i];
			}
		}
		_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
	}
}
