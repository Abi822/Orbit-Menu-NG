#include "Items/Serialization.h"
#include "Items/ItemKind.h"
#include "Items/Pages.h"
#include "Items/SlotStore.h"

namespace Orbit::Items::Serialization
{
	namespace
	{
		constexpr std::uint32_t kCoSaveID = 'ORBM';
		constexpr std::uint32_t kSlotsRecord = 'SLTS';
		constexpr std::uint32_t kSlotsVersion = 1;
		constexpr std::uint32_t kPagesRecord = 'PAGS';
		constexpr std::uint32_t kPagesVersion = 1;
		constexpr std::size_t kMaxNameLength = 512;

		[[nodiscard]] bool WriteSlots(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots)
		{
			bool written = a_intfc.WriteRecordData(static_cast<std::uint32_t>(a_slots.size()));
			for (const auto &slot : a_slots)
			{
				const auto length = static_cast<std::uint16_t>(std::min(slot.name.size(), kMaxNameLength));
				written = written && a_intfc.WriteRecordData(slot.formID) && a_intfc.WriteRecordData(length) &&
						  (length == 0 || a_intfc.WriteRecordData(slot.name.data(), length));
			}
			return written;
		}

		[[nodiscard]] bool ReadSlots(SKSE::SerializationInterface &a_intfc, SlotArray &a_slots)
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

				if (i >= a_slots.size() || savedID == 0)
				{
					continue;
				}

				RE::FormID formID = 0;
				const auto *form = a_intfc.ResolveFormID(savedID, formID) ? RE::TESForm::LookupByID(formID) : nullptr;
				if (GetItemKind(form) == ItemKind::kNone)
				{
					logger::warn("Serialization: slot {} item {:08X} ({}) no longer exists, the slot is cleared!", i + 1, savedID, name);
					continue;
				}
				a_slots[i] = AssignedItem{formID, std::move(name)};
			}
			return true;
		}

		void SaveFirstPage(SKSE::SerializationInterface &a_intfc, const SlotArray &a_slots)
		{
			// If no slots assigned, nothing goes in the co-save (.skse).
			if (std::ranges::all_of(a_slots, &AssignedItem::IsEmpty))
			{
				return;
			}
			if (!a_intfc.OpenRecord(kSlotsRecord, kSlotsVersion))
			{
				logger::error("Serialization: SKSE co-save could not open the slot record!");
				return;
			}
			if (!WriteSlots(a_intfc, a_slots))
			{
				logger::error("Serialization: SKSE co-save writing the slots failed and will load with empty slots!");
			}
		}

		void SaveExtraPages(SKSE::SerializationInterface &a_intfc, const Pages::PageArray &a_pages)
		{
			const auto settings = Config::SettingsStore::Get().Snapshot();
			if (!settings.enablePages)
			{
				return;
			}
			std::vector<SlotArray> pages(a_pages.begin() + 1, a_pages.begin() + settings.numberOfPages);
			while (!pages.empty() && std::ranges::all_of(pages.back(), &AssignedItem::IsEmpty))
			{
				pages.pop_back();
			}
			if (pages.empty())
			{
				return;
			}
			if (!a_intfc.OpenRecord(kPagesRecord, kPagesVersion))
			{
				logger::error("Serialization: SKSE co-save could not open the pages record!");
				return;
			}
			bool written = a_intfc.WriteRecordData(static_cast<std::uint32_t>(pages.size()));
			for (const auto &slots : pages)
			{
				written = written && WriteSlots(a_intfc, slots);
			}
			if (!written)
			{
				logger::error("Serialization: SKSE co-save writing pages 2 and up failed and they will load empty!");
			}
		}

		void LoadExtraPages(SKSE::SerializationInterface &a_intfc)
		{
			if (!Config::SettingsStore::Get().Snapshot().enablePages)
			{
				logger::info("Serialization: SKSE co-save Pages is off, so pages 2 and up in this save are not used");
				return;
			}
			Pages::PageArray pages{};
			std::uint32_t count = 0;
			bool read = a_intfc.ReadRecordData(count) == sizeof(count);
			for (std::uint32_t i = 0; read && i < count; ++i)
			{
				SlotArray slots{};
				read = ReadSlots(a_intfc, slots);
				if (read && i + 1 < pages.size())
				{
					pages[i + 1] = std::move(slots);
				}
			}
			if (!read)
			{
				logger::error("Serialization: SKSE co-save pages record is damaged, pages 2 and up are left empty!");
				return;
			}
			Pages::Get().Restore(std::move(pages));
			logger::info("Serialization: SKSE co-save {} more page(s) loaded", count);
		}

		void OnSave(SKSE::SerializationInterface *a_intfc)
		{
			const auto pages = Pages::Get().Snapshot();
			SaveFirstPage(*a_intfc, pages[0]);
			SaveExtraPages(*a_intfc, pages);
		}

		void OnLoad(SKSE::SerializationInterface *a_intfc)
		{
			SlotArray slots{};
			std::uint32_t type = 0;
			std::uint32_t version = 0;
			std::uint32_t length = 0;
			while (a_intfc->GetNextRecordInfo(type, version, length))
			{
				if (type == kSlotsRecord && version == kSlotsVersion)
				{
					if (!ReadSlots(*a_intfc, slots))
					{
						logger::error("Serialization: SKSE co-save slot record is damaged, the slots are left empty!");
						slots = {};
					}
				}
				else if (type == kPagesRecord && version == kPagesVersion)
				{
					LoadExtraPages(*a_intfc);
				}
				else
				{
					logger::warn("Serialization: SKSE co-save skipped unknown record {:08X} version {}", type, version);
				}
			}

			SlotStore::Get().ReplaceAll(slots);
			logger::info("Serialization: SKSE co-save {} slot(s) loaded", std::ranges::count_if(slots, [](const AssignedItem &a_slot)
																							 { return !a_slot.IsEmpty(); }));
		}

		void OnRevert(SKSE::SerializationInterface *)
		{
			SlotStore::Get().ReplaceAll({});
			Pages::Get().Revert();
		}
	}

	void Register()
	{
		const auto *serialization = SKSE::GetSerializationInterface();
		if (!serialization)
		{
			logger::error("Serialization: SKSE serialization is unavailable, the slots will not be saved!");
			return;
		}
		serialization->SetUniqueID(kCoSaveID);
		serialization->SetSaveCallback(OnSave);
		serialization->SetLoadCallback(OnLoad);
		serialization->SetRevertCallback(OnRevert);
	}
}
