#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	void Painter::DrawBackdrop() const
	{
		if (_scene.settings.showBackground && _colors.background.w > 0.0f)
		{
			_drawList.AddRectFilled({0.0f, 0.0f}, _display, Color(_colors.background, _alpha));
		}
	}
}
