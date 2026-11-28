#pragma once

#include "Input/InputManager.h"

namespace Orbit::Items
{
	class EquipController final : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		static EquipController &Get();
		void Register();
		void EquipSlot(int a_slot, Input::Hand a_hand);
		void SwapHands();
		std::uint32_t ChangeCount() const { return _changes; }
		int LastEquippedSlot() const;
		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent *a_event, RE::BSTEventSource<RE::TESEquipEvent> *a_source) override;

	private:
		EquipController() = default;
		std::atomic<std::uint32_t> _changes{0};
		mutable std::mutex _lock;
		int _lastEquippedPage = -1;
		int _lastEquippedSlot = -1;
	};
}
