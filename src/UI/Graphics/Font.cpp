#include "UI/Graphics/Font.h"
#include "JostMedium.h"
#include <imgui.h>

namespace RMS::UI::Graphics
{
	namespace
	{
		constexpr float kFontSize = 32.0f;

		[[nodiscard]] std::filesystem::path FontFile(std::string_view a_fontPath)
		{
			const std::filesystem::path path{std::u8string{a_fontPath.begin(), a_fontPath.end()}};
			std::filesystem::path font;
			if (path.has_root_name() || path.relative_path().empty())
			{
				return font;
			}
			std::error_code error;
			for (std::filesystem::directory_iterator it{std::filesystem::path{"Data"} / path.relative_path(), error}, end; !error && it != end; it.increment(error))
			{
				std::error_code fileError;
				const auto extension = it->path().extension();
				if (it->is_regular_file(fileError) && (_wcsicmp(extension.c_str(), L".ttf") == 0 || _wcsicmp(extension.c_str(), L".otf") == 0) &&
					(font.empty() || _wcsicmp(it->path().filename().c_str(), font.filename().c_str()) < 0))
				{
					font = it->path();
				}
			}
			return font;
		}

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

	ImFont *LoadFont(ImFontAtlas &a_atlas, std::string_view a_fontPath)
	{
		if (const auto file = FontFile(a_fontPath); !file.empty())
		{
			if (ImFont *font = AddFont(a_atlas, file))
			{
				return font;
			}
		}

		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;
		if (ImFont *font = a_atlas.AddFontFromMemoryTTF(const_cast<unsigned char *>(kJostMedium), static_cast<int>(sizeof(kJostMedium)), kFontSize, &config))
		{
			logger::info("Font: Using Default font");
			return font;
		}

		logger::warn("Font: No font found. Using ImGui built-in font.");
		return a_atlas.AddFontDefault();
	}
}
