#include "UI/Graphics/TextMeasure.h"
#include <cfloat>

namespace Orbit::UI::Graphics
{
	namespace
	{
		constexpr auto kEllipsis = "...";

		void PopCharacter(std::string &a_text)
		{
			while (!a_text.empty() && (static_cast<unsigned char>(a_text.back()) & 0xC0) == 0x80)
			{
				a_text.pop_back();
			}
			if (!a_text.empty())
			{
				a_text.pop_back();
			}
		}
	}

	float TextMeasure::Width(float a_size, std::string_view a_text) const
	{
		if (a_text.empty())
		{
			return 0.0f;
		}
		return _font->CalcTextSizeA(a_size, FLT_MAX, 0.0f, a_text.data(), a_text.data() + a_text.size()).x;
	}

	std::vector<std::string> TextMeasure::Wrap(float a_size, std::string_view a_text, float a_maxWidth, std::size_t a_maxLines) const
	{
		std::vector<std::string> lines;
		std::string line;
		std::size_t start = 0;
		while (start < a_text.size())
		{
			std::size_t end = a_text.find(' ', start);
			if (end == std::string_view::npos)
			{
				end = a_text.size();
			}
			const std::string_view word = a_text.substr(start, end - start);
			std::string candidate = line.empty() ? std::string{word} : line + ' ' + std::string{word};
			if (!line.empty() && Width(a_size, candidate) > a_maxWidth && lines.size() + 1 < a_maxLines)
			{
				lines.push_back(std::move(line));
				candidate = std::string{word};
			}
			line = std::move(candidate);
			start = end + 1;
		}
		if (!line.empty())
		{
			lines.push_back(std::move(line));
		}
		if (!lines.empty() && Width(a_size, lines.back()) > a_maxWidth)
		{
			Shorten(a_size, lines.back(), a_maxWidth);
		}
		return lines;
	}

	void TextMeasure::Shorten(float a_size, std::string &a_line, float a_maxWidth) const
	{
		while (!a_line.empty() && Width(a_size, a_line + kEllipsis) > a_maxWidth)
		{
			PopCharacter(a_line);
		}
		while (!a_line.empty() && a_line.back() == ' ')
		{
			a_line.pop_back();
		}
		a_line += kEllipsis;
	}
}
