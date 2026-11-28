#include "UI/OrbitMenu/MenuPainter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kCircleSegments = 64;
		constexpr std::size_t kSlots = static_cast<std::size_t>(Core::kSlotCount);
		constexpr std::size_t kEdgeSegments = 8;
		constexpr int kImageGrid = 8;
		constexpr int kSlotKeyBadgeSegments = 24;
		constexpr int kPageIndicatorSegments = 24;
		constexpr float kPointerHalfWidth = 0.393f;
		constexpr float kPointerStartRadius = 148.0f;
		constexpr float kControlsHintLineSpacing = 1.3f;
		constexpr float kSlotInnerRingInset = 6.0f;
		constexpr float kSlotHandTextMargin = 8.0f;
		constexpr float kHidden = 0.01f;
		constexpr float kNoneLeftAlpha = 0.45f;

		ImVec2 PositionSide(Config::BadgePosition a_position)
		{
			const auto index = static_cast<int>(a_position);
			return {static_cast<float>(index % 3 - 1), index < 3 ? -1.0f : 1.0f};
		}

		ImVec2 BadgeAt(ImVec2 a_center, float a_radius, Config::BadgePosition a_position)
		{
			return a_center + PositionSide(a_position) * (a_radius * MenuLayout::kBadgeInset);
		}

		ImVec2 TextInsideCircle(ImVec2 a_center, float a_radius, Config::BadgePosition a_position, ImVec2 a_box)
		{
			const ImVec2 side = PositionSide(a_position);
			const ImVec2 edge = side.x == 0.0f ? ImVec2{0.0f, side.y * std::sqrt(std::max(0.0f, a_radius * a_radius - a_box.x * a_box.x * 0.25f))}
											   : side * (a_radius * std::numbers::sqrt2_v<float> * 0.5f);
			const float left = side.x < 0.0f ? edge.x : side.x > 0.0f ? edge.x - a_box.x
																	  : -a_box.x * 0.5f;
			const float top = side.y < 0.0f ? edge.y : edge.y - a_box.y;
			return a_center + ImVec2{left, top};
		}

		MenuLayout LayoutFor(ImVec2 a_display, const Scene &a_scene)
		{
			const auto &settings = a_scene.settings;
			const ImVec2 offset{static_cast<float>(settings.positionX), static_cast<float>(settings.positionY)};
			return MenuLayout::Compute(a_display, offset, settings.orbitMenuScale, a_scene.motion.scale, static_cast<float>(settings.slotDistance),
									   static_cast<float>(settings.slotSize) * 0.5f, static_cast<float>(settings.itemDescriptionBackgroundSize) * 0.5f);
		}
	}

	MenuPainter::MenuPainter(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display, const Scene &a_scene) : _drawList(a_drawList),
																											   _font(a_font),
																											   _display(a_display),
																											   _scene(a_scene),
																											   _colors(a_scene.content.Colors()),
																											   _layout(LayoutFor(a_display, a_scene)),
																											   _curve(OrbitMenuCurved::Make(a_scene.settings, _layout, a_display)),
																											   _alpha(a_scene.motion.alpha),
																											   _strongestSlot(a_scene.motion.StrongestSlot()),
																											   _strength(a_scene.motion.hover[static_cast<std::size_t>(_strongestSlot)])
	{
	}

	void MenuPainter::Paint() const
	{
		DrawBackdrop();
		const int firstVertex = _drawList.VtxBuffer.Size;
		if (_scene.settings.showItemDescriptionBackground)
		{
			DrawCentre();
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
		DrawIcons();
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			DrawSlotLabels(slot);
		} 
		DrawBadges();
		DrawHands();
		DrawDescription();
		const int controlsHintVertex = _drawList.VtxBuffer.Size;
		DrawControlsHint();
		DrawPageIndicator();
		Bend(firstVertex, controlsHintVertex);
	}

	float MenuPainter::Px(float a_base) const
	{
		return a_base * _layout.scale;
	}

	float MenuPainter::TextPx(std::int32_t a_size) const
	{
		return std::max(8.0f, Px(static_cast<float>(a_size)));
	}

	float MenuPainter::SlotRadius(int a_slot) const
	{
		return _layout.slotRadius * (1.0f + MenuLayout::kSlotHoverGrowth * _scene.motion.hover[static_cast<std::size_t>(a_slot)]);
	}

	float MenuPainter::SlotAlpha(int a_slot) const
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

	ImVec2 MenuPainter::NameAt(int a_slot) const
	{
		const ImVec2 center = _layout.SlotCenter(a_slot);
		return {center.x, center.y + SlotRadius(a_slot) + Px(8.0f)};
	}

	template <class Visit>
	void MenuPainter::ForEachBadge(const Visit &a_visit) const
	{
		const auto &settings = _scene.settings;
		const auto position = static_cast<Config::BadgePosition>(settings.slotKeyPosition);
		const float size = TextPx(settings.slotKeyTextSize);
		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			const auto &key = _scene.content.Text(slot).key;
			if (!key.text.empty())
			{
				const float alpha = SlotAlpha(slot);
				a_visit(Badge{BadgeAt(_layout.SlotCenter(slot), SlotRadius(slot), position), size, key, Fade(_colors.slotKey, alpha), alpha});
			}
		}
	}

	void MenuPainter::TextAt(float a_size, ImVec2 a_topLeft, ImVec4 a_color, std::string_view a_text) const
	{
		const char *end = a_text.data() + a_text.size();
		const ImVec2 pos{std::floor(a_topLeft.x), std::floor(a_topLeft.y)};
		const float shadow = std::max(1.0f, Px(1.5f));
		_drawList.AddText(&_font, a_size, {pos.x + shadow, pos.y + shadow}, Color(_colors.textShadow, a_color.w * _alpha), a_text.data(), end);
		_drawList.AddText(&_font, a_size, pos, Color(a_color, _alpha), a_text.data(), end);
	}

	void MenuPainter::Text(float a_size, ImVec2 a_topCenter, ImVec4 a_color, const TextLine &a_line) const
	{
		TextAt(a_size, {a_topCenter.x - a_line.Width(a_size) * 0.5f, a_topCenter.y}, a_color, a_line.text);
	}

	void MenuPainter::TextCentered(float a_size, ImVec2 a_center, ImVec4 a_color, const TextLine &a_line) const
	{
		Text(a_size, {a_center.x, a_center.y - a_size * 0.5f}, a_color, a_line);
	}

	void MenuPainter::Line(ImVec2 a_from, ImVec2 a_to, ImU32 a_color, float a_thickness) const
	{
		if (!_curve.Active())
		{
			_drawList.AddLine(a_from, a_to, a_color, a_thickness);
			return;
		}
		std::array<ImVec2, kEdgeSegments + 1> points{};
		for (std::size_t i = 0; i < points.size(); ++i)
		{
			points[i] = a_from + (a_to - a_from) * (static_cast<float>(i) / static_cast<float>(kEdgeSegments));
		}
		_drawList.AddPolyline(points.data(), static_cast<int>(points.size()), a_color, ImDrawFlags_None, a_thickness);
	}

	void MenuPainter::Image(const Graphics::CachedImage &a_image, ImVec2 a_center, float a_size, ImU32 a_color) const
	{
		const ImVec2 half{a_size * 0.5f, a_size * 0.5f};
		if (!_curve.Active())
		{
			const ImVec2 min{std::floor(a_center.x - half.x + 0.5f), std::floor(a_center.y - half.y + 0.5f)};
			_drawList.AddImage(a_image.texture, min, min + ImVec2{a_size, a_size}, a_image.uv0, a_image.uv1, a_color);
			return;
		}
		const ImVec2 origin = a_center - half;
		const ImVec2 step{a_size / kImageGrid, a_size / kImageGrid};
		const ImVec2 uvStep = (a_image.uv1 - a_image.uv0) / static_cast<float>(kImageGrid);
		_drawList.PushTexture(a_image.texture);
		_drawList.PrimReserve(kImageGrid * kImageGrid * 6, kImageGrid * kImageGrid * 4);
		for (int row = 0; row < kImageGrid; ++row)
		{
			for (int column = 0; column < kImageGrid; ++column)
			{
				const ImVec2 cell{static_cast<float>(column), static_cast<float>(row)};
				const ImVec2 next = cell + ImVec2{1.0f, 1.0f};
				_drawList.PrimRectUV(origin + step * cell, origin + step * next, a_image.uv0 + uvStep * cell, a_image.uv0 + uvStep * next, a_color);
			}
		}
		_drawList.PopTexture();
	}

	void MenuPainter::DrawBackdrop() const
	{
		if (_scene.settings.showBackground && _colors.background.w > 0.0f)
		{
			_drawList.AddRectFilled({0.0f, 0.0f}, _display, Color(_colors.background, _alpha));
		}
	}

	void MenuPainter::DrawCentre() const
	{
		const auto skin = _scene.skin.Find(Skins::Part::kCentre);
		Core::PerSlot<ImVec2> outer{};
		for (std::size_t i = 0; i < kSlots; ++i)
		{
			outer[i] = _layout.ItemDescriptionBackgroundVertex(static_cast<int>(i), _layout.itemDescriptionBackgroundRadius);
		}
		const auto next = [](std::size_t a_index)
		{ return (a_index + 1) % kSlots; };

		if (skin)
		{
			Graphics::PremultipliedDraw premultiplied{_drawList};
			Image(*skin, _layout.center, _layout.itemDescriptionBackgroundRadius * 2.0f, Graphics::PremultipliedColor(_alpha));
		}
		else
		{
			Core::PerSlot<ImVec2> inner{};
			for (std::size_t i = 0; i < kSlots; ++i)
			{
				inner[i] = _layout.ItemDescriptionBackgroundVertex(static_cast<int>(i), _layout.itemDescriptionBackgroundRadius - Px(9.0f));
			}
			if (_curve.Active())
			{
				std::array<ImVec2, kSlots * kEdgeSegments> fill{};
				for (std::size_t i = 0; i < fill.size(); ++i)
				{
					const std::size_t edge = i / kEdgeSegments;
					const float t = static_cast<float>(i % kEdgeSegments) / static_cast<float>(kEdgeSegments);
					fill[i] = outer[edge] + (outer[next(edge)] - outer[edge]) * t;
				}
				_drawList.AddConvexPolyFilled(fill.data(), static_cast<int>(fill.size()), Color(_colors.centreFill, _alpha));
			}
			else
			{
				_drawList.AddConvexPolyFilled(outer.data(), static_cast<int>(outer.size()), Color(_colors.centreFill, _alpha));
			}
			for (std::size_t i = 0; i < kSlots; ++i)
			{
				Line(outer[i], outer[next(i)], Color(_colors.centreBorder, _alpha), std::max(1.0f, Px(1.5f)));
				Line(inner[i], inner[next(i)], Color(_colors.centreInnerBorder, _alpha), std::max(1.0f, Px(1.0f)));
			}
		}

		const auto &motion = _scene.motion;
		if (skin || _strength <= kHidden || !motion.pointerValid)
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

	void MenuPainter::DrawPointer() const
	{
		const auto &motion = _scene.motion;
		if (_strength <= kHidden || !motion.pointerValid)
		{
			return;
		}
		const float angle = motion.pointerAngle;
		const float size = Px(static_cast<float>(_scene.settings.pointerSize));
		const float baseDistance = MenuLayout::InnerRadius(Px(kPointerStartRadius)) + Px(static_cast<float>(_scene.settings.pointerDistance));
		const float rotation = static_cast<float>(_scene.settings.pointerRotation) * Core::kPi / 180.0f;
		if (const auto image = _scene.skin.Find(Skins::Part::kPointer))
		{
			const ImVec2 center = _layout.PointAt(angle, baseDistance + size * 0.5f);
			const ImVec2 up = Core::DirectionFromAngle(angle + rotation) * (size * 0.5f);
			const ImVec2 right{-up.y, up.x};
			Graphics::PremultipliedDraw premultiplied{_drawList};
			_drawList.AddImageQuad(image->texture, center + up - right, center + up + right, center - up + right, center - up - right, image->uv0,
								   {image->uv1.x, image->uv0.y}, image->uv1, {image->uv0.x, image->uv1.y}, Graphics::PremultipliedColor(_strength * _alpha));
			return;
		}
		const float spread = std::atan2(size * kPointerHalfWidth, baseDistance);
		ImVec2 tip = _layout.PointAt(angle, baseDistance + size * 0.5f);
		ImVec2 left = _layout.PointAt(angle - spread, baseDistance);
		ImVec2 right = _layout.PointAt(angle + spread, baseDistance);
		if (rotation != 0.0f)
		{
			const ImVec2 pivot = _layout.PointAt(angle, baseDistance + size * 0.25f);
			const float cosine = std::cos(rotation);
			const float sine = std::sin(rotation);
			for (ImVec2 *point : {&tip, &left, &right})
			{
				const ImVec2 offset = *point - pivot;
				*point = pivot + ImVec2{offset.x * cosine - offset.y * sine, offset.x * sine + offset.y * cosine};
			}
		}
		_drawList.AddTriangleFilled(left, tip, right, Color(_colors.pointer, _strength * _alpha));
	}

	void MenuPainter::DrawSlotGlow(int a_slot) const
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
			Image(*image, center, radius * 2.0f * MenuLayout::kSlotGlowRatio, Graphics::PremultipliedColor(glow * _alpha));
			return;
		}
		_drawList.AddCircleFilled(center, radius + Px(14.0f + 10.0f * flash), Color(_colors.highlight, 0.06f * glow * _alpha), kCircleSegments);
		_drawList.AddCircleFilled(center, radius + Px(7.0f), Color(_colors.highlight, 0.10f * glow * _alpha), kCircleSegments);
	}

	void MenuPainter::DrawSlotShape(int a_slot) const
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

	void MenuPainter::DrawSkinSlot(int a_slot) const
	{
		const auto &skin = _scene.skin;
		const bool equipped = _scene.content.View(a_slot).state == Core::SlotState::kEquipped;
		const auto equippedImage = equipped ? skin.Find(Skins::Part::kSlotEquipped) : std::nullopt;
		const auto base = equippedImage ? equippedImage : skin.Find(Skins::Part::kSlot);
		const auto hovered = skin.Find(Skins::Part::kSlotHovered);
		const float hover = _scene.motion.hover[static_cast<std::size_t>(a_slot)];
		const ImVec2 center = _layout.SlotCenter(a_slot);
		const float size = SlotRadius(a_slot) * 2.0f;

		if (base)
		{
			Image(*base, center, size, Graphics::PremultipliedColor(_alpha * (hovered ? 1.0f - hover : 1.0f)));
		}
		if (hovered && hover > kHidden)
		{
			Image(*hovered, center, size, Graphics::PremultipliedColor(_alpha * hover));
		}
	}

	void MenuPainter::DrawIcons() const
	{
		const auto &settings = _scene.settings;
		const auto &content = _scene.content;
		const auto &icons = _scene.icons;
		Graphics::PremultipliedDraw premultiplied{_drawList};

		if (settings.showItemIconDescription && _strength > kHidden)
		{
			const auto &view = content.View(_strongestSlot);
			if (const auto icon = icons.Find(_strongestSlot, IconSize::kCentre); icon && view.state != Core::SlotState::kEmpty)
			{
				const float iconSize = static_cast<float>(settings.itemIconDescriptionSize);
				const float top = settings.showItemDescription ? content.Description(_strongestSlot).iconTop : -iconSize * 0.5f;
				const float size = std::floor(Px(iconSize));
				ImVec2 center{_layout.center.x, _layout.center.y + Px(top) + size * 0.5f};
				if (settings.staticItemDescription)
				{
					center = _layout.center + ImVec2{Px(static_cast<float>(settings.itemIconDescriptionX)), Px(static_cast<float>(settings.itemIconDescriptionY))};
				}
				Image(*icon, center, size, Graphics::PremultipliedColor((view.state == Core::SlotState::kMissing ? kNoneLeftAlpha : 1.0f) * _strength * _alpha));
			}
		}

		for (int slot = 0; slot < Core::kSlotCount; ++slot)
		{
			if (content.View(slot).state == Core::SlotState::kEmpty)
			{
				continue;
			}
			if (const auto icon = icons.Find(slot, IconSize::kSlot))
			{
				const float hover = _scene.motion.hover[static_cast<std::size_t>(slot)];
				const float size = std::floor(Px(static_cast<float>(settings.slotIconSize)) * (1.0f + MenuLayout::kSlotHoverGrowth * hover));
				Image(*icon, _layout.SlotCenter(slot), size, Graphics::PremultipliedColor(SlotAlpha(slot) * _alpha, 0.8f + 0.2f * hover));
			}
		}
	}

	void MenuPainter::DrawDescription() const
	{
		using TextRole = ItemDescriptionLayout::TextRole;
		if (!_scene.settings.showItemDescription || _strength <= kHidden)
		{
			return;
		}
		for (const auto &piece : _scene.content.Description(_strongestSlot).pieces)
		{
			ImVec4 color = _colors.descriptionBright;
			if (piece.role == TextRole::kLabel)
			{
				color = _colors.descriptionDim;
			}
			else if (piece.role == TextRole::kText)
			{
				color = _colors.description;
			}
			TextAt(piece.size * _layout.scale, _layout.center + piece.topLeft * _layout.scale, Fade(color, _strength), piece.text);
		}
	}

	void MenuPainter::DrawSlotLabels(int a_slot) const
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
			textAt.y += nameSize * MenuContent::kTitleLineSpacing;
		}
		if (missing)
		{
			Text(nameSize * MenuContent::kStatusRatio, textAt, _colors.slotNameDim, text.status);
		}
	}

	void MenuPainter::DrawBadges() const
	{
		if (const auto image = _scene.skin.Find(Skins::Part::kSlotKeyBadge))
		{
			Graphics::PremultipliedDraw premultiplied{_drawList};
			ForEachBadge([&](const Badge &a_badge)
						 { Image(*image, a_badge.at, a_badge.size * 2.0f, Graphics::PremultipliedColor(a_badge.alpha * _alpha)); });
		}
		else
		{
			ForEachBadge([&](const Badge &a_badge)
						 {
				_drawList.AddCircleFilled(a_badge.at, a_badge.size, Color(_colors.keyBadge, _alpha), kSlotKeyBadgeSegments);
				_drawList.AddCircle(a_badge.at, a_badge.size, Color(_colors.slotBorder, a_badge.alpha * _alpha), kSlotKeyBadgeSegments, std::max(1.0f, Px(1.0f))); });
		}
		ForEachBadge([&](const Badge &a_badge)
					 { TextCentered(a_badge.size, a_badge.at, a_badge.color, a_badge.text); });
	}

	void MenuPainter::DrawHands() const
	{
		const auto &settings = _scene.settings;
		const auto position = static_cast<Config::BadgePosition>(settings.handPosition);
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
			if (position == Config::BadgePosition::kNextToName)
			{
				const ImVec2 name = NameAt(slot);
				TextAt(size, {name.x + text.title.front().Width(nameSize) * 0.5f + Px(4.0f), name.y + (nameSize - size) * 0.5f}, color, text.hand.text);
				continue;
			}
			const float inside = SlotRadius(slot) - Px(kSlotInnerRingInset + kSlotHandTextMargin);
			TextAt(size, TextInsideCircle(_layout.SlotCenter(slot), inside, position, {width, size}), color, text.hand.text);
		}
	}

	void MenuPainter::DrawControlsHint() const
	{
		const auto &settings = _scene.settings;
		if (!settings.showControlsHint || !_scene.input.open)
		{
			return;
		}
		const float size = TextPx(settings.controlsHintTextSize);
		const auto &pageLine = _scene.content.PageLine();
		const float pageLineGap = pageLine.text.empty() ? 0.0f : size * kControlsHintLineSpacing;
		ImVec2 at = _layout.center + ImVec2{Px(static_cast<float>(settings.controlsHintX)), Px(static_cast<float>(settings.controlsHintY))};
		at.y = std::min(at.y, _display.y - size * 1.6f - pageLineGap);
		Text(size, at, _colors.controlsHint, _scene.content.ControlsHint());
		if (pageLineGap > 0.0f)
		{
			at.y += pageLineGap;
			Text(size, at, _colors.controlsHint, pageLine);
		}
	}

	void MenuPainter::DrawPageIndicator() const
	{
		const auto &settings = _scene.settings;
		if (!settings.enablePages || !settings.showPageIndicator || !_scene.input.open)
		{
			return;
		}
		const int count = settings.numberOfPages;
		const int current = _scene.content.Page();
		const int columns = std::min(settings.pageIndicatorColumns, count);
		const int rows = (count + columns - 1) / columns;
		const float size = Px(static_cast<float>(settings.pageIndicatorSize));
		const float step = size + Px(static_cast<float>(settings.pageIndicatorSpacing));
		const ImVec2 middle = _layout.center + ImVec2{Px(static_cast<float>(settings.pageIndicatorX)), Px(static_cast<float>(settings.pageIndicatorY))};
		const float rotation = static_cast<float>(settings.pageIndicatorRotation) * Core::kPi / 180.0f;
		const ImVec2 across = Core::DirectionFromAngle(rotation + Core::kPi * 0.5f);
		const ImVec2 down = Core::DirectionFromAngle(rotation + Core::kPi);

		std::array<ImVec2, static_cast<std::size_t>(Config::Settings::kMaxNumberOfPages)> spots{};
		for (int page = 0; page < count; ++page)
		{
			const int row = page / columns;
			const int inRow = row == rows - 1 ? count - row * columns : columns;
			const float x = (static_cast<float>(page % columns) - static_cast<float>(inRow - 1) * 0.5f) * step;
			const float y = (static_cast<float>(row) - static_cast<float>(rows - 1) * 0.5f) * step;
			spots[static_cast<std::size_t>(page)] = middle + across * x + down * y;
		}

		const auto active = _scene.skin.Find(Skins::Part::kPageIndicatorActive);
		const auto inactive = _scene.skin.Find(Skins::Part::kPageIndicatorInactive);
		if (active || inactive)
		{
			const ImVec2 up = down * (-size * 0.5f);
			const ImVec2 right = across * (size * 0.5f);
			Graphics::PremultipliedDraw premultiplied{_drawList};
			for (int page = 0; page < count; ++page)
			{
				if (const auto &image = page == current ? active : inactive)
				{
					const ImVec2 at = spots[static_cast<std::size_t>(page)];
					_drawList.AddImageQuad(image->texture, at + up - right, at + up + right, at - up + right, at - up - right, image->uv0,
										   {image->uv1.x, image->uv0.y}, image->uv1, {image->uv0.x, image->uv1.y}, Graphics::PremultipliedColor(_alpha));
				}
			}
		}
		for (int page = 0; page < count; ++page)
		{
			const bool isCurrent = page == current;
			if (isCurrent ? active : inactive)
			{
				continue;
			}
			const ImVec2 at = spots[static_cast<std::size_t>(page)];
			if (isCurrent)
			{
				_drawList.AddCircleFilled(at, size * 0.5f, Color(_colors.highlight, _alpha), kPageIndicatorSegments);
			}
			else
			{
				_drawList.AddCircle(at, size * 0.5f - Px(1.0f), Color(_colors.highlight, 0.6f * _alpha), kPageIndicatorSegments, std::max(1.0f, Px(1.5f)));
			}
		}
	}

	void MenuPainter::Bend(int a_firstVertex, int a_controlsHintVertex) const
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
