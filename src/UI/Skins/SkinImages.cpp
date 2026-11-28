#include "UI/Skins/SkinImages.h"
#include "UI/OrbitMenu/Layout.h"
#include "UI/Skins/SvgImage.h"

namespace Orbit::UI::Skins
{
	namespace
	{
		constexpr int kGap = 2;

		float BaseSize(Part a_part, float a_slotKeySize, float a_pointerSize, float a_slotSize, float a_itemDescriptionBackgroundSize, float a_pageIndicatorSize)
		{
			const float largestSlot = OrbitMenu::Layout::LargestSlotRadius(a_slotSize * 0.5f) * 2.0f;
			switch (a_part)
			{
			case Part::kItemDescriptionBackground:
				return a_itemDescriptionBackgroundSize;
			case Part::kSlotGlow:
				return largestSlot * OrbitMenu::Layout::kSlotGlowRatio;
			case Part::kPointer:
				return a_pointerSize;
			case Part::kSlotKeyBackground:
				return a_slotKeySize;
			case Part::kPageIndicatorActive:
			case Part::kPageIndicatorInactive:
				return a_pageIndicatorSize;
			default:
				return largestSlot;
			}
		}
	}

	void SkinImages::Update(const Skin &a_skin, float a_pixelScale, const Config::Settings &a_settings)
	{
		const float slotKeySize = 2.0f * static_cast<float>(a_settings.slotKeyTextSize);
		const auto pointerSize = static_cast<float>(a_settings.pointerSize);
		const auto slotSize = static_cast<float>(a_settings.slotSize);
		const auto itemDescriptionBackgroundSize = static_cast<float>(a_settings.itemDescriptionBackgroundSize);
		const auto pageIndicatorSize = static_cast<float>(a_settings.pageIndicatorSize);
		if (a_skin.Generation() == _generation && a_pixelScale == _pixelScale && slotKeySize == _slotKeySize && pointerSize == _pointerSize &&
			slotSize == _slotSize && itemDescriptionBackgroundSize == _itemDescriptionBackgroundSize && pageIndicatorSize == _pageIndicatorSize)
		{
			return;
		}

		_generation = a_skin.Generation();
		_pixelScale = a_pixelScale;
		_slotKeySize = slotKeySize;
		_pointerSize = pointerSize;
		_slotSize = slotSize;
		_itemDescriptionBackgroundSize = itemDescriptionBackgroundSize;
		_pageIndicatorSize = pageIndicatorSize;
		_images = {};
		std::array<std::vector<std::uint8_t>, kPartCount> pixels{};
		std::array<int, kPartCount> sizes{};
		std::array<int, kPartCount> offsets{};
		int width = kGap;
		int height = 0;

		for (std::size_t i = 0; i < kPartCount; ++i)
		{
			const auto part = static_cast<Part>(i);
			const auto &file = a_skin.PartFile(part);
			if (file.empty())
			{
				continue;
			}
			sizes[i] = static_cast<int>(std::ceil(BaseSize(part, slotKeySize, pointerSize, slotSize, itemDescriptionBackgroundSize, pageIndicatorSize) * a_pixelScale));
			pixels[i] = SvgImage{file}.Rasterize(sizes[i]);
			if (pixels[i].empty())
			{
				continue;
			}
			offsets[i] = width;
			width += sizes[i] + kGap;
			height = std::max(height, sizes[i] + kGap * 2);
		}

		if (height == 0)
		{
			_texture.Reset();
			return;
		}
		if (!_texture.Create(width, height))
		{
			return;
		}
		_texture.Clear();
		for (std::size_t i = 0; i < kPartCount; ++i)
		{
			if (!pixels[i].empty())
			{
				_texture.Upload(offsets[i], kGap, sizes[i], pixels[i]);
				_images[i] = _texture.Image(offsets[i], kGap, sizes[i]);
			}
		}
	}

	std::optional<Graphics::CachedImage> SkinImages::Find(Part a_part) const
	{
		return _images[static_cast<std::size_t>(a_part)];
	}
}
