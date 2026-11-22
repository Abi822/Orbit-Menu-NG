#pragma once

#include <nlohmann/json_fwd.hpp>

namespace RMS::Integration
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

		[[nodiscard]] static I4Addon &Get() noexcept;

		[[nodiscard]] static bool IsInstalled() noexcept;

		void Load();

		[[nodiscard]] Look Evaluate(const RE::TESForm &a_form) const;

	private:
		using Test = std::function<bool(const RE::TESForm &)>;

		struct IconMapping
		{
			std::optional<RE::FormType> formType;
			std::vector<Test> tests;
			std::optional<std::string> iconSource;
			std::optional<std::string> iconLabel;
			std::optional<std::uint32_t> iconColor;
		};

		I4Addon() = default;

		[[nodiscard]] static std::optional<IconMapping> ParseMapping(const nlohmann::json &a_rule);

		std::vector<IconMapping> _mappings;
	};
}
