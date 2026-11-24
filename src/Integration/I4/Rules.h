#pragma once

#include "Integration/I4/ItemData.h"

namespace Orbit::Integration::I4
{
	struct Condition
	{
		enum class Type
		{
			kEquals,
			kRange,
			kKeyword,
			kAnyBit,
			kMainPart,
			kAnyOf
		};

		Type type = Type::kEquals;
		Value value;
		double min = -std::numeric_limits<double>::infinity();
		double max = std::numeric_limits<double>::infinity();
		std::vector<Condition> parts;

		bool Matches(const Value &a_fact) const;
	};

	struct Rule
	{
		std::map<std::string, Condition, std::less<>> match;
		Values info;
		Values icon;

		bool Matches(const Values &a_facts) const;
	};
}
