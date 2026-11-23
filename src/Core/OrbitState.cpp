#include "Core/OrbitState.h"

namespace Orbit::Core
{
	void OrbitState::Open() noexcept
	{
		_open = true;
		_chosen = false;
		_pointer = {};
		_hovered = -1;
		++_openCount;
	}

	void OrbitState::AddPointerDelta(ImVec2 a_delta, const SelectionTuning &a_tuning) noexcept
	{
		if (!_open)
		{
			return;
		}

		_pointer += a_delta;
		if (const float length = std::hypot(_pointer.x, _pointer.y); length > a_tuning.pointerRadius)
		{
			_pointer *= a_tuning.pointerRadius / length;
		}
		UpdateHover(a_tuning);
	}

	void OrbitState::SetPointer(ImVec2 a_position, const SelectionTuning &a_tuning) noexcept
	{
		if (!_open)
		{
			return;
		}
		_pointer = a_position;
		UpdateHover(a_tuning);
	}

	void OrbitState::Select(int a_slot) noexcept
	{
		if (!_open || !IsValidSlot(a_slot))
		{
			return;
		}
		_hovered = a_slot;
		_pointer = {};
	}

	void OrbitState::UpdateHover(const SelectionTuning &a_tuning) noexcept
	{
		if (std::hypot(_pointer.x, _pointer.y) <= a_tuning.deadzone)
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
			if (fromCurrent < kSliceAngle * 0.5f + a_tuning.hysteresis)
			{
				return;
			}
		}

		_hovered = candidate;
	}

	std::optional<int> OrbitState::Choose() noexcept
	{
		if (!_open || _hovered < 0)
		{
			return std::nullopt;
		}
		_chosen = true;
		return _hovered;
	}

	std::optional<int> OrbitState::Close(bool a_commit) noexcept
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
