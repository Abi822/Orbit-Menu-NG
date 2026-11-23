#pragma once

namespace Orbit::Game
{
	[[nodiscard]] bool IsInGameplay();

	[[nodiscard]] bool IsInItemMenu();

	void ShowHudMessage(std::string a_text);
}
