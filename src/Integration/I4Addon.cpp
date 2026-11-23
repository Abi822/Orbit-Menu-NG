#include "Integration/I4Addon.h"
#include <charconv>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Orbit::Integration
{
	namespace
	{
		using Json = nlohmann::json;
		using Condition = I4Addon::Condition;
		using Type = Condition::Type;

		std::optional<std::uint32_t> ParseHex(std::string_view a_text)
		{
			std::string_view digits = a_text;
			if (digits.starts_with('#'))
			{
				digits.remove_prefix(1);
			}
			else if (digits.starts_with("0x") || digits.starts_with("0X"))
			{
				digits.remove_prefix(2);
			}
			std::uint32_t value = 0;
			const auto [end, error] = std::from_chars(digits.data(), digits.data() + digits.size(), value, 16);
			if (error != std::errc{} || end != digits.data() + digits.size())
			{
				logger::warn("I4 Addon: '{}' is not a hex number", a_text);
				return std::nullopt;
			}
			return value;
		}

		std::optional<RE::FormType> ParseFormType(std::string_view a_name)
		{
			static constexpr std::array<std::pair<std::string_view, RE::FormType>, 8> kTypes{{
				{"Weapon", RE::FormType::Weapon},
				{"Armor", RE::FormType::Armor},
				{"Light", RE::FormType::Light},
				{"Ammo", RE::FormType::Ammo},
				{"Scroll", RE::FormType::Scroll},
				{"Spell", RE::FormType::Spell},
				{"Potion", RE::FormType::AlchemyItem},
				{"Ingredient", RE::FormType::Ingredient},
			}};
			for (const auto &[name, type] : kTypes)
			{
				if (name.size() == a_name.size() && _strnicmp(name.data(), a_name.data(), name.size()) == 0)
				{
					return type;
				}
			}
			return std::nullopt;
		}

		std::optional<std::uint32_t> ParseColor(const Json &a_value)
		{
			if (a_value.is_number_unsigned())
			{
				return a_value.get<std::uint32_t>();
			}
			return a_value.is_string() ? ParseHex(a_value.get_ref<const std::string &>()) : std::nullopt;
		}

		std::optional<RE::WEAPON_TYPE> ParseWeaponType(std::string_view a_name)
		{
			static constexpr std::array<std::pair<std::string_view, RE::WEAPON_TYPE>, 10> kTypes{{
				{"HandToHandMelee", RE::WEAPON_TYPE::kHandToHandMelee},
				{"OneHandSword", RE::WEAPON_TYPE::kOneHandSword},
				{"OneHandDagger", RE::WEAPON_TYPE::kOneHandDagger},
				{"OneHandAxe", RE::WEAPON_TYPE::kOneHandAxe},
				{"OneHandMace", RE::WEAPON_TYPE::kOneHandMace},
				{"TwoHandSword", RE::WEAPON_TYPE::kTwoHandSword},
				{"TwoHandAxe", RE::WEAPON_TYPE::kTwoHandAxe},
				{"Bow", RE::WEAPON_TYPE::kBow},
				{"Staff", RE::WEAPON_TYPE::kStaff},
				{"Crossbow", RE::WEAPON_TYPE::kCrossbow},
			}};
			for (const auto &[name, type] : kTypes)
			{
				if (name == a_name)
				{
					return type;
				}
			}
			return std::nullopt;
		}

		std::optional<Condition> ParseValue(Type a_type, const std::string &a_value)
		{
			Condition condition{a_type};
			switch (a_type)
			{
			case Type::kFormID:
			{
				const auto separator = a_value.find('|');
				const auto localID = separator == std::string::npos ? std::nullopt : ParseHex(std::string_view{a_value}.substr(separator + 1));
				auto *data = RE::TESDataHandler::GetSingleton();
				if (!localID || !data)
				{
					return std::nullopt;
				}
				condition.formID = data->LookupFormID(*localID, a_value.substr(0, separator));
				return condition;
			}
			case Type::kKeyword:
				condition.keyword = a_value;
				return condition;
			case Type::kWeaponType:
				if (const auto weaponType = ParseWeaponType(a_value))
				{
					condition.weaponType = *weaponType;
					return condition;
				}
				return std::nullopt;
			default:
				return std::nullopt;
			}
		}

		std::optional<Condition> ParseCondition(Type a_type, const Json &a_spec)
		{
			if (a_spec.is_string())
			{
				return ParseValue(a_type, a_spec.get<std::string>());
			}

			const bool anyOf = a_spec.is_object() && a_spec.contains("anyOf") && a_spec["anyOf"].is_array();
			if (!a_spec.is_array() && !anyOf)
			{
				return std::nullopt;
			}

			Condition group{anyOf ? Type::kAnyOf : Type::kAllOf};
			for (const auto &item : anyOf ? a_spec["anyOf"] : a_spec)
			{
				auto part = ParseCondition(a_type, item);
				if (!part)
				{
					return std::nullopt;
				}
				group.parts.push_back(std::move(*part));
			}
			return group;
		}
	}

	bool I4Addon::Condition::Matches(const RE::TESForm &a_form) const
	{
		const auto matches = [&a_form](const Condition &a_part)
		{ return a_part.Matches(a_form); };
		switch (type)
		{
		case Type::kFormID:
			return formID != 0 && a_form.GetFormID() == formID;
		case Type::kKeyword:
		{
			const auto *keywords = a_form.As<RE::BGSKeywordForm>();
			return keywords && keywords->HasKeywordString(keyword);
		}
		case Type::kWeaponType:
		{
			const auto *weapon = a_form.As<RE::TESObjectWEAP>();
			return weapon && weapon->GetWeaponType() == weaponType;
		}
		case Type::kAnyOf:
			return std::ranges::any_of(parts, matches);
		default:
			return std::ranges::all_of(parts, matches);
		}
	}

	std::optional<I4Addon::IconMapping> I4Addon::ParseMapping(const Json &a_rule)
	{
		const auto match = a_rule.find("match");
		const auto assign = a_rule.find("assign");
		if (match == a_rule.end() || assign == a_rule.end() || !match->is_object() || !assign->is_object() || match->empty())
		{
			return std::nullopt;
		}

		IconMapping mapping;
		if (const auto formType = match->find("formType"); formType != match->end())
		{
			if (!formType->is_string())
			{
				return std::nullopt;
			}
			mapping.formType = ParseFormType(formType->get_ref<const std::string &>());
			if (!mapping.formType)
			{
				return std::nullopt;
			}
		}

		for (const auto &[key, spec] : match->items())
		{
			if (key == "formType" || key.starts_with('$'))
			{
				continue;
			}
			std::optional<Condition> condition;
			if (key == "formId")
			{
				condition = ParseCondition(Type::kFormID, spec);
			}
			else if (key == "keywords")
			{
				condition = ParseCondition(Type::kKeyword, spec);
			}
			else if (key == "weaponType")
			{
				condition = ParseCondition(Type::kWeaponType, spec);
			}
			if (!condition)
			{
				return std::nullopt;
			}
			mapping.conditions.push_back(std::move(*condition));
		}

		if (const auto source = assign->find("iconSource"); source != assign->end() && source->is_string())
		{
			mapping.iconSource = source->get<std::string>();
		}
		if (const auto label = assign->find("iconLabel"); label != assign->end() && label->is_string())
		{
			mapping.iconLabel = label->get<std::string>();
		}
		if (const auto color = assign->find("iconColor"); color != assign->end())
		{
			mapping.iconColor = ParseColor(*color);
		}
		if (!mapping.iconSource && !mapping.iconLabel && !mapping.iconColor)
		{
			return std::nullopt;
		}
		return mapping;
	}

	I4Addon &I4Addon::Get()
	{
		static I4Addon singleton;
		return singleton;
	}

	bool I4Addon::IsInstalled()
	{
		return GetModuleHandleW(L"InventoryInjector.dll") != nullptr;
	}

	void I4Addon::Load()
	{
		_mappings.clear();
		if (!IsInstalled())
		{
			logger::info("I4 Addon: is not installed.");
			return;
		}
		auto *data = RE::TESDataHandler::GetSingleton();
		if (!data)
		{
			return;
		}

		std::size_t files = 0;
		for (const auto *file : data->files)
		{
			if (!file || file->recordFlags.none(RE::TESFile::RecordFlag::kChecked))
			{
				continue;
			}
			const auto path = std::filesystem::path{"Data/SKSE/Plugins/InventoryInjector"} /
							  std::filesystem::path{file->fileName}.replace_extension(".json");
			std::ifstream stream{path};
			if (!stream)
			{
				continue;
			}
			const auto root = Json::parse(stream, nullptr, false, true);
			if (root.is_discarded() || !root.contains("rules") || !root["rules"].is_array())
			{
				logger::warn("I4 Addon: could not read {}", path.string());
				continue;
			}
			++files;
			for (const auto &rule : root["rules"])
			{
				if (auto parsed = ParseMapping(rule))
				{
					_mappings.push_back(std::move(*parsed));
				}
			}
		}
		logger::info("I4 Addon: {} icon mapping(s) from {} rule file(s)", _mappings.size(), files);
	}

	I4Addon::Look I4Addon::Evaluate(const RE::TESForm &a_form) const
	{
		Look look;
		for (const auto &mapping : _mappings)
		{
			if (mapping.formType && a_form.GetFormType() != *mapping.formType)
			{
				continue;
			}
			if (std::ranges::all_of(mapping.conditions, [&a_form](const Condition &a_condition)
									{ return a_condition.Matches(a_form); }))
			{
				if (mapping.iconSource)
				{
					look.iconSource = *mapping.iconSource;
				}
				if (mapping.iconLabel)
				{
					look.iconLabel = *mapping.iconLabel;
				}
				if (mapping.iconColor)
				{
					look.iconColor = mapping.iconColor;
				}
			}
		}
		return look;
	}
}
