#include "Integration/I4/ValueNames.h"

namespace Orbit::Integration::I4
{
	bool SameName(std::string_view a_lhs, std::string_view a_rhs)
	{
		return a_lhs.size() == a_rhs.size() && _strnicmp(a_lhs.data(), a_rhs.data(), a_lhs.size()) == 0;
	}

	std::optional<double> IndexOf(std::span<const std::string_view> a_names, std::string_view a_name)
	{
		for (std::size_t i = 0; i < a_names.size(); ++i)
		{
			if (SameName(a_names[i], a_name))
			{
				return static_cast<double>(i);
			}
		}
		return std::nullopt;
	}

	std::optional<double> ValueOf(std::span<const Name> a_names, std::string_view a_name)
	{
		for (const auto &entry : a_names)
		{
			if (SameName(entry.name, a_name))
			{
				return static_cast<double>(entry.value);
			}
		}
		return std::nullopt;
	}

	std::span<const std::string_view> SubTypeNames(RE::FormType a_formType)
	{
		switch (a_formType)
		{
		case RE::FormType::Armor:
			return kArmorSubTypes;
		case RE::FormType::Weapon:
			return kWeaponSubTypes;
		case RE::FormType::Ammo:
			return kAmmoSubTypes;
		case RE::FormType::AlchemyItem:
			return kPotionSubTypes;
		default:
			return {};
		}
	}

	std::span<const Name> FlagNames(std::string_view a_key, RE::FormType a_formType)
	{
		if (a_key == "effectFlags")
		{
			return kEffectFlags;
		}
		if (a_key == "flags" && a_formType == RE::FormType::AlchemyItem)
		{
			return kPotionFlags;
		}
		if (a_key == "flags" && a_formType == RE::FormType::Ammo)
		{
			return kAmmoFlags;
		}
		return {};
	}
}
