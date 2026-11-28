#include "Items/Assignment.h"
#include "Game/GameState.h"
#include "Input/InputManager.h"
#include "Items/ItemKind.h"
#include "Translation/Translation.h"

namespace Orbit::Items
{
	namespace
	{
		std::optional<AssignedItem> ReadHighlightedRow(RE::IMenu &a_menu)
		{
			if (!a_menu.uiMovie)
			{
				return std::nullopt;
			}

			RE::GFxValue entry;
			RE::GFxValue formID;
			if (!a_menu.uiMovie->GetVariable(&entry, "_root.Menu_mc.inventoryLists.itemList.selectedEntry") || !entry.IsObject() ||
				!entry.GetMember("formId", &formID) || !formID.IsNumber())
			{
				return std::nullopt;
			}
			AssignedItem row{static_cast<RE::FormID>(formID.GetNumber()), {}};
			if (RE::GFxValue text; entry.GetMember("text", &text) && text.IsString() && text.GetString())
			{
				row.name = text.GetString();
			}
			return row;
		}
	}

	void Assignment::OnHighlightChanged(std::string_view a_menuName)
	{
		std::optional<AssignedItem> row;
		auto *ui = RE::UI::GetSingleton();
		if (const auto menu = ui ? ui->GetMenu(a_menuName) : nullptr)
		{
			row = ReadHighlightedRow(*menu);
		}
		std::scoped_lock lock{_lock};
		_highlighted = std::move(row);
	}

	bool Assignment::AssignHighlighted(int a_slot)
	{
		if (!Game::IsInItemMenu())
		{
			return false;
		}

		const int number = a_slot + 1;
		std::optional<AssignedItem> row;
		{
			std::scoped_lock lock{_lock};
			row = _highlighted;
		}
		if (!row)
		{
			logger::info("Assignment: Slot {}: no highlighted row", number);
			return true;
		}

		auto *form = RE::TESForm::LookupByID(row->formID);
		if (GetItemKind(form) == ItemKind::kNone)
		{
			logger::info("Assignment: Slot {}: '{}' ({:08X}) can't go on the Orbit Menu", number, row->name, row->formID);
			Game::ShowHudMessage(Translation::Translate("{} can't go on the Orbit Menu", row->name));
			return true;
		}

		const AssignedItem chosen{form->GetFormID(), row->name.empty() ? form->GetName() : row->name};

		const bool fromSlotKey = !Input::InputManager::Get().IsOrbitMenuOpen();

		auto &store = SlotStore::Get();
		if (store.Slot(a_slot) == chosen)
		{
			store.Clear(a_slot);
			logger::info("Assignment: Slot {} cleared: ({})", number, chosen.name);
			if (fromSlotKey)
			{
				Game::ShowHudMessage(Translation::Translate("{} removed from slot {}", chosen.name, number));
			}
			return true;
		}

		if (const auto previous = store.Find(chosen); previous && *previous != a_slot)
		{
			store.Clear(*previous);
		}
		store.Assign(a_slot, chosen);
		logger::info("Assignment: Slot {} assigned: {} ({:08X})", number, chosen.name, chosen.formID);
		if (fromSlotKey)
		{
			Game::ShowHudMessage(Translation::Translate("{} assigned to slot {}", chosen.name, number));
		}
		return true;
	}
}
