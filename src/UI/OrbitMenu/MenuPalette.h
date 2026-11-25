#pragma once

#include "Config/Settings.h"
#include <imgui.h>

namespace Orbit::UI::OrbitMenu
{
	struct MenuPalette
	{
		ImVec4 highlight;
		ImVec4 slotFill;
		ImVec4 keyBadge;
		ImVec4 background;
		ImVec4 slotBorder;
		ImVec4 slotInnerRing;
		ImVec4 centreBorder;
		ImVec4 centreInnerBorder;
		ImVec4 slotName;
		ImVec4 slotNameHovered;
		ImVec4 slotNameDim;
		ImVec4 slotEmpty;
		ImVec4 slotKey;
		ImVec4 hand;
		ImVec4 description;
		ImVec4 descriptionBright;
		ImVec4 descriptionDim;
		ImVec4 hint;
		ImVec4 centreFill{0.02f, 0.02f, 0.025f, 0.55f};
		ImVec4 textShadow{0.0f, 0.0f, 0.0f, 0.75f};
		static MenuPalette Make(const Config::Settings &a_settings);
	};

	ImU32 Color(ImVec4 a_color, float a_alpha);
	ImVec4 Mix(ImVec4 a_from, ImVec4 a_to, float a_t);
	ImVec4 Fade(ImVec4 a_color, float a_factor);
}
