#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kCircleSegments = 64;
	}

	void Painter::DrawSlotGlow(int a_slot) const
	{
		const auto &motion = _scene.motion;
		const float hover = motion.hover[static_cast<std::size_t>(a_slot)];
		const float flash = a_slot == motion.flashSlot ? motion.flash : 0.0f;
		if (hover <= kHidden && flash <= kHidden)
		{
			return;
		}
		const ImVec2 center = _layout.SlotCenter(a_slot);
		const float radius = SlotRadius(a_slot);
		const float glow = std::max(hover, flash);
		if (const auto image = _scene.skin.Find(Skins::Part::kSlotGlow))
		{
			Graphics::PremultipliedDraw premultiplied{_drawList};
			Image(*image, center, radius * 2.0f * Layout::kSlotGlowRatio, Graphics::PremultipliedColor(glow * _alpha));
			return;
		}
		_drawList.AddCircleFilled(center, radius + Px(14.0f + 10.0f * flash), Color(_colors.slotGlow, 0.06f * glow * _alpha), kCircleSegments);
		_drawList.AddCircleFilled(center, radius + Px(7.0f), Color(_colors.slotGlow, 0.10f * glow * _alpha), kCircleSegments);
	}

	void Painter::DrawSlotShape(int a_slot) const
	{
		const ImVec2 center = _layout.SlotCenter(a_slot);
		const float radius = SlotRadius(a_slot);
		const float hover = _scene.motion.hover[static_cast<std::size_t>(a_slot)];
		const bool equipped = _scene.content.View(a_slot).state == Core::SlotState::kEquipped;

		_drawList.AddCircleFilled(center, radius, Color(_colors.slotFill, _alpha), kCircleSegments);
		_drawList.AddCircle(center, radius - Px(kSlotInnerRingInset), Color(equipped ? _colors.highlight : _colors.slotInnerRing, (equipped ? 0.5f : 1.0f) * _alpha),
							kCircleSegments, std::max(1.0f, Px(1.0f)));
		_drawList.AddCircle(center, radius, Color(Mix(_colors.slotBorder, _colors.highlight, hover), SlotAlpha(a_slot) * _alpha), kCircleSegments,
							std::max(1.0f, Px(2.0f + hover)));
	}

	void Painter::DrawSkinSlot(int a_slot) const
	{
		const auto &skin = _scene.skin;
		const bool equipped = _scene.content.View(a_slot).state == Core::SlotState::kEquipped;
		const auto equippedImage = equipped ? skin.Find(Skins::Part::kSlotEquipped) : std::nullopt;
		const auto base = equippedImage ? equippedImage : skin.Find(Skins::Part::kSlot);
		const auto hovered = skin.Find(Skins::Part::kSlotHovered);
		const float hover = _scene.motion.hover[static_cast<std::size_t>(a_slot)];
		const ImVec2 center = _layout.SlotCenter(a_slot);
		const float size = SlotRadius(a_slot) * 2.0f;

		const float baseAlpha = hovered ? 1.0f - hover : 1.0f;
		if (base && baseAlpha > kHidden)
		{
			Image(*base, center, size, Graphics::PremultipliedColor(_alpha * baseAlpha));
		}
		if (hovered && hover > kHidden)
		{
			Image(*hovered, center, size, Graphics::PremultipliedColor(_alpha * hover));
		}
	}

	void Painter::DrawSlotIcons() const
	{
		const auto &settings = _scene.settings;
		const auto &content = _scene.content;
		const auto &icons = _scene.icons;
		const ImVec2 offset{Px(static_cast<float>(settings.slotIconX)), Px(static_cast<float>(settings.slotIconY))};
		Graphics::PremultipliedDraw premultiplied{_drawList};
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			if (content.View(slot).state == Core::SlotState::kEmpty)
			{
				continue;
			}
			if (const auto icon = icons.Find(slot, IconSize::kSlot))
			{
				const float hover = _scene.motion.hover[static_cast<std::size_t>(slot)];
				const float size = std::floor(Px(static_cast<float>(settings.slotIconSize)) * (1.0f + Layout::kSlotHoverGrowth * hover));
				Image(*icon, _layout.SlotCenter(slot) + offset, size, Graphics::PremultipliedColor(SlotAlpha(slot) * _alpha, 0.8f + 0.2f * hover));
			}
		}
	}
}
