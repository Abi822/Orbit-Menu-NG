#pragma once

struct NSVGimage;

namespace RMS::UI::Skins
{
	class SvgImage
	{
	public:
		explicit SvgImage(const std::filesystem::path &a_file);

		[[nodiscard]] std::vector<std::uint8_t> Rasterize(int a_size) const;

	private:
		struct Delete
		{
			void operator()(NSVGimage *a_image) const noexcept;
		};

		std::unique_ptr<NSVGimage, Delete> _image;
	};
}
