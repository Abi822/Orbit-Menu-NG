#pragma once

namespace Orbit::Items::Icons
{
	struct SkyUIIcon
	{
		std::string_view label;
		std::optional<std::uint32_t> color;
	};

	SkyUIIcon SkyUIIconFor(const RE::TESForm &a_form);
}
