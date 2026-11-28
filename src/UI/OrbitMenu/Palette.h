#pragma once

#include "Config/Settings.h"
#include <imgui.h>

namespace Orbit::UI::OrbitMenu
{
	struct Palette
	{
		ImVec4 highlight;
		ImVec4 slotGlow;
		ImVec4 pointer;
		ImVec4 slotFill;
		ImVec4 slotKeyBackground;
		ImVec4 background;
		ImVec4 slotBorder;
		ImVec4 slotInnerRing;
		ImVec4 itemDescriptionBorder;
		ImVec4 itemDescriptionInnerBorder;
		ImVec4 slotName;
		ImVec4 slotNameHovered;
		ImVec4 slotNameDim;
		ImVec4 slotEmpty;
		ImVec4 slotKey;
		ImVec4 hand;
		ImVec4 description;
		ImVec4 descriptionBright;
		ImVec4 descriptionDim;
		ImVec4 controlsHint;
		ImVec4 pageIndicatorActive;
		ImVec4 pageIndicatorInactive;
		ImVec4 itemDescriptionFill;
		ImVec4 textShadow{0.0f, 0.0f, 0.0f, 0.75f};
		static Palette Make(const Config::Settings &a_settings);
	};

	ImU32 Color(ImVec4 a_color, float a_alpha);
	ImVec4 Mix(ImVec4 a_from, ImVec4 a_to, float a_t);
	ImVec4 Fade(ImVec4 a_color, float a_factor);
}
