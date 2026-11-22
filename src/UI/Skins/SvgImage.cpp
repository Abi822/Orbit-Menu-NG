#include "UI/Skins/SvgImage.h"
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <fstream>
#include <iterator>

namespace RMS::UI::Skins
{
	void SvgImage::Delete::operator()(NSVGimage *a_image) const noexcept
	{
		nsvgDelete(a_image);
	}

	SvgImage::SvgImage(const std::filesystem::path &a_file)
	{
		std::ifstream stream{a_file, std::ios::binary};
		std::string text{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
		if (text.empty())
		{
			logger::warn("Svg Image: Skin {} could not be read", SKSE::stl::utf16_to_utf8(a_file.filename().native()).value_or(std::string{}));
			return;
		}
		_image.reset(nsvgParse(text.data(), "px", 96.0f));
		if (_image && (_image->width <= 0.0f || _image->height <= 0.0f))
		{
			_image.reset();
		}
		if (!_image)
		{
			logger::warn("Svg Image: Skin {} could not be read as an SVG", SKSE::stl::utf16_to_utf8(a_file.filename().native()).value_or(std::string{}));
		}
	}

	std::vector<std::uint8_t> SvgImage::Rasterize(int a_size) const
	{
		if (!_image || a_size < 1)
		{
			return {};
		}
		const std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)> rasterizer{nsvgCreateRasterizer(), nsvgDeleteRasterizer};
		if (!rasterizer)
		{
			return {};
		}

		const float size = static_cast<float>(a_size);
		const float scale = std::min(size / _image->width, size / _image->height);
		std::vector<std::uint8_t> pixels(static_cast<std::size_t>(a_size) * static_cast<std::size_t>(a_size) * 4);
		nsvgRasterize(rasterizer.get(), _image.get(), (size - _image->width * scale) * 0.5f, (size - _image->height * scale) * 0.5f, scale,
					  pixels.data(), a_size, a_size, a_size * 4);

		for (std::size_t i = 0; i < pixels.size(); i += 4)
		{
			const unsigned alpha = pixels[i + 3];
			for (std::size_t channel = 0; channel < 3; ++channel)
			{
				pixels[i + channel] = static_cast<std::uint8_t>((pixels[i + channel] * alpha + 127) / 255);
			}
		}
		return pixels;
	}
}
