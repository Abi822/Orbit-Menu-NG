#pragma once

#include "Core/OrbitMath.h"
#include <cstdint>
#include <limits>
#include <optional>

namespace Orbit::Core
{
	struct PointerLimits
	{
		float deadzone = 0.0f;
		float maxDistance = std::numeric_limits<float>::max();
		float stickiness = 0.07f;
	};

	class OrbitState
	{
	public:
		void Open();
		void AddPointerDelta(ImVec2 a_delta, const PointerLimits &a_limits);
		void SetPointer(ImVec2 a_position, const PointerLimits &a_limits);
		void Select(int a_slot);
		[[nodiscard]] std::optional<int> Choose();
		[[nodiscard]] std::optional<int> Close(bool a_commit);
		bool IsOpen() const { return _open; }
		int HoveredSlot() const { return _hovered; }
		std::uint32_t OpenCount() const { return _openCount; }

	private:
		void UpdateHover(const PointerLimits &a_limits);

		bool _open = false;
		bool _chosen = false;
		int _hovered = -1;
		ImVec2 _pointer{};
		std::uint32_t _openCount = 0;
	};
}
