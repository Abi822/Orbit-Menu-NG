#pragma once

struct ImFont;
struct ImFontAtlas;

namespace Orbit::UI::Graphics
{
	ImFont *LoadFont(ImFontAtlas &a_atlas, std::string_view a_fontPath);
}
