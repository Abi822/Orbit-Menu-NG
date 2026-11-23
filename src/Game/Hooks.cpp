#include "Game/Hooks.h"
#include "Input/InputManager.h"
#include "Input/VirtualKeys.h"
#include "Items/Assignment.h"
#include "UI/ImGuiLayer.h"

namespace Orbit::Game::Hooks
{
	namespace
	{
		constexpr std::uint8_t kCall = 0xE8;
		constexpr std::uint8_t kJump = 0xE9;

		bool HasOpcode(std::uintptr_t a_address, std::uint8_t a_opcode)
		{
			return *reinterpret_cast<const std::uint8_t *>(a_address) == a_opcode;
		}

		struct DispatchInputEvents
		{
			static void thunk(RE::BSTEventSource<RE::InputEvent *> *a_dispatcher, RE::InputEvent *const *a_events)
			{
				auto &input = Input::InputManager::Get();
				RE::InputEvent *head = a_events ? *a_events : nullptr;
				RE::InputEvent *previous = nullptr;
				for (RE::InputEvent *event = head; event;)
				{
					RE::InputEvent *next = event->next;
					if (Input::VirtualKeys::TakeArmedPress(*event) || input.ProcessEvent(event))
					{
						if (previous)
						{
							previous->next = next;
						}
						else
						{
							head = next;
						}
					}
					else
					{
						previous = event;
					}
					event = next;
				}

				// the command slots' virtual keys of this frame go first, as if they were pressed on the keyboard
				RE::InputEvent *const filtered[] = {Input::VirtualKeys::Prepend(head)};
				func(a_dispatcher, filtered);
				Input::VirtualKeys::Release();
			}

			static inline REL::Relocation<decltype(thunk)> func;

			static bool Install(SKSE::Trampoline &a_trampoline)
			{
				const REL::Relocation<std::uintptr_t> call{RELOCATION_ID(67315, 68617), REL::Relocate(0x7B, 0x7B)};
				if (!HasOpcode(call.address(), kCall))
				{
					logger::error("Hooks: {:X} Input hook not installed! Orbit Menu can't be opened!", call.address());
					return false;
				}
				func = a_trampoline.write_call<5>(call.address(), thunk);
				return true;
			}
		};

		constexpr std::size_t kPostDisplay = 0x6;

		template <class Menu>
		struct MenuPostDisplay
		{
			static void thunk(RE::IMenu *a_menu)
			{
				func(a_menu);
				if constexpr (std::is_same_v<Menu, RE::LoadingMenu>)
				{
					UI::ImGuiLayer::OnLoadingScreenDrawn();
				}
				else
				{
					UI::ImGuiLayer::OnMenuDrawn(!std::is_same_v<Menu, RE::HUDMenu>);
				}
			}

			static inline REL::Relocation<decltype(thunk)> func;

			static void Install()
			{
				REL::Relocation<std::uintptr_t> vtable{Menu::VTABLE[0]};
				func = vtable.write_vfunc(kPostDisplay, thunk);
			}
		};

		template <class Menu>
		struct HighlightLookup
		{
			static void *thunk(void *a_list)
			{
				auto *item = func(a_list);
				Items::Assignment::OnHighlightChanged(Menu::MENU_NAME);
				return item;
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};

		template <class Menu>
		struct PreviewRemoved
		{
			static void thunk(RE::Inventory3DManager *a_manager)
			{
				func(a_manager);
				Items::Assignment::OnHighlightChanged(Menu::MENU_NAME);
			}

			static inline REL::Relocation<decltype(thunk)> func;
		};

		bool InstallInventoryHighlight(SKSE::Trampoline &a_trampoline)
		{
			const REL::Relocation<std::uintptr_t> lookup{RELOCATION_ID(50972, 51851), REL::Relocate(0x31, 0x31)};
			const REL::Relocation<std::uintptr_t> unload{RELOCATION_ID(50972, 51851), REL::Relocate(0x61, 0x5B)};
			if (!HasOpcode(lookup.address(), kCall) || !HasOpcode(unload.address(), kCall))
			{
				logger::error("Hooks: {:X} or {:X} Inventory highlight not installed! 1-8 keys can't be assigned in Inventory!", lookup.address(), unload.address());
				return false;
			}
			HighlightLookup<RE::InventoryMenu>::func = a_trampoline.write_call<5>(lookup.address(), HighlightLookup<RE::InventoryMenu>::thunk);
			PreviewRemoved<RE::InventoryMenu>::func = a_trampoline.write_call<5>(unload.address(), PreviewRemoved<RE::InventoryMenu>::thunk);
			return true;
		}

		bool InstallMagicHighlight(SKSE::Trampoline &a_trampoline)
		{
			const REL::Relocation<std::uintptr_t> lookup{RELOCATION_ID(51151, 52031), REL::Relocate(0x1B, 0x1B)};
			const REL::Relocation<std::uintptr_t> unload{RELOCATION_ID(51151, 52031), REL::Relocate(0x4C, 0x4C)};
			if (!HasOpcode(lookup.address(), kCall) || !HasOpcode(unload.address(), kJump))
			{
				logger::error("Hooks: {:X} or {:X} Magic menu highlight not installed! 1-8 keys can't be assigned in Magic menu!", lookup.address(), unload.address());
				return false;
			}
			HighlightLookup<RE::MagicMenu>::func = a_trampoline.write_call<5>(lookup.address(), HighlightLookup<RE::MagicMenu>::thunk);
			PreviewRemoved<RE::MagicMenu>::func = a_trampoline.write_branch<5>(unload.address(), PreviewRemoved<RE::MagicMenu>::thunk);
			return true;
		}
	}

	void Install()
	{
		auto &trampoline = SKSE::GetTrampoline();
		const bool inputHooked = DispatchInputEvents::Install(trampoline);
		const bool inventoryHooked = InstallInventoryHighlight(trampoline);
		const bool magicHooked = InstallMagicHighlight(trampoline);

		MenuPostDisplay<RE::HUDMenu>::Install();
		MenuPostDisplay<RE::InventoryMenu>::Install();
		MenuPostDisplay<RE::MagicMenu>::Install();
		MenuPostDisplay<RE::LoadingMenu>::Install();

		logger::info("Hooks: Installed for {}{}{}PostDisplay", inputHooked ? "Dispatch Input, " : "", inventoryHooked ? "Inventory highlight, " : "", magicHooked ? "Magic highlight, " : "");
	}
}
