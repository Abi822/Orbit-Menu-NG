#include "Integration/I4/ItemData.h"
#include "Integration/I4/ValueNames.h"
#include "Items/ItemIcons.h"

namespace Orbit::Integration::I4
{
	namespace
	{
		Keywords KeywordsOf(const RE::BGSKeywordForm &a_form)
		{
			Keywords keywords;
			for (const auto *keyword : std::span(a_form.keywords, a_form.numKeywords))
			{
				const char *name = keyword ? keyword->GetFormEditorID() : nullptr;
				if (name && *name)
				{
					keywords.emplace_back(name);
				}
			}
			return keywords;
		}

		Value WeightClassOf(const RE::TESObjectARMO &a_armor)
		{
			std::string_view name;
			if (a_armor.IsLightArmor())
			{
				name = "Light";
			}
			else if (a_armor.IsHeavyArmor())
			{
				name = "Heavy";
			}
			else if (a_armor.HasKeywordString("VendorItemClothing"))
			{
				name = "Clothing";
			}
			else if (a_armor.HasKeywordString("VendorItemJewelry"))
			{
				name = "Jewelry";
			}
			const auto number = ValueOf(kWeightClasses, name);
			return number ? Value{*number} : Value{};
		}

		void AddMagicData(const RE::MagicItem &a_magic, Values &a_itemData)
		{
			a_itemData["spellName"] = std::string{a_magic.GetFullName()};

			Keywords effectKeywords;
			for (const auto *effect : a_magic.effects)
			{
				if (const auto *base = effect ? effect->baseEffect : nullptr)
				{
					const auto keywords = KeywordsOf(*base);
					effectKeywords.insert(effectKeywords.end(), keywords.begin(), keywords.end());
				}
			}
			a_itemData["effectKeywords"] = std::move(effectKeywords);

			if (const auto *spell = a_magic.As<RE::SpellItem>())
			{
				a_itemData["spellType"] = static_cast<double>(std::to_underlying(spell->GetSpellType()));
				if (const auto *slot = spell->GetEquipSlot())
				{
					a_itemData["equipSlot"] = static_cast<double>(slot->GetFormID());
				}
			}
			if (const auto *potion = a_magic.As<RE::AlchemyItem>())
			{
				a_itemData["flags"] = static_cast<double>(potion->data.flags.underlying());
			}

			const auto *effect = a_magic.GetCostliestEffectItem();
			if (!effect || !effect->baseEffect)
			{
				return;
			}
			const auto &data = effect->baseEffect->data;
			a_itemData["magnitude"] = static_cast<double>(effect->effectItem.magnitude);
			a_itemData["duration"] = static_cast<double>(effect->effectItem.duration);
			a_itemData["area"] = static_cast<double>(effect->effectItem.area);
			a_itemData["effectName"] = std::string{effect->baseEffect->GetFullName()};
			a_itemData["effectFlags"] = static_cast<double>(data.flags.underlying());
			a_itemData["school"] = static_cast<double>(std::to_underlying(data.associatedSkill));
			a_itemData["skillLevel"] = static_cast<double>(data.minimumSkill);
			a_itemData["archetype"] = static_cast<double>(std::to_underlying(data.archetype));
			a_itemData["deliveryType"] = static_cast<double>(std::to_underlying(data.delivery));
			a_itemData["castType"] = static_cast<double>(std::to_underlying(data.castingType));
			a_itemData["actorValue"] = static_cast<double>(std::to_underlying(data.primaryAV));
			a_itemData["resistance"] = static_cast<double>(std::to_underlying(data.resistVariable));
			a_itemData.try_emplace("subType", static_cast<double>(std::to_underlying(data.associatedSkill)));
		}
	}

	Values ItemDataFor(const RE::TESForm &a_form, bool a_equipped)
	{
		Values itemData{{"formType", static_cast<double>(std::to_underlying(a_form.GetFormType()))},
					 {"formId", static_cast<double>(a_form.GetFormID())},
					 {"weight", static_cast<double>(a_form.GetWeight())},
					 {"isEquipped", a_equipped}};
		if (const auto *keywords = a_form.As<RE::BGSKeywordForm>())
		{
			itemData["keywords"] = KeywordsOf(*keywords);
		}
		if (const auto subType = IndexOf(SubTypeNames(a_form.GetFormType()), Items::Icons::SkyUISubType(a_form)))
		{
			itemData["subType"] = *subType;
		}
		if (const auto *armor = a_form.As<RE::TESObjectARMO>())
		{
			itemData["partMask"] = static_cast<double>(armor->GetSlotMask().underlying());
			itemData["weightClass"] = WeightClassOf(*armor);
			itemData["isEnchanted"] = armor->formEnchanting != nullptr;
		}
		if (const auto *weapon = a_form.As<RE::TESObjectWEAP>())
		{
			itemData["weaponType"] = static_cast<double>(std::to_underlying(weapon->GetWeaponType()));
			itemData["speed"] = static_cast<double>(weapon->weaponData.speed);
			itemData["reach"] = static_cast<double>(weapon->weaponData.reach);
			itemData["stagger"] = static_cast<double>(weapon->weaponData.staggerValue);
			itemData["minRange"] = static_cast<double>(weapon->weaponData.minRange);
			itemData["maxRange"] = static_cast<double>(weapon->weaponData.maxRange);
			itemData["critDamage"] = static_cast<double>(weapon->criticalData.damage);
			itemData["baseDamage"] = static_cast<double>(weapon->GetAttackDamage());
			itemData["isEnchanted"] = weapon->formEnchanting != nullptr;
			if (const auto *slot = weapon->GetEquipSlot())
			{
				itemData["equipSlot"] = static_cast<double>(slot->GetFormID());
			}
		}
		if (const auto *ammo = a_form.As<RE::TESAmmo>())
		{
			itemData["flags"] = static_cast<double>(ammo->GetRuntimeData().data.flags.underlying());
		}
		if (const auto *magic = a_form.As<RE::MagicItem>())
		{
			AddMagicData(*magic, itemData);
		}
		return itemData;
	}
}
