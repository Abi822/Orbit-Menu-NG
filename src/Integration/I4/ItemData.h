#pragma once

#include <map>
#include <variant>

namespace Orbit::Integration::I4
{
	using Keywords = std::vector<std::string_view>;
	using Value = std::variant<std::monostate, double, bool, std::string, Keywords>;
	using Values = std::map<std::string, Value, std::less<>>;
	Values ItemDataFor(const RE::TESForm &a_form, bool a_equipped);
}
