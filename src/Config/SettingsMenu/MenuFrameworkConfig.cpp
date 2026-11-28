#include "Config/SettingsMenu/MenuFrameworkConfig.h"
#include "Config/SettingsMenu/MenuFrameworkBase.h"
#include <SKSEMenuFramework.h>
#include "Config/Settings.h"
#include "Input/InputManager.h"
#include "Input/KeyBinds.h"
#include "Integration/I4/Icons.h"
#include "Items/Pages.h"
#include "Items/SlotStore.h"
#include "Translation/Translation.h"
#include "UI/OrbitMenu/OrbitRenderer.h"

namespace Orbit::Config::SettingsMenu
{
	namespace
	{
		// Used by MenuFrameworkBase::BoundKey, which returns where a binding's key is stored in the settings
		constexpr std::array kSlotTextPositions{"Top left", "Top middle", "Top right", "Bottom left", "Bottom middle", "Bottom right", "Next to name"};
	}

	class MenuFrameworkConfig::Page : public MenuFrameworkBase
	{
	public:
		Page(Input::InputManager &a_input, Settings &a_settings) : _input(a_input), _settings(a_settings), _capturing(a_input.CapturingBinding())
		{
		}

		void DrawSettings()
		{
			using namespace ImGuiMCP;

			SeparatorText("Orbit Menu");
			KeyRow("Hold to open:", Input::kBindingActivation);
			KeyRow("Activate Orbit Menu with:", Input::kBindingActivationModifier);
			if (_settings.activationKey == Input::kKeyBindNone && _settings.gamepadActivationKey == Input::kKeyBindNone)
			{
				Text("%s", Translation::Translate("No key is bound, Orbit Menu can not be opened!"));
			}
			Check("Toggle Orbit Menu##orbit", _settings.toggleOrbitMenu);
			Check("Use Right Stick on Controller##orbit", _settings.useRightStick);
			Slider("Mouse Sensitivity##orbit", _settings.mouseSensitivity, Settings::kMinSensitivity, Settings::kMaxSensitivity, "%.2f",
				   "The lower it is, the more you have to move the mouse sideways to swing over to the next slot.");
			Slider("Mouse DPI##orbit", _settings.mouseDPI, Settings::kMinMouseDPI, Settings::kMaxMouseDPI,
				   "Mouse DPI, also known as deadzone, is how far the mouse has to move from the middle before the first slot is highlighted.\n"
				   "Makes sure a small bump when you press the key doesn't highlight anything.\n"
				   "The lower it is, the more you have to move the mouse, so slots are less likely to be highlighted by accident.");

			NewLine();
			SeparatorText(Translation::Translate("Slots"));
			TextDisabled("%s", Translation::Translate("Hover over an item in the Inventory menu or a spell in the Magic menu, then press a slot key to assign it."));
			const float em = GetFontSize();
			const auto slots = Items::SlotStore::Get().Snapshot();
			for (int i = 0; i < Core::kSlotCount; ++i)
			{
				const auto &slot = slots[static_cast<std::size_t>(i)];
				Text("%s %d", Translation::Translate("Slot"), i + 1);
				SameLine(em * 4.0f);
				if (slot.IsEmpty())
				{
					TextDisabled("%s", Translation::Translate("Empty"));
				}
				else
				{
					Text("%s", slot.name.c_str());
				}
				SameLine(em * 18.0f);
				KeyButtons(i);
				if (!slot.IsEmpty())
				{
					SameLine();
					if (Button(std::format("{}##orbit_clear_{}", Translation::Translate("Clear"), i).c_str()))
					{
						Items::SlotStore::Get().Clear(i);
					}
				}
			}

			NewLine();
			SeparatorText(Translation::Translate("Slot Controls"));
			Check("Release to equip##orbit", _settings.releaseToEquip, "Letting go of the Orbit Menu key will equip the last highlighted slot.\n"
																	   "It's off by default because just opening the menu to look could accidentally equip or use something.");
			KeyRow("Equip in left hand:", Input::kBindingLeftHand);
			KeyRow("Equip in right hand:", Input::kBindingRightHand);
			KeyRow("Swap hands:", Input::kBindingSwapHands);

			NewLine();
			SeparatorText("I4 (Inventory Interface Information Injector)");
			if (Integration::I4::Icons::IsInstalled())
			{
				Check("Use I4 icons##orbit", _settings.useI4);
			}
			else
			{
				bool unavailable = false;
				BeginDisabled();
				Checkbox(Label("Use I4 icons##orbit"), &unavailable);
				EndDisabled();
				if (IsItemHovered(ImGuiMCP::ImGuiHoveredFlags_AllowWhenDisabled))
				{
					SetTooltip("%s", Translation::Translate("Inventory Interface Information Injector not installed!"));
				}
			}

			SeparatorText(Translation::Translate("Messages"));
			Check("Show HUD messages##orbit", _settings.showHudMessages);

			SeparatorText("");
			Spacing();
			if (Button(Label("Reset Settings To Defaults##orbit")))
			{
				_input.CancelKeyCapture();
				Settings defaults = SettingsStore::Get().Defaults();
				defaults.CopyDisplay(_settings);
				defaults.CopyPages(_settings);
				defaults.CopySlowMotion(_settings);
				_settings = defaults;
				RequestSave();
			}
		}

		void DrawDisplay()
		{
			using namespace ImGuiMCP;
			auto &renderer = UI::OrbitMenu::OrbitRenderer::Get();

			const bool preview = renderer.IsPreviewing();
			if (RadioButton(Label("Show Orbit Menu in Background##orbit"), preview))
			{
				renderer.SetPreview(!preview);
			}

			SeparatorText(Translation::Translate("Display"));
			Slider("Menu Size##orbit", _settings.orbitMenuScale, Settings::kMinOrbitMenuScale, Settings::kMaxOrbitMenuScale, "%.2f");
			Text("%s", Translation::Translate("Position"));
			Number("X##orbit_position_x", _settings.positionX, Settings::kMinPosition, Settings::kMaxPosition, 10);
			Number("Y##orbit_position_y", _settings.positionY, Settings::kMinPosition, Settings::kMaxPosition, 10);
			Number("Curve##orbit_curve", _settings.curve, Settings::kMinCurve, Settings::kMaxCurve, 5);
			Color("Background Colour##orbit", _settings.backgroundColor);
			Color("Highlight Colour##orbit", _settings.highlightColor);
			Slider("Animation Speed##orbit", _settings.animationSpeed, Settings::kMinAnimationSpeed, Settings::kMaxAnimationSpeed, "%.2f",
				   "0 to turn off animations.");
			Check("Show Background##orbit", _settings.showBackground);
			Check("Show the Controls Hint##orbit", _settings.showControlsHint);

			NewLine();
			SeparatorText(Translation::Translate("Slot"));
			Check("Show Slot Key##orbit", _settings.showSlotKeys);
			Check("Show Slot Key if a controller is detected##orbit", _settings.showSlotKeysOnController);
			Check("Show Hand##orbit", _settings.showHand);
			Check("Show Slot Empty##orbit", _settings.showSlotEmpty);
			Check("Show Slot Glow##orbit", _settings.showSlotGlow);
			Check("Show Slot Background##orbit", _settings.showSlotBackground);
			Color("Slot Colour##orbit", _settings.slotColor);
			Color("Slot Key Background Colour##orbit", _settings.slotKeyBackgroundColor);
			Color("Slot Glow Colour##orbit", _settings.slotGlowColor);
			Number("Slot Size##orbit", _settings.slotSize, Settings::kMinSlotSize, Settings::kMaxSlotSize, 2);
			Number("Slot Icon Size##orbit", _settings.slotIconSize, Settings::kMinSlotIconSize, Settings::kMaxSlotIconSize, 2);
			Text("%s", Translation::Translate("Slot Icon Position"));
			Number("X##orbit_slot_icon_x", _settings.slotIconX, Settings::kMinSlotPosition, Settings::kMaxSlotPosition);
			Number("Y##orbit_slot_icon_y", _settings.slotIconY, Settings::kMinSlotPosition, Settings::kMaxSlotPosition);
			Text("%s", Translation::Translate("Slot Name Position"));
			Number("X##orbit_slot_name_x", _settings.slotNameX, Settings::kMinSlotPosition, Settings::kMaxSlotPosition);
			Number("Y##orbit_slot_name_y", _settings.slotNameY, Settings::kMinSlotPosition, Settings::kMaxSlotPosition);
			Number("Slot Distance##orbit", _settings.slotDistance, Settings::kMinSlotDistance, Settings::kMaxSlotDistance, 5);
			Choice("Slot Key Position##orbit", _settings.slotKeyPosition, Settings::kLastSlotKeyPosition);
			Choice("Hand Position##orbit", _settings.handPosition, Settings::kLastHandPosition);

			NewLine();
			SeparatorText(Translation::Translate("Item Description"));
			Check("Show Last Equipped Item##orbit", _settings.showLastEquippedItem);
			Check("Show Item Description##orbit", _settings.showItemDescription);
			Check("Show Item Icon Description##orbit", _settings.showItemIconDescription);
			Check("Show Item Description Background##orbit", _settings.showItemDescriptionBackground);
			Color("Item Description Background Colour##orbit", _settings.itemDescriptionBackgroundColor);
			Number("Item Description Background Size##orbit", _settings.itemDescriptionBackgroundSize, Settings::kMinItemDescriptionBackgroundSize, Settings::kMaxItemDescriptionBackgroundSize, 2);
			Number("Item Icon Description Size##orbit", _settings.itemIconDescriptionSize, Settings::kMinItemIconDescriptionSize, Settings::kMaxItemIconDescriptionSize);
			Check("Static Item Description##orbit", _settings.staticItemDescription);
			if (_settings.staticItemDescription)
			{
				Text("%s", Translation::Translate("Icon"));
				Number("X##orbit_icon_x", _settings.itemIconDescriptionX, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Number("Y##orbit_icon_y", _settings.itemIconDescriptionY, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Text("%s", Translation::Translate("Title"));
				Number("X##orbit_title_x", _settings.itemDescriptionTitleX, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Number("Y##orbit_title_y", _settings.itemDescriptionTitleY, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Number("Width##orbit_title_width", _settings.itemDescriptionTitleWidth, Settings::kMinItemDescriptionWidth, Settings::kMaxItemDescriptionWidth, 2);
				Text("%s", Translation::Translate("Description"));
				Number("X##orbit_description_x", _settings.itemDescriptionTextX, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Number("Y##orbit_description_y", _settings.itemDescriptionTextY, Settings::kMinItemDescriptionPosition, Settings::kMaxItemDescriptionPosition);
				Number("Width##orbit_description_width", _settings.itemDescriptionWidth, Settings::kMinItemDescriptionWidth, Settings::kMaxItemDescriptionWidth, 2);
				Slider("Line Spacing##orbit_description_line_spacing", _settings.itemDescriptionLineSpacing, Settings::kMinItemDescriptionLineSpacing, Settings::kMaxItemDescriptionLineSpacing, "%.2f");
				Check("Follow Title##orbit_description_follow_title", _settings.itemDescriptionTextFollowsTitle, "When the title is too wide to fit on a single line, the description moves down so they don't overlap.");
			}

			NewLine();
			SeparatorText(Translation::Translate("Pointer"));
			Check("Show Pointer##orbit", _settings.showPointer);
			Color("Pointer Colour##orbit", _settings.pointerColor);
			Number("Pointer Size##orbit", _settings.pointerSize, Settings::kMinPointerSize, Settings::kMaxPointerSize);
			Number("Pointer Distance##orbit", _settings.pointerDistance, Settings::kMinPointerDistance, Settings::kMaxPointerDistance);
			Slider("Pointer Rotation##orbit", _settings.pointerRotation, 0, Settings::kMaxPointerRotation);

			NewLine();
			SeparatorText(Translation::Translate("Controls Hint"));
			Text("%s", Translation::Translate("Controls Hint Position"));
			Number("X##orbit_controls_hint_x", _settings.controlsHintX, Settings::kMinPosition, Settings::kMaxPosition, 5);
			Number("Y##orbit_controls_hint_y", _settings.controlsHintY, Settings::kMinPosition, Settings::kMaxPosition, 5);

			NewLine();
			SeparatorText(Translation::Translate("Text"));
			Text("%s", Translation::Translate("Slot Name"));
			Number("##orbit_slot_name_size", _settings.slotNameTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_slot_name_colour", _settings.slotNameColor);
			Spacing();
			Text("%s", Translation::Translate("Slot Empty"));
			Number("##orbit_slot_empty_size", _settings.slotEmptyTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_slot_empty_colour", _settings.slotEmptyColor);
			Spacing();
			Text("%s", Translation::Translate("Slot Key"));
			Number("##orbit_slot_key_size", _settings.slotKeyTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_slot_key_colour", _settings.slotKeyColor);
			Spacing();
			Text("%s", Translation::Translate("Hand"));
			Number("##orbit_hand_size", _settings.handTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_hand_colour", _settings.handColor);
			Spacing();
			Text("%s", Translation::Translate("Item Description"));
			Number("Title##orbit_item_title_size", _settings.itemDescriptionTitleTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Number("Description##orbit_item_description_size", _settings.itemDescriptionTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_item_description_colour", _settings.itemDescriptionColor);
			Spacing();
			Text("%s", Translation::Translate("Controls Hint"));
			Number("##orbit_controls_hint_size", _settings.controlsHintTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##orbit_controls_hint_colour", _settings.controlsHintColor);

			Spacing();
			SeparatorText("");
			if (Button(Label("Reset Display To Defaults##orbit")))
			{
				_settings.CopyDisplay(SettingsStore::Get().Defaults());
				RequestSave();
			}
		}

		void DrawPages()
		{
			using namespace ImGuiMCP;
			const bool enabled = _settings.enablePages;
			const auto count = _settings.numberOfPages;

			SeparatorText(Translation::Translate("Pages"));
			Check("Enable Pages##orbit", _settings.enablePages);
			Number("Number of Pages##orbit", _settings.numberOfPages, Settings::kMinNumberOfPages, Settings::kMaxNumberOfPages);
			KeyRow("Next Page:", Input::kBindingNextPage);
			KeyRow("Previous Page:", Input::kBindingPreviousPage);

			NewLine();
			SeparatorText(Translation::Translate("Page Indicator"));
			Check("Show Page Indicator##orbit", _settings.showPageIndicator);
			Text("%s", Translation::Translate("Position"));
			Number("X##orbit_page_indicator_x", _settings.pageIndicatorX, Settings::kMinPosition, Settings::kMaxPosition, 5);
			Number("Y##orbit_page_indicator_y", _settings.pageIndicatorY, Settings::kMinPosition, Settings::kMaxPosition, 5);
			Slider("Rotation##orbit_page_indicator_rotation", _settings.pageIndicatorRotation, 0, Settings::kMaxPageIndicatorRotation);
			Number("Columns##orbit_page_indicator_columns", _settings.pageIndicatorColumns, 1, Settings::kMaxNumberOfPages);
			Number("Spacing##orbit_page_indicator_spacing", _settings.pageIndicatorSpacing, 0, Settings::kMaxPageIndicatorSpacing);
			Number("Size##orbit_page_indicator_size", _settings.pageIndicatorSize, Settings::kMinPageIndicatorSize, Settings::kMaxPageIndicatorSize);
			Color("Active Colour##orbit_page_indicator_active_colour", _settings.pageIndicatorActiveColor);
			Color("Inactive Colour##orbit_page_indicator_inactive_colour", _settings.pageIndicatorInactiveColor);

			Spacing();
			SeparatorText("");
			if (Button(Label("Clear All Pages##orbit")))
			{
				Items::Pages::Get().ClearExtraPages();
			}
			Tooltip("Only clears from page 2 and beyond. Page 1 is kept.");
			Spacing();
			if (Button(Label("Reset Pages To Defaults##orbit")))
			{
				_input.CancelKeyCapture();
				_settings.CopyPages(SettingsStore::Get().Defaults());
				RequestSave();
			}

			if (_settings.enablePages != enabled || _settings.numberOfPages != count)
			{
				Items::Pages::Get().Fit(_settings);
			}
		}

		void DrawSlowMotion()
		{
			using namespace ImGuiMCP;
			SeparatorText(Translation::Translate("Slow Motion"));
			Check("Enable Slow Motion##orbit", _settings.enableSlowMotion);
			Slider("Time Speed##orbit", _settings.slowMotionTimeSpeed, Settings::kMinSlowMotionTimeSpeed, Settings::kMaxSlowMotionTimeSpeed, "%.2f",
				   "1 is off. I do not recommend 0.30 or lower, it can mess up physics and mods like HDT-SMP.");
			Slider("Fade In##orbit", _settings.slowMotionFadeIn, 0.0f, Settings::kMaxSlowMotionFade, "%.2f s", "0 is off");
			Slider("Fade Out##orbit", _settings.slowMotionFadeOut, 0.0f, Settings::kMaxSlowMotionFade, "%.2f s", "0 is off");

			Spacing();
			SeparatorText("");
			if (Button(Label("Reset Slow Motion To Defaults##orbit")))
			{
				_settings.CopySlowMotion(SettingsStore::Get().Defaults());
				RequestSave();
			}
		}

	private:
		void Choice(const char *a_label, std::int32_t &a_value, std::int32_t a_last)
		{
			std::array<const char *, kSlotTextPositions.size()> items{};
			std::ranges::transform(kSlotTextPositions, items.begin(), [](const char *a_item)
								   { return Translation::Translate(a_item); });
			_save |= ImGuiMCP::Combo(Label(a_label), &a_value, items.data(), a_last + 1);
		}

		void KeyRow(const char *a_label, int a_binding)
		{
			ImGuiMCP::Text("%s", Translation::Translate(a_label));
			ImGuiMCP::SameLine();
			KeyButtons(a_binding);
		}

		void KeyButtons(int a_binding)
		{
			bool first = true;
			for (const bool gamepad : {false, true})
			{
				if (auto *key = BoundKey(_settings, {a_binding, gamepad}))
				{
					if (!first)
					{
						ImGuiMCP::SameLine();
					}
					KeyButton({a_binding, gamepad}, *key);
					first = false;
				}
			}
		}

		void KeyButton(Input::KeyBinding a_binding, std::uint32_t &a_key)
		{
			using namespace ImGuiMCP;
			if (_capturing == a_binding)
			{
				Text("%s", Translation::Translate(a_binding.gamepad ? "Press a controller button..." : "Press a keyboard or mouse button..."));
				return;
			}

			const int pad = a_binding.gamepad ? 1 : 0;
			const auto name = Input::KeyName(a_key);
			const auto label = a_binding.gamepad ? Translation::Translate("Controller: {}", name) : name;
			if (Button(std::format("{}##orbit_bind_{}_{}", label, a_binding.id, pad).c_str()))
			{
				_input.BeginKeyCapture(a_binding);
			}
			if (a_binding.id == Input::kBindingActivationModifier)
			{
				Tooltip("Click, then press the new controller activation button.");
			}
			else
			{
				Tooltip(a_binding.gamepad ? "Click, then press the new controller button." : "Click, then press the new keyboard or mouse button.");
			}
			if (a_key != Input::kKeyBindNone)
			{
				SameLine();
				if (Button(std::format("{}##orbit_unbind_{}_{}", Translation::Translate("Unbind"), a_binding.id, pad).c_str()))
				{
					a_key = Input::kKeyBindNone;
					_save = true;
				}
			}
		}

		Input::InputManager &_input;
		Settings &_settings;
		std::optional<Input::KeyBinding> _capturing;
	};

	MenuFrameworkConfig &MenuFrameworkConfig::Get()
	{
		static MenuFrameworkConfig instance;
		return instance;
	}

	void __stdcall MenuFrameworkConfig::RenderSettings()
	{
		Get().Render(&Page::DrawSettings);
	}

	void __stdcall MenuFrameworkConfig::RenderDisplay()
	{
		Get().Render(&Page::DrawDisplay);
	}

	void __stdcall MenuFrameworkConfig::RenderPages()
	{
		Get().Render(&Page::DrawPages);
	}

	void __stdcall MenuFrameworkConfig::RenderSlowMotion()
	{
		Get().Render(&Page::DrawSlowMotion);
	}

	void MenuFrameworkConfig::OnMenuClosed()
	{
		auto &self = Get();
		UI::OrbitMenu::OrbitRenderer::Get().SetPreview(false);
		Input::InputManager::Get().CancelKeyCapture();
		if (self._unsavedChanges.load())
		{
			self.SaveNow();
		}
	}

	void MenuFrameworkConfig::Render(void (Page::*a_draw)())
	{
		auto &store = SettingsStore::Get();
		auto &input = Input::InputManager::Get();
		Settings settings = store.Snapshot();
		const Settings before = settings;
		Page page{input, settings};

		if (const auto captured = input.TakeCapturedKey())
		{
			if (auto *key = MenuFrameworkBase::BoundKey(settings, {captured->binding, Input::IsGamepadBind(captured->code)}))
			{
				*key = captured->code;
				page.RequestSave();
			}
		}

		(page.*a_draw)();

		if (const auto status = Status(); !status.empty())
		{
			ImGuiMCP::TextDisabled("%s", status.c_str());
		}

		if (settings != before)
		{
			store.Set(settings);
			_unsavedChanges.store(true);
		}
		if (page.SaveRequested() && _unsavedChanges.load())
		{
			SaveNow();
		}
	}

	void MenuFrameworkConfig::SaveNow()
	{
		if (SettingsStore::Get().Save())
		{
			_unsavedChanges.store(false);
			SetStatus({});
		}
		else
		{
			SetStatus(Translation::Translate("Could not write the settings file - see OrbitMenu.log."));
		}
	}

	void MenuFrameworkConfig::SetStatus(std::string a_text)
	{
		std::scoped_lock lock{_statusLock};
		_status = std::move(a_text);
	}

	std::string MenuFrameworkConfig::Status() const
	{
		std::scoped_lock lock{_statusLock};
		return _status;
	}
}
