#pragma once

namespace Orbit::Items
{
	enum class ItemKind
	{
		kNone,
		kWeapon,
		kShield,
		kArmor,
		kTorch,
		kAmmo,
		kScroll,
		kSpell,
		kPower,
		kShout,
		kConsumable
	};

	inline ItemKind GetItemKind(const RE::TESForm *a_form)
	{
		if (!a_form)
		{
			return ItemKind::kNone;
		}
		switch (a_form->GetFormType())
		{
		case RE::FormType::Weapon:
			return a_form->As<RE::TESObjectWEAP>()->IsBound() ? ItemKind::kNone : ItemKind::kWeapon;
		case RE::FormType::Armor:
			return a_form->As<RE::TESObjectARMO>()->IsShield() ? ItemKind::kShield : ItemKind::kArmor;
		case RE::FormType::Light:
			return a_form->As<RE::TESObjectLIGH>()->CanBeCarried() ? ItemKind::kTorch : ItemKind::kNone;
		case RE::FormType::Ammo:
			return ItemKind::kAmmo;
		case RE::FormType::Scroll:
			return ItemKind::kScroll;
		case RE::FormType::Spell:
			switch (a_form->As<RE::SpellItem>()->GetSpellType())
			{
			case RE::MagicSystem::SpellType::kSpell:
				return ItemKind::kSpell;
			case RE::MagicSystem::SpellType::kPower:
			case RE::MagicSystem::SpellType::kLesserPower:
				return ItemKind::kPower;
			default:
				return ItemKind::kNone;
			}
		case RE::FormType::Shout:
			return ItemKind::kShout;
		case RE::FormType::AlchemyItem:
			return a_form->As<RE::AlchemyItem>()->IsPoison() ? ItemKind::kNone : ItemKind::kConsumable;
		case RE::FormType::Ingredient:
			return ItemKind::kConsumable;
		default:
			return ItemKind::kNone;
		}
	}

	inline bool IsTwoHanded(const RE::TESForm &a_form)
	{
		if (const auto *weapon = a_form.As<RE::TESObjectWEAP>())
		{
			return weapon->IsTwoHandedSword() || weapon->IsTwoHandedAxe() || weapon->IsBow() || weapon->IsCrossbow();
		}
		const auto *magic = a_form.As<RE::MagicItem>();
		return magic && magic->IsTwoHanded();
	}

	inline const RE::EffectSetting *CostliestEffect(const RE::MagicItem &a_item)
	{
		const auto *effect = a_item.GetCostliestEffectItem();
		return effect ? effect->baseEffect : nullptr;
	}
}
