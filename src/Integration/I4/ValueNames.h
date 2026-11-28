#pragma once

#include <span>

namespace Orbit::Integration::I4
{
	struct Name
	{
		std::string_view name;
		std::uint32_t value;
	};

	inline constexpr std::array kFormTypes{
		Name{"Ammo", std::to_underlying(RE::FormType::Ammo)},
		Name{"Armor", std::to_underlying(RE::FormType::Armor)},
		Name{"Book", std::to_underlying(RE::FormType::Book)},
		Name{"Enchantment", std::to_underlying(RE::FormType::Enchantment)},
		Name{"Ingredient", std::to_underlying(RE::FormType::Ingredient)},
		Name{"Key", std::to_underlying(RE::FormType::KeyMaster)},
		Name{"Light", std::to_underlying(RE::FormType::Light)},
		Name{"MiscItem", std::to_underlying(RE::FormType::Misc)},
		Name{"Potion", std::to_underlying(RE::FormType::AlchemyItem)},
		Name{"Scroll", std::to_underlying(RE::FormType::Scroll)},
		Name{"Spell", std::to_underlying(RE::FormType::Spell)},
		Name{"SoulGem", std::to_underlying(RE::FormType::SoulGem)},
		Name{"Weapon", std::to_underlying(RE::FormType::Weapon)},
	};

	inline constexpr std::array kWeightClasses{Name{"Light", 0}, Name{"Heavy", 1}, Name{"Clothing", 3}, Name{"Jewelry", 4}};

	inline constexpr std::array kEffectFlags{
		Name{"Hostile", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kHostile)},
		Name{"Recover", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kRecover)},
		Name{"Detrimental", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kDetrimental)},
		Name{"SnapToNavMesh", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kSnapToNavMesh)},
		Name{"NoHitEvent", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoHitEvent)},
		Name{"DispelWithKeywords", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kDispelWithKeywords)},
		Name{"NoDuration", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoDuration)},
		Name{"NoArea", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoArea)},
		Name{"FXPersist", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kFXPersist)},
		Name{"GoryVisuals", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kGoryVisuals)},
		Name{"HideInUI", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kHideInUI)},
		Name{"NoRecast", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoRecast)},
		Name{"PowerAffectsMagnitude", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsMagnitude)},
		Name{"PowerAffectsDuration", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsDuration)},
		Name{"Painless", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kPainless)},
		Name{"NoHitEffect", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoHitEffect)},
		Name{"NoDeathDispel", std::to_underlying(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel)},
	};

	inline constexpr std::array kPotionFlags{
		Name{"ManualCalc", std::to_underlying(RE::AlchemyItem::AlchemyFlag::kCostOverride)},
		Name{"Food", std::to_underlying(RE::AlchemyItem::AlchemyFlag::kFoodItem)},
		Name{"Medicine", std::to_underlying(RE::AlchemyItem::AlchemyFlag::kMedicine)},
		Name{"Poison", std::to_underlying(RE::AlchemyItem::AlchemyFlag::kPoison)},
	};

	inline constexpr std::array kAmmoFlags{
		Name{"IgnoresNormalWeaponResistance", std::to_underlying(RE::AMMO_DATA::Flag::kIgnoresNormalWeaponResistance)},
		Name{"NonPlayable", std::to_underlying(RE::AMMO_DATA::Flag::kNonPlayable)},
		Name{"NonBolt", std::to_underlying(RE::AMMO_DATA::Flag::kNonBolt)},
	};

	inline constexpr auto kWeaponTypes = std::to_array<std::string_view>({"HandToHandMelee", "OneHandSword", "OneHandDagger", "OneHandAxe", "OneHandMace",
																		  "TwoHandSword", "TwoHandAxe", "Bow", "Staff", "Crossbow"});

	inline constexpr auto kSpellTypes = std::to_array<std::string_view>({"Spell", "Disease", "Power", "LesserPower", "Ability", "Poison", "Enchantment",
																		 "Potion", "WortCraft", "LeveledSpell", "Addiction", "VoicePower", "StaffEnchantment", "Scroll"});

	inline constexpr auto kDeliveryTypes = std::to_array<std::string_view>({"Self", "Touch", "Aimed", "TargetActor", "TargetLocation"});

	inline constexpr auto kCastTypes = std::to_array<std::string_view>({"ConstantEffect", "FireAndForget", "Concentration", "Scroll"});

	inline constexpr auto kArchetypes = std::to_array<std::string_view>({"ValueModifier", "Script", "Dispel", "CureDisease", "Absorb", "DualValueModifier",
																		 "Calm", "Demoralize", "Frenzy", "Disarm", "CommandSummoned", "Invisibility", "Light",
																		 "Darkness", "NightEye", "Lock", "Open", "BoundWeapon", "SummonCreature", "DetectLife",
																		 "Telekinesis", "Paralysis", "Reanimate", "SoulTrap", "TurnUndead", "Guide",
																		 "WerewolfFeed", "CureParalysis", "CureAddiction", "CurePoison", "Concussion",
																		 "ValueAndParts", "AccumulateMagnitude", "Stagger", "PeakValueModifier", "Cloak",
																		 "Werewolf", "SlowTime", "Rally", "EnhanceWeapon", "SpawnHazard", "Etherealize", "Banish",
																		 "SpawnScriptedRef", "Disguise", "GrabActor", "VampireLord"});

	inline constexpr auto kArmorSubTypes = std::to_array<std::string_view>({"Head", "Hair", "LongHair", "Body", "Forearms", "Hands", "Shield", "Calves",
																			"Feet", "Circlet", "Amulet", "Ears", "Ring", "Tail"});

	inline constexpr auto kWeaponSubTypes = std::to_array<std::string_view>({"Melee", "Sword", "Dagger", "WarAxe", "Mace", "Greatsword", "Battleaxe",
																			 "Warhammer", "Bow", "Crossbow", "Staff", "Pickaxe", "WoodAxe"});

	inline constexpr auto kAmmoSubTypes = std::to_array<std::string_view>({"Arrow", "Bolt"});

	inline constexpr auto kPotionSubTypes = std::to_array<std::string_view>({"Health", "HealRate", "HealRateMult", "Magicka", "MagickaRate",
																			 "MagickaRateMult", "Stamina", "StaminaRate", "StaminaRateMult", "FireResist",
																			 "ElectricResist", "FrostResist", "Potion", "Drink", "Food", "Poison"});

	bool SameName(std::string_view a_lhs, std::string_view a_rhs);

	std::optional<double> IndexOf(std::span<const std::string_view> a_names, std::string_view a_name);

	std::optional<double> ValueOf(std::span<const Name> a_names, std::string_view a_name);

	std::span<const std::string_view> SubTypeNames(RE::FormType a_formType);

	std::span<const Name> FlagNames(std::string_view a_key, RE::FormType a_formType);
}
