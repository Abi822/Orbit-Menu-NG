#pragma once

#include "Core/SlotView.h"
#include "UI/Graphics/TextMeasure.h"

namespace Orbit::UI::OrbitMenu
{
	struct ItemDescriptionLayout
	{
		enum class TextRole
		{
			kName,
			kLabel,
			kValue,
			kText
		};

		struct TextPiece
		{
			std::string text;
			float size = 0.0f;
			ImVec2 topLeft{};
			TextRole role = TextRole::kText;
		};

		std::vector<TextPiece> pieces;
		float iconTop = 0.0f;

		static ItemDescriptionLayout Compute(const Graphics::TextMeasure &a_text, const Core::ItemDescription &a_description, float a_textSize, float a_iconSize, float a_centerBackgroundRadius);
	};
}
