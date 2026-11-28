#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr std::size_t kSlots = static_cast<std::size_t>(Core::kSlotCount);
	}

	void Painter::DrawItemDescriptionBackground() const
	{
		if (const auto skin = _scene.skin.Find(Skins::Part::kItemDescriptionBackground))
		{
			Graphics::PremultipliedDraw premultiplied{_drawList};
			Image(*skin, _layout.center, _layout.itemDescriptionBackgroundRadius * 2.0f, Graphics::PremultipliedColor(_alpha));
			return;
		}

		Core::PerSlot<ImVec2> outer{};
		Core::PerSlot<ImVec2> inner{};
		for (std::size_t i = 0; i < kSlots; ++i)
		{
			outer[i] = _layout.ItemDescriptionBackgroundVertex(static_cast<int>(i), _layout.itemDescriptionBackgroundRadius);
			inner[i] = _layout.ItemDescriptionBackgroundVertex(static_cast<int>(i), _layout.itemDescriptionBackgroundRadius - Px(9.0f));
		}
		const auto next = [](std::size_t a_index)
		{ return (a_index + 1) % kSlots; };

		if (_curve.Active())
		{
			std::array<ImVec2, kSlots * kEdgeSegments> fill{};
			for (std::size_t i = 0; i < fill.size(); ++i)
			{
				const std::size_t edge = i / kEdgeSegments;
				const float t = static_cast<float>(i % kEdgeSegments) / static_cast<float>(kEdgeSegments);
				fill[i] = outer[edge] + (outer[next(edge)] - outer[edge]) * t;
			}
			_drawList.AddConvexPolyFilled(fill.data(), static_cast<int>(fill.size()), Color(_colors.itemDescriptionFill, _alpha));
		}
		else
		{
			_drawList.AddConvexPolyFilled(outer.data(), static_cast<int>(outer.size()), Color(_colors.itemDescriptionFill, _alpha));
		}
		for (std::size_t i = 0; i < kSlots; ++i)
		{
			Line(outer[i], outer[next(i)], Color(_colors.itemDescriptionBorder, _alpha), std::max(1.0f, Px(1.5f)));
			Line(inner[i], inner[next(i)], Color(_colors.itemDescriptionInnerBorder, _alpha), std::max(1.0f, Px(1.0f)));
		}

		const auto &motion = _scene.motion;
		if (_strength <= kHidden || !motion.pointerValid)
		{
			return;
		}
		for (std::size_t i = 0; i < kSlots; ++i)
		{
			const float distance = Core::AngularDistance(Core::SlotCenterAngle(static_cast<int>(i)), motion.pointerAngle);
			const float strength = std::clamp(1.0f - distance / Core::kSliceAngle, 0.0f, 1.0f) * _strength;
			if (strength <= kHidden)
			{
				continue;
			}
			Line(outer[i], outer[next(i)], Color(_colors.highlight, 0.18f * strength * _alpha), Px(9.0f));
			Line(outer[i], outer[next(i)], Color(_colors.highlight, strength * _alpha), std::max(1.5f, Px(3.0f)));
		}
	}

	void Painter::DrawItemDescriptionIcon() const
	{
		const auto &settings = _scene.settings;
		const auto &content = _scene.content;
		if (!settings.showItemIconDescription || _descriptionStrength <= kHidden)
		{
			return;
		}
		const auto &view = content.View(_descriptionSlot);
		if (const auto icon = _scene.icons.Find(_descriptionSlot, IconSize::kItemDescription); icon && view.state != Core::SlotState::kEmpty)
		{
			const float iconSize = static_cast<float>(settings.itemIconDescriptionSize);
			const float top = settings.showItemDescription ? content.Description(_descriptionSlot).iconTop : -iconSize * 0.5f;
			const float size = std::floor(Px(iconSize));
			ImVec2 center{_layout.center.x, _layout.center.y + Px(top) + size * 0.5f};
			if (settings.staticItemDescription)
			{
				center = _layout.center + ImVec2{Px(static_cast<float>(settings.itemIconDescriptionX)), Px(static_cast<float>(settings.itemIconDescriptionY))};
			}
			Graphics::PremultipliedDraw premultiplied{_drawList};
			Image(*icon, center, size, Graphics::PremultipliedColor((view.state == Core::SlotState::kMissing ? kNoneLeftAlpha : 1.0f) * _descriptionStrength * _alpha));
		}
	}

	void Painter::DrawItemDescription() const
	{
		if (!_scene.settings.showItemDescription || _descriptionStrength <= kHidden)
		{
			return;
		}
		for (const auto &piece : _scene.content.Description(_descriptionSlot).pieces)
		{
			ImVec4 color = _colors.descriptionBright;
			if (piece.role == ItemDescriptionLayout::TextRole::kLabel)
			{
				color = _colors.descriptionDim;
			}
			else if (piece.role == ItemDescriptionLayout::TextRole::kText)
			{
				color = _colors.description;
			}
			TextAt(piece.size * _layout.scale, _layout.center + piece.topLeft * _layout.scale, Fade(color, _descriptionStrength), piece.text);
		}
	}
}
