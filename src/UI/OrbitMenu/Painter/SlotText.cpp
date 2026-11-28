#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kSlotKeySegments = 24;
		constexpr float kSlotHandTextMargin = 8.0f;

		ImVec2 PositionSide(Config::SlotTextPosition a_position)
		{
			const auto index = static_cast<int>(a_position);
			return {static_cast<float>(index % 3 - 1), index < 3 ? -1.0f : 1.0f};
		}

		ImVec2 SlotKeyAt(ImVec2 a_center, float a_radius, Config::SlotTextPosition a_position)
		{
			return a_center + PositionSide(a_position) * (a_radius * Layout::kSlotKeyInset);
		}

		ImVec2 TextInsideCircle(ImVec2 a_center, float a_radius, Config::SlotTextPosition a_position, ImVec2 a_box)
		{
			const ImVec2 side = PositionSide(a_position);
			const ImVec2 edge = side.x == 0.0f ? ImVec2{0.0f, side.y * std::sqrt(std::max(0.0f, a_radius * a_radius - a_box.x * a_box.x * 0.25f))}
											   : side * (a_radius * std::numbers::sqrt2_v<float> * 0.5f);
			const float left = side.x < 0.0f ? edge.x : side.x > 0.0f ? edge.x - a_box.x
																	  : -a_box.x * 0.5f;
			const float top = side.y < 0.0f ? edge.y : edge.y - a_box.y;
			return a_center + ImVec2{left, top};
		}
	}

	template <class Visit>
	void Painter::ForEachSlotKey(const Visit &a_visit) const
	{
		const auto &settings = _scene.settings;
		const auto position = static_cast<Config::SlotTextPosition>(settings.slotKeyPosition);
		const float size = TextPx(settings.slotKeyTextSize);
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			const auto &key = _scene.content.Text(slot).key;
			if (!key.text.empty())
			{
				const float alpha = SlotAlpha(slot);
				a_visit(SlotKey{SlotKeyAt(_layout.SlotCenter(slot), SlotRadius(slot), position), size, key, Fade(_colors.slotKey, alpha), alpha});
			}
		}
	}

	void Painter::DrawSlotLabels(int a_slot) const
	{
		const auto &settings = _scene.settings;
		const auto &content = _scene.content;
		const auto &view = content.View(a_slot);
		const auto &text = content.Text(a_slot);
		const float hover = _scene.motion.hover[static_cast<std::size_t>(a_slot)];
		const bool missing = view.state == Core::SlotState::kMissing;
		const float alpha = SlotAlpha(a_slot);

		if (view.state == Core::SlotState::kEmpty)
		{
			if (settings.showSlotEmpty)
			{
				TextCentered(TextPx(settings.slotEmptyTextSize), _layout.SlotCenter(a_slot), Fade(_colors.slotEmpty, alpha), content.EmptyLabel());
			}
			return;
		}
		const float nameSize = TextPx(settings.slotNameTextSize);
		const ImVec4 textColor = Fade(Mix(_colors.slotName, _colors.slotNameHovered, hover), alpha);
		ImVec2 textAt = NameAt(a_slot);
		for (const auto &line : text.title)
		{
			Text(nameSize, textAt, textColor, line);
			textAt.y += nameSize * Content::kTitleLineSpacing;
		}
		if (missing)
		{
			Text(nameSize * Content::kStatusRatio, textAt, _colors.slotNameDim, text.status);
		}
	}

	void Painter::DrawSlotKeys() const
	{
		if (const auto image = _scene.skin.Find(Skins::Part::kSlotKeyBackground))
		{
			Graphics::PremultipliedDraw premultiplied{_drawList};
			ForEachSlotKey([&](const SlotKey &a_key)
						   { Image(*image, a_key.at, a_key.size * 2.0f, Graphics::PremultipliedColor(a_key.alpha * _alpha)); });
		}
		else
		{
			ForEachSlotKey([&](const SlotKey &a_key)
						   {
				_drawList.AddCircleFilled(a_key.at, a_key.size, Color(_colors.slotKeyBackground, _alpha), kSlotKeySegments);
				_drawList.AddCircle(a_key.at, a_key.size, Color(_colors.slotBorder, a_key.alpha * _alpha), kSlotKeySegments, std::max(1.0f, Px(1.0f))); });
		}
		ForEachSlotKey([&](const SlotKey &a_key)
					   { TextCentered(a_key.size, a_key.at, a_key.color, a_key.text); });
	}

	void Painter::DrawHands() const
	{
		const auto &settings = _scene.settings;
		const auto position = static_cast<Config::SlotTextPosition>(settings.handPosition);
		const float size = TextPx(settings.handTextSize);
		const float nameSize = TextPx(settings.slotNameTextSize);
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			const auto &text = _scene.content.Text(slot);
			if (text.hand.text.empty() || text.title.empty())
			{
				continue;
			}
			const ImVec4 color = Fade(_colors.hand, SlotAlpha(slot));
			const float width = text.hand.Width(size);
			if (position == Config::SlotTextPosition::kNextToName)
			{
				const ImVec2 name = NameAt(slot);
				TextAt(size, {name.x + text.title.front().Width(nameSize) * 0.5f + Px(4.0f), name.y + (nameSize - size) * 0.5f}, color, text.hand.text);
				continue;
			}
			const float inside = SlotRadius(slot) - Px(kSlotInnerRingInset + kSlotHandTextMargin);
			TextAt(size, TextInsideCircle(_layout.SlotCenter(slot), inside, position, {width, size}), color, text.hand.text);
		}
	}
}
