#include "Integration/I4Addon.h"
#include <charconv>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Orbit::Integration
{
	namespace
	{
		using Json = nlohmann::json;
		using Test = std::function<bool(const RE::TESForm &)>;
		using TestFactory = std::optional<Test> (*)(const std::string &);

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

		std::optional<Test> FormIDTest(const std::string &a_value)
		{
			const auto separator = a_value.find('|');
			if (separator == std::string::npos)
			{
				return std::nullopt;
			}
			const auto localID = ParseHex(std::string_view{a_value}.substr(separator + 1));
			auto *data = RE::TESDataHandler::GetSingleton();
			if (!localID || !data)
			{
				return std::nullopt;
			}
			const RE::FormID formID = data->LookupFormID(*localID, a_value.substr(0, separator));
			return Test{[formID](const RE::TESForm &a_form)
						{ return formID != 0 && a_form.GetFormID() == formID; }};
		}

		std::optional<Test> KeywordTest(const std::string &a_value)
		{
			return Test{[a_value](const RE::TESForm &a_form)
						{
							const auto *keywords = a_form.As<RE::BGSKeywordForm>();
							return keywords && keywords->HasKeywordString(a_value);
						}};
		}

		std::optional<Test> WeaponTypeTest(const std::string &a_value)
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
				if (name == a_value)
				{
					return Test{[type](const RE::TESForm &a_form)
								{
									const auto *weapon = a_form.As<RE::TESObjectWEAP>();
									return weapon && weapon->GetWeaponType() == type;
								}};
				}
			}
			return std::nullopt;
		}

		std::optional<Test> ParseSpec(const Json &a_spec, TestFactory a_single)
		{
			if (a_spec.is_string())
			{
				return a_single(a_spec.get<std::string>());
			}

			const bool anyOf = a_spec.is_object() && a_spec.contains("anyOf") && a_spec["anyOf"].is_array();
			if (!a_spec.is_array() && !anyOf)
			{
				return std::nullopt;
			}

			std::vector<Test> parts;
			for (const auto &item : anyOf ? a_spec["anyOf"] : a_spec)
			{
				auto part = ParseSpec(item, a_single);
				if (!part)
				{
					return std::nullopt;
				}
				parts.push_back(std::move(*part));
			}
			return Test{[parts = std::move(parts), anyOf](const RE::TESForm &a_form)
						{
							const auto check = [&a_form](const Test &a_test)
							{ return a_test(a_form); };
							return anyOf ? std::ranges::any_of(parts, check) : std::ranges::all_of(parts, check);
						}};
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
			std::optional<Test> test;
			if (key == "formId")
			{
				test = ParseSpec(spec, FormIDTest);
			}
			else if (key == "keywords")
			{
				test = ParseSpec(spec, KeywordTest);
			}
			else if (key == "weaponType")
			{
				test = ParseSpec(spec, WeaponTypeTest);
			}
			if (!test)
			{
				return std::nullopt;
			}
			mapping.tests.push_back(std::move(*test));
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

	I4Addon &I4Addon::Get() noexcept
	{
		static I4Addon singleton;
		return singleton;
	}

	bool I4Addon::IsInstalled() noexcept
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
			if (std::ranges::all_of(mapping.tests, [&a_form](const Test &a_test)
									{ return a_test(a_form); }))
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
