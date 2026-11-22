#include "Config/Settings.h"
#include "Config/Translation.h"
#include "Game/Hooks.h"
#include "Game/SlowMotion.h"
#include "Input/InputManager.h"
#include "Input/KeyBinds.h"
#include "Integration/I4Addon.h"
#include "Integration/MenuFrameworkBridge.h"
#include "Items/Assignment.h"
#include "Items/EquipController.h"
#include "Items/Pages.h"
#include "Items/SlotStore.h"
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
		auto &input = RMS::Input::InputManager::Get();

		switch (a_message->type)
		{
		case SKSE::MessagingInterface::kPostLoad:
			RMS::Integration::MenuFramework::Install();
			break;

		case SKSE::MessagingInterface::kDataLoaded:
		{
			RMS::Integration::I4Addon::Get().Load();
			RMS::Items::EquipController::Get().Register();
			RMS::UI::ImGuiLayer::Setup();
			input.Enable();
			const auto settings = RMS::Config::SettingsStore::Get().Snapshot();
			const auto key = settings.activationKey != RMS::Input::kKeyBindNone ? settings.activationKey : settings.gamepadActivationKey;
			if (key == RMS::Input::kKeyBindNone)
			{
				logger::warn("ImGui: Ready: but no key or controller button is bound to open the Orbit Menu");
			}
			else
			{
				logger::info("ImGui: Ready: hold {} in game to open the Orbit Menu", RMS::Input::KeyName(key));
			}
		}
		break;

		case SKSE::MessagingInterface::kPreLoadGame:
			input.ForceClose();
			RMS::Game::SlowMotion::Get().Reset();
			break;

		case SKSE::MessagingInterface::kPostLoadGame:
		case SKSE::MessagingInterface::kNewGame:
			input.ForceClose();
			RMS::Game::SlowMotion::Get().Reset();
			RMS::UI::ImGuiLayer::Preload();
			break;

		default:
			break;
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *a_skse)
{
	SKSE::Init(a_skse, {.log = false, .trampoline = true, .trampolineSize = RMS::Game::Hooks::kTrampolineSize});
	OpenLog();

	logger::info("Orbit Menu Starting");
	logger::info("ImGui: Orbit Menu loading on Skyrim {}", REL::Module::get().version().string("."));

	RMS::Config::SettingsStore::Get().Load();
	RMS::Config::LoadTranslation(RMS::Config::SettingsStore::Get().Snapshot().languagePath);
	RMS::Items::SlotStore::RegisterSerialization();

	auto &input = RMS::Input::InputManager::Get();
	input.SetUiBusyPredicate(RMS::Integration::MenuFramework::IsBlockingWindowOpen);
	input.SetAssignHandler(RMS::Items::Assignment::AssignHighlighted);
	input.SetCommitHandler([](int a_slot, RMS::Input::Hand a_hand)
						   { RMS::Items::EquipController::Get().EquipSlot(a_slot, a_hand); });
	input.SetSwapHandler([]
						 { RMS::Items::EquipController::Get().SwapHands(); });
	input.SetPageHandler([](int a_step)
						 { RMS::Items::Pages::Get().Turn(a_step); });

	const auto *messaging = SKSE::GetMessagingInterface();
	if (!messaging || !messaging->RegisterListener(OnSKSEMessage))
	{
		logger::critical("ImGui: Could not register for SKSE messages! Orbit Menu will not run!");
		return false;
	}

	RMS::Game::Hooks::Install();
	return true;
}
