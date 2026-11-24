#pragma once

#include "Integration/I4/Rules.h"
#include <nlohmann/json_fwd.hpp>

namespace Orbit::Integration::I4
{
	std::optional<Rule> ParseRule(const nlohmann::json &a_rule);
}
