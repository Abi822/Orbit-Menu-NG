#pragma once

namespace Orbit::UI::Graphics
{
	class SwfIcon
	{
	public:
		SwfIcon(std::string_view a_source, std::string_view a_label, std::optional<std::uint32_t> a_tint);

		bool Valid() const { return _view != nullptr; }

		void Render(int a_x, int a_y, int a_size, int a_targetWidth, int a_targetHeight);

	private:
		void Fit(float a_size);

		RE::GPtr<RE::GFxMovieView> _view;
		RE::GFxValue _root;
		RE::GRectF _stage{};
		RE::GRectF _content{};
	};
}
