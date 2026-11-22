#include "Core/RadialState.h"

namespace RMS::Core
{
	void RadialState::Open() noexcept
	{
		_open = true;
		_chosen = false;
		_pointer = {};
		_hovered = -1;
		++_openCount;
	}

	void RadialState::AddPointerDelta(ImVec2 a_delta, const SelectionTuning &a_tuning) noexcept
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

	void RadialState::SetPointer(ImVec2 a_position, const SelectionTuning &a_tuning) noexcept
	{
		if (!_open)
		{
			return;
		}
		_pointer = a_position;
		UpdateHover(a_tuning);
	}

	void RadialState::Select(int a_slot) noexcept
	{
		if (!_open || !IsValidSlot(a_slot))
		{
			return;
		}
		_hovered = a_slot;
		_pointer = {};
	}

	void RadialState::UpdateHover(const SelectionTuning &a_tuning) noexcept
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

	std::optional<int> RadialState::Choose() noexcept
	{
		if (!_open || _hovered < 0)
		{
			return std::nullopt;
		}
		_chosen = true;
		return _hovered;
	}

	std::optional<int> RadialState::Close(bool a_commit) noexcept
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
