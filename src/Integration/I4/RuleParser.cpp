#include "Integration/I4/RuleParser.h"
#include "Integration/I4/ValueNames.h"
#include <charconv>
#include <nlohmann/json.hpp>

namespace Orbit::Integration::I4
{
	namespace
	{
		using Json = nlohmann::json;
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
				logger::warn("I4 RuleParser: '{}' is not a hex number", a_text);
				return std::nullopt;
			}
			return value;
		}

		Value ColorOf(std::string_view a_text)
		{
			const auto color = ParseHex(a_text);
			return color ? Value{static_cast<double>(*color)} : Value{};
		}

		double FormIDOf(const std::string &a_text)
		{
			const auto separator = a_text.find('|');
			const auto localID = separator == std::string::npos ? std::nullopt : ParseHex(std::string_view{a_text}.substr(separator + 1));
			auto *data = RE::TESDataHandler::GetSingleton();
			return localID && data ? static_cast<double>(data->LookupFormID(*localID, a_text.substr(0, separator))) : 0.0;
		}

		std::optional<double> ActorValueOf(const std::string &a_name)
		{
			const auto value = RE::ActorValueList::LookupActorValueByName(a_name.c_str());
			return value == RE::ActorValue::kNone ? std::nullopt : std::optional{static_cast<double>(std::to_underlying(value))};
		}

		double PartBit(double a_slot)
		{
			const auto slot = static_cast<int>(a_slot);
			return slot >= 30 && slot <= 61 ? static_cast<double>(1u << (slot - 30)) : 0.0;
		}

		std::optional<Value> EnumValue(std::string_view a_key, RE::FormType a_formType, const std::string &a_name)
		{
			std::optional<double> number;
			if (a_key == "weightClass")
			{
				number = ValueOf(kWeightClasses, a_name);
			}
			else if (a_key == "weaponType")
			{
				number = IndexOf(kWeaponTypes, a_name);
			}
			else if (a_key == "spellType")
			{
				number = IndexOf(kSpellTypes, a_name);
			}
			else if (a_key == "archetype")
			{
				number = IndexOf(kArchetypes, a_name);
			}
			else if (a_key == "deliveryType")
			{
				number = IndexOf(kDeliveryTypes, a_name);
			}
			else if (a_key == "castType")
			{
				number = IndexOf(kCastTypes, a_name);
			}
			else if (a_key == "actorValue" || a_key == "resistance" || a_key == "school" || a_key == "teachesSkill")
			{
				number = ActorValueOf(a_name);
			}
			else if (a_key == "subType" && !SubTypeNames(a_formType).empty())
			{
				number = IndexOf(SubTypeNames(a_formType), a_name);
			}
			else
			{
				return std::nullopt;
			}

			if (number)
			{
				return *number;
			}
			if (SameName(a_name, "Other"))
			{
				return Value{};
			}
			return a_name;
		}

		Condition TextCondition(std::string_view a_key, RE::FormType a_formType, const std::string &a_text)
		{
			if (a_key == "keywords" || a_key == "effectKeywords")
			{
				return {Type::kKeyword, a_text};
			}
			if (a_key == "formType")
			{
				return {Type::kEquals, ValueOf(kFormTypes, a_text).value_or(-1.0)};
			}
			if (a_key == "formId" || a_key == "equipSlot" || a_key == "teachesSpell")
			{
				return {Type::kEquals, FormIDOf(a_text)};
			}
			if (a_key == "iconColor")
			{
				return {Type::kEquals, ColorOf(a_text)};
			}
			if (const auto flag = ValueOf(FlagNames(a_key, a_formType), a_text))
			{
				return {Type::kAnyBit, *flag};
			}
			if (auto value = EnumValue(a_key, a_formType, a_text))
			{
				return {Type::kEquals, std::move(*value)};
			}
			return {Type::kEquals, a_text};
		}

		Condition NumberCondition(std::string_view a_key, double a_number)
		{
			if (a_key == "parts")
			{
				return {Type::kAnyBit, PartBit(a_number)};
			}
			if (a_key == "mainPart")
			{
				return {Type::kMainPart, PartBit(a_number)};
			}
			return {Type::kEquals, a_number};
		}

		void ParseSpec(const Json &a_spec, std::string_view a_key, RE::FormType a_formType, std::vector<Condition> &a_out)
		{
			if (a_spec.is_string())
			{
				a_out.push_back(TextCondition(a_key, a_formType, a_spec.get<std::string>()));
			}
			else if (a_spec.is_number())
			{
				a_out.push_back(NumberCondition(a_key, a_spec.get<double>()));
			}
			else if (a_spec.is_boolean())
			{
				a_out.push_back({Type::kEquals, a_spec.get<bool>()});
			}
			else if (a_spec.is_null())
			{
				a_out.emplace_back();
			}
			else if (a_spec.is_array())
			{
				for (const auto &item : a_spec)
				{
					ParseSpec(item, a_key, a_formType, a_out);
				}
			}
			else if (a_spec.is_object())
			{
				if (const auto anyOf = a_spec.find("anyOf"); anyOf != a_spec.end() && anyOf->is_array())
				{
					Condition any{Type::kAnyOf};
					for (const auto &item : *anyOf)
					{
						ParseSpec(item, a_key, a_formType, any.parts);
					}
					a_out.push_back(std::move(any));
				}
				const auto min = a_spec.find("min");
				const auto max = a_spec.find("max");
				const bool hasMin = min != a_spec.end() && min->is_number();
				const bool hasMax = max != a_spec.end() && max->is_number();
				if (hasMin || hasMax)
				{
					Condition range{Type::kRange};
					if (hasMin)
					{
						range.min = min->get<double>();
					}
					if (hasMax)
					{
						range.max = max->get<double>();
					}
					a_out.push_back(std::move(range));
				}
			}
		}

		std::optional<Value> AssignedValue(std::string_view a_key, RE::FormType a_formType, const Json &a_data)
		{
			if (a_data.is_boolean())
			{
				return a_data.get<bool>();
			}
			if (a_data.is_number())
			{
				return a_data.get<double>();
			}
			if (!a_data.is_string())
			{
				return std::nullopt;
			}
			const auto &text = a_data.get_ref<const std::string &>();
			if (a_key == "iconColor")
			{
				return ColorOf(text);
			}
			if (a_key == "subType" || a_key == "weightClass")
			{
				if (const auto number = IndexOf(a_key == "subType" ? SubTypeNames(a_formType) : kArmorSubTypes, text))
				{
					return *number;
				}
			}
			return text;
		}

		bool IconSourceExists(const std::string &a_source)
		{
			static std::map<std::string, bool, std::less<>> checked;
			if (const auto known = checked.find(a_source); known != checked.end())
			{
				return known->second;
			}
			RE::BSResourceNiBinaryStream stream{std::format("Interface/{}", a_source)};
			return checked.emplace(a_source, stream.good()).first->second;
		}
	}

	std::optional<Rule> ParseRule(const Json &a_rule)
	{
		const auto match = a_rule.find("match");
		const auto assign = a_rule.find("assign");
		if (match == a_rule.end() || assign == a_rule.end() || !match->is_object() || !assign->is_object())
		{
			return std::nullopt;
		}

		auto formType = RE::FormType::None;
		if (const auto name = match->find("formType"); name != match->end() && name->is_string())
		{
			if (const auto value = ValueOf(kFormTypes, name->get_ref<const std::string &>()))
			{
				formType = static_cast<RE::FormType>(static_cast<int>(*value));
			}
		}

		Rule rule;
		for (const auto &[key, spec] : match->items())
		{
			if (key.empty() || key.starts_with('$'))
			{
				continue;
			}
			std::vector<Condition> conditions;
			ParseSpec(spec, key, formType, conditions);
			if (!conditions.empty())
			{
				rule.match[key == "parts" || key == "mainPart" ? "partMask" : key] = std::move(conditions.back());
			}
		}

		for (const auto &[key, data] : assign->items())
		{
			if (key.empty() || key.starts_with('$'))
			{
				continue;
			}
			if (auto value = AssignedValue(key, formType, data))
			{
				(key.starts_with("icon") ? rule.icon : rule.info)[key] = std::move(*value);
			}
		}

		if (const auto source = rule.icon.find("iconSource"); source != rule.icon.end())
		{
			const auto *path = std::get_if<std::string>(&source->second);
			if (!path || !IconSourceExists(*path))
			{
				rule.icon.erase(source);
				rule.icon.erase("iconLabel");
			}
		}
		else if (rule.icon.contains("iconLabel"))
		{
			rule.icon["iconSource"] = Value{};
		}

		if (rule.match.empty() || (rule.info.empty() && rule.icon.empty()))
		{
			return std::nullopt;
		}
		return rule;
	}
}
