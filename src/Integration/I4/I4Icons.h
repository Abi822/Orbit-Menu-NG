#pragma once

#include "Integration/I4/Rules.h"

namespace Orbit::Integration
{
	class I4Icons
	{
	public:
		struct Look
		{
			std::string iconSource;
			std::string iconLabel;
			std::optional<std::uint32_t> iconColor;
		};

		static I4Icons &Get();
		static bool IsInstalled();
		void Load();
		Look Evaluate(const RE::TESForm &a_form, bool a_equipped, Look a_look) const;

	private:
		I4Icons() = default;
		std::vector<I4::Rule> _rules;
	};
}
