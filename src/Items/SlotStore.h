#pragma once

#include "Core/Slots.h"

namespace RMS::Items
{
	struct AssignedItem
	{
		RE::FormID formID = 0;
		std::string name;

		[[nodiscard]] bool IsEmpty() const noexcept { return formID == 0; }
		bool operator==(const AssignedItem &) const = default;
	};

	using SlotArray = Core::PerSlot<AssignedItem>;

	class SlotStore
	{
	public:
		[[nodiscard]] static SlotStore &Get() noexcept;

		[[nodiscard]] AssignedItem Slot(int a_slot) const;
		[[nodiscard]] SlotArray Snapshot() const;
		[[nodiscard]] std::optional<int> Find(const AssignedItem &a_item) const;
		[[nodiscard]] std::uint32_t ChangeCount() const;

		void Assign(int a_slot, AssignedItem a_item);
		void Clear(int a_slot);
		void ReplaceAll(SlotArray a_slots);
		SlotArray Exchange(SlotArray a_slots);

		static void RegisterSerialization();
		[[nodiscard]] static bool WriteSlots(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots);
		[[nodiscard]] static bool ReadSlots(SKSE::SerializationInterface &a_intfc, SlotArray &a_slots);

	private:
		SlotStore() = default;

		mutable std::mutex _lock;
		SlotArray _slots{};
		std::uint32_t _changeCount = 0;
	};
}
