#pragma once

#include "Config/Settings.h"
#include "Core/SlotView.h"
#include "Core/Slots.h"
#include "Input/KeyBinds.h"
#include "UI/Graphics/TextMeasure.h"
#include "UI/OrbitMenu/IconCache.h"
#include "UI/OrbitMenu/ItemDescriptionLayout.h"
#include "UI/OrbitMenu/MenuAnimation.h"
#include "UI/OrbitMenu/MenuPalette.h"
#include "UI/Skins/Skin.h"

namespace Orbit::UI::OrbitMenu
{
	struct TextLine
	{
		std::string text;
		float widthPerPixel = 0.0f;

		float Width(float a_size) const { return widthPerPixel * a_size; }
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
		static constexpr float kStatusRatio = 0.8125f;
		static constexpr float kTitleWidthRatio = 10.625f;
		static constexpr float kTitleLineSpacing = 1.15f;

		void Refresh(ImFont &a_font, const Config::Settings &a_settings, bool a_settingsChanged, const Skins::Skin &a_skin, const MenuInput &a_input);

		const Core::SlotView &View(int a_slot) const { return _views[static_cast<std::size_t>(a_slot)]; }
		const SlotText &Text(int a_slot) const { return _text[static_cast<std::size_t>(a_slot)]; }
		const ItemDescriptionLayout &Description(int a_slot) const { return _descriptions[static_cast<std::size_t>(a_slot)]; }
		const Core::PerSlot<IconRequest> &IconRequests() const { return _icons; }
		std::uint32_t IconRevision() const { return _iconRevision; }
		const MenuPalette &Colors() const { return _colors; }
		const TextLine &Hint() const { return _hint; }
		const TextLine &PageHint() const { return _pageHint; }
		const TextLine &EmptyLabel() const { return _empty; }
		int FirstFilledSlot() const;

	private:
		struct HintKeys
		{
			std::uint32_t open = Input::kKeyBindNone;
			std::uint32_t left = Input::kKeyBindNone;
			std::uint32_t right = Input::kKeyBindNone;
			std::uint32_t swap = Input::kKeyBindNone;
			bool assign = false;
			bool releaseToEquip = false;
			bool toggle = false;
		};

		bool DescribeSlots(const Config::Settings &a_settings, bool a_force);
		void LayoutSlots(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings, const Skins::Skin &a_skin);
		void LayoutKeys(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings);
		void LayoutHint(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings);

		static TextLine Measure(const Graphics::TextMeasure &a_text, std::string a_line);
		static std::string BuildHint(const HintKeys &a_keys);

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
