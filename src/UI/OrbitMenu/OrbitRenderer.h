#pragma once

#include "Config/Settings.h"
#include "UI/OrbitMenu/Animation.h"
#include "UI/OrbitMenu/Content.h"
#include "UI/OrbitMenu/IconCache.h"
#include "UI/Skins/Skin.h"
#include "UI/Skins/SkinImages.h"

namespace Orbit::UI::OrbitMenu
{
	class OrbitRenderer
	{
	public:
		static OrbitRenderer &Get();
		void LoadSkin();
		void PreloadContent(ImFont &a_font);
		void UpdateImages(ImVec2 a_display);
		void Draw(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display);
		bool IsVisible() const { return _animation.IsVisible(); }
		bool IsPreviewing() const { return _animation.IsPreviewing(); }
		void SetPreview(bool a_preview) { _animation.SetPreview(a_preview); }

	private:
		OrbitRenderer() = default;
		bool SyncSettings();
		void UpdateContent(ImFont &a_font, const MenuInput &a_input);
		Animation _animation;
		Config::Settings _settings;
		std::optional<std::uint32_t> _settingsVersion;
		Skins::Skin _skin;
		Skins::SkinImages _skinImages;
		Content _content;
		IconCache _icons;
	};
}
