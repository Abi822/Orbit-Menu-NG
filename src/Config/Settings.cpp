#include "Config/Settings.h"
#include "Config/OrbitMenuIni.h"
#include "Input/KeyBinds.h"

namespace Orbit::Config
{
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
			OrbitMenuIni file;
			if (const SI_Error result = file.Load(OrbitMenuIni::kDefaultsPath); result >= 0)
			{
				file.Read(defaults);
				haveDefaults = true;
			}
			else
			{
				logger::error("Settings: {} could not be read! Ini error {}",
							  OrbitMenuIni::kDefaultsPath.string(), static_cast<int>(result));
			}
			defaults.Clamp(Settings{});
		}

		Settings loaded = defaults;
		if (std::error_code ec; std::filesystem::is_regular_file(OrbitMenuIni::kUserPath, ec))
		{
			OrbitMenuIni file;
			if (const SI_Error result = file.Load(OrbitMenuIni::kUserPath); result >= 0)
			{
				file.Read(loaded);
			}
			else
			{
				logger::warn("Settings: {} could not be read! Ini error {}, using the default settings!",
							 OrbitMenuIni::kUserPath.string(), static_cast<int>(result));
			}
		}
		else if (haveDefaults)
		{
			OrbitMenuIni file;
			file.CopyDefaultsFile();
			file.Write(defaults);
			if (file.SaveAs(OrbitMenuIni::kUserPath))
			{
				logger::info("Settings: Created {} with the default settings", OrbitMenuIni::kUserPath.string());
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
		OrbitMenuIni file;
		if (file.Load(OrbitMenuIni::kUserPath) < 0)
		{
			file.CopyDefaultsFile();
		}
		file.RemoveOldKeys();
		file.Write(Snapshot());
		return file.SaveAs(OrbitMenuIni::kUserPath);
	}
}
