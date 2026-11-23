#include "Items/ItemIcons.h"
#include "Items/ItemKind.h"

namespace Orbit::Items::Icons
{
	namespace
	{
		using Slot = RE::BGSBipedObjectForm::BipedObjectSlot;

		constexpr RE::FormID kPotionUseSound = 0x000B6435;
		constexpr std::array kPickaxes{0x0E3C16u, 0x06A707u, 0x1019D4u};
		constexpr std::array kWoodAxes{0x02F2F4u, 0x0AE086u};
		constexpr RE::FormID kWeddingWreath = 0x08895A;
		constexpr RE::FormID kVampireLordArmor = 0x011A84;

		constexpr std::uint32_t kWeaponColor = 0xA4A5BF;
		constexpr std::uint32_t kAmmoColor = 0xA89E8C;
		constexpr std::uint32_t kArmorColor = 0xEDDA87;
		constexpr std::uint32_t kLightArmorColor = 0x756000;
		constexpr std::uint32_t kHeavyArmorColor = 0x6B7585;

		constexpr std::array kPartPrecedence{Slot::kBody, Slot::kHair, Slot::kHands, Slot::kForearms, Slot::kFeet, Slot::kCalves, Slot::kShield,
											 Slot::kAmulet, Slot::kRing, Slot::kLongHair, Slot::kEars, Slot::kHead, Slot::kCirclet, Slot::kTail};

		enum class Weight
		{
			kLight,
			kHeavy,
			kClothing,
			kJewelry
		};

		[[nodiscard]] RE::FormID BaseId(const RE::TESForm &a_form)
		{
			return a_form.GetFormID() & 0x00FFFFFF;
		}

		[[nodiscard]] std::string_view WeaponLabel(const RE::TESObjectWEAP &a_weapon)
		{
			const auto base = BaseId(a_weapon);
			if (std::ranges::find(kPickaxes, base) != kPickaxes.end())
			{
				return "weapon_pickaxe";
			}
			if (std::ranges::find(kWoodAxes, base) != kWoodAxes.end())
			{
				return "weapon_woodaxe";
			}

			switch (a_weapon.GetWeaponType())
			{
			case RE::WEAPON_TYPE::kOneHandSword:
				return "weapon_sword";
			case RE::WEAPON_TYPE::kOneHandDagger:
				return "weapon_dagger";
			case RE::WEAPON_TYPE::kOneHandAxe:
				return "weapon_waraxe";
			case RE::WEAPON_TYPE::kOneHandMace:
				return "weapon_mace";
			case RE::WEAPON_TYPE::kTwoHandSword:
				return "weapon_greatsword";
			case RE::WEAPON_TYPE::kTwoHandAxe:
				return a_weapon.HasKeywordString("WeapTypeWarhammer") ? "weapon_hammer" : "weapon_battleaxe";
			case RE::WEAPON_TYPE::kBow:
				return "weapon_bow";
			case RE::WEAPON_TYPE::kStaff:
				return "weapon_staff";
			case RE::WEAPON_TYPE::kCrossbow:
				return "weapon_crossbow";
			default:
				return "default_weapon";
			}
		}

		[[nodiscard]] Slot MainPart(const RE::TESObjectARMO &a_armor)
		{
			if (BaseId(a_armor) == kVampireLordArmor)
			{
				return Slot::kBody;
			}
			for (const auto part : kPartPrecedence)
			{
				if (a_armor.HasPartOf(part))
				{
					return part;
				}
			}
			return Slot::kNone;
		}

		[[nodiscard]] Weight WeightOf(const RE::TESObjectARMO &a_armor, Slot a_part)
		{
			if (BaseId(a_armor) == kWeddingWreath)
			{
				return Weight::kJewelry;
			}
			if (a_armor.IsLightArmor())
			{
				return Weight::kLight;
			}
			if (a_armor.IsHeavyArmor())
			{
				return Weight::kHeavy;
			}
			if (a_armor.HasKeywordString("VendorItemClothing"))
			{
				return Weight::kClothing;
			}
			if (a_armor.HasKeywordString("VendorItemJewelry"))
			{
				return Weight::kJewelry;
			}
			switch (a_part)
			{
			case Slot::kAmulet:
			case Slot::kRing:
			case Slot::kCirclet:
			case Slot::kEars:
				return Weight::kJewelry;
			default:
				return Weight::kClothing;
			}
		}

		[[nodiscard]] std::string_view JewelryLabel(Slot a_part)
		{
			switch (a_part)
			{
			case Slot::kAmulet:
				return "armor_amulet";
			case Slot::kRing:
				return "armor_ring";
			case Slot::kCirclet:
				return "armor_circlet";
			default:
				return "default_armor";
			}
		}

		[[nodiscard]] std::string_view ArmorLabel(Weight a_weight, Slot a_part)
		{
			if (a_weight == Weight::kJewelry)
			{
				return JewelryLabel(a_part);
			}
			const auto byWeight = [a_weight](std::string_view a_heavy, std::string_view a_light, std::string_view a_clothing)
			{
				if (a_weight == Weight::kHeavy)
				{
					return a_heavy;
				}
				return a_weight == Weight::kLight ? a_light : a_clothing;
			};
			switch (a_part)
			{
			case Slot::kHead:
			case Slot::kHair:
			case Slot::kLongHair:
				return byWeight("armor_head", "lightarmor_head", "clothing_head");
			case Slot::kBody:
			case Slot::kTail:
				return byWeight("armor_body", "lightarmor_body", "clothing_body");
			case Slot::kHands:
				return byWeight("armor_hands", "lightarmor_hands", "clothing_hands");
			case Slot::kForearms:
				return byWeight("armor_forearms", "lightarmor_forearms", "clothing_forearms");
			case Slot::kFeet:
				return byWeight("armor_feet", "lightarmor_feet", "clothing_feet");
			case Slot::kCalves:
				return byWeight("armor_calves", "lightarmor_calves", "clothing_calves");
			case Slot::kShield:
				return byWeight("armor_shield", "lightarmor_shield", "clothing_shield");
			case Slot::kAmulet:
			case Slot::kRing:
			case Slot::kCirclet:
				return a_weight == Weight::kClothing ? "default_armor" : JewelryLabel(a_part);
			default:
				return "default_armor";
			}
		}

		[[nodiscard]] SkyUIIcon ArmorIcon(const RE::TESObjectARMO &a_armor)
		{
			const auto part = MainPart(a_armor);
			const auto weight = WeightOf(a_armor, part);
			std::uint32_t color = kArmorColor;
			if (weight == Weight::kLight)
			{
				color = kLightArmorColor;
			}
			else if (weight == Weight::kHeavy)
			{
				color = kHeavyArmorColor;
			}
			return {ArmorLabel(weight, part), color};
		}

		[[nodiscard]] RE::ActorValue Resistance(const RE::MagicItem &a_item)
		{
			const auto *effect = CostliestEffect(a_item);
			return effect ? effect->data.resistVariable : RE::ActorValue::kNone;
		}

		[[nodiscard]] SkyUIIcon ScrollIcon(const RE::ScrollItem &a_scroll)
		{
			switch (Resistance(a_scroll))
			{
			case RE::ActorValue::kResistFire:
				return {"default_scroll", 0xC73636};
			case RE::ActorValue::kResistShock:
				return {"default_scroll", 0xFFFF00};
			case RE::ActorValue::kResistFrost:
				return {"default_scroll", 0x1FFBFF};
			default:
				return {"default_scroll", std::nullopt};
			}
		}

		[[nodiscard]] SkyUIIcon PotionIcon(const RE::AlchemyItem &a_potion)
		{
			const auto *effect = CostliestEffect(a_potion);
			switch (effect ? effect->data.primaryAV : RE::ActorValue::kNone)
			{
			case RE::ActorValue::kHealth:
			case RE::ActorValue::kHealRate:
			case RE::ActorValue::kHealRateMult:
				return {"potion_health", 0xDB2E73};
			case RE::ActorValue::kMagicka:
			case RE::ActorValue::kMagickaRate:
			case RE::ActorValue::kMagickaRateMult:
				return {"potion_magic", 0x2E9FDB};
			case RE::ActorValue::kStamina:
			case RE::ActorValue::kStaminaRate:
			case RE::ActorValue::kStaminaRateMult:
				return {"potion_stam", 0x51DB2E};
			case RE::ActorValue::kResistFire:
				return {"potion_fire", 0xC73636};
			case RE::ActorValue::kResistShock:
				return {"potion_shock", 0xEAAB00};
			case RE::ActorValue::kResistFrost:
				return {"potion_frost", 0x1FFBFF};
			default:
				return {"default_potion", std::nullopt};
			}
		}

		[[nodiscard]] SkyUIIcon DestructionIcon(const RE::SpellItem &a_spell)
		{
			switch (Resistance(a_spell))
			{
			case RE::ActorValue::kResistFire:
				return {"magic_fire", 0xC73636};
			case RE::ActorValue::kResistShock:
				return {"magic_shock", 0xEAAB00};
			case RE::ActorValue::kResistFrost:
				return {"magic_frost", 0x1FFBFF};
			default:
				return {"default_destruction", std::nullopt};
			}
		}

		[[nodiscard]] SkyUIIcon SpellIcon(const RE::SpellItem &a_spell)
		{
			const auto *effect = CostliestEffect(a_spell);
			switch (effect ? effect->GetMagickSkill() : RE::ActorValue::kNone)
			{
			case RE::ActorValue::kAlteration:
				return {"default_alteration", std::nullopt};
			case RE::ActorValue::kConjuration:
				return {"default_conjuration", std::nullopt};
			case RE::ActorValue::kDestruction:
				return DestructionIcon(a_spell);
			case RE::ActorValue::kIllusion:
				return {"default_illusion", std::nullopt};
			case RE::ActorValue::kRestoration:
				return {"default_restoration", std::nullopt};
			default:
				return {"default_power", std::nullopt};
			}
		}
	}

	SkyUIIcon SkyUIIconFor(const RE::TESForm &a_form)
	{
		switch (a_form.GetFormType())
		{
		case RE::FormType::Weapon:
			return {WeaponLabel(*a_form.As<RE::TESObjectWEAP>()), kWeaponColor};
		case RE::FormType::Armor:
			return ArmorIcon(*a_form.As<RE::TESObjectARMO>());
		case RE::FormType::Light:
			return {"misc_torch", std::nullopt};
		case RE::FormType::Ammo:
			return {a_form.As<RE::TESAmmo>()->GetRuntimeData().data.flags.none(RE::AMMO_DATA::Flag::kNonBolt) ? "weapon_bolt" : "weapon_arrow", kAmmoColor};
		case RE::FormType::Scroll:
			return ScrollIcon(*a_form.As<RE::ScrollItem>());
		case RE::FormType::Spell:
			return SpellIcon(*a_form.As<RE::SpellItem>());
		case RE::FormType::Shout:
			return {"default_shout", std::nullopt};
		case RE::FormType::AlchemyItem:
		{
			const auto *potion = a_form.As<RE::AlchemyItem>();
			if (!potion->IsFood())
			{
				return PotionIcon(*potion);
			}
			const auto *sound = potion->data.consumptionSound;
			return {sound && sound->GetFormID() == kPotionUseSound ? "food_wine" : "default_food", std::nullopt};
		}
		case RE::FormType::Ingredient:
			return {"default_ingredient", std::nullopt};
		default:
			return {};
		}
	}
}
