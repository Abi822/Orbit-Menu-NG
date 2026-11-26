#include "Integration/I4/Rules.h"
#include "Items/ItemIcons.h"

namespace Orbit::Integration::I4
{
	bool Condition::Matches(const Value &a_itemValue) const
	{
		const auto *number = std::get_if<double>(&a_itemValue);
		switch (type)
		{
		case Type::kRange:
			return number && *number >= min && *number <= max;
		case Type::kKeyword:
		{
			const auto *keywords = std::get_if<Keywords>(&a_itemValue);
			const auto *keyword = std::get_if<std::string>(&value);
			return keywords && keyword && std::ranges::find(*keywords, *keyword) != keywords->end();
		}
		case Type::kAnyBit:
			return number && (static_cast<std::uint32_t>(*number) & static_cast<std::uint32_t>(std::get<double>(value))) != 0;
		case Type::kMainPart:
		{
			const auto part = static_cast<std::uint32_t>(std::get<double>(value));
			return number && part != 0 && Items::Icons::MainPartOf(static_cast<std::uint32_t>(*number)) == part;
		}
		case Type::kAnyOf:
			return std::ranges::any_of(parts, [&a_itemValue](const Condition &a_part)
									   { return a_part.Matches(a_itemValue); });
		default:
			return a_itemValue == value;
		}
	}

	bool Rule::Matches(const Values &a_itemData) const
	{
		const Value missing;
		return std::ranges::all_of(match, [&](const auto &a_entry)
								   {
			const auto itemValue = a_itemData.find(a_entry.first);
			return a_entry.second.Matches(itemValue == a_itemData.end() ? missing : itemValue->second); });
	}
}
