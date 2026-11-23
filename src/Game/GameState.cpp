#include "Game/GameState.h"
#include "Config/Settings.h"

namespace Orbit::Game
{
	namespace
	{
		bool IsGameWindowForeground()
		{
			HWND foreground = GetForegroundWindow();
			if (!foreground)
			{
				return false;
			}
			DWORD processId = 0;
			GetWindowThreadProcessId(foreground, &processId);
			return processId == GetCurrentProcessId();
		}

		// Stop Orbit from opening in Beast Form and Vampire Lord
		bool IsInBeastForm()
		{
			const auto *menuControls = RE::MenuControls::GetSingleton();
			return menuControls && menuControls->InBeastForm();
		}
	}

	bool IsInGameplay()
	{
		auto *player = RE::PlayerCharacter::GetSingleton();
		auto *ui = RE::UI::GetSingleton();
		if (!player || !player->Is3DLoaded() || !ui || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) || IsInBeastForm())
		{
			return false;
		}
		if (!ui->IsShowingMenus() || ui->GameIsPaused() || ui->IsMenuOpen(RE::CursorMenu::MENU_NAME))
		{
			return false;
		}

		const auto *controls = RE::ControlMap::GetSingleton();
		if (!controls || !controls->IsLookingControlsEnabled() || !controls->IsFightingControlsEnabled())
		{
			return false;
		}

		const auto &contexts = controls->GetRuntimeData().contextPriorityStack;
		if (contexts.empty() || contexts.back() != RE::ControlMap::InputContextID::kGameplay)
		{
			return false;
		}
		return IsGameWindowForeground();
	}

	bool IsInItemMenu()
	{
		auto *ui = RE::UI::GetSingleton();
		if (!ui || (!ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME) && !ui->IsMenuOpen(RE::MagicMenu::MENU_NAME)) || IsInBeastForm())
		{
			return false;
		}
		if (ui->IsMenuOpen(RE::MessageBoxMenu::MENU_NAME) || ui->IsMenuOpen(RE::Console::MENU_NAME))
		{
			return false;
		}
		const auto *controls = RE::ControlMap::GetSingleton();
		if (controls && controls->GetRuntimeData().textEntryCount > 0)
		{
			return false;
		}
		return IsGameWindowForeground();
	}

	void ShowHudMessage(std::string a_text)
	{
		if (!Config::SettingsStore::Get().Snapshot().showHudMessages)
		{
			return;
		}
		RE::SendHUDMessage::ShowHUDMessage(a_text.c_str());
	}
}
