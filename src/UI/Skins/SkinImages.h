#pragma once

#include "Config/Settings.h"
#include "UI/Graphics/GpuTexture.h"
#include "UI/Skins/Skin.h"

namespace Orbit::UI::Skins
{
	class SkinImages
	{
	public:
		void Update(const Skin &a_skin, float a_pixelScale, const Config::Settings &a_settings);

		std::optional<Graphics::CachedImage> Find(Part a_part) const;

	private:
		Graphics::GpuTexture _texture;
		std::array<std::optional<Graphics::CachedImage>, kPartCount> _images{};
		std::uint32_t _generation = 0;
		float _pixelScale = 0.0f;
		float _slotKeySize = 0.0f;
		float _pointerSize = 0.0f;
		float _slotSize = 0.0f;
		float _itemDescriptionBackgroundSize = 0.0f;
		float _pageIndicatorSize = 0.0f;
	};
}
