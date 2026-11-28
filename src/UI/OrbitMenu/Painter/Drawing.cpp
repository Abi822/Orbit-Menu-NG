#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kImageGrid = 8;
	}

	void Painter::TextAt(float a_size, ImVec2 a_topLeft, ImVec4 a_color, std::string_view a_text) const
	{
		const char *end = a_text.data() + a_text.size();
		const ImVec2 pos{std::floor(a_topLeft.x), std::floor(a_topLeft.y)};
		const float shadow = std::max(1.0f, Px(1.5f));
		_drawList.AddText(&_font, a_size, {pos.x + shadow, pos.y + shadow}, Color(_colors.textShadow, a_color.w * _alpha), a_text.data(), end);
		_drawList.AddText(&_font, a_size, pos, Color(a_color, _alpha), a_text.data(), end);
	}

	void Painter::Text(float a_size, ImVec2 a_topCenter, ImVec4 a_color, const TextLine &a_line) const
	{
		TextAt(a_size, {a_topCenter.x - a_line.Width(a_size) * 0.5f, a_topCenter.y}, a_color, a_line.text);
	}

	void Painter::TextCentered(float a_size, ImVec2 a_center, ImVec4 a_color, const TextLine &a_line) const
	{
		Text(a_size, {a_center.x, a_center.y - a_size * 0.5f}, a_color, a_line);
	}

	void Painter::Line(ImVec2 a_from, ImVec2 a_to, ImU32 a_color, float a_thickness) const
	{
		if (!_curve.Active())
		{
			_drawList.AddLine(a_from, a_to, a_color, a_thickness);
			return;
		}
		std::array<ImVec2, kEdgeSegments + 1> points{};
		for (std::size_t i = 0; i < points.size(); ++i)
		{
			points[i] = a_from + (a_to - a_from) * (static_cast<float>(i) / static_cast<float>(kEdgeSegments));
		}
		_drawList.AddPolyline(points.data(), static_cast<int>(points.size()), a_color, ImDrawFlags_None, a_thickness);
	}

	void Painter::Image(const Graphics::CachedImage &a_image, ImVec2 a_center, float a_size, ImU32 a_color) const
	{
		const ImVec2 half{a_size * 0.5f, a_size * 0.5f};
		if (!_curve.Active())
		{
			const ImVec2 min{std::floor(a_center.x - half.x + 0.5f), std::floor(a_center.y - half.y + 0.5f)};
			_drawList.AddImage(a_image.texture, min, min + ImVec2{a_size, a_size}, a_image.uv0, a_image.uv1, a_color);
			return;
		}
		const ImVec2 origin = a_center - half;
		const ImVec2 step{a_size / kImageGrid, a_size / kImageGrid};
		const ImVec2 uvStep = (a_image.uv1 - a_image.uv0) / static_cast<float>(kImageGrid);
		_drawList.PushTexture(a_image.texture);
		_drawList.PrimReserve(kImageGrid * kImageGrid * 6, kImageGrid * kImageGrid * 4);
		for (int row = 0; row < kImageGrid; ++row)
		{
			for (int column = 0; column < kImageGrid; ++column)
			{
				const ImVec2 cell{static_cast<float>(column), static_cast<float>(row)};
				const ImVec2 next = cell + ImVec2{1.0f, 1.0f};
				_drawList.PrimRectUV(origin + step * cell, origin + step * next, a_image.uv0 + uvStep * cell, a_image.uv0 + uvStep * next, a_color);
			}
		}
		_drawList.PopTexture();
	}

	void Painter::RotatedImage(const Graphics::CachedImage &a_image, ImVec2 a_center, ImVec2 a_up, ImVec2 a_right, ImU32 a_color) const
	{
		_drawList.AddImageQuad(a_image.texture, a_center + a_up - a_right, a_center + a_up + a_right, a_center - a_up + a_right, a_center - a_up - a_right, a_image.uv0,
							   {a_image.uv1.x, a_image.uv0.y}, a_image.uv1, {a_image.uv0.x, a_image.uv1.y}, a_color);
	}
}
