#include "Items/Pages.h"
#include "Input/KeyBinds.h"
#include "Translation/Translation.h"

namespace Orbit::Items
{
	Pages &Pages::Get()
	{
		static Pages instance;
		return instance;
	}

	void Pages::Turn(int a_step)
	{
		const auto settings = Config::SettingsStore::Get().Snapshot();
		if (!settings.enablePages)
		{
			return;
		}
		const int count = settings.numberOfPages;
		std::scoped_lock lock{_lock};
		ShowLocked(((Current() + a_step) % count + count) % count);
	}

	void Pages::Fit(const Config::Settings &a_settings)
	{
		std::scoped_lock lock{_lock};
		if (!a_settings.enablePages || Current() >= a_settings.numberOfPages)
		{
			ShowLocked(0);
		}
	}

	void Pages::ClearExtraPages()
	{
		std::scoped_lock lock{_lock};
		ShowLocked(0);
		_pages = {};
		logger::info("Pages: User has cleared page");
	}

	std::string Pages::Line(const Config::Settings &a_settings, bool a_gamepad) const
	{
		std::string line = a_settings.showPageIndicator ? std::string{} : Translation::Translate("Page {} of {}", Current() + 1, a_settings.numberOfPages);
		const auto previous = a_gamepad ? a_settings.gamepadPreviousPageKey : a_settings.previousPageKey;
		const auto next = a_gamepad ? a_settings.gamepadNextPageKey : a_settings.nextPageKey;
		std::string keys;
		for (const auto key : {previous, next})
		{
			if (key != Input::kKeyBindNone)
			{
				keys += keys.empty() ? Input::KeyName(key) : ", " + Input::KeyName(key);
			}
		}
		if (!keys.empty())
		{
			const auto select = Translation::Translate("Select Page: {}", keys);
			line += line.empty() ? select : "   |   " + select;
		}
		return line;
	}

	Pages::PageArray Pages::Snapshot() const
	{
		std::scoped_lock lock{_lock};
		PageArray pages = _pages;
		pages[static_cast<std::size_t>(Current())] = SlotStore::Get().Snapshot();
		return pages;
	}

	void Pages::Restore(PageArray a_pages)
	{
		std::scoped_lock lock{_lock};
		_pages = std::move(a_pages);
	}

	void Pages::Revert()
	{
		std::scoped_lock lock{_lock};
		_pages = {};
		_current = 0;
	}

	void Pages::ShowLocked(int a_page)
	{
		const int current = Current();
		if (a_page == current)
		{
			return;
		}
		auto &shown = _pages[static_cast<std::size_t>(a_page)];
		_pages[static_cast<std::size_t>(current)] = SlotStore::Get().Exchange(std::exchange(shown, SlotArray{}));
		_current = a_page;
	}
}
