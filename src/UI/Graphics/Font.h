#pragma once

struct ImFont;
struct ImFontAtlas;

namespace RMS::UI::Graphics
{
	[[nodiscard]] ImFont *LoadFont(ImFontAtlas &a_atlas, const std::filesystem::path &a_skinFont);
}
