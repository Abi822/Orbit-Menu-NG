#pragma once

#include "Config/Settings.h"
#include "Items/SlotStore.h"

namespace Orbit::Items
{
	class Pages
	{
	public:
		static constexpr std::uint32_t kRecord = 'PAGS';
		static constexpr std::uint32_t kVersion = 1;

		static Pages &Get();

		int Current() const { return _current; }
		void Turn(int a_step);
		void Fit(const Config::Settings &a_settings);
		void ClearExtraPages();
		std::string Hint(const Config::Settings &a_settings, bool a_gamepad) const;

		SlotArray FirstPage() const;
		void Save(SKSE::SerializationInterface &a_intfc) const;
		void Load(SKSE::SerializationInterface &a_intfc);
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
