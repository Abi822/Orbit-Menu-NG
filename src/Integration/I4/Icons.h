#pragma once

#include "Integration/I4/Rules.h"

namespace Orbit::Integration::I4
{
	class Icons
	{
	public:
		struct Look
		{
			std::string iconSource;
			std::string iconLabel;
			std::optional<std::uint32_t> iconColor;
		};

		static Icons &Get();
		static bool IsInstalled();
		void Load();
		Look Evaluate(const RE::TESForm &a_form, bool a_equipped, Look a_look) const;

	private:
		Icons() = default;
		std::vector<Rule> _rules;
	};
}
