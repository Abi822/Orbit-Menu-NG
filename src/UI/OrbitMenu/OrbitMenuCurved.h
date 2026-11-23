#pragma once

#include "Config/Settings.h"
#include "UI/OrbitMenu/MenuLayout.h"

namespace Orbit::UI::OrbitMenu
{
	struct OrbitMenuCurved
	{
		ImVec2 center{};
		float bendRadius = 0.0f;
		float turnCos = 1.0f;
		float turnSin = 0.0f;
		float distance = 1.0f;

		static OrbitMenuCurved Make(const Config::Settings &a_settings, const MenuLayout &a_layout, ImVec2 a_display)
		{
			constexpr float kViewDistance = 1.2f;
			if (a_settings.curve == 0)
			{
				return {};
			}
			const float halfAngle = static_cast<float>(a_settings.curve) * Core::kPi / 360.0f;
			const float bendRadius = (a_layout.ringRadius + a_layout.slotRadius) / std::abs(halfAngle);
			return {a_layout.center, bendRadius, std::cos(halfAngle), std::sin(halfAngle), a_display.y * kViewDistance};
		}

		bool Active() const { return bendRadius > 0.0f; }

		void Project(ImVec2 &a_point, bool a_bend) const
		{
			const ImVec2 local = a_point - center;
			float bentX = local.x;
			float bentZ = 0.0f;
			if (a_bend)
			{
				const float angle = local.x / bendRadius;
				bentX = bendRadius * std::sin(angle);
				bentZ = bendRadius * (std::cos(angle) - 1.0f);
			}
			const float x = bentX * turnCos + bentZ * turnSin;
			const float z = bentZ * turnCos - bentX * turnSin;
			a_point = center + ImVec2{x, local.y} * (distance / (distance + z));
		}
	};
}
