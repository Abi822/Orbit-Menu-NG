#include "Core/OrbitState.h"

namespace Orbit::Core
{
	void OrbitState::Open()
	{
		_open = true;
		_chosen = false;
		_pointer = {};
		_hovered = -1;
		++_openCount;
	}

	void OrbitState::AddPointerDelta(ImVec2 a_delta, const PointerLimits &a_limits)
	{
		if (!_open)
		{
			return;
		}

		_pointer += a_delta;
		if (const float length = std::hypot(_pointer.x, _pointer.y); length > a_limits.maxDistance)
		{
			_pointer *= a_limits.maxDistance / length;
		}
		UpdateHover(a_limits);
	}

	void OrbitState::SetPointer(ImVec2 a_position, const PointerLimits &a_limits)
	{
		if (!_open)
		{
			return;
		}
		_pointer = a_position;
		UpdateHover(a_limits);
	}

	void OrbitState::Select(int a_slot)
	{
		if (!_open || !IsValidSlot(a_slot))
		{
			return;
		}
		_hovered = a_slot;
		_pointer = {};
	}

	void OrbitState::UpdateHover(const PointerLimits &a_limits)
	{
		if (std::hypot(_pointer.x, _pointer.y) <= a_limits.deadzone)
		{
			return;
		}

		const float angle = AngleFromUp(_pointer);
		const int candidate = SlotFromAngle(angle);
		if (candidate == _hovered)
		{
			return;
		}

		if (_hovered >= 0)
		{
			const float fromCurrent = AngularDistance(angle, SlotCenterAngle(_hovered));
			if (fromCurrent < kSliceAngle * 0.5f + a_limits.stickiness)
			{
				return;
			}
		}

		_hovered = candidate;
	}

	std::optional<int> OrbitState::Choose()
	{
		if (!_open || _hovered < 0)
		{
			return std::nullopt;
		}
		_chosen = true;
		return _hovered;
	}

	std::optional<int> OrbitState::Close(bool a_commit)
	{
		if (!_open)
		{
			return std::nullopt;
		}
		const auto slot = a_commit && !_chosen ? Choose() : std::nullopt;
		_open = false;
		_pointer = {};
		return slot;
	}
}
