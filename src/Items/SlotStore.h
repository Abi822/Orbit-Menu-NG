#pragma once

#include "Core/Slots.h"

namespace Orbit::Items
{
	struct AssignedItem
	{
		RE::FormID formID = 0;
		std::string name;
		// A command slot: no form, choosing it taps this virtual key (see Input/VirtualKeys.h).
		std::uint32_t key = 0;

		bool IsEmpty() const { return formID == 0 && key == 0; }
		bool IsCommand() const { return formID == 0 && key != 0; }
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

		static void RegisterSerialization();
		[[nodiscard]] static bool WriteSlots(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots);
		// Version 1: form and name. Version 2: the command key as well.
		static constexpr std::uint32_t kSlotsVersion = 2;
		[[nodiscard]] static bool ReadSlots(SKSE::SerializationInterface &a_intfc, SlotArray &a_slots, std::uint32_t a_version);

	private:
		SlotStore() = default;

		mutable std::mutex _lock;
		SlotArray _slots{};
		std::uint32_t _changeCount = 0;
	};
}
