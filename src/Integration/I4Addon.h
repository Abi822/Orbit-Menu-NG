#pragma once

#include <nlohmann/json_fwd.hpp>

namespace Orbit::Integration
{
	class I4Addon
	{
	public:
		struct Look
		{
			std::string iconSource;
			std::string iconLabel;
			std::optional<std::uint32_t> iconColor;
		};

		struct Condition
		{
			enum class Type
			{
				kFormID,
				kKeyword,
				kWeaponType,
				kAllOf,
				kAnyOf
			};

			Type type = Type::kAllOf;
			RE::FormID formID = 0;
			std::string keyword;
			RE::WEAPON_TYPE weaponType{};
			std::vector<Condition> parts;

			bool Matches(const RE::TESForm &a_form) const;
		};

		static I4Addon &Get();

		static bool IsInstalled();

		void Load();

		Look Evaluate(const RE::TESForm &a_form) const;

	private:
		struct IconMapping
		{
			std::optional<RE::FormType> formType;
			std::vector<Condition> conditions;
			std::optional<std::string> iconSource;
			std::optional<std::string> iconLabel;
			std::optional<std::uint32_t> iconColor;
		};

		I4Addon() = default;

		static std::optional<IconMapping> ParseMapping(const nlohmann::json &a_rule);

		std::vector<IconMapping> _mappings;
	};
}
