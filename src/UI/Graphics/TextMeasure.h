#pragma once

#include <imgui.h>

namespace Orbit::UI::Graphics
{
	class TextMeasure
	{
	public:
		explicit TextMeasure(ImFont &a_font) : _font(&a_font) {}
		float Width(float a_size, std::string_view a_text) const;
		std::vector<std::string> Wrap(float a_size, std::string_view a_text, float a_maxWidth, std::size_t a_maxLines) const;
		void Shorten(float a_size, std::string &a_line, float a_maxWidth) const;

	private:
		ImFont *_font;
	};
}
