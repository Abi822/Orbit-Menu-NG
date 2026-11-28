#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		Layout LayoutFor(ImVec2 a_display, const Scene &a_scene)
		{
			const auto &settings = a_scene.settings;
			const ImVec2 offset{static_cast<float>(settings.positionX), static_cast<float>(settings.positionY)};
			return Layout::Compute(a_display, offset, settings.orbitMenuScale, a_scene.motion.scale, static_cast<float>(settings.slotDistance),
								   static_cast<float>(settings.slotSize) * 0.5f, static_cast<float>(settings.itemDescriptionBackgroundSize) * 0.5f);
		}
	}

	Painter::Painter(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display, const Scene &a_scene) : _drawList(a_drawList),
																									   _font(a_font),
																									   _display(a_display),
																									   _scene(a_scene),
																									   _colors(a_scene.content.Colors()),
																									   _layout(LayoutFor(a_display, a_scene)),
																									   _curve(Curve::Make(a_scene.settings, _layout, a_display)),
																									   _alpha(a_scene.motion.alpha),
																									   _strongestSlot(a_scene.motion.StrongestSlot()),
																									   _strength(a_scene.motion.hover[static_cast<std::size_t>(_strongestSlot)]),
																									   _descriptionSlot(_strongestSlot),
																									   _descriptionStrength(_strength)
	{
		const int last = a_scene.input.lastEquippedSlot;
		const bool closingAfterHighlight = !a_scene.input.open && a_scene.motion.pointerValid;
		if (a_scene.settings.showLastEquippedItem && last >= 0 && !closingAfterHighlight && 1.0f - _strength > _strength &&
			a_scene.content.View(last).state == Core::SlotState::kEquipped)
		{
			_descriptionSlot = last;
			_descriptionStrength = 1.0f - _strength;
		}
	}

	void Painter::Paint() const
	{
		DrawBackdrop();
		const int firstVertex = _drawList.VtxBuffer.Size;
		if (_scene.settings.showItemDescriptionBackground)
		{
			DrawItemDescriptionBackground();
		}
		if (_scene.settings.showPointer)
		{
			DrawPointer();
		}
		if (_scene.settings.showSlotGlow)
		{
			for (int slot = 0; slot < Core::kSlotCount; ++slot)
			{
				DrawSlotGlow(slot);
			}
		}
		if (_scene.settings.showSlotBackground)
		{
			if (_scene.skin.Find(Skins::Part::kSlot))
			{
				Graphics::PremultipliedDraw premultiplied{_drawList};
				for (int slot = 0; slot < Core::kSlotCount; ++slot)
				{
					DrawSkinSlot(slot);
				}
			}
			else
			{
				for (int slot = 0; slot < Core::kSlotCount; ++slot)
				{
					DrawSlotShape(slot);
				}
			}
		}
		DrawItemDescriptionIcon();
		DrawSlotIcons();
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			DrawSlotLabels(slot);
		}
		DrawSlotKeys();
		DrawHands();
		DrawItemDescription();
		const int controlsHintVertex = _drawList.VtxBuffer.Size;
		DrawControlsHint();
		DrawPageIndicator();
		Bend(firstVertex, controlsHintVertex);
	}

	float Painter::Px(float a_base) const
	{
		return a_base * _layout.scale;
	}

	float Painter::TextPx(std::int32_t a_size) const
	{
		return std::max(8.0f, Px(static_cast<float>(a_size)));
	}

	float Painter::SlotRadius(int a_slot) const
	{
		return _layout.slotRadius * (1.0f + Layout::kSlotHoverGrowth * _scene.motion.hover[static_cast<std::size_t>(a_slot)]);
	}

	float Painter::SlotAlpha(int a_slot) const
	{
		switch (_scene.content.View(a_slot).state)
		{
		case Core::SlotState::kMissing:
			return kNoneLeftAlpha;
		case Core::SlotState::kEmpty:
			return 0.7f;
		default:
			return 1.0f;
		}
	}

	ImVec2 Painter::NameAt(int a_slot) const
	{
		const ImVec2 center = _layout.SlotCenter(a_slot);
		const auto &settings = _scene.settings;
		return {center.x + Px(static_cast<float>(settings.slotNameX)), center.y + SlotRadius(a_slot) + Px(8.0f + static_cast<float>(settings.slotNameY))};
	}

	void Painter::Bend(int a_firstVertex, int a_controlsHintVertex) const
	{
		if (!_curve.Active())
		{
			return;
		}
		for (int i = a_firstVertex; i < _drawList.VtxBuffer.Size; ++i)
		{
			_curve.Project(_drawList.VtxBuffer[i].pos, i < a_controlsHintVertex);
		}
	}
}
