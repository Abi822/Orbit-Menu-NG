#include "UI/OrbitMenu/MenuPalette.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr float kBrighter = 1.216f;
		constexpr float kDimmer = 0.75f;

		ImVec4 FromRgb(std::uint32_t a_rgb, float a_alpha)
		{
			return {static_cast<float>((a_rgb >> 16) & 0xFF) / 255.0f, static_cast<float>((a_rgb >> 8) & 0xFF) / 255.0f,
					static_cast<float>(a_rgb & 0xFF) / 255.0f, a_alpha};
		}

		ImVec4 Shade(ImVec4 a_color, float a_factor, float a_alpha)
		{
			return {std::min(a_color.x * a_factor, 1.0f), std::min(a_color.y * a_factor, 1.0f), std::min(a_color.z * a_factor, 1.0f), a_alpha};
		}

		ImVec4 Grey(ImVec4 a_color)
		{
			const float grey = 0.2126f * a_color.x + 0.7152f * a_color.y + 0.0722f * a_color.z;
			return {grey, grey, grey, 1.0f};
		}
	}

	MenuPalette MenuPalette::Make(const Config::Settings &a_settings)
	{
		MenuPalette colors;
		colors.highlight = FromRgb(a_settings.highlightColor, 1.0f);
		colors.slotFill = FromRgb(a_settings.slotColor, 0.92f);
		colors.keyBadge = FromRgb(a_settings.slotColor, 0.95f);
		colors.background = FromRgb(a_settings.backgroundColor, 1.0f);
		const ImVec4 tint = Mix(Grey(colors.highlight), colors.highlight, 0.5f);
		colors.slotBorder = Shade(tint, 0.64f, 0.85f);
		colors.slotInnerRing = Shade(tint, 1.0f, 0.07f);
		colors.centreBorder = Shade(tint, 0.86f, 0.55f);
		colors.centreInnerBorder = Shade(tint, 1.0f, 0.10f);
		colors.slotName = FromRgb(a_settings.slotNameColor, 1.0f);
		colors.slotNameHovered = Shade(colors.slotName, kBrighter, 1.0f);
		colors.slotNameDim = Shade(colors.slotName, kDimmer, 1.0f);
		colors.slotEmpty = FromRgb(a_settings.slotEmptyColor, 1.0f);
		colors.slotKey = FromRgb(a_settings.slotKeyColor, 1.0f);
		colors.hand = FromRgb(a_settings.handColor, 1.0f);
		colors.description = FromRgb(a_settings.itemDescriptionColor, 1.0f);
		colors.descriptionBright = Shade(colors.description, kBrighter, 1.0f);
		colors.descriptionDim = Shade(colors.description, kDimmer, 1.0f);
		colors.hint = FromRgb(a_settings.hintColor, 1.0f);
		return colors;
	}

	ImU32 Color(ImVec4 a_color, float a_alpha)
	{
		return ImGui::ColorConvertFloat4ToU32(Fade(a_color, a_alpha));
	}

	ImVec4 Mix(ImVec4 a_from, ImVec4 a_to, float a_t)
	{
		return {a_from.x + (a_to.x - a_from.x) * a_t, a_from.y + (a_to.y - a_from.y) * a_t,
				a_from.z + (a_to.z - a_from.z) * a_t, a_from.w + (a_to.w - a_from.w) * a_t};
	}

	ImVec4 Fade(ImVec4 a_color, float a_factor)
	{
		a_color.w *= a_factor;
		return a_color;
	}
}
