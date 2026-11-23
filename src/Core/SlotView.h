#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Orbit::Core
{
	enum class SlotState
	{
		kEmpty,
		kReady,
		kEquipped,
		kMissing
	};

	enum class HeldIn
	{
		kNone,
		kLeft,
		kRight,
		kBoth
	};

	struct ItemStat
	{
		std::string label;
		std::string value;
		std::string suffix;

		bool operator==(const ItemStat &) const = default;
	};

	struct ItemDescription
	{
		std::string name;
		std::string text;
		std::vector<ItemStat> footer;

		bool operator==(const ItemDescription &) const = default;
	};

	struct SlotView
	{
		std::string title;
		std::string status;
		SlotState state = SlotState::kEmpty;
		HeldIn heldIn = HeldIn::kNone;
		std::string iconSource;
		std::string iconLabel;
		std::optional<std::uint32_t> iconColor;
		ItemDescription description;
		RE::FormID formID = 0;

		bool operator==(const SlotView &) const = default;
	};
}
