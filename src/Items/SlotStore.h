#pragma once

#include "Core/Slots.h"

namespace Orbit::Items
{
	struct AssignedItem
	{
		RE::FormID formID = 0;
		std::string name;
		bool IsEmpty() const { return formID == 0; }
		bool operator==(const AssignedItem &) const = default;
	};

	using SlotArray = Core::PerSlot<AssignedItem>;

	class SlotStore
	{
	public:
		static SlotStore &Get();
		AssignedItem Slot(int a_slot) const;
		SlotArray Snapshot() const;
		std::optional<int> Find(const AssignedItem &a_item) const;
		std::uint32_t ChangeCount() const;
		void Assign(int a_slot, AssignedItem a_item);
		void Clear(int a_slot);
		void ReplaceAll(SlotArray a_slots);
		SlotArray Exchange(SlotArray a_slots);

	private:
		SlotStore() = default;
		mutable std::mutex _lock;
		SlotArray _slots{};
		std::uint32_t _changeCount = 0;
	};
}
