#pragma once

namespace RMS::Config
{
	void LoadTranslation(std::string_view a_path);

	[[nodiscard]] std::string_view Translate(std::string_view a_english);

	[[nodiscard]] const char *Translate(const char *a_english);

	template <class... Args>
	[[nodiscard]] std::string Translate(std::string_view a_english, const Args &...a_values)
	{
		try
		{
			return std::vformat(Translate(a_english), std::make_format_args(a_values...));
		}
		catch (const std::format_error &)
		{
			return std::vformat(a_english, std::make_format_args(a_values...));
		}
	}
}
