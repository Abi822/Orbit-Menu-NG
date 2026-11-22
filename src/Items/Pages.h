#pragma once

#include "Config/Settings.h"
#include "Items/SlotStore.h"

namespace RMS::Items
{
	class Pages
	{
	public:
		static constexpr std::uint32_t kRecord = 'PAGS';
		static constexpr std::uint32_t kVersion = 1;

		[[nodiscard]] static Pages &Get() noexcept;

		[[nodiscard]] int Current() const noexcept { return _current.load(std::memory_order_relaxed); }
		void Turn(int a_step);
		void Fit(const Config::Settings &a_settings);
		void ClearExtraPages();
		[[nodiscard]] std::string Hint(const Config::Settings &a_settings, bool a_gamepad) const;

		[[nodiscard]] SlotArray FirstPage() const;
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
