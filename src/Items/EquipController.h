#pragma once

#include "Input/InputManager.h"

namespace RMS::Items
{
	class EquipController final : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		[[nodiscard]] static EquipController &Get() noexcept;

		void Register();

		void EquipSlot(int a_slot, Input::Hand a_hand);

		void SwapHands();

		[[nodiscard]] std::uint32_t ChangeCount() const noexcept { return _changes.load(std::memory_order_relaxed); }

		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent *a_event, RE::BSTEventSource<RE::TESEquipEvent> *a_source) override;

	private:
		EquipController() = default;

		std::atomic<std::uint32_t> _changes{0};
	};
}
