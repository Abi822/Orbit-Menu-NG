#include "UI/OrbitMenu/MenuContent.h"
#include "Config/Translation.h"
#include "Items/EquipController.h"
#include "Items/Pages.h"
#include "Items/SlotStore.h"
#include "Items/SlotViewBuilder.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr std::size_t kTitleLines = 2;

		[[nodiscard]] IconRequest IconFor(const Core::SlotView &a_view, const Skins::Skin &a_skin)
		{
			IconRequest request;
			if (a_view.state == Core::SlotState::kEmpty)
			{
				return request;
			}
			if (const auto *form = RE::TESForm::LookupByID(a_view.formID))
			{
				request.skinFile = a_skin.IconFile(*form);
			}
			if (request.skinFile.empty())
			{
				request.source = a_view.iconSource;
				request.label = a_view.iconLabel;
				request.tint = a_view.iconColor;
			}
			return request;
		}

		[[nodiscard]] std::string_view HandLabel(Core::HeldIn a_heldIn) noexcept
		{
			switch (a_heldIn)
			{
			case Core::HeldIn::kLeft:
				return Config::Translate("L");
			case Core::HeldIn::kRight:
				return Config::Translate("R");
			case Core::HeldIn::kBoth:
				return Config::Translate("LR");
			default:
				return {};
			}
		}
	}

	void MenuContent::Refresh(ImFont &a_font, const Config::Settings &a_settings, bool a_settingsChanged, const Skins::Skin &a_skin, const MenuInput &a_input)
	{
		const bool settingsChanged = a_settingsChanged || !_ready;
		const bool skinChanged = a_skin.Generation() != _skinGeneration;
		const bool modeChanged = !_ready || a_input.gamepad != _gamepad || a_input.assign != _assign;
		const bool fontChanged = &a_font != _font;
		_font = &a_font;
		if (settingsChanged)
		{
			_colors = MenuPalette::Make(a_settings);
		}

		const Graphics::TextMeasure text{a_font};
		const bool viewsChanged = a_input.open && DescribeSlots(a_settings, a_input.justOpened || settingsChanged);
		if (viewsChanged || settingsChanged || skinChanged || fontChanged)
		{
			_skinGeneration = a_skin.Generation();
			LayoutSlots(text, a_settings, a_skin);
		}
		if (settingsChanged || modeChanged || fontChanged)
		{
			_gamepad = a_input.gamepad;
			_assign = a_input.assign;
			LayoutKeys(text, a_settings);
			LayoutHint(text, a_settings);
		}
		const int page = a_settings.enablePages ? Items::Pages::Get().Current() : 0;
		if (settingsChanged || modeChanged || fontChanged || page != _page)
		{
			_page = page;
			_pageHint = a_settings.enablePages ? Measure(text, Items::Pages::Get().Hint(a_settings, _gamepad)) : TextLine{};
		}
		_ready = true;
	}

	int MenuContent::FirstFilledSlot() const
	{
		const auto filled = std::ranges::find_if(_views, [](const Core::SlotView &a_view)
												 { return a_view.state != Core::SlotState::kEmpty; });
		return filled == _views.end() ? -1 : static_cast<int>(filled - _views.begin());
	}

	bool MenuContent::DescribeSlots(const Config::Settings &a_settings, bool a_force)
	{
		const auto slotChanges = Items::SlotStore::Get().ChangeCount();
		const auto equipChanges = Items::EquipController::Get().ChangeCount();
		if (!a_force && slotChanges == _seenSlotChanges && equipChanges == _seenEquipChanges)
		{
			return false;
		}
		_seenSlotChanges = slotChanges;
		_seenEquipChanges = equipChanges;

		auto views = Items::SlotViewBuilder::DescribeAll(a_settings);
		if (views == _views)
		{
			return false;
		}
		_views = std::move(views);
		return true;
	}

	void MenuContent::LayoutSlots(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings, const Skins::Skin &a_skin)
	{
		const float nameSize = static_cast<float>(a_settings.slotNameTextSize);
		_empty = Measure(a_text, Config::Translate("Empty"));

		auto icons = _icons;
		for (std::size_t i = 0; i < _views.size(); ++i)
		{
			const auto &view = _views[i];
			icons[i] = IconFor(view, a_skin);

			auto &text = _text[i];
			text.title.clear();
			for (auto &line : a_text.Wrap(nameSize, view.title, nameSize * kTitleWidthRatio, kTitleLines))
			{
				text.title.push_back(Measure(a_text, std::move(line)));
			}
			text.status = view.state == Core::SlotState::kMissing ? Measure(a_text, view.status) : TextLine{};
			const auto hand = a_settings.showHand ? HandLabel(view.heldIn) : std::string_view{};
			text.hand = hand.empty() ? TextLine{} : Measure(a_text, std::string{hand});

			_descriptions[i] = a_settings.showItemDescription
								   ? ItemDescriptionLayout::Compute(a_text, view.description, static_cast<float>(a_settings.itemDescriptionTextSize),
																	a_settings.showItemIconDescription && !icons[i].Empty() ? static_cast<float>(a_settings.itemIconDescriptionSize) : 0.0f,
																	static_cast<float>(a_settings.centerBackgroundSize) * 0.5f)
								   : ItemDescriptionLayout{};
		}
		if (icons != _icons)
		{
			_icons = std::move(icons);
			++_iconRevision;
		}
	}

	void MenuContent::LayoutKeys(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings)
	{
		const bool show = a_settings.showSlotKeys && (!_gamepad || a_settings.showSlotKeysOnController);
		for (std::size_t i = 0; i < _text.size(); ++i)
		{
			const auto key = a_settings.slotKeys[i];
			if (!show || key == Input::kKeyBindNone)
			{
				_text[i].key = {};
				continue;
			}
			auto label = Input::KeyName(key);
			if (label.size() > 3)
			{
				label = std::to_string(i + 1);
			}
			_text[i].key = Measure(a_text, std::move(label));
		}
	}

	void MenuContent::LayoutHint(const Graphics::TextMeasure &a_text, const Config::Settings &a_settings)
	{
		const auto &s = a_settings;
		HintKeys keys = _gamepad ? HintKeys{s.gamepadActivationKey, s.gamepadLeftHandKey, s.gamepadRightHandKey, s.gamepadSwapHandsKey}
								 : HintKeys{s.activationKey, s.leftHandKey, s.rightHandKey, s.swapHandsKey};
		keys.assign = _assign;
		keys.releaseToEquip = s.releaseToEquip;
		keys.toggle = s.toggleOrbitMenu;
		_hint = Measure(a_text, BuildHint(keys));
	}

	TextLine MenuContent::Measure(const Graphics::TextMeasure &a_text, std::string a_line)
	{
		constexpr float kReferenceSize = 32.0f;
		const float width = a_text.Width(kReferenceSize, a_line);
		return {std::move(a_line), width / kReferenceSize};
	}

	std::string MenuContent::BuildHint(const HintKeys &a_keys)
	{
		using Config::Translate;
		using Input::KeyName;
		const bool equip = a_keys.releaseToEquip && !a_keys.assign;
		std::string hint = a_keys.toggle ? Translate(equip ? "Press {} to equip" : "Press {} to close", KeyName(a_keys.open))
										 : Translate(equip ? "Release {} to equip" : "Release {} to close", KeyName(a_keys.open));
		const bool left = a_keys.left != Input::kKeyBindNone;
		const bool right = a_keys.right != Input::kKeyBindNone;
		if (a_keys.assign)
		{
			if (left && right)
			{
				hint += "   |   " + Translate("Assign: {} or {}", KeyName(a_keys.left), KeyName(a_keys.right));
			}
			else if (left || right)
			{
				hint += "   |   " + Translate("Assign: {}", KeyName(left ? a_keys.left : a_keys.right));
			}
			return hint;
		}
		if (left && right)
		{
			hint += "   |   " + Translate("Equip in hand: {}, {} or both", KeyName(a_keys.left), KeyName(a_keys.right));
		}
		else if (left || right)
		{
			hint += "   |   " + Translate(left ? "Left hand: {}" : "Right hand: {}", KeyName(left ? a_keys.left : a_keys.right));
		}
		if (a_keys.swap != Input::kKeyBindNone && a_keys.swap != a_keys.open)
		{
			hint += "   |   " + Translate("Swap hands: {}", KeyName(a_keys.swap));
		}
		return hint;
	}
}
