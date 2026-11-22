#include "UI/Graphics/Font.h"
#include <imgui.h>

namespace RMS::UI::Graphics
{
	namespace
	{
		constexpr float kFontSize = 32.0f;

		[[nodiscard]] ImFont *AddFont(ImFontAtlas &a_atlas, const std::filesystem::path &a_path)
		{
			std::error_code ec;
			if (!std::filesystem::is_regular_file(a_path, ec))
			{
				return nullptr;
			}
			const auto name = SKSE::stl::utf16_to_utf8(a_path.native()).value_or(std::string{});
			if (ImFont *font = a_atlas.AddFontFromFileTTF(name.c_str(), kFontSize))
			{
				logger::info("Font: Orbit Menu font: {}", name);
				return font;
			}
			logger::warn("Font: Could not load font {}", name);
			return nullptr;
		}
	}

	ImFont *LoadFont(ImFontAtlas &a_atlas, const std::filesystem::path &a_skinFont)
	{
		if (!a_skinFont.empty())
		{
			if (ImFont *font = AddFont(a_atlas, a_skinFont))
			{
				return font;
			}
		}

		wchar_t windowsDir[MAX_PATH]{};
		if (const UINT length = GetWindowsDirectoryW(windowsDir, MAX_PATH); length > 0 && length < MAX_PATH)
		{
			const std::filesystem::path fonts = std::filesystem::path{windowsDir} / "Fonts";
			for (const auto *file : {L"seguisb.ttf", L"segoeui.ttf"})
			{
				if (ImFont *font = AddFont(a_atlas, fonts / file))
				{
					return font;
				}
			}
		}

		logger::warn("Font: No font found. Using ImGui built-in font.");
		return a_atlas.AddFontDefault();
	}
}
