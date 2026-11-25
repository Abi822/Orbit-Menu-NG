#pragma once

#include "Core/SlotView.h"
#include "Items/ItemKind.h"

namespace Orbit::Items
{
	Core::ItemDescription DescribeItem(RE::PlayerCharacter &a_player, RE::TESForm &a_form, std::string_view a_title, ItemKind a_kind, RE::ExtraDataList *a_extraList);
	Core::ItemDescription DescribeMissingItem(std::string_view a_title, std::string_view a_status);
}
