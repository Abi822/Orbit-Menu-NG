#include "Config/Settings.h"
#include "Game/Hooks.h"
#include "Game/SlowMotion.h"
#include "Input/InputManager.h"
#include "Input/KeyBinds.h"
#include "Integration/I4/Icons.h"
#include "Integration/MenuFramework/MenuFrameworkBridge.h"
#include "Items/EquipController.h"
#include "Items/SlotStore.h"
#include "Translation/Translation.h"
#include "UI/ImGuiLayer.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace
{
	void OpenLog()
	{
		auto path = logger::log_directory();
		if (!path)
		{
			return;
		}
		*path /= std::format("{}.log", SKSE::GetPluginName());

		std::vector<spdlog::sink_ptr> sinks{std::make_shared<spdlog::sinks::msvc_sink_mt>(),
											std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true)};
		auto log = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%T.%e] [%=5t] [%L] %v");
	}

	void OnSKSEMessage(SKSE::MessagingInterface::Message *a_message)
	{
		auto &input = Orbit::Input::InputManager::Get();

		switch (a_message->type)
		{
		case SKSE::MessagingInterface::kPostLoad:
			Orbit::Integration::MenuFramework::Install();
			break;

		case SKSE::MessagingInterface::kDataLoaded:
		{
			Orbit::Integration::I4::Icons::Get().Load();
			Orbit::Items::EquipController::Get().Register();
			Orbit::UI::ImGuiLayer::Setup();
			input.Enable();
			const auto settings = Orbit::Config::SettingsStore::Get().Snapshot();
			const auto key = settings.activationKey != Orbit::Input::kKeyBindNone ? settings.activationKey : settings.gamepadActivationKey;
			if (key == Orbit::Input::kKeyBindNone)
			{
				logger::warn("No key or controller button is bound to open the Orbit Menu");
			}
			else
			{
				logger::info("Hold {} in game to open the Orbit Menu", Orbit::Input::KeyName(key));
			}
		}
		break;

		case SKSE::MessagingInterface::kPreLoadGame:
			input.ForceClose();
			Orbit::Game::SlowMotion::Get().Reset();
			break;

		case SKSE::MessagingInterface::kPostLoadGame:
		case SKSE::MessagingInterface::kNewGame:
			input.ForceClose();
			Orbit::Game::SlowMotion::Get().Reset();
			Orbit::UI::ImGuiLayer::Preload();
			break;

		default:
			break;
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *a_skse)
{
	SKSE::Init(a_skse, {.log = false, .trampoline = true, .trampolineSize = 70});
	OpenLog();

	logger::info("Orbit Menu Starting...");
	logger::info("Orbit Menu loading on Skyrim {}", REL::Module::get().version().string("."));

	Orbit::Config::SettingsStore::Get().Load();
	Orbit::Translation::LoadTranslation(Orbit::Config::SettingsStore::Get().Snapshot().languagePath);
	Orbit::Items::SlotStore::RegisterSerialization();

	const auto *messaging = SKSE::GetMessagingInterface();
	if (!messaging || !messaging->RegisterListener(OnSKSEMessage))
	{
		logger::critical("Could not register for SKSE messages! Orbit Menu will not run!");
		return false;
	}

	Orbit::Game::Hooks::Install();
	return true;
}
