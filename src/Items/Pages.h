#pragma once

#include "Config/Settings.h"
#include "Items/SlotStore.h"

namespace Orbit::Items
{
	class Pages
	{
	public:
		static constexpr std::uint32_t kRecord = 'PAGS';
		static constexpr std::uint32_t kVersion = SlotStore::kSlotsVersion;

		static Pages &Get();

		int Current() const { return _current; }
		void Turn(int a_step);
		void Fit(const Config::Settings &a_settings);
		void ClearExtraPages();
		std::string Hint(const Config::Settings &a_settings, bool a_gamepad) const;

		SlotArray FirstPage() const;
		void Save(SKSE::SerializationInterface &a_intfc) const;
		void Load(SKSE::SerializationInterface &a_intfc, std::uint32_t a_version);
		// The virtual keys of the command slots on every page.
		std::vector<std::uint32_t> CommandKeys() const;
		void Revert();

	private:
		using PageArray = std::array<SlotArray, Config::Settings::kMaxNumberOfPages>;

		Pages() = default;

		void ShowLocked(int a_page);

		mutable std::mutex _lock;
		PageArray _pages{};
		std::atomic<int> _current{0};
	};
}
