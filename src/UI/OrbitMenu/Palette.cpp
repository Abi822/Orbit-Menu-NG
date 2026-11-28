#include "UI/OrbitMenu/Palette.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr float kBrighter = 1.216f;
		constexpr float kDimmer = 0.75f;

		ImVec4 FromArgb(std::uint32_t a_argb, float a_alpha)
		{
			return {static_cast<float>((a_argb >> 16) & 0xFF) / 255.0f, static_cast<float>((a_argb >> 8) & 0xFF) / 255.0f,
					static_cast<float>(a_argb & 0xFF) / 255.0f, a_alpha * static_cast<float>((a_argb >> 24) & 0xFF) / 255.0f};
		}

		ImVec4 Shade(ImVec4 a_color, float a_factor, float a_alpha)
		{
			return {std::min(a_color.x * a_factor, 1.0f), std::min(a_color.y * a_factor, 1.0f), std::min(a_color.z * a_factor, 1.0f), a_alpha * a_color.w};
		}

		ImVec4 Grey(ImVec4 a_color)
		{
			const float grey = 0.2126f * a_color.x + 0.7152f * a_color.y + 0.0722f * a_color.z;
			return {grey, grey, grey, a_color.w};
		}
	}

	Palette Palette::Make(const Config::Settings &a_settings)
	{
		Palette colors;
		colors.highlight = FromArgb(a_settings.highlightColor, 1.0f);
		colors.pointer = FromArgb(a_settings.pointerColor, 1.0f);
		colors.itemDescriptionFill = FromArgb(a_settings.itemDescriptionBackgroundColor, 1.0f);
		colors.slotFill = FromArgb(a_settings.slotColor, 0.92f);
		colors.slotKeyBackground = FromArgb(a_settings.slotKeyBackgroundColor, 0.95f);
		colors.background = FromArgb(a_settings.backgroundColor, 1.0f);
		const ImVec4 tint = Mix(Grey(colors.highlight), colors.highlight, 0.5f);
		colors.slotBorder = Shade(tint, 0.64f, 0.85f);
		colors.slotInnerRing = Shade(tint, 1.0f, 0.07f);
		colors.itemDescriptionBorder = Shade(tint, 0.86f, 0.55f);
		colors.itemDescriptionInnerBorder = Shade(tint, 1.0f, 0.10f);
		colors.slotName = FromArgb(a_settings.slotNameColor, 1.0f);
		colors.slotNameHovered = Shade(colors.slotName, kBrighter, 1.0f);
		colors.slotNameDim = Shade(colors.slotName, kDimmer, 1.0f);
		colors.slotEmpty = FromArgb(a_settings.slotEmptyColor, 1.0f);
		colors.slotKey = FromArgb(a_settings.slotKeyColor, 1.0f);
		colors.hand = FromArgb(a_settings.handColor, 1.0f);
		colors.description = FromArgb(a_settings.itemDescriptionColor, 1.0f);
		colors.descriptionBright = Shade(colors.description, kBrighter, 1.0f);
		colors.descriptionDim = Shade(colors.description, kDimmer, 1.0f);
		colors.controlsHint = FromArgb(a_settings.controlsHintColor, 1.0f);
		colors.pageIndicatorActive = FromArgb(a_settings.pageIndicatorActiveColor, 1.0f);
		colors.pageIndicatorInactive = FromArgb(a_settings.pageIndicatorInactiveColor, 1.0f);
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
