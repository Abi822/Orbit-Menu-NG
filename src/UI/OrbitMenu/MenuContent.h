#pragma once

#include "Config/Settings.h"
#include "Core/SlotView.h"
#include "Core/Slots.h"
#include "Input/KeyBinds.h"
#include "UI/Graphics/TextMeasure.h"
#include "UI/OrbitMenu/IconCache.h"
#include "UI/OrbitMenu/ItemDescriptionLayout.h"
#include "UI/OrbitMenu/MenuPalette.h"
#include "UI/OrbitMenu/MenuState.h"
#include "UI/Skins/Skin.h"

namespace RMS::UI::OrbitMenu
{
	struct TextLine
	{
		std::string text;
		float widthPerPixel = 0.0f;

		[[nodiscard]] float Width(float a_size) const noexcept { return widthPerPixel * a_size; }
	};

	struct SlotText
	{
		std::vector<TextLine> title;
		TextLine status;
		TextLine key;
		TextLine hand;
	};

	class MenuContent
	{
	public:
		static constexpr float kStatusRatio = 13.0f / 16.0f;
		static constexpr float kTitleWidthRatio = 170.0f / 16.0f;
		static constexpr float kTitleLineSpacing = 1.15f;

		void Refresh(ImFont &a_font, const Config::Settings &a_settings, bool a_settingsChanged, const Skins::Skin &a_skin, const MenuInput &a_input);

		[[nodiscard]] const Core::SlotView &View(int a_slot) const { return _views[static_cast<std::size_t>(a_slot)]; }
		[[nodiscard]] const SlotText &Text(int a_slot) const { return _text[static_cast<std::size_t>(a_slot)]; }
		[[nodiscard]] const ItemDescriptionLayout &Description(int a_slot) const { return _descriptions[static_cast<std::size_t>(a_slot)]; }
		[[nodiscard]] const Core::PerSlot<IconRequest> &IconRequests() const noexcept { return _icons; }
		[[nodiscard]] std::uint32_t IconRevision() const noexcept { return _iconRevision; }
		[[nodiscard]] const MenuPalette &Colors() const noexcept { return _colors; }
		[[nodiscard]] const TextLine &Hint() const noexcept { return _hint; }
		[[nodiscard]] const TextLine &PageHint() const noexcept { return _pageHint; }
		[[nodiscard]] const TextLine &EmptyLabel() const noexcept { return _empty; }
		[[nodiscard]] int FirstFilledSlot() const;

	private:
		struct HintKeys
		{
			std::uint32_t open = Input::kKeyBindNone;
			std::uint32_t left = Input::kKeyBindNone;
			std::uint32_t right = Input::kKeyBindNone;
			std::uint32_t swap = Input::kKeyBindNone;
			bool assign = false;
			bool releaseToEquip = false;
		};

		[[nodiscard]] bool DescribeSlots(const Config::Settings &a_settings, bool a_force);
		void LayoutSlots(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings, const Skins::Skin &a_skin);
		void LayoutKeys(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings);
		void LayoutHint(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings);

		[[nodiscard]] static TextLine Measure(const Graphics::TextMeasure &a_text, std::string a_line);
		[[nodiscard]] static std::string BuildHint(const HintKeys &a_keys);

		const ImFont *_font = nullptr;
		bool _ready = false;
		bool _gamepad = false;
		bool _assign = false;
		int _page = 0;
		std::uint32_t _seenSlotChanges = 0;
		std::uint32_t _seenEquipChanges = 0;
		std::uint32_t _skinGeneration = 0;
		std::uint32_t _iconRevision = 1;
		Core::PerSlot<Core::SlotView> _views{};
		Core::PerSlot<IconRequest> _icons{};
		Core::PerSlot<SlotText> _text{};
		Core::PerSlot<ItemDescriptionLayout> _descriptions{};
		MenuPalette _colors{};
		TextLine _hint;
		TextLine _pageHint;
		TextLine _empty;
	};
}
