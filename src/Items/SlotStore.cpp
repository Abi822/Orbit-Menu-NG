#include "Items/SlotStore.h"

namespace Orbit::Items
{
	SlotStore &SlotStore::Get()
	{
		static SlotStore instance;
		return instance;
	}

	AssignedItem SlotStore::Slot(int a_slot) const
	{
		std::scoped_lock lock{_lock};
		return Core::IsValidSlot(a_slot) ? _slots[static_cast<std::size_t>(a_slot)] : AssignedItem{};
	}

	SlotArray SlotStore::Snapshot() const
	{
		std::scoped_lock lock{_lock};
		return _slots;
	}

	std::optional<int> SlotStore::Find(const AssignedItem &a_item) const
	{
		if (a_item.IsEmpty())
		{
			return std::nullopt;
		}
		std::scoped_lock lock{_lock};
		const auto it = std::ranges::find(_slots, a_item);
		return it != _slots.end() ? std::optional{static_cast<int>(it - _slots.begin())} : std::nullopt;
	}

	std::uint32_t SlotStore::ChangeCount() const
	{
		std::scoped_lock lock{_lock};
		return _changeCount;
	}

	void SlotStore::Assign(int a_slot, AssignedItem a_item)
	{
		if (!Core::IsValidSlot(a_slot))
		{
			return;
		}
		std::scoped_lock lock{_lock};
		_slots[static_cast<std::size_t>(a_slot)] = std::move(a_item);
		++_changeCount;
	}

	void SlotStore::Clear(int a_slot)
	{
		Assign(a_slot, AssignedItem{});
	}

	void SlotStore::ReplaceAll(SlotArray a_slots)
	{
		Exchange(std::move(a_slots));
	}

	SlotArray SlotStore::Exchange(SlotArray a_slots)
	{
		std::scoped_lock lock{_lock};
		++_changeCount;
		return std::exchange(_slots, std::move(a_slots));
	}
}
