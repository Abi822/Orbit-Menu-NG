#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kPageIndicatorSegments = 24;
	}

	void Painter::DrawPageIndicator() const
	{
		const auto &settings = _scene.settings;
		if (!settings.enablePages || !settings.showPageIndicator || !_scene.input.open)
		{
			return;
		}
		const int count = settings.numberOfPages;
		const int current = _scene.content.Page();
		const int columns = std::min(settings.pageIndicatorColumns, count);
		const int rows = (count + columns - 1) / columns;
		const float size = Px(static_cast<float>(settings.pageIndicatorSize));
		const float step = size + Px(static_cast<float>(settings.pageIndicatorSpacing));
		const ImVec2 middle = _layout.center + ImVec2{Px(static_cast<float>(settings.pageIndicatorX)), Px(static_cast<float>(settings.pageIndicatorY))};
		const float rotation = static_cast<float>(settings.pageIndicatorRotation) * Core::kPi / 180.0f;
		const ImVec2 across = Core::DirectionFromAngle(rotation + Core::kPi * 0.5f);
		const ImVec2 down = Core::DirectionFromAngle(rotation + Core::kPi);

		std::array<ImVec2, static_cast<std::size_t>(Config::Settings::kMaxNumberOfPages)> spots{};
		for (int page = 0; page < count; ++page)
		{
			const int row = page / columns;
			const int inRow = row == rows - 1 ? count - row * columns : columns;
			const float x = (static_cast<float>(page % columns) - static_cast<float>(inRow - 1) * 0.5f) * step;
			const float y = (static_cast<float>(row) - static_cast<float>(rows - 1) * 0.5f) * step;
			spots[static_cast<std::size_t>(page)] = middle + across * x + down * y;
		}

		const auto active = _scene.skin.Find(Skins::Part::kPageIndicatorActive);
		const auto inactive = _scene.skin.Find(Skins::Part::kPageIndicatorInactive);
		if (active || inactive)
		{
			const ImVec2 up = down * (-size * 0.5f);
			const ImVec2 right = across * (size * 0.5f);
			Graphics::PremultipliedDraw premultiplied{_drawList};
			for (int page = 0; page < count; ++page)
			{
				if (const auto &image = page == current ? active : inactive)
				{
					RotatedImage(*image, spots[static_cast<std::size_t>(page)], up, right, Graphics::PremultipliedColor(_alpha));
				}
			}
		}
		for (int page = 0; page < count; ++page)
		{
			const bool isCurrent = page == current;
			if (isCurrent ? active : inactive)
			{
				continue;
			}
			const ImVec2 at = spots[static_cast<std::size_t>(page)];
			if (isCurrent)
			{
				_drawList.AddCircleFilled(at, size * 0.5f, Color(_colors.pageIndicatorActive, _alpha), kPageIndicatorSegments);
			}
			else
			{
				_drawList.AddCircle(at, size * 0.5f - Px(1.0f), Color(_colors.pageIndicatorInactive, _alpha), kPageIndicatorSegments, std::max(1.0f, Px(1.5f)));
			}
		}
	}
}
