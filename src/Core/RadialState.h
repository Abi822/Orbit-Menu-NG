#pragma once

#include "Core/RadialMath.h"
#include <cstdint>
#include <limits>
#include <optional>

namespace RMS::Core
{
	struct SelectionTuning
	{
		float deadzone = 0.0f;
		float pointerRadius = std::numeric_limits<float>::max();
		float hysteresis = 0.07f;
	};

	class RadialState
	{
	public:
		void Open() noexcept;

		void AddPointerDelta(ImVec2 a_delta, const SelectionTuning &a_tuning) noexcept;

		void SetPointer(ImVec2 a_position, const SelectionTuning &a_tuning) noexcept;

		void Select(int a_slot) noexcept;

		[[nodiscard]] std::optional<int> Choose() noexcept;

		[[nodiscard]] std::optional<int> Close(bool a_commit) noexcept;

		[[nodiscard]] bool IsOpen() const noexcept { return _open; }
		[[nodiscard]] int HoveredSlot() const noexcept { return _hovered; }
		[[nodiscard]] std::uint32_t OpenCount() const noexcept { return _openCount; }

	private:
		void UpdateHover(const SelectionTuning &a_tuning) noexcept;

		bool _open = false;
		bool _chosen = false;
		int _hovered = -1;
		ImVec2 _pointer{};
		std::uint32_t _openCount = 0;
	};
}
