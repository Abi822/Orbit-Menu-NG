#include "Integration/MenuFrameworkBridge.h"
#include <SKSEMenuFramework.h>
#include "Config/MenuFrameworkConfig.h"
#include "Config/Translation.h"
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
				Config::MenuFrameworkConfig::OnMenuClosed();
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
		SKSEMenuFramework::AddSectionItem(Config::Translate("Settings"), Config::MenuFrameworkConfig::RenderSettings);
		SKSEMenuFramework::AddSectionItem(Config::Translate("Display"), Config::MenuFrameworkConfig::RenderDisplay);
		SKSEMenuFramework::AddSectionItem(Config::Translate("Pages"), Config::MenuFrameworkConfig::RenderPages);
		SKSEMenuFramework::AddSectionItem(Config::Translate("Commands"), Config::MenuFrameworkConfig::RenderCommands);
		SKSEMenuFramework::AddSectionItem(Config::Translate("Slow Motion"), Config::MenuFrameworkConfig::RenderSlowMotion);

		SKSEMenuFramework::AddInputEvent(OnInputEvent);
		SKSEMenuFramework::AddEvent(OnFrameworkEvent, 0.0f);
	}

	bool IsBlockingWindowOpen()
	{
		return SKSEMenuFramework::IsAnyBlockingWindowOpened();
	}
}
