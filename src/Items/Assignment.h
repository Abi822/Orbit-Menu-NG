#pragma once

#include "Items/SlotStore.h"

namespace RMS::Items
{
	class Assignment
	{
	public:
		static void OnHighlightChanged(std::string_view a_menuName);

		static bool AssignHighlighted(int a_slot);

	private:
		static inline std::mutex _lock;
		static inline std::optional<AssignedItem> _highlighted;
	};
}
