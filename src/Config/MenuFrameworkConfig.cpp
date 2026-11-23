#include "Config/MenuFrameworkConfig.h"
#include <SKSEMenuFramework.h>
#include "Config/Settings.h"
#include "Config/Translation.h"
#include <map>
#include "Input/InputManager.h"
#include "Input/KeyBinds.h"
#include "Integration/I4Addon.h"
#include "Items/Pages.h"
#include "Items/SlotStore.h"
#include "UI/OrbitMenu/OrbitRenderer.h"

namespace Orbit::Config
{
	namespace
	{
		constexpr std::array kBadgePositions{"Top left", "Top middle", "Top right", "Bottom left", "Bottom middle", "Bottom right", "Next to name"};

		// Holds the key for a binding.
		template <class T>
		auto *BoundKey(T &a_settings, Input::KeyBinding a_binding)
		{
			const bool pad = a_binding.gamepad;
			switch (a_binding.id)
			{
			case Input::kBindingActivation:
				return pad ? &a_settings.gamepadActivationKey : &a_settings.activationKey;
			case Input::kBindingLeftHand:
				return pad ? &a_settings.gamepadLeftHandKey : &a_settings.leftHandKey;
			case Input::kBindingRightHand:
				return pad ? &a_settings.gamepadRightHandKey : &a_settings.rightHandKey;
			case Input::kBindingSwapHands:
				return pad ? &a_settings.gamepadSwapHandsKey : &a_settings.swapHandsKey;
			case Input::kBindingNextPage:
				return pad ? &a_settings.gamepadNextPageKey : &a_settings.nextPageKey;
			case Input::kBindingPreviousPage:
				return pad ? &a_settings.gamepadPreviousPageKey : &a_settings.previousPageKey;
			default:
				return pad ? nullptr : &a_settings.slotKeys[static_cast<std::size_t>(a_binding.id)];
			}
		}

		const char *Label(const char *a_label)
		{
			const std::string_view label{a_label};
			const auto id = label.find("##");
			const auto english = label.substr(0, id);
			const auto text = Translate(english);
			if (text == english)
			{
				return a_label;
			}
			static std::map<std::string, std::string, std::less<>> labels;
			auto found = labels.find(label);
			if (found == labels.end())
			{
				found = labels.emplace(label, std::string{text} + std::string{id == std::string_view::npos ? std::string_view{} : label.substr(id)}).first;
			}
			return found->second.c_str();
		}
	}

	class MenuFrameworkConfig::Page
	{
	public:
		Page(Input::InputManager &a_input, Settings &a_settings) : _input(a_input), _settings(a_settings), _capturing(a_input.CapturingBinding())
		{
		}

		[[nodiscard]] bool SaveRequested() const noexcept { return _save; }
		void RequestSave() noexcept { _save = true; }

		void DrawSettings()
		{
			using namespace ImGuiMCP;

			SeparatorText("Orbit Menu");
			KeyRow("Hold to open:", Input::kBindingActivation);
			if (_settings.activationKey == Input::kKeyBindNone && _settings.gamepadActivationKey == Input::kKeyBindNone)
			{
				Text("%s", Translate("No key is bound, so Orbit Menu can not be opened."));
			}
			Check("Toggle Orbit Menu##wr", _settings.toggleOrbitMenu);
			Check("Controller use right stick##wr", _settings.useRightStick);
			Slider("Mouse sensitivity##wr", _settings.mouseSensitivity, Settings::kMinSensitivity, Settings::kMaxSensitivity, "%.2f",
				   "The lower it is, the more you have to move the mouse sideways to swing over to the next slot.");
			Slider("Mouse DPI##wr", _settings.mouseDPI, Settings::kMinMouseDPI, Settings::kMaxMouseDPI,
				   "Mouse DPI, also known as deadzone, is how far the mouse has to move from the middle before the first slot is highlighted.\n"
				   "It protects a small bump when you press the key doesn't highlight anything.\n"
				   "Works like mouse DPI - the lower it is, the more you have to move the mouse, so slots are less likely to be highlighted by accident.");

			SeparatorText(Translate("Slots"));
			TextDisabled("%s", Translate("Hover over an item in the Inventory menu or a spell in the Magic menu, then press a slot key to assign it."));
			const float em = GetFontSize();
			const auto slots = Items::SlotStore::Get().Snapshot();
			for (int i = 0; i < Core::kSlotCount; ++i)
			{
				const auto &slot = slots[static_cast<std::size_t>(i)];
				Text("%s %d", Translate("Slot"), i + 1);
				SameLine(em * 4.0f);
				if (slot.IsEmpty())
				{
					TextDisabled("%s", Translate("Empty"));
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
					if (Button(std::format("{}##wr_clear_{}", Translate("Clear"), i).c_str()))
					{
						Items::SlotStore::Get().Clear(i);
					}
				}
			}
			if (const auto message = _input.CaptureMessage(); !message.empty())
			{
				Text("%s", message.c_str());
			}

			SeparatorText(Translate("Slot Controls"));
			Check("Release to equip##wr", _settings.releaseToEquip, "Letting go of the Orbit Menu key will equip last highlighted slot.\n"
																	"It's off by default because just by opening the menu to look could accidentally equip or use something.");
			KeyRow("Equip in left hand:", Input::kBindingLeftHand);
			KeyRow("Equip in right hand:", Input::kBindingRightHand);
			KeyRow("Swap hands:", Input::kBindingSwapHands);

			SeparatorText("I4 (Inventory Interface Information Injector)");
			if (Integration::I4Addon::IsInstalled())
			{
				Check("Use I4 icons##wr", _settings.useI4);
			}
			else
			{
				bool unavailable = false;
				BeginDisabled();
				Checkbox(Label("Use I4 icons##wr"), &unavailable);
				EndDisabled();
				if (IsItemHovered(ImGuiMCP::ImGuiHoveredFlags_AllowWhenDisabled))
				{
					SetTooltip("%s", Translate("Inventory Interface Information Injector not installed!"));
				}
			}

			SeparatorText(Translate("Messages"));
			Check("Show HUD messages##wr", _settings.showHudMessages);

			SeparatorText(Translate("Settings"));
			Spacing();
			if (Button(Label("Reset settings to defaults##wr")))
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
			if (RadioButton(Label("Show Orbit Menu in background##wr"), preview))
			{
				renderer.SetPreview(!preview);
			}

			SeparatorText(Translate("Display"));
			Slider("Menu size##wr", _settings.orbitMenuScale, Settings::kMinOrbitMenuScale, Settings::kMaxOrbitMenuScale, "%.2f");
			Text("%s", Translate("Position"));
			Number("X##wr_position_x", _settings.positionX, Settings::kMinPosition, Settings::kMaxPosition, 10);
			Number("Y##wr_position_y", _settings.positionY, Settings::kMinPosition, Settings::kMaxPosition, 10);
			Number("Curve##wr_curve", _settings.curve, Settings::kMinCurve, Settings::kMaxCurve, 5);
			Slider("Background Opacity##wr", _settings.backgroundOpacity, Settings::kMinOpacity, Settings::kMaxOpacity, "%.2f");
			Color("Background colour##wr", _settings.backgroundColor);
			Color("Slot colour##wr", _settings.slotColor);
			Color("Highlight colour##wr", _settings.highlightColor);
			Slider("Animation speed##wr", _settings.animationSpeed, Settings::kMinAnimationSpeed, Settings::kMaxAnimationSpeed, "%.2f",
				   "0 turns the animations off.");
			Check("Show the Controls Hint##wr", _settings.showHint);

			SeparatorText(Translate("Slot"));
			Check("Show Slot Keys##wr", _settings.showSlotKeys);
			Check("Show Slot Keys if controller detected##wr", _settings.showSlotKeysOnController);
			Check("Show Hand##wr", _settings.showHand);
			Check("Show Slot Empty##wr", _settings.showSlotEmpty);
			Check("Show Slot Glow##wr", _settings.showSlotGlow);
			Check("Show Slot Background##wr", _settings.showSlotBackground);
			Number("Slot Size##wr", _settings.slotSize, Settings::kMinSlotSize, Settings::kMaxSlotSize, 2);
			Number("Slot Icon Size##wr", _settings.slotIconSize, Settings::kMinSlotIconSize, Settings::kMaxSlotIconSize, 2);
			Number("Slot Distance##wr", _settings.slotDistance, Settings::kMinSlotDistance, Settings::kMaxSlotDistance, 5);
			Choice("Slot Key Position##wr", _settings.slotKeyPosition, Settings::kLastSlotKeyPosition);
			Choice("Hand Position##wr", _settings.handPosition, Settings::kLastHandPosition);

			SeparatorText(Translate("Item Description"));
			Check("Show Item Description##wr", _settings.showItemDescription);
			Check("Show Item Icon Description##wr", _settings.showItemIconDescription);
			Check("Show Center Background##wr", _settings.showCenterBackground);
			Number("Center Background Size##wr", _settings.centerBackgroundSize, Settings::kMinCenterBackgroundSize, Settings::kMaxCenterBackgroundSize, 2);
			Number("Item Icon Description Size##wr", _settings.itemIconDescriptionSize, Settings::kMinItemIconDescriptionSize, Settings::kMaxItemIconDescriptionSize);

			SeparatorText(Translate("Pointer"));
			Check("Show Pointer##wr", _settings.showPointer);
			Number("Pointer Size##wr", _settings.pointerSize, Settings::kMinPointerSize, Settings::kMaxPointerSize);
			Number("Pointer Distance##wr", _settings.pointerDistance, Settings::kMinPointerDistance, Settings::kMaxPointerDistance);

			SeparatorText(Translate("Text"));
			Text("%s", Translate("Slot Name"));
			Number("##wr_slot_name_size", _settings.slotNameTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_slot_name_colour", _settings.slotNameColor);
			Spacing();
			Text("%s", Translate("Slot Empty"));
			Number("##wr_slot_empty_size", _settings.slotEmptyTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_slot_empty_colour", _settings.slotEmptyColor);
			Spacing();
			Text("%s", Translate("Slot keys"));
			Number("##wr_slot_key_size", _settings.slotKeyTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_slot_key_colour", _settings.slotKeyColor);
			Spacing();
			Text("%s", Translate("Hand"));
			Number("##wr_hand_size", _settings.handTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_hand_colour", _settings.handColor);
			Spacing();
			Text("%s", Translate("Item Description"));
			Number("##wr_item_description_size", _settings.itemDescriptionTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_item_description_colour", _settings.itemDescriptionColor);
			Spacing();
			Text("%s", Translate("Controls Hint"));
			Number("##wr_hint_size", _settings.hintTextSize, Settings::kMinTextSize, Settings::kMaxTextSize);
			Color("##wr_hint_colour", _settings.hintColor);

			Separator();
			Spacing();
			if (Button(Label("Reset display to defaults##wr")))
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

			SeparatorText(Translate("Pages"));
			Check("Enable Pages##wr", _settings.enablePages);
			Number("Number of Pages##wr", _settings.numberOfPages, Settings::kMinNumberOfPages, Settings::kMaxNumberOfPages);
			KeyRow("Next Page:", Input::kBindingNextPage);
			KeyRow("Previous Page:", Input::kBindingPreviousPage);
			if (const auto message = _input.CaptureMessage(); !message.empty())
			{
				Text("%s", message.c_str());
			}

			Separator();
			Spacing();
			if (Button(Label("Clear All Pages##wr")))
			{
				Items::Pages::Get().ClearExtraPages();
			}
			Tooltip("Clears pages 2 and up. Page 1 is kept.");
			Spacing();
			if (Button(Label("Reset pages to defaults##wr")))
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
			SeparatorText(Translate("Slow Motion"));
			Check("Enable Slow Motion##wr", _settings.enableSlowMotion);
			Slider("Time Speed##wr", _settings.slowMotionTimeSpeed, Settings::kMinSlowMotionTimeSpeed, Settings::kMaxSlowMotionTimeSpeed, "%.2f",
				   "1 is off, 0.30 and lower I do not recommend, can mess up physics, and mods like HDT-SMP");
			Slider("Fade In##wr", _settings.slowMotionFadeIn, 0.0f, Settings::kMaxSlowMotionFade, "%.2f s", "0 is off");
			Slider("Fade Out##wr", _settings.slowMotionFadeOut, 0.0f, Settings::kMaxSlowMotionFade, "%.2f s", "0 is off");

			Separator();
			Spacing();
			if (Button(Label("Reset slow motion to defaults##wr")))
			{
				_settings.CopySlowMotion(SettingsStore::Get().Defaults());
				RequestSave();
			}
		}

	private:
		void Slider(const char *a_label, float &a_value, float a_min, float a_max, const char *a_format, const char *a_tooltip = nullptr)
		{
			ImGuiMCP::SliderFloat(Label(a_label), &a_value, a_min, a_max, a_format, ImGuiMCP::ImGuiSliderFlags_AlwaysClamp);
			_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
			Tooltip(a_tooltip);
		}

		void Slider(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, const char *a_tooltip = nullptr)
		{
			ImGuiMCP::SliderInt(Label(a_label), &a_value, a_min, a_max, "%d", ImGuiMCP::ImGuiSliderFlags_AlwaysClamp);
			_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
			Tooltip(a_tooltip);
		}

		void Number(const char *a_label, std::int32_t &a_value, std::int32_t a_min, std::int32_t a_max, std::int32_t a_step = 1, const char *a_tooltip = nullptr)
		{
			if (ImGuiMCP::InputInt(Label(a_label), &a_value, a_step, a_step * 4))
			{
				a_value = std::clamp(a_value, a_min, a_max);
			}
			_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
			Tooltip(a_tooltip);
		}

		void Check(const char *a_label, bool &a_value, const char *a_tooltip = nullptr)
		{
			_save |= ImGuiMCP::Checkbox(Label(a_label), &a_value);
			Tooltip(a_tooltip);
		}

		void Choice(const char *a_label, std::int32_t &a_value, std::int32_t a_last)
		{
			std::array<const char *, kBadgePositions.size()> items{};
			std::ranges::transform(kBadgePositions, items.begin(), [](const char *a_item)
								   { return Translate(a_item); });
			_save |= ImGuiMCP::Combo(Label(a_label), &a_value, items.data(), a_last + 1);
		}

		void Color(const char *a_label, std::uint32_t &a_rgb)
		{
			float rgb[3]{};
			for (int i = 0; i < 3; ++i)
			{
				rgb[i] = static_cast<float>((a_rgb >> (16 - 8 * i)) & 0xFF) / 255.0f;
			}
			if (ImGuiMCP::ColorEdit3(Label(a_label), rgb, ImGuiMCP::ImGuiColorEditFlags_NoAlpha))
			{
				a_rgb = 0;
				for (const float channel : rgb)
				{
					a_rgb = (a_rgb << 8) | static_cast<std::uint32_t>(std::lround(std::clamp(channel, 0.0f, 1.0f) * 255.0f));
				}
			}
			_save |= ImGuiMCP::IsItemDeactivatedAfterEdit();
		}

		void KeyRow(const char *a_label, int a_binding)
		{
			ImGuiMCP::Text("%s", Translate(a_label));
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
				Text("%s", Translate(a_binding.gamepad ? "Press a controller button..." : "Press a keyboard or mouse button..."));
				return;
			}

			const int pad = a_binding.gamepad ? 1 : 0;
			const auto name = Input::KeyName(a_key);
			const auto label = a_binding.gamepad ? Translate("Controller: {}", name) : name;
			if (Button(std::format("{}##wr_bind_{}_{}", label, a_binding.id, pad).c_str()))
			{
				_input.BeginKeyCapture(a_binding);
			}
			Tooltip(a_binding.gamepad ? "Click, then press the new controller button." : "Click, then press the new keyboard or mouse button.");
			if (a_key != Input::kKeyBindNone)
			{
				SameLine();
				if (Button(std::format("{}##wr_unbind_{}_{}", Translate("Unbind"), a_binding.id, pad).c_str()))
				{
					a_key = Input::kKeyBindNone;
					_save = true;
				}
			}
		}

		static void Tooltip(const char *a_text)
		{
			if (a_text && ImGuiMCP::IsItemHovered())
			{
				ImGuiMCP::SetTooltip("%s", Translate(a_text));
			}
		}

		Input::InputManager &_input;
		Settings &_settings;
		std::optional<Input::KeyBinding> _capturing;
		bool _save = false;
	};

	MenuFrameworkConfig &MenuFrameworkConfig::Get() noexcept
	{
		static MenuFrameworkConfig singleton;
		return singleton;
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
			if (auto *key = BoundKey(settings, {captured->binding, Input::IsGamepadBind(captured->code)}))
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
			SetStatus(Translate("Could not write the settings file - see OrbitMenu.log."));
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
