#pragma once

#include "Core/OrbitMath.h"
#include <algorithm>

namespace Orbit::UI::OrbitMenu
{
	struct MenuLayout
	{
		static constexpr float kBaseScreenHeight = 1080.0f;
		static constexpr float kBaseRingRadius = 300.0f;
		static constexpr float kSlotHoverGrowth = 0.08f;
		static constexpr float kSlotGlowRatio = 1.5f;
		static constexpr float kBadgeInset = 0.72f;
		ImVec2 center{};
		float scale = 1.0f;
		float ringRadius = kBaseRingRadius;
		float slotRadius = 0.0f;
		float itemDescriptionBackgroundRadius = 0.0f;

		static float ScreenScale(ImVec2 a_display)
		{
			return std::min(a_display.x, a_display.y) / kBaseScreenHeight;
		}

		static constexpr float LargestSlotRadius(float a_slotRadius)
		{
			return a_slotRadius * (1.0f + kSlotHoverGrowth);
		}

		static MenuLayout Compute(ImVec2 a_display, ImVec2 a_offset, float a_userScale, float a_animScale, float a_ringRadius, float a_slotRadius, float a_itemDescriptionBackgroundRadius)
		{
			MenuLayout layout;
			const float screenScale = ScreenScale(a_display);
			layout.center = a_display * 0.5f + a_offset * screenScale;
			layout.scale = screenScale * a_userScale * a_animScale;
			layout.ringRadius = a_ringRadius * layout.scale;
			layout.slotRadius = a_slotRadius * layout.scale;
			layout.itemDescriptionBackgroundRadius = a_itemDescriptionBackgroundRadius * layout.scale;
			return layout;
		}

		ImVec2 PointAt(float a_angle, float a_distance) const
		{
			return center + Core::DirectionFromAngle(a_angle) * a_distance;
		}

		ImVec2 SlotCenter(int a_slot) const
		{
			return PointAt(Core::SlotCenterAngle(a_slot), ringRadius);
		}

		ImVec2 ItemDescriptionBackgroundVertex(int a_index, float a_radius) const
		{
			return PointAt(Core::SlotCenterAngle(a_index) - Core::kSliceAngle * 0.5f, a_radius);
		}

		static float InnerRadius(float a_radius)
		{
			return a_radius * std::cos(Core::kSliceAngle * 0.5f);
		}
	};
}
