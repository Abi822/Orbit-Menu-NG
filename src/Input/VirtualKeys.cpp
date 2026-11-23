#include "Input/VirtualKeys.h"
#include "Config/Settings.h"
#include "Input/KeyBinds.h"

namespace Orbit::Input::VirtualKeys
{
	namespace
	{
		// DirectInput scan codes with no key on PC keyboards (gaps of the DIK_ table), first and last of each run.
		constexpr std::array<std::pair<std::uint32_t, std::uint32_t>, 6> kRanges{{{0x59, 0x63}, {0x67, 0x6F}, {0x71, 0x72}, {0x74, 0x78}, {0x80, 0x8C}, {0xF0, 0xFE}}};

		struct Pending
		{
			std::uint32_t code;
			bool down;  // already sent as pressed: the release is next
		};

		std::mutex lock;
		std::vector<Pending> pending;
		std::atomic<std::uint32_t> armed{0};
		std::vector<RE::ButtonEvent *> sent;  // input thread only
	}

	bool IsVirtual(std::uint32_t a_code)
	{
		return std::ranges::any_of(kRanges, [&](const auto &a_range)
								   { return a_code >= a_range.first && a_code <= a_range.second; });
	}

	std::uint32_t NextFree(const std::vector<std::uint32_t> &a_used)
	{
		for (const auto &[first, last] : kRanges)
		{
			for (auto code = first; code <= last; ++code)
			{
				if (std::ranges::find(a_used, code) == a_used.end())
				{
					return code;
				}
			}
		}
		return 0;
	}

	void Tap(std::uint32_t a_code)
	{
		if (a_code == 0 || a_code >= InputMap::kMacro_MouseButtonOffset)
		{
			return;
		}
		std::scoped_lock guard{lock};
		pending.push_back({a_code, false});
	}

	void Arm(std::uint32_t a_code)
	{
		armed = a_code;
	}

	std::uint32_t Armed()
	{
		return armed;
	}

	bool TakeArmedPress(const RE::InputEvent &a_event)
	{
		const auto code = armed.load();
		if (code == 0)
		{
			return false;
		}
		const auto *button = a_event.AsButtonEvent();
		if (!button || !button->IsDown())
		{
			return false;
		}
		auto *ui = RE::UI::GetSingleton();
		if (!ui || !ui->IsMenuOpen(RE::JournalMenu::MENU_NAME))
		{
			return false;
		}
		const auto bind = ToKeyBind(*button);
		const auto settings = Config::SettingsStore::Get().Snapshot();
		if (!bind || (*bind != settings.activationKey && *bind != settings.gamepadActivationKey))
		{
			return false;
		}
		armed = 0;
		Tap(code);
		logger::info("Virtual Keys: key {} sent to the open menu for binding", code);
		return true;
	}

	RE::InputEvent *Prepend(RE::InputEvent *a_head)
	{
		std::scoped_lock guard{lock};
		for (auto it = pending.begin(); it != pending.end();)
		{
			// pressed: value 1, held 0; released a frame later: value 0, held > 0
			auto *event = RE::ButtonEvent::Create(RE::INPUT_DEVICE::kKeyboard, "", it->code, it->down ? 0.0f : 1.0f, it->down ? 0.05f : 0.0f);
			if (event)
			{
				event->next = a_head;
				a_head = event;
				sent.push_back(event);
			}
			if (it->down)
			{
				it = pending.erase(it);
			}
			else
			{
				it->down = true;
				++it;
			}
		}
		return a_head;
	}

	void Release()
	{
		for (auto *event : sent)
		{
			RE::free(event);
		}
		sent.clear();
	}
}
