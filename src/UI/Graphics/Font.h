#pragma once

struct ImFont;
struct ImFontAtlas;

namespace RMS::UI::Graphics
{
	[[nodiscard]] ImFont *LoadFont(ImFontAtlas &a_atlas, std::string_view a_fontPath);
}
