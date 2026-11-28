#include "Integration/I4/Icons.h"
#include "Integration/I4/ItemData.h"
#include "Integration/I4/RuleParser.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace Orbit::Integration::I4
{
	Icons &Icons::Get()
	{
		static Icons instance;
		return instance;
	}

	bool Icons::IsInstalled()
	{
		return GetModuleHandleW(L"InventoryInjector.dll") != nullptr;
	}

	void Icons::Load()
	{
		_rules.clear();
		if (!IsInstalled())
		{
			logger::info("I4 Icons: I4 is not installed.");
			return;
		}
		auto *data = RE::TESDataHandler::GetSingleton();
		if (!data)
		{
			return;
		}

		std::size_t files = 0;
		for (const auto *file : data->files)
		{
			if (!file || file->recordFlags.none(RE::TESFile::RecordFlag::kChecked))
			{
				continue;
			}
			const auto path = std::filesystem::path{"Data/SKSE/Plugins/InventoryInjector"} /
							  std::filesystem::path{file->fileName}.replace_extension(".json");
			std::ifstream stream{path};
			if (!stream)
			{
				continue;
			}
			const auto root = nlohmann::json::parse(stream, nullptr, false, true);
			if (root.is_discarded() || !root.contains("rules") || !root["rules"].is_array())
			{
				logger::warn("I4 Icons: could not read {}", path.string());
				continue;
			}
			++files;
			for (const auto &rule : root["rules"])
			{
				if (auto parsed = ParseRule(rule))
				{
					_rules.push_back(std::move(*parsed));
				}
			}
		}
		logger::info("I4 Icons: {} icon mapping(s) from {} rule file(s)", _rules.size(), files);
	}

	Icons::Look Icons::Evaluate(const RE::TESForm &a_form, bool a_equipped, Look a_look) const
	{
		auto itemData = ItemDataFor(a_form, a_equipped);
		itemData["iconLabel"] = a_look.iconLabel;
		if (a_look.iconColor)
		{
			itemData["iconColor"] = static_cast<double>(*a_look.iconColor);
		}

		for (const auto &rule : _rules)
		{
			if (rule.Matches(itemData))
			{
				for (const auto &[name, value] : rule.info)
				{
					itemData[name] = value;
				}
			}
		}
		for (const auto &rule : _rules)
		{
			if (rule.Matches(itemData))
			{
				for (const auto &[name, value] : rule.icon)
				{
					itemData[name] = value;
				}
			}
		}

		if (const auto source = itemData.find("iconSource"); source != itemData.end())
		{
			const auto *path = std::get_if<std::string>(&source->second);
			a_look.iconSource = path ? *path : std::string{};
		}
		if (const auto *label = std::get_if<std::string>(&itemData["iconLabel"]))
		{
			a_look.iconLabel = *label;
		}
		const auto *color = std::get_if<double>(&itemData["iconColor"]);
		a_look.iconColor = color ? std::optional{static_cast<std::uint32_t>(*color)} : std::nullopt;
		return a_look;
	}
}
