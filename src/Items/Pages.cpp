#include "Items/Pages.h"
#include "Config/Translation.h"
#include "Input/KeyBinds.h"

namespace Orbit::Items
{
	Pages &Pages::Get()
	{
		static Pages singleton;
		return singleton;
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

	std::string Pages::Hint(const Config::Settings &a_settings, bool a_gamepad) const
	{
		std::string hint = Config::Translate("Page {} of {}", Current() + 1, a_settings.numberOfPages);
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
			hint += "   |   " + Config::Translate("Select Page: {}", keys);
		}
		return hint;
	}

	SlotArray Pages::FirstPage() const
	{
		std::scoped_lock lock{_lock};
		return Current() == 0 ? SlotStore::Get().Snapshot() : _pages[0];
	}

	void Pages::Save(SKSE::SerializationInterface &a_intfc) const
	{
		const auto settings = Config::SettingsStore::Get().Snapshot();
		if (!settings.enablePages)
		{
			return;
		}
		std::vector<SlotArray> pages;
		{
			std::scoped_lock lock{_lock};
			for (int page = 1; page < settings.numberOfPages; ++page)
			{
				pages.push_back(page == Current() ? SlotStore::Get().Snapshot() : _pages[static_cast<std::size_t>(page)]);
			}
		}
		while (!pages.empty() && std::ranges::all_of(pages.back(), &AssignedItem::IsEmpty))
		{
			pages.pop_back();
		}
		if (pages.empty())
		{
			return;
		}
		if (!a_intfc.OpenRecord(kRecord, kVersion))
		{
			logger::error("Pages: SKSE co-save could not open the pages record!");
			return;
		}
		bool written = a_intfc.WriteRecordData(static_cast<std::uint32_t>(pages.size()));
		for (const auto &slots : pages)
		{
			written = written && SlotStore::WriteSlots(a_intfc, slots);
		}
		if (!written)
		{
			logger::error("Pages: SKSE co-save writing pages 2 and up failed and they will load empty!");
		}
	}

	void Pages::Load(SKSE::SerializationInterface &a_intfc, std::uint32_t a_version)
	{
		if (!Config::SettingsStore::Get().Snapshot().enablePages)
		{
			logger::info("Pages: SKSE co-save Pages is off, so pages 2 and up in this save are not used");
			return;
		}
		PageArray pages{};
		std::uint32_t count = 0;
		bool read = a_intfc.ReadRecordData(count) == sizeof(count);
		for (std::uint32_t i = 0; read && i < count; ++i)
		{
			SlotArray slots{};
			read = SlotStore::ReadSlots(a_intfc, slots, a_version);
			if (read && i + 1 < pages.size())
			{
				pages[i + 1] = std::move(slots);
			}
		}
		if (!read)
		{
			logger::error("Pages: SKSE co-save pages record is damaged pages 2 and up are left empty!");
			return;
		}
		std::scoped_lock lock{_lock};
		_pages = std::move(pages);
		logger::info("Pages: SKSE co-save {} more page(s) loaded", count);
	}

	std::vector<std::uint32_t> Pages::CommandKeys() const
	{
		std::vector<std::uint32_t> keys;
		const auto collect = [&](const SlotArray &a_slots)
		{
			for (const auto &slot : a_slots)
			{
				if (slot.IsCommand())
				{
					keys.push_back(slot.key);
				}
			}
		};
		std::scoped_lock lock{_lock};
		collect(SlotStore::Get().Snapshot());
		for (const auto &page : _pages)
		{
			collect(page);
		}
		return keys;
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
