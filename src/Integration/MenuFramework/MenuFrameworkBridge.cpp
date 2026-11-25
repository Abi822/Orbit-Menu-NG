#include "Integration/MenuFramework/MenuFrameworkBridge.h"
#include <SKSEMenuFramework.h>
#include "Config/SettingsMenu/MenuFrameworkConfig.h"
#include "Translation/Translation.h"
#include "Input/InputManager.h"

namespace Orbit::Integration::MenuFramework
{
	namespace
	{
		bool __stdcall OnInputEvent(RE::InputEvent *a_event)
		{
			return Input::InputManager::Get().ProcessCaptureEvent(a_event);
		}

		void __stdcall OnFrameworkEvent(SKSEMenuFramework::Model::EventType a_type)
		{
			if (a_type == SKSEMenuFramework::Model::EventType::kCloseMenu)
			{
				Config::SettingsMenu::MenuFrameworkConfig::OnMenuClosed();
			}
		}
	}

	void Install()
	{
		if (!GetMenuFrameworkModule())
		{
			logger::warn("SKSE Menu Framework is not installed.");
			return;
		}

		logger::info("SKSE Menu Framework found version {}", SKSEMenuFramework::GetMenuFrameworkVersion());
		SKSEMenuFramework::SetSection("Orbit Menu");
		SKSEMenuFramework::AddSectionItem(Translation::Translate("Settings"), Config::SettingsMenu::MenuFrameworkConfig::RenderSettings);
		SKSEMenuFramework::AddSectionItem(Translation::Translate("Display"), Config::SettingsMenu::MenuFrameworkConfig::RenderDisplay);
		SKSEMenuFramework::AddSectionItem(Translation::Translate("Pages"), Config::SettingsMenu::MenuFrameworkConfig::RenderPages);
		SKSEMenuFramework::AddSectionItem(Translation::Translate("Slow Motion"), Config::SettingsMenu::MenuFrameworkConfig::RenderSlowMotion);
		SKSEMenuFramework::AddInputEvent(OnInputEvent);
		SKSEMenuFramework::AddEvent(OnFrameworkEvent, 0.0f);
	}

	bool IsBlockingWindowOpen()
	{
		return SKSEMenuFramework::IsAnyBlockingWindowOpened();
	}
}
