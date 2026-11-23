#pragma once

#include "Config/Settings.h"
#include "Core/SlotView.h"
#include "Core/Slots.h"
#include "Items/Inventory.h"
#include "Items/ItemKind.h"
#include "Items/SlotStore.h"

namespace Orbit::Items
{
	class SlotViewBuilder
	{
	public:
		[[nodiscard]] static Core::PerSlot<Core::SlotView> DescribeAll(const Config::Settings &a_settings);

	private:
		SlotViewBuilder(RE::PlayerCharacter &a_player, const Config::Settings &a_settings) noexcept;

		[[nodiscard]] Core::SlotView Describe(const AssignedItem &a_slot) const;
		[[nodiscard]] Core::SlotView DescribeForm(RE::TESForm &a_form, const std::string &a_name, ItemKind a_kind) const;
		[[nodiscard]] Core::SlotView Missing(const std::string &a_name, std::string a_status) const;
		void AddIcon(Core::SlotView &a_view, const RE::TESForm &a_form) const;

		RE::PlayerCharacter &_player;
		Inventory _inventory;
		const Config::Settings &_settings;
	};
}
