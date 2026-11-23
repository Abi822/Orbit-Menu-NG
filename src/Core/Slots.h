#pragma once

#include <array>
#include <cstddef>

namespace Orbit::Core
{
	inline constexpr int kSlotCount = 8;

	[[nodiscard]] constexpr bool IsValidSlot(int a_slot) noexcept
	{
		return a_slot >= 0 && a_slot < kSlotCount;
	}

	template <class T>
	using PerSlot = std::array<T, static_cast<std::size_t>(kSlotCount)>;
}
