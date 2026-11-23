#include "Items/SlotStore.h"
#include "Items/ItemKind.h"
#include "Items/Pages.h"

namespace Orbit::Items
{
	namespace
	{
		constexpr std::uint32_t kCoSaveID = 'ORBM';
		constexpr std::uint32_t kSlotsRecord = 'SLTS';
		constexpr std::size_t kMaxNameLength = 512;

		void SaveFirstPage(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots)
		{
			// If no slots assigned, nothing goes in the co-save (.skse).
			if (std::ranges::all_of(a_slots, &AssignedItem::IsEmpty))
			{
				return;
			}
			if (!a_intfc.OpenRecord(kSlotsRecord, SlotStore::kSlotsVersion))
			{
				logger::error("Slot Store: SKSE co-save could not open the slot record!");
				return;
			}
			if (!SlotStore::WriteSlots(a_intfc, a_slots))
			{
				logger::error("Slot Store: SKSE co-save writing the slots failed and will load with empty slots!");
			}
		}

		void OnSave(SKSE::SerializationInterface *a_intfc)
		{
			auto &pages = Pages::Get();
			SaveFirstPage(*a_intfc, pages.FirstPage());
			pages.Save(*a_intfc);
		}

		void OnLoad(SKSE::SerializationInterface *a_intfc)
		{
			SlotArray slots{};
			std::uint32_t type = 0;
			std::uint32_t version = 0;
			std::uint32_t length = 0;
			while (a_intfc->GetNextRecordInfo(type, version, length))
			{
				if (type == kSlotsRecord && version >= 1 && version <= SlotStore::kSlotsVersion)
				{
					if (!SlotStore::ReadSlots(*a_intfc, slots, version))
					{
						logger::error("Slot Store: SKSE co-save slot record is damaged the slots are left empty!");
						slots = {};
					}
				}
				else if (type == Pages::kRecord && version >= 1 && version <= Pages::kVersion)
				{
					Pages::Get().Load(*a_intfc, version);
				}
				else
				{
					logger::warn("Slot Store: SKSE co-save skipped unknown record {:08X} version {}", type, version);
				}
			}

			SlotStore::Get().ReplaceAll(slots);
			logger::info("Slot Store: SKSE co-save {} slot(s) loaded", std::ranges::count_if(slots, [](const AssignedItem &a_slot)
																							 { return !a_slot.IsEmpty(); }));
		}

		void OnRevert(SKSE::SerializationInterface *)
		{
			SlotStore::Get().ReplaceAll({});
			Pages::Get().Revert();
		}
	}

	SlotStore &SlotStore::Get()
	{
		static SlotStore singleton;
		return singleton;
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

	void SlotStore::RegisterSerialization()
	{
		const auto *serialization = SKSE::GetSerializationInterface();
		if (!serialization)
		{
			logger::error("SKSE serialization unavailable the slots will not be saved!");
			return;
		}
		serialization->SetUniqueID(kCoSaveID);
		serialization->SetSaveCallback(OnSave);
		serialization->SetLoadCallback(OnLoad);
		serialization->SetRevertCallback(OnRevert);
	}

	bool SlotStore::WriteSlots(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots)
	{
		bool written = a_intfc.WriteRecordData(static_cast<std::uint32_t>(a_slots.size()));
		for (const auto &slot : a_slots)
		{
			const auto length = static_cast<std::uint16_t>(std::min(slot.name.size(), kMaxNameLength));
			written = written && a_intfc.WriteRecordData(slot.formID) && a_intfc.WriteRecordData(length) &&
					  (length == 0 || a_intfc.WriteRecordData(slot.name.data(), length)) && a_intfc.WriteRecordData(slot.key);
		}
		return written;
	}

	bool SlotStore::ReadSlots(SKSE::SerializationInterface &a_intfc, SlotArray &a_slots, std::uint32_t a_version)
	{
		std::uint32_t count = 0;
		if (a_intfc.ReadRecordData(count) != sizeof(count))
		{
			return false;
		}

		for (std::uint32_t i = 0; i < count; ++i)
		{
			RE::FormID savedID = 0;
			std::uint16_t length = 0;
			if (a_intfc.ReadRecordData(savedID) != sizeof(savedID) || a_intfc.ReadRecordData(length) != sizeof(length) ||
				length > kMaxNameLength)
			{
				return false;
			}
			std::string name(length, '\0');
			if (length > 0 && a_intfc.ReadRecordData(name.data(), length) != length)
			{
				return false;
			}

			std::uint32_t key = 0;
			if (a_version >= 2 && a_intfc.ReadRecordData(key) != sizeof(key))
			{
				return false;
			}

			if (i >= a_slots.size())
			{
				continue;
			}
			if (savedID == 0)
			{
				if (key != 0)
				{
					a_slots[i] = AssignedItem{0, std::move(name), key};
				}
				continue;
			}

			RE::FormID formID = 0;
			const auto *form = a_intfc.ResolveFormID(savedID, formID) ? RE::TESForm::LookupByID(formID) : nullptr;
			if (GetItemKind(form) == ItemKind::kNone)
			{
				logger::warn("SKSE co-save: slot {} item {:08X} ({}) no longer exists the slot cleared!", i + 1, savedID, name);
				continue;
			}
			a_slots[i] = AssignedItem{formID, std::move(name)};
		}
		return true;
	}
}
