#include "UI/Skins/SkinIconNames.h"
#include "Items/ItemKind.h"

namespace RMS::UI::Skins
{
	namespace
	{
		using Slot = RE::BGSBipedObjectForm::BipedObjectSlot;

		[[nodiscard]] std::string_view WeaponName(const RE::TESObjectWEAP &a_weapon)
		{
			switch (a_weapon.GetWeaponType())
			{
			case RE::WEAPON_TYPE::kHandToHandMelee:
				return "hand_to_hand";
			case RE::WEAPON_TYPE::kOneHandSword:
				return "sword_one_handed";
			case RE::WEAPON_TYPE::kOneHandDagger:
				return "dagger";
			case RE::WEAPON_TYPE::kOneHandAxe:
				return "axe_one_handed";
			case RE::WEAPON_TYPE::kOneHandMace:
				return "mace";
			case RE::WEAPON_TYPE::kTwoHandSword:
				return "sword_two_handed";
			case RE::WEAPON_TYPE::kTwoHandAxe:
				return a_weapon.HasKeywordString("WeapTypeWarhammer") ? "warhammer_two_handed" : "axe_two_handed";
			case RE::WEAPON_TYPE::kBow:
				return "bow";
			case RE::WEAPON_TYPE::kStaff:
				return "staff";
			case RE::WEAPON_TYPE::kCrossbow:
				return "crossbow";
			default:
				return {};
			}
		}

		[[nodiscard]] std::string_view ArmorName(const RE::TESObjectARMO &a_armor)
		{
			const auto byWeight = [&a_armor](std::string_view a_light, std::string_view a_heavy, std::string_view a_clothing)
			{
				if (a_armor.IsLightArmor())
				{
					return a_light;
				}
				return a_armor.IsHeavyArmor() ? a_heavy : a_clothing;
			};
			if (a_armor.IsShield())
			{
				return byWeight("armor_light_shield", "armor_heavy_shield", "shield");
			}
			if (a_armor.HasPartOf(Slot::kBody))
			{
				return byWeight("armor_light_chest", "armor_heavy_chest", "armor_clothing_chest");
			}
			if (a_armor.HasPartOf(Slot::kHead) || a_armor.HasPartOf(Slot::kHair) || a_armor.HasPartOf(Slot::kLongHair))
			{
				return byWeight("armor_light_head", "armor_heavy_head", "armor_clothing_head");
			}
			if (a_armor.HasPartOf(Slot::kHands) || a_armor.HasPartOf(Slot::kForearms))
			{
				return byWeight("armor_light_arm", "armor_heavy_arm", "armor_clothing_arm");
			}
			if (a_armor.HasPartOf(Slot::kFeet) || a_armor.HasPartOf(Slot::kCalves))
			{
				return byWeight("armor_light_foot", "armor_heavy_foot", "armor_clothing_foot");
			}
			if (a_armor.HasPartOf(Slot::kAmulet))
			{
				return "armor_necklace";
			}
			if (a_armor.HasPartOf(Slot::kRing))
			{
				return "armor_ring";
			}
			if (a_armor.HasPartOf(Slot::kCirclet))
			{
				return "armor_circlet";
			}
			return "armor_default";
		}

		[[nodiscard]] std::string_view SpellName(const RE::SpellItem &a_spell)
		{
			const auto type = a_spell.GetSpellType();
			if (type == RE::MagicSystem::SpellType::kPower || type == RE::MagicSystem::SpellType::kLesserPower)
			{
				return "power";
			}
			const auto *effect = Items::CostliestEffect(a_spell);
			switch (effect ? effect->GetMagickSkill() : RE::ActorValue::kNone)
			{
			case RE::ActorValue::kAlteration:
				return "alteration";
			case RE::ActorValue::kConjuration:
				return "conjuration";
			case RE::ActorValue::kIllusion:
				return "illusion";
			case RE::ActorValue::kRestoration:
				return "restoration";
			case RE::ActorValue::kDestruction:
				switch (effect->data.resistVariable)
				{
				case RE::ActorValue::kResistFire:
					return "destruction_fire";
				case RE::ActorValue::kResistFrost:
					return "destruction_frost";
				case RE::ActorValue::kResistShock:
					return "destruction_shock";
				default:
					return "destruction";
				}
			default:
				return "spell_default";
			}
		}

		[[nodiscard]] std::string_view PotionName(const RE::AlchemyItem &a_potion)
		{
			if (a_potion.IsFood())
			{
				return "food";
			}
			if (a_potion.IsPoison())
			{
				return "poison_default";
			}
			const auto *effect = Items::CostliestEffect(a_potion);
			switch (effect ? effect->data.primaryAV : RE::ActorValue::kNone)
			{
			case RE::ActorValue::kHealth:
			case RE::ActorValue::kHealRate:
			case RE::ActorValue::kHealRateMult:
				return "potion_health";
			case RE::ActorValue::kMagicka:
			case RE::ActorValue::kMagickaRate:
			case RE::ActorValue::kMagickaRateMult:
				return "potion_magicka";
			case RE::ActorValue::kStamina:
			case RE::ActorValue::kStaminaRate:
			case RE::ActorValue::kStaminaRateMult:
				return "potion_stamina";
			case RE::ActorValue::kResistFire:
				return "potion_fire_resist";
			case RE::ActorValue::kResistFrost:
				return "potion_frost_resist";
			case RE::ActorValue::kResistShock:
				return "potion_shock_resist";
			case RE::ActorValue::kResistMagic:
				return "potion_magic_resist";
			default:
				return "potion_default";
			}
		}
	}

	std::string_view TypeIconName(const RE::TESForm &a_form)
	{
		switch (a_form.GetFormType())
		{
		case RE::FormType::Weapon:
			return WeaponName(*a_form.As<RE::TESObjectWEAP>());
		case RE::FormType::Armor:
			return ArmorName(*a_form.As<RE::TESObjectARMO>());
		case RE::FormType::Ammo:
			return "arrow";
		case RE::FormType::Light:
			return "torch";
		case RE::FormType::Scroll:
			return "scroll";
		case RE::FormType::Spell:
			return SpellName(*a_form.As<RE::SpellItem>());
		case RE::FormType::Shout:
			return "shout";
		case RE::FormType::AlchemyItem:
			return PotionName(*a_form.As<RE::AlchemyItem>());
		default:
			return {};
		}
	}
}
