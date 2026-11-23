#include "Items/Inventory.h"
#include "Items/ItemKind.h"

namespace Orbit::Items
{
	RE::TESForm *Inventory::HeldItem(bool a_leftHand) const
	{
		if (auto *item = _player.GetEquippedObject(a_leftHand))
		{
			return item;
		}
		if (!a_leftHand)
		{
			return nullptr;
		}
		const auto &biped = _player.GetBiped();
		auto *shield = biped ? biped->objects[RE::BIPED_OBJECTS::kShield].item : nullptr;
		return shield && shield->Is(RE::FormType::Armor) ? shield : nullptr;
	}

	bool Inventory::IsInHand(const RE::TESForm &a_item, std::string_view a_name, bool a_leftHand) const
	{
		if (HeldItem(a_leftHand) != &a_item)
		{
			return false;
		}
		if (a_item.Is(RE::FormType::Spell))
		{
			return true;
		}
		auto *entry = _player.GetEquippedEntryData(a_leftHand);
		const char *name = entry ? entry->GetDisplayName() : nullptr;
		return !name || a_name == name;
	}

	bool Inventory::IsEquipped(const RE::TESForm &a_item, std::string_view a_name) const
	{
		if (a_item.IsAmmo())
		{
			return _player.GetCurrentAmmo() == &a_item;
		}
		if (_player.GetActorRuntimeData().selectedPower == &a_item)
		{
			return true;
		}
		if (const auto *armor = a_item.As<RE::TESObjectARMO>(); armor && !armor->IsShield())
		{
			return _player.GetWornArmor(armor->GetFormID()) != nullptr;
		}
		return IsInHand(a_item, a_name, kRightHand) || IsInHand(a_item, a_name, kLeftHand);
	}

	Core::HeldIn Inventory::HandsHolding(const RE::TESForm &a_item, std::string_view a_name) const
	{
		const bool left = IsInHand(a_item, a_name, kLeftHand);
		const bool right = IsInHand(a_item, a_name, kRightHand);
		if (!left && !right)
		{
			return Core::HeldIn::kNone;
		}
		if ((left && right) || IsTwoHanded(a_item))
		{
			return Core::HeldIn::kBoth;
		}
		return left ? Core::HeldIn::kLeft : Core::HeldIn::kRight;
	}

	bool Inventory::Knows(RE::TESForm &a_magic) const
	{
		if (auto *shout = a_magic.As<RE::TESShout>())
		{
			return _player.HasShout(shout);
		}
		auto *spell = a_magic.As<RE::SpellItem>();
		if (!spell)
		{
			return false;
		}
		if (_player.HasSpell(spell))
		{
			return true;
		}
		const auto *race = _player.GetRace();
		return race && race->actorEffects && race->actorEffects->GetIndex(spell).has_value();
	}

	RE::ExtraDataList *Inventory::HeldExtraList(bool a_leftHand) const
	{
		auto *entry = _player.GetEquippedEntryData(a_leftHand);
		if (!entry || !entry->extraLists)
		{
			return nullptr;
		}
		const auto worn = a_leftHand ? RE::ExtraDataType::kWornLeft : RE::ExtraDataType::kWorn;
		for (auto *list : *entry->extraLists)
		{
			if (list && list->HasType(worn))
			{
				return list;
			}
		}
		return entry->extraLists->empty() ? nullptr : entry->extraLists->front();
	}

	RE::ExtraDataList *Inventory::WornExtraList(RE::TESBoundObject &a_object) const
	{
		const auto entry = Entry(a_object).second;
		if (!entry || !entry->extraLists)
		{
			return nullptr;
		}
		for (auto *list : *entry->extraLists)
		{
			if (list && list->HasType(RE::ExtraDataType::kWorn))
			{
				return list;
			}
		}
		return nullptr;
	}

	std::optional<CarriedCopy> Inventory::FindCarriedCopy(RE::TESBoundObject &a_object, std::string_view a_name, const CarriedCopy *a_taken) const
	{
		const auto [total, entry] = Entry(a_object);
		if (total <= 0)
		{
			return std::nullopt;
		}

		const bool inRight = HeldItem(kRightHand) == &a_object;
		const bool inLeft = HeldItem(kLeftHand) == &a_object;
		std::int32_t countInLists = 0;
		RE::ExtraDataList *firstFree = nullptr;
		RE::ExtraDataList *namedFree = nullptr;
		RE::ExtraDataList *firstHeld = nullptr;
		RE::ExtraDataList *namedHeld = nullptr;
		if (entry && entry->extraLists)
		{
			for (auto *list : *entry->extraLists)
			{
				if (!list)
				{
					continue;
				}
				countInLists += list->GetCount();
				if (a_taken && list == a_taken->extraList)
				{
					continue;
				}
				const bool held = (inRight && list->HasType(RE::ExtraDataType::kWorn)) || (inLeft && list->HasType(RE::ExtraDataType::kWornLeft));
				auto &first = held ? firstHeld : firstFree;
				auto &named = held ? namedHeld : namedFree;
				const char *listName = list->GetDisplayName(&a_object);
				if (!first)
				{
					first = list;
				}
				if (!named && listName && a_name == listName)
				{
					named = list;
				}
			}
		}

		const std::int32_t plain = total - countInLists - (a_taken && !a_taken->extraList ? 1 : 0);
		if (namedFree)
		{
			return CarriedCopy{namedFree, total};
		}
		if (plain > 0)
		{
			return CarriedCopy{nullptr, total};
		}
		for (auto *list : {firstFree, namedHeld, firstHeld})
		{
			if (list)
			{
				return CarriedCopy{list, total};
			}
		}
		return std::nullopt;
	}

	RE::TESObjectREFR::InventoryItemMap::mapped_type Inventory::Entry(RE::TESBoundObject &a_object) const
	{
		auto inventory = _player.GetInventory([&a_object](RE::TESBoundObject &a_candidate)
											  { return &a_candidate == &a_object; });
		const auto it = inventory.find(&a_object);
		if (it == inventory.end())
		{
			return {0, nullptr};
		}
		return std::move(it->second);
	}
}
