#include "UI/OrbitMenu/OrbitRenderer.h"
#include "Input/InputManager.h"
#include "UI/OrbitMenu/Layout.h"
#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr MenuInput kPreloadInput{.open = true, .justOpened = true};

		int Pixels(float a_base, float a_pixelScale)
		{
			return static_cast<int>(std::ceil(a_base * a_pixelScale));
		}
	}

	OrbitRenderer &OrbitRenderer::Get()
	{
		static auto *renderer = new OrbitRenderer;
		return *renderer;
	}

	void OrbitRenderer::LoadSkin()
	{
		_skin.Load(Config::SettingsStore::Get().Snapshot().skinPath);
	}

	void OrbitRenderer::PreloadContent(ImFont &a_font)
	{
		UpdateContent(a_font, kPreloadInput);
	}

	void OrbitRenderer::UpdateImages(ImVec2 a_display)
	{
		const float pixelScale = Layout::ScreenScale(a_display) * _settings.orbitMenuScale;
		_skinImages.Update(_skin, pixelScale, _settings);
		const float slotIcon = static_cast<float>(_settings.slotIconSize) * (1.0f + Layout::kSlotHoverGrowth);
		_icons.Update(_content.IconRequests(), _content.IconRevision(), Pixels(slotIcon, pixelScale), Pixels(static_cast<float>(_settings.itemIconDescriptionSize), pixelScale));
	}

	void OrbitRenderer::Draw(ImDrawList &a_drawList, ImFont &a_font, ImVec2 a_display)
	{
		const auto input = _animation.Track(Input::InputManager::Get().GetSnapshot());
		UpdateContent(a_font, input);
		UpdateImages(a_display);
		const int hovered = input.previewing ? _content.FirstFilledSlot() : input.hoveredSlot;
		if (!_animation.Animate(hovered, _settings.animationSpeed))
		{
			return;
		}
		const Scene scene{_settings, input, _animation.GetMotion(), _content, _icons, _skinImages};
		Painter{a_drawList, a_font, a_display, scene}.Paint();
	}

	bool OrbitRenderer::SyncSettings()
	{
		auto &store = Config::SettingsStore::Get();
		const auto version = store.ChangeCount();
		if (version == _settingsVersion)
		{
			return false;
		}
		_settingsVersion = version;
		_settings = store.Snapshot();
		return true;
	}

	void OrbitRenderer::UpdateContent(ImFont &a_font, const MenuInput &a_input)
	{
		const bool settingsChanged = SyncSettings();
		if (settingsChanged)
		{
			_skin.Load(_settings.skinPath);
		}
		_content.Refresh(a_font, _settings, settingsChanged, _skin, a_input);
	}
}
