#include "Config/OrbitMenuIni.h"
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
					logger::warn("Settings Ini: {} = {} is not a colour", a_key, text);
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
	}

	void OrbitMenuIni::CopyDefaultsFile()
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

	void OrbitMenuIni::RemoveOldKeys()
	{
		for (const auto &[section, key] : kRemovedKeys)
		{
			_ini.Delete(section, key);
		}
	}

	void OrbitMenuIni::Read(Settings &a_settings) const
	{
		ForEachSetting(a_settings, IniReader{_ini});
		if (const double opacity = _ini.GetDoubleValue(kDisplay, "fBackgroundOpacity", -1.0); opacity >= 0.0)
		{
			const auto alpha = static_cast<std::uint32_t>(std::lround(std::clamp(opacity, 0.0, 1.0) * 255.0));
			a_settings.backgroundColor = alpha << 24 | (a_settings.backgroundColor & 0xFFFFFF);
		}
	}

	void OrbitMenuIni::Write(Settings a_settings)
	{
		ForEachSetting(a_settings, IniWriter{_ini});
	}

	bool OrbitMenuIni::SaveAs(const std::filesystem::path &a_path) const
	{
		std::error_code error;
		std::filesystem::create_directories(a_path.parent_path(), error);
		const SI_Error result = _ini.SaveFile(a_path.c_str(), false);
		if (result < 0)
		{
			logger::error("Settings Ini: Could not write {} Ini error {}", a_path.string(), static_cast<int>(result));
			return false;
		}
		return true;
	}
}
