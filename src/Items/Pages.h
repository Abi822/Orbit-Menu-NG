#pragma once

#include "Config/Settings.h"
#include "Items/SlotStore.h"

namespace Orbit::Items
{
	class Pages
	{
	public:
		using PageArray = std::array<SlotArray, Config::Settings::kMaxNumberOfPages>;
		static Pages &Get();
		int Current() const { return _current; }
		void Turn(int a_step);
		void Fit(const Config::Settings &a_settings);
		void ClearExtraPages();
		std::string Line(const Config::Settings &a_settings, bool a_gamepad) const;
		PageArray Snapshot() const;
		void Restore(PageArray a_pages);
		void Revert();

	private:
		Pages() = default;
		void ShowLocked(int a_page);
		mutable std::mutex _lock;
		PageArray _pages{};
		std::atomic<int> _current{0};
	};
}
