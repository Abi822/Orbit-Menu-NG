#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr float kPointerHalfWidth = 0.393f;
		constexpr float kPointerStartRadius = 148.0f;
	}

	void Painter::DrawPointer() const
	{
		const auto &motion = _scene.motion;
		if (_strength <= kHidden || !motion.pointerValid)
		{
			return;
		}
		const float angle = motion.pointerAngle;
		const float size = Px(static_cast<float>(_scene.settings.pointerSize));
		const float baseDistance = Layout::InnerRadius(Px(kPointerStartRadius)) + Px(static_cast<float>(_scene.settings.pointerDistance));
		const float rotation = static_cast<float>(_scene.settings.pointerRotation) * Core::kPi / 180.0f;
		if (const auto image = _scene.skin.Find(Skins::Part::kPointer))
		{
			const ImVec2 center = _layout.PointAt(angle, baseDistance + size * 0.5f);
			const ImVec2 up = Core::DirectionFromAngle(angle + rotation) * (size * 0.5f);
			const ImVec2 right{-up.y, up.x};
			Graphics::PremultipliedDraw premultiplied{_drawList};
			RotatedImage(*image, center, up, right, Graphics::PremultipliedColor(_strength * _alpha));
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
}
