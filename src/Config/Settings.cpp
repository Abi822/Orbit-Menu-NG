#include "Config/Settings.h"
#include "Input/KeyBinds.h"
#include <SimpleIni.h>
#include <charconv>
#include <fstream>
#include <iterator>

namespace Orbit::Config
{
	namespace
	{
		constexpr auto kControls = "Controls";
		constexpr auto kAssignment = "Assignment";
		constexpr auto kPages = "Pages";
		constexpr auto kSlowMotion = "SlowMotion";
		constexpr auto kDisplay = "Display";
		constexpr auto kIntegration = "Integration";
		constexpr auto kMessages = "Messages";
		constexpr auto kSkins = "Skins";
		constexpr auto kTranslation = "Translation";
		constexpr std::array kRemovedKeys{std::pair{kDisplay, "fTextScale"}, std::pair{kDisplay, "fSlotKeyTextScale"}, std::pair{kDisplay, "fItemDescriptionTextScale"},
										  std::pair{kDisplay, "sSkinPath"}, std::pair{kDisplay, "iOctagonSize"}, std::pair{kDisplay, "sAccentColor"},
										  std::pair{kDisplay, "bShowHands"}, std::pair{kDisplay, "bShowItemIcon"}, std::pair{kDisplay, "iItemIconSize"},
										  std::pair{kSlowMotion, "fSlowMotionSpeed"}, std::pair{kPages, "iPageCount"},
										  std::pair{kDisplay, "fBackgroundOpacity"}, std::pair{kDisplay, "bShowCenterBackground"},
										  std::pair{kDisplay, "iCenterBackgroundSize"}, std::pair{kDisplay, "sCenterBackgroundColor"},
										  std::pair{kDisplay, "bShowHint"}, std::pair{kDisplay, "iHintTextSize"}, std::pair{kDisplay, "sHintColor"}};
		const std::filesystem::path kDefaultsPath{"Data/SKSE/Plugins/Orbit Menu/OrbitMenu_Default.ini"};
		const std::filesystem::path kUserPath{"Data/SKSE/Plugins/Orbit Menu/OrbitMenu.ini"};

		struct Color
		{
			std::uint32_t &value;
		};

		template <class Visit>
		void ForEachSetting(Settings &a_settings, const Visit &a_visit)
		{
			a_visit(kControls, "iActivationKey", a_settings.activationKey);
			a_visit(kControls, "fMouseSensitivity", a_settings.mouseSensitivity);
			a_visit(kControls, "iMouseDPI", a_settings.mouseDPI);
			a_visit(kControls, "iLeftHandKey", a_settings.leftHandKey);
			a_visit(kControls, "iRightHandKey", a_settings.rightHandKey);
			a_visit(kControls, "iSwapHandsKey", a_settings.swapHandsKey);
			a_visit(kControls, "iGamepadActivationKey", a_settings.gamepadActivationKey);
			a_visit(kControls, "iGamepadActivationModifierKey", a_settings.gamepadActivationModifierKey);
			a_visit(kControls, "iGamepadLeftHandKey", a_settings.gamepadLeftHandKey);
			a_visit(kControls, "iGamepadRightHandKey", a_settings.gamepadRightHandKey);
			a_visit(kControls, "iGamepadSwapHandsKey", a_settings.gamepadSwapHandsKey);
			a_visit(kControls, "bReleaseToEquip", a_settings.releaseToEquip);
			a_visit(kControls, "bToggleOrbitMenu", a_settings.toggleOrbitMenu);
			a_visit(kControls, "bUseRightStick", a_settings.useRightStick);
			a_visit(kPages, "bEnablePages", a_settings.enablePages);
			a_visit(kPages, "iNumberOfPages", a_settings.numberOfPages);
			a_visit(kPages, "iNextPageKey", a_settings.nextPageKey);
			a_visit(kPages, "iPreviousPageKey", a_settings.previousPageKey);
			a_visit(kPages, "iGamepadNextPageKey", a_settings.gamepadNextPageKey);
			a_visit(kPages, "iGamepadPreviousPageKey", a_settings.gamepadPreviousPageKey);
			a_visit(kPages, "bShowPageIndicator", a_settings.showPageIndicator);
			a_visit(kPages, "iPageIndicatorX", a_settings.pageIndicatorX);
			a_visit(kPages, "iPageIndicatorY", a_settings.pageIndicatorY);
			a_visit(kPages, "iPageIndicatorRotation", a_settings.pageIndicatorRotation);
			a_visit(kPages, "iPageIndicatorColumns", a_settings.pageIndicatorColumns);
			a_visit(kPages, "iPageIndicatorSpacing", a_settings.pageIndicatorSpacing);
			a_visit(kPages, "iPageIndicatorSize", a_settings.pageIndicatorSize);
			a_visit(kPages, "sPageIndicatorActiveColor", Color{a_settings.pageIndicatorActiveColor});
			a_visit(kPages, "sPageIndicatorInactiveColor", Color{a_settings.pageIndicatorInactiveColor});
			a_visit(kSlowMotion, "bEnableSlowMotion", a_settings.enableSlowMotion);
			a_visit(kSlowMotion, "fSlowMotionTimeSpeed", a_settings.slowMotionTimeSpeed);
			a_visit(kSlowMotion, "fSlowMotionFadeIn", a_settings.slowMotionFadeIn);
			a_visit(kSlowMotion, "fSlowMotionFadeOut", a_settings.slowMotionFadeOut);
			a_visit(kDisplay, "fOrbitMenuScale", a_settings.orbitMenuScale);
			a_visit(kDisplay, "iPositionX", a_settings.positionX);
			a_visit(kDisplay, "iPositionY", a_settings.positionY);
			a_visit(kDisplay, "iCurve", a_settings.curve);
			a_visit(kDisplay, "sBackgroundColor", Color{a_settings.backgroundColor});
			a_visit(kDisplay, "sSlotColor", Color{a_settings.slotColor});
			a_visit(kDisplay, "sSlotKeyBackgroundColor", Color{a_settings.slotKeyBackgroundColor});
			a_visit(kDisplay, "sHighlightColor", Color{a_settings.highlightColor});
			a_visit(kDisplay, "sSlotGlowColor", Color{a_settings.slotGlowColor});
			a_visit(kDisplay, "sPointerColor", Color{a_settings.pointerColor});
			a_visit(kDisplay, "sItemDescriptionBackgroundColor", Color{a_settings.itemDescriptionBackgroundColor});
			a_visit(kDisplay, "sSlotNameColor", Color{a_settings.slotNameColor});
			a_visit(kDisplay, "sSlotEmptyColor", Color{a_settings.slotEmptyColor});
			a_visit(kDisplay, "sSlotKeyColor", Color{a_settings.slotKeyColor});
			a_visit(kDisplay, "sHandColor", Color{a_settings.handColor});
			a_visit(kDisplay, "sItemDescriptionColor", Color{a_settings.itemDescriptionColor});
			a_visit(kDisplay, "sControlsHintColor", Color{a_settings.controlsHintColor});
			a_visit(kDisplay, "iSlotNameTextSize", a_settings.slotNameTextSize);
			a_visit(kDisplay, "iSlotEmptyTextSize", a_settings.slotEmptyTextSize);
			a_visit(kDisplay, "iSlotKeyTextSize", a_settings.slotKeyTextSize);
			a_visit(kDisplay, "iHandTextSize", a_settings.handTextSize);
			a_visit(kDisplay, "iItemDescriptionTitleTextSize", a_settings.itemDescriptionTitleTextSize);
			a_visit(kDisplay, "iItemDescriptionTextSize", a_settings.itemDescriptionTextSize);
			a_visit(kDisplay, "iControlsHintTextSize", a_settings.controlsHintTextSize);
			a_visit(kDisplay, "bShowSlotKeys", a_settings.showSlotKeys);
			a_visit(kDisplay, "bShowSlotKeysOnController", a_settings.showSlotKeysOnController);
			a_visit(kDisplay, "iSlotDistance", a_settings.slotDistance);
			a_visit(kDisplay, "iSlotSize", a_settings.slotSize);
			a_visit(kDisplay, "iSlotIconSize", a_settings.slotIconSize);
			a_visit(kDisplay, "iSlotIconX", a_settings.slotIconX);
			a_visit(kDisplay, "iSlotIconY", a_settings.slotIconY);
			a_visit(kDisplay, "iSlotNameX", a_settings.slotNameX);
			a_visit(kDisplay, "iSlotNameY", a_settings.slotNameY);
			a_visit(kDisplay, "bShowBackground", a_settings.showBackground);
			a_visit(kDisplay, "bShowSlotBackground", a_settings.showSlotBackground);
			a_visit(kDisplay, "bShowItemDescriptionBackground", a_settings.showItemDescriptionBackground);
			a_visit(kDisplay, "iItemDescriptionBackgroundSize", a_settings.itemDescriptionBackgroundSize);
			a_visit(kDisplay, "iSlotKeyPosition", a_settings.slotKeyPosition);
			a_visit(kDisplay, "bShowHand", a_settings.showHand);
			a_visit(kDisplay, "iHandPosition", a_settings.handPosition);
			a_visit(kDisplay, "bShowSlotEmpty", a_settings.showSlotEmpty);
			a_visit(kDisplay, "bShowControlsHint", a_settings.showControlsHint);
			a_visit(kDisplay, "iControlsHintX", a_settings.controlsHintX);
			a_visit(kDisplay, "iControlsHintY", a_settings.controlsHintY);
			a_visit(kDisplay, "bShowItemDescription", a_settings.showItemDescription);
			a_visit(kDisplay, "bShowLastEquippedItem", a_settings.showLastEquippedItem);
			a_visit(kDisplay, "bShowItemIconDescription", a_settings.showItemIconDescription);
			a_visit(kDisplay, "iItemIconDescriptionSize", a_settings.itemIconDescriptionSize);
			a_visit(kDisplay, "bStaticItemDescription", a_settings.staticItemDescription);
			a_visit(kDisplay, "iItemIconDescriptionX", a_settings.itemIconDescriptionX);
			a_visit(kDisplay, "iItemIconDescriptionY", a_settings.itemIconDescriptionY);
			a_visit(kDisplay, "iItemDescriptionTitleX", a_settings.itemDescriptionTitleX);
			a_visit(kDisplay, "iItemDescriptionTitleY", a_settings.itemDescriptionTitleY);
			a_visit(kDisplay, "iItemDescriptionTitleWidth", a_settings.itemDescriptionTitleWidth);
			a_visit(kDisplay, "iItemDescriptionTextX", a_settings.itemDescriptionTextX);
			a_visit(kDisplay, "iItemDescriptionTextY", a_settings.itemDescriptionTextY);
			a_visit(kDisplay, "bItemDescriptionTextFollowsTitle", a_settings.itemDescriptionTextFollowsTitle);
			a_visit(kDisplay, "iItemDescriptionWidth", a_settings.itemDescriptionWidth);
			a_visit(kDisplay, "fItemDescriptionLineSpacing", a_settings.itemDescriptionLineSpacing);
			a_visit(kDisplay, "bShowSlotGlow", a_settings.showSlotGlow);
			a_visit(kDisplay, "bShowPointer", a_settings.showPointer);
			a_visit(kDisplay, "iPointerSize", a_settings.pointerSize);
			a_visit(kDisplay, "iPointerDistance", a_settings.pointerDistance);
			a_visit(kDisplay, "iPointerRotation", a_settings.pointerRotation);
			a_visit(kDisplay, "fAnimationSpeed", a_settings.animationSpeed);
			a_visit(kSkins, "sSkinPath", a_settings.skinPath);
			a_visit(kSkins, "sFontPath", a_settings.fontPath);
			a_visit(kTranslation, "sLanguagePath", a_settings.languagePath);
			a_visit(kIntegration, "bUseI4", a_settings.useI4);
			a_visit(kMessages, "bShowHudMessages", a_settings.showHudMessages);

			for (std::size_t i = 0; i < a_settings.slotKeys.size(); ++i)
			{
				a_visit(kAssignment, std::format("iSlotKey{}", i + 1).c_str(), a_settings.slotKeys[i]);
			}
		}

		class IniReader
		{
		public:
			explicit IniReader(const CSimpleIniA &a_ini) : _ini(a_ini) {}

			void operator()(const char *a_section, const char *a_key, std::uint32_t &a_value) const
			{
				a_value = static_cast<std::uint32_t>(_ini.GetLongValue(a_section, a_key, static_cast<long>(a_value)));
			}

			void operator()(const char *a_section, const char *a_key, std::int32_t &a_value) const
			{
				a_value = static_cast<std::int32_t>(_ini.GetLongValue(a_section, a_key, a_value));
			}

			void operator()(const char *a_section, const char *a_key, float &a_value) const
			{
				const auto value = static_cast<float>(_ini.GetDoubleValue(a_section, a_key, a_value));
				if (std::isfinite(value))
				{
					a_value = value;
				}
			}

			void operator()(const char *a_section, const char *a_key, bool &a_value) const
			{
				a_value = _ini.GetBoolValue(a_section, a_key, a_value);
			}

			void operator()(const char *a_section, const char *a_key, std::string &a_value) const
			{
				if (const char *text = _ini.GetValue(a_section, a_key))
				{
					a_value = text;
				}
			}

			void operator()(const char *a_section, const char *a_key, Color a_color) const
			{
				const char *text = _ini.GetValue(a_section, a_key);
				if (!text)
				{
					return;
				}
				std::string_view digits{text};
				if (digits.starts_with("0x") || digits.starts_with("0X"))
				{
					digits.remove_prefix(2);
				}
				std::uint32_t value = 0;
				const auto [end, error] = std::from_chars(digits.data(), digits.data() + digits.size(), value, 16);
				if (digits.empty() || digits.size() > 8 || error != std::errc{} || end != digits.data() + digits.size())
				{
					logger::warn("Settings: {} = {} is not a colour", a_key, text);
					return;
				}
				a_color.value = digits.size() == 8 ? value : value | 0xFF000000;
			}

		private:
			const CSimpleIniA &_ini;
		};

		class IniWriter
		{
		public:
			explicit IniWriter(CSimpleIniA &a_ini) : _ini(a_ini) {}

			void operator()(const char *a_section, const char *a_key, std::uint32_t &a_value) const
			{
				_ini.SetLongValue(a_section, a_key, static_cast<long>(a_value));
			}

			void operator()(const char *a_section, const char *a_key, std::int32_t &a_value) const
			{
				_ini.SetLongValue(a_section, a_key, a_value);
			}

			void operator()(const char *a_section, const char *a_key, float &a_value) const
			{
				_ini.SetValue(a_section, a_key, std::format("{:.3f}", a_value).c_str());
			}

			void operator()(const char *a_section, const char *a_key, bool &a_value) const
			{
				_ini.SetBoolValue(a_section, a_key, a_value);
			}

			void operator()(const char *a_section, const char *a_key, std::string &a_value) const
			{
				_ini.SetValue(a_section, a_key, a_value.c_str());
			}

			void operator()(const char *a_section, const char *a_key, Color a_color) const
			{
				_ini.SetValue(a_section, a_key, std::format("0x{:08X}", a_color.value).c_str());
			}

		private:
			CSimpleIniA &_ini;
		};

		class SettingsFile
		{
		public:
			SettingsFile() { _ini.SetUnicode(); }

			[[nodiscard]] SI_Error Load(const std::filesystem::path &a_path) { return _ini.LoadFile(a_path.c_str()); }

			void CopyDefaultsFile()
			{
				std::string defaults;
				if (std::ifstream file{kDefaultsPath, std::ios::binary})
				{
					defaults.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
				}
				const auto firstSection = defaults.starts_with('[') ? 0 : defaults.find("\n[");
				if (firstSection != std::string::npos)
				{
					_ini.LoadData(defaults.substr(firstSection == 0 ? 0 : firstSection + 1));
				}
			}

			void RemoveOldKeys()
			{
				for (const auto &[section, key] : kRemovedKeys)
				{
					_ini.Delete(section, key);
				}
			}

			void Read(Settings &a_settings) const
			{
				ForEachSetting(a_settings, IniReader{_ini});
				if (const double opacity = _ini.GetDoubleValue(kDisplay, "fBackgroundOpacity", -1.0); opacity >= 0.0)
				{
					const auto alpha = static_cast<std::uint32_t>(std::lround(std::clamp(opacity, 0.0, 1.0) * 255.0));
					a_settings.backgroundColor = alpha << 24 | (a_settings.backgroundColor & 0xFFFFFF);
				}
			}

			void Write(Settings a_settings) { ForEachSetting(a_settings, IniWriter{_ini}); }

			[[nodiscard]] bool SaveAs(const std::filesystem::path &a_path) const
			{
				std::error_code error;
				std::filesystem::create_directories(a_path.parent_path(), error);
				const SI_Error result = _ini.SaveFile(a_path.c_str(), false);
				if (result < 0)
				{
					logger::error("Settings: Could not write {} Ini error {}", a_path.string(), static_cast<int>(result));
					return false;
				}
				return true;
			}

		private:
			CSimpleIniA _ini;
		};
	}

	void Settings::Clamp(const Settings &a_valueDefault)
	{
		const auto checkKey = [](std::uint32_t &a_key, std::uint32_t a_valueDefault, bool a_gamepad)
		{
			if (a_key != Input::kKeyBindNone && Input::IsGamepadBind(a_key) != a_gamepad)
			{
				a_key = a_valueDefault;
			}
		};
		checkKey(activationKey, a_valueDefault.activationKey, false);
		checkKey(leftHandKey, a_valueDefault.leftHandKey, false);
		checkKey(rightHandKey, a_valueDefault.rightHandKey, false);
		checkKey(swapHandsKey, a_valueDefault.swapHandsKey, false);
		checkKey(gamepadActivationKey, a_valueDefault.gamepadActivationKey, true);
		checkKey(gamepadActivationModifierKey, a_valueDefault.gamepadActivationModifierKey, true);
		checkKey(gamepadLeftHandKey, a_valueDefault.gamepadLeftHandKey, true);
		checkKey(gamepadRightHandKey, a_valueDefault.gamepadRightHandKey, true);
		checkKey(gamepadSwapHandsKey, a_valueDefault.gamepadSwapHandsKey, true);
		for (std::size_t i = 0; i < slotKeys.size(); ++i)
		{
			checkKey(slotKeys[i], a_valueDefault.slotKeys[i], false);
		}
		checkKey(nextPageKey, a_valueDefault.nextPageKey, false);
		checkKey(previousPageKey, a_valueDefault.previousPageKey, false);
		checkKey(gamepadNextPageKey, a_valueDefault.gamepadNextPageKey, true);
		checkKey(gamepadPreviousPageKey, a_valueDefault.gamepadPreviousPageKey, true);
		numberOfPages = std::clamp(numberOfPages, kMinNumberOfPages, kMaxNumberOfPages);
		pageIndicatorX = std::clamp(pageIndicatorX, kMinPosition, kMaxPosition);
		pageIndicatorY = std::clamp(pageIndicatorY, kMinPosition, kMaxPosition);
		pageIndicatorRotation = std::clamp(pageIndicatorRotation, 0, kMaxPageIndicatorRotation);
		pageIndicatorColumns = std::clamp(pageIndicatorColumns, 1, kMaxNumberOfPages);
		pageIndicatorSpacing = std::clamp(pageIndicatorSpacing, 0, kMaxPageIndicatorSpacing);
		pageIndicatorSize = std::clamp(pageIndicatorSize, kMinPageIndicatorSize, kMaxPageIndicatorSize);
		slowMotionTimeSpeed = std::clamp(slowMotionTimeSpeed, kMinSlowMotionTimeSpeed, kMaxSlowMotionTimeSpeed);
		slowMotionFadeIn = std::clamp(slowMotionFadeIn, 0.0f, kMaxSlowMotionFade);
		slowMotionFadeOut = std::clamp(slowMotionFadeOut, 0.0f, kMaxSlowMotionFade);
		mouseSensitivity = std::clamp(mouseSensitivity, kMinSensitivity, kMaxSensitivity);
		mouseDPI = std::clamp(mouseDPI, kMinMouseDPI, kMaxMouseDPI);
		orbitMenuScale = std::clamp(orbitMenuScale, kMinOrbitMenuScale, kMaxOrbitMenuScale);
		positionX = std::clamp(positionX, kMinPosition, kMaxPosition);
		positionY = std::clamp(positionY, kMinPosition, kMaxPosition);
		controlsHintX = std::clamp(controlsHintX, kMinPosition, kMaxPosition);
		controlsHintY = std::clamp(controlsHintY, kMinPosition, kMaxPosition);
		curve = std::clamp(curve, kMinCurve, kMaxCurve);
		slotNameTextSize = std::clamp(slotNameTextSize, kMinTextSize, kMaxTextSize);
		slotEmptyTextSize = std::clamp(slotEmptyTextSize, kMinTextSize, kMaxTextSize);
		slotKeyTextSize = std::clamp(slotKeyTextSize, kMinTextSize, kMaxTextSize);
		handTextSize = std::clamp(handTextSize, kMinTextSize, kMaxTextSize);
		itemDescriptionTitleTextSize = std::clamp(itemDescriptionTitleTextSize, kMinTextSize, kMaxTextSize);
		itemDescriptionTextSize = std::clamp(itemDescriptionTextSize, kMinTextSize, kMaxTextSize);
		controlsHintTextSize = std::clamp(controlsHintTextSize, kMinTextSize, kMaxTextSize);
		slotDistance = std::clamp(slotDistance, kMinSlotDistance, kMaxSlotDistance);
		slotSize = std::clamp(slotSize, kMinSlotSize, kMaxSlotSize);
		slotIconSize = std::clamp(slotIconSize, kMinSlotIconSize, kMaxSlotIconSize);
		for (auto *position : {&slotIconX, &slotIconY, &slotNameX, &slotNameY})
		{
			*position = std::clamp(*position, kMinSlotPosition, kMaxSlotPosition);
		}
		itemDescriptionBackgroundSize = std::clamp(itemDescriptionBackgroundSize, kMinItemDescriptionBackgroundSize, kMaxItemDescriptionBackgroundSize);
		slotKeyPosition = std::clamp(slotKeyPosition, 0, kLastSlotKeyPosition);
		handPosition = std::clamp(handPosition, 0, kLastHandPosition);
		itemIconDescriptionSize = std::clamp(itemIconDescriptionSize, kMinItemIconDescriptionSize, kMaxItemIconDescriptionSize);
		for (auto *position : {&itemIconDescriptionX, &itemIconDescriptionY, &itemDescriptionTitleX, &itemDescriptionTitleY, &itemDescriptionTextX, &itemDescriptionTextY})
		{
			*position = std::clamp(*position, kMinItemDescriptionPosition, kMaxItemDescriptionPosition);
		}
		itemDescriptionTitleWidth = std::clamp(itemDescriptionTitleWidth, kMinItemDescriptionWidth, kMaxItemDescriptionWidth);
		itemDescriptionWidth = std::clamp(itemDescriptionWidth, kMinItemDescriptionWidth, kMaxItemDescriptionWidth);
		itemDescriptionLineSpacing = std::clamp(itemDescriptionLineSpacing, kMinItemDescriptionLineSpacing, kMaxItemDescriptionLineSpacing);
		pointerSize = std::clamp(pointerSize, kMinPointerSize, kMaxPointerSize);
		pointerDistance = std::clamp(pointerDistance, kMinPointerDistance, kMaxPointerDistance);
		pointerRotation = std::clamp(pointerRotation, 0, kMaxPointerRotation);
		animationSpeed = std::clamp(animationSpeed, kMinAnimationSpeed, kMaxAnimationSpeed);
	}

	void Settings::CopyDisplay(const Settings &a_from)
	{
		orbitMenuScale = a_from.orbitMenuScale;
		positionX = a_from.positionX;
		positionY = a_from.positionY;
		curve = a_from.curve;
		backgroundColor = a_from.backgroundColor;
		slotColor = a_from.slotColor;
		slotKeyBackgroundColor = a_from.slotKeyBackgroundColor;
		highlightColor = a_from.highlightColor;
		slotGlowColor = a_from.slotGlowColor;
		pointerColor = a_from.pointerColor;
		itemDescriptionBackgroundColor = a_from.itemDescriptionBackgroundColor;
		slotNameColor = a_from.slotNameColor;
		slotEmptyColor = a_from.slotEmptyColor;
		slotKeyColor = a_from.slotKeyColor;
		handColor = a_from.handColor;
		itemDescriptionColor = a_from.itemDescriptionColor;
		controlsHintColor = a_from.controlsHintColor;
		slotNameTextSize = a_from.slotNameTextSize;
		slotEmptyTextSize = a_from.slotEmptyTextSize;
		slotKeyTextSize = a_from.slotKeyTextSize;
		handTextSize = a_from.handTextSize;
		itemDescriptionTitleTextSize = a_from.itemDescriptionTitleTextSize;
		itemDescriptionTextSize = a_from.itemDescriptionTextSize;
		controlsHintTextSize = a_from.controlsHintTextSize;
		showSlotKeys = a_from.showSlotKeys;
		showSlotKeysOnController = a_from.showSlotKeysOnController;
		slotDistance = a_from.slotDistance;
		slotSize = a_from.slotSize;
		slotIconSize = a_from.slotIconSize;
		slotIconX = a_from.slotIconX;
		slotIconY = a_from.slotIconY;
		slotNameX = a_from.slotNameX;
		slotNameY = a_from.slotNameY;
		showBackground = a_from.showBackground;
		showSlotBackground = a_from.showSlotBackground;
		showItemDescriptionBackground = a_from.showItemDescriptionBackground;
		itemDescriptionBackgroundSize = a_from.itemDescriptionBackgroundSize;
		slotKeyPosition = a_from.slotKeyPosition;
		showHand = a_from.showHand;
		handPosition = a_from.handPosition;
		showSlotEmpty = a_from.showSlotEmpty;
		showControlsHint = a_from.showControlsHint;
		controlsHintX = a_from.controlsHintX;
		controlsHintY = a_from.controlsHintY;
		showItemDescription = a_from.showItemDescription;
		showLastEquippedItem = a_from.showLastEquippedItem;
		showItemIconDescription = a_from.showItemIconDescription;
		itemIconDescriptionSize = a_from.itemIconDescriptionSize;
		staticItemDescription = a_from.staticItemDescription;
		itemIconDescriptionX = a_from.itemIconDescriptionX;
		itemIconDescriptionY = a_from.itemIconDescriptionY;
		itemDescriptionTitleX = a_from.itemDescriptionTitleX;
		itemDescriptionTitleY = a_from.itemDescriptionTitleY;
		itemDescriptionTitleWidth = a_from.itemDescriptionTitleWidth;
		itemDescriptionTextX = a_from.itemDescriptionTextX;
		itemDescriptionTextY = a_from.itemDescriptionTextY;
		itemDescriptionTextFollowsTitle = a_from.itemDescriptionTextFollowsTitle;
		itemDescriptionWidth = a_from.itemDescriptionWidth;
		itemDescriptionLineSpacing = a_from.itemDescriptionLineSpacing;
		showSlotGlow = a_from.showSlotGlow;
		showPointer = a_from.showPointer;
		pointerSize = a_from.pointerSize;
		pointerDistance = a_from.pointerDistance;
		pointerRotation = a_from.pointerRotation;
		animationSpeed = a_from.animationSpeed;
		skinPath = a_from.skinPath;
	}

	void Settings::CopyPages(const Settings &a_from)
	{
		enablePages = a_from.enablePages;
		numberOfPages = a_from.numberOfPages;
		nextPageKey = a_from.nextPageKey;
		previousPageKey = a_from.previousPageKey;
		gamepadNextPageKey = a_from.gamepadNextPageKey;
		gamepadPreviousPageKey = a_from.gamepadPreviousPageKey;
		showPageIndicator = a_from.showPageIndicator;
		pageIndicatorX = a_from.pageIndicatorX;
		pageIndicatorY = a_from.pageIndicatorY;
		pageIndicatorRotation = a_from.pageIndicatorRotation;
		pageIndicatorColumns = a_from.pageIndicatorColumns;
		pageIndicatorSpacing = a_from.pageIndicatorSpacing;
		pageIndicatorSize = a_from.pageIndicatorSize;
		pageIndicatorActiveColor = a_from.pageIndicatorActiveColor;
		pageIndicatorInactiveColor = a_from.pageIndicatorInactiveColor;
	}

	void Settings::CopySlowMotion(const Settings &a_from)
	{
		enableSlowMotion = a_from.enableSlowMotion;
		slowMotionTimeSpeed = a_from.slowMotionTimeSpeed;
		slowMotionFadeIn = a_from.slowMotionFadeIn;
		slowMotionFadeOut = a_from.slowMotionFadeOut;
	}

	SettingsStore &SettingsStore::Get()
	{
		static SettingsStore instance;
		return instance;
	}

	Settings SettingsStore::Snapshot() const
	{
		std::scoped_lock lock{_lock};
		return _settings;
	}

	Settings SettingsStore::Defaults() const
	{
		std::scoped_lock lock{_lock};
		return _defaults;
	}

	void SettingsStore::Set(Settings a_settings)
	{
		std::scoped_lock lock{_lock};
		a_settings.Clamp(_defaults);
		_settings = a_settings;
		++_changes;
	}

	void SettingsStore::Load()
	{
		Settings defaults;
		bool haveDefaults = false;
		{
			SettingsFile file;
			if (const SI_Error result = file.Load(kDefaultsPath); result >= 0)
			{
				file.Read(defaults);
				haveDefaults = true;
			}
			else
			{
				logger::error("Settings: {} Could not be read! Ini error {}",
							  kDefaultsPath.string(), static_cast<int>(result));
			}
			defaults.Clamp(Settings{});
		}

		Settings loaded = defaults;
		if (std::error_code ec; std::filesystem::is_regular_file(kUserPath, ec))
		{
			SettingsFile file;
			if (const SI_Error result = file.Load(kUserPath); result >= 0)
			{
				file.Read(loaded);
			}
			else
			{
				logger::warn("Settings: {} Could not be read! Ini error {}, using the defaults settings!",
							 kUserPath.string(), static_cast<int>(result));
			}
		}
		else if (haveDefaults)
		{
			SettingsFile file;
			file.CopyDefaultsFile();
			file.Write(defaults);
			if (file.SaveAs(kUserPath))
			{
				logger::info("Settings: Created {} with the default settings", kUserPath.string());
			}
		}
		loaded.Clamp(defaults);

		{
			std::scoped_lock lock{_lock};
			_defaults = defaults;
			_settings = loaded;
			++_changes;
		}
	}

	bool SettingsStore::Save() const
	{
		SettingsFile file;
		if (file.Load(kUserPath) < 0)
		{
			file.CopyDefaultsFile();
		}
		file.RemoveOldKeys();
		file.Write(Snapshot());
		return file.SaveAs(kUserPath);
	}
}
