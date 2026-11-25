#include "Items/SlotViewBuilder.h"
#include "Translation/Translation.h"
#include "Integration/I4/I4Icons.h"
#include "Items/ItemDescription.h"
#include "Items/ItemIcons.h"

namespace Orbit::Items
{
	Core::PerSlot<Core::SlotView> SlotViewBuilder::DescribeAll(const Config::Settings &a_settings)
	{
		Core::PerSlot<Core::SlotView> views{};
		auto *player = RE::PlayerCharacter::GetSingleton();
		if (!player)
		{
			return views;
		}
		const SlotViewBuilder describer{*player, a_settings};
		const auto slots = SlotStore::Get().Snapshot();
		for (std::size_t i = 0; i < views.size(); ++i)
		{
			if (!slots[i].IsEmpty())
			{
				views[i] = describer.Describe(slots[i]);
			}
		}
		return views;
	}

	SlotViewBuilder::SlotViewBuilder(RE::PlayerCharacter &a_player, const Config::Settings &a_settings) : _player(a_player),
																										  _inventory(a_player),
																										  _settings(a_settings)
	{
	}

	Core::SlotView SlotViewBuilder::Describe(const AssignedItem &a_slot) const
	{
		auto *form = RE::TESForm::LookupByID(a_slot.formID);
		const auto kind = GetItemKind(form);
		Core::SlotView view{a_slot.name, Translation::Translate("Missing"), Core::SlotState::kMissing};
		if (kind != ItemKind::kNone)
		{
			view = DescribeForm(*form, a_slot.name, kind);
			AddIcon(view, *form);
		}
		view.formID = a_slot.formID;
		return view;
	}

	Core::SlotView SlotViewBuilder::DescribeForm(RE::TESForm &a_form, const std::string &a_name, ItemKind a_kind) const
	{
		std::int32_t count = 0;
		RE::ExtraDataList *extraList = nullptr;
		if (a_kind == ItemKind::kSpell || a_kind == ItemKind::kPower || a_kind == ItemKind::kShout)
		{
			if (!_inventory.Knows(a_form))
			{
				return Missing(a_name, Translation::Translate("Not known"));
			}
		}
		else
		{
			const auto copy = _inventory.FindCarriedCopy(*a_form.As<RE::TESBoundObject>(), a_name);
			if (!copy)
			{
				return Missing(a_name, Translation::Translate("None left"));
			}
			extraList = copy->extraList;
			if (a_kind == ItemKind::kConsumable || a_kind == ItemKind::kAmmo || a_kind == ItemKind::kScroll || a_kind == ItemKind::kMisc)
			{
				count = copy->total;
			}
		}
		const auto state = _inventory.IsEquipped(a_form, a_name) ? Core::SlotState::kEquipped : Core::SlotState::kReady;
		Core::SlotView view{count > 1 ? std::format("{} ({})", a_name, count) : a_name, {}, state};
		if (state == Core::SlotState::kEquipped)
		{
			view.heldIn = _inventory.HandsHolding(a_form, a_name);
		}
		if (_settings.showItemDescription)
		{
			view.description = DescribeItem(_player, a_form, view.title, a_kind, extraList);
		}
		return view;
	}

	Core::SlotView SlotViewBuilder::Missing(const std::string &a_name, std::string a_status) const
	{
		Core::SlotView view{a_name, std::move(a_status), Core::SlotState::kMissing};
		if (_settings.showItemDescription)
		{
			view.description = DescribeMissingItem(view.title, view.status);
		}
		return view;
	}

	void SlotViewBuilder::AddIcon(Core::SlotView &a_view, const RE::TESForm &a_form) const
	{
		const auto skyui = Icons::SkyUIIconFor(a_form);
		a_view.iconLabel = skyui.label;
		a_view.iconColor = skyui.color;
		if (!_settings.useI4)
		{
			return;
		}
		const bool equipped = a_view.state == Core::SlotState::kEquipped;
		auto look = Integration::I4Icons::Get().Evaluate(a_form, equipped, {{}, a_view.iconLabel, a_view.iconColor});
		a_view.iconSource = std::move(look.iconSource);
		a_view.iconLabel = std::move(look.iconLabel);
		a_view.iconColor = look.iconColor;
	}
}
