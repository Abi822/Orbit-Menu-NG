#include "UI/OrbitMenu/Painter/Painter.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr float kControlsHintLineSpacing = 1.3f;
	}

	void Painter::DrawControlsHint() const
	{
		const auto &settings = _scene.settings;
		if (!settings.showControlsHint || !_scene.input.open)
		{
			return;
		}
		const float size = TextPx(settings.controlsHintTextSize);
		const auto &pageLine = _scene.content.PageLine();
		const float pageLineGap = pageLine.text.empty() ? 0.0f : size * kControlsHintLineSpacing;
		ImVec2 at = _layout.center + ImVec2{Px(static_cast<float>(settings.controlsHintX)), Px(static_cast<float>(settings.controlsHintY))};
		at.y = std::min(at.y, _display.y - size * 1.6f - pageLineGap);
		Text(size, at, _colors.controlsHint, _scene.content.ControlsHint());
		if (pageLineGap > 0.0f)
		{
			at.y += pageLineGap;
			Text(size, at, _colors.controlsHint, pageLine);
		}
	}
}
