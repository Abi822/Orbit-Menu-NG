#include "Translation/Translation.h"
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>

namespace Orbit::Translation
{
	namespace
	{
		std::map<std::string, std::string, std::less<>> g_texts;

		void LoadFile(const std::filesystem::path &a_file)
		{
			const auto name = SKSE::stl::utf16_to_utf8(a_file.filename().native()).value_or(std::string{});
			std::ifstream stream{a_file};
			const auto root = nlohmann::json::parse(stream, nullptr, false, true);
			if (root.is_discarded() || !root.is_object())
			{
				logger::warn("Translation: Could not read {}", name);
				return;
			}
			std::size_t count = 0;
			for (const auto &[english, text] : root.items())
			{
				if (!text.is_string())
				{
					continue;
				}
				const auto &translated = text.get_ref<const std::string &>();
				if (!translated.empty() && translated != english && g_texts.emplace(english, translated).second)
				{
					++count;
				}
			}
			logger::info("Translation: {} loaded from {}", count, name);
		}
	}

	void LoadTranslation(std::string_view a_path)
	{
		const std::filesystem::path path{std::u8string{a_path.begin(), a_path.end()}};
		if (path.has_root_name() || path.relative_path().empty())
		{
			return;
		}
		std::error_code error;
		for (std::filesystem::directory_iterator it{std::filesystem::path{"Data"} / path.relative_path(), error}, end; !error && it != end; it.increment(error))
		{
			std::error_code fileError;
			if (it->is_regular_file(fileError) && _wcsicmp(it->path().extension().c_str(), L".json") == 0)
			{
				LoadFile(it->path());
			}
		}
	}

	std::string_view Translate(std::string_view a_english)
	{
		if (g_texts.empty())
		{
			return a_english;
		}
		const auto found = g_texts.find(a_english);
		return found == g_texts.end() ? a_english : std::string_view{found->second};
	}

	const char *Translate(const char *a_english)
	{
		if (g_texts.empty())
		{
			return a_english;
		}
		const auto found = g_texts.find(std::string_view{a_english});
		return found == g_texts.end() ? a_english : found->second.c_str();
	}
}
