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
		explicit Inventory(RE::PlayerCharacter &a_player) noexcept : _player(a_player) {}

		[[nodiscard]] std::optional<CarriedCopy> FindCarriedCopy(RE::TESBoundObject &a_object, std::string_view a_name, const CarriedCopy *a_taken = nullptr) const;

		[[nodiscard]] RE::ExtraDataList *HeldExtraList(bool a_leftHand) const;

		[[nodiscard]] RE::ExtraDataList *WornExtraList(RE::TESBoundObject &a_object) const;

		[[nodiscard]] RE::TESForm *HeldItem(bool a_leftHand) const;

		[[nodiscard]] bool IsInHand(const RE::TESForm &a_item, std::string_view a_name, bool a_leftHand) const;

		[[nodiscard]] bool IsEquipped(const RE::TESForm &a_item, std::string_view a_name) const;

		[[nodiscard]] Core::HeldIn HandsHolding(const RE::TESForm &a_item, std::string_view a_name) const;

		[[nodiscard]] bool Knows(RE::TESForm &a_magic) const;

	private:
		[[nodiscard]] RE::TESObjectREFR::InventoryItemMap::mapped_type Entry(RE::TESBoundObject &a_object) const;

		RE::PlayerCharacter &_player;
	};
}
