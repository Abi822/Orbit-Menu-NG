#pragma once

#include "Core/Slots.h"
#include <imgui.h>
#include <cmath>
#include <numbers>

namespace Orbit::Core
{
	inline constexpr float kPi = std::numbers::pi_v<float>;
	inline constexpr float kTwoPi = 2.0f * kPi;
	inline constexpr float kSliceAngle = kTwoPi / static_cast<float>(kSlotCount);

	inline float AngleFromUp(ImVec2 a_v)
	{
		float angle = std::atan2(a_v.x, -a_v.y);
		if (angle < 0.0f)
		{
			angle += kTwoPi;
		}
		return angle;
	}

	inline ImVec2 DirectionFromAngle(float a_angle)
	{
		return {std::sin(a_angle), -std::cos(a_angle)};
	}

	inline float SlotCenterAngle(int a_slot)
	{
		return static_cast<float>(a_slot) * kSliceAngle;
	}

	inline int SlotFromAngle(float a_angle)
	{
		const int slot = static_cast<int>(std::floor((a_angle + kSliceAngle * 0.5f) / kSliceAngle));
		return ((slot % kSlotCount) + kSlotCount) % kSlotCount;
	}

	inline float AngularDistance(float a_lhs, float a_rhs)
	{
		const float diff = std::fmod(std::fabs(a_lhs - a_rhs), kTwoPi);
		return diff > kPi ? kTwoPi - diff : diff;
	}
}
