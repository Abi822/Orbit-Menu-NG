#pragma once

#include "Core/SlotView.h"

namespace Orbit::Items
{
	inline constexpr bool kLeftHand = true;
	inline constexpr bool kRightHand = false;

	struct CarriedCopy
	{
		RE::ExtraDataList *extraList = nullptr;
		std::int32_t total = 0;
	};

	class Inventory
	{
	public:
		explicit Inventory(RE::PlayerCharacter &a_player) : _player(a_player) {}

		std::optional<CarriedCopy> FindCarriedCopy(RE::TESBoundObject &a_object, std::string_view a_name, const CarriedCopy *a_taken = nullptr) const;

		RE::ExtraDataList *HeldExtraList(bool a_leftHand) const;

		RE::ExtraDataList *WornExtraList(RE::TESBoundObject &a_object) const;

		RE::TESForm *HeldItem(bool a_leftHand) const;

		bool IsInHand(const RE::TESForm &a_item, std::string_view a_name, bool a_leftHand) const;

		bool IsEquipped(const RE::TESForm &a_item, std::string_view a_name) const;

		Core::HeldIn HandsHolding(const RE::TESForm &a_item, std::string_view a_name) const;

		bool Knows(RE::TESForm &a_magic) const;

	private:
		RE::TESObjectREFR::InventoryItemMap::mapped_type Entry(RE::TESBoundObject &a_object) const;

		RE::PlayerCharacter &_player;
	};
}
