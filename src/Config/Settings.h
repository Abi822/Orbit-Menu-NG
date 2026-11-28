#pragma once

#include "Core/Slots.h"

namespace Orbit::Config
{
	enum class SlotTextPosition : std::int32_t
	{
		kTopLeft,
		kTopMiddle,
		kTopRight,
		kBottomLeft,
		kBottomMiddle,
		kBottomRight,
		kNextToName
	};

	struct Settings
	{
		// Defaults come from OrbitMenu_Default.ini, then are overwritten by the user's OrbitMenu.ini.
		std::uint32_t activationKey{};
		float mouseSensitivity{};
		std::int32_t mouseDPI{};
		std::uint32_t leftHandKey{};
		std::uint32_t rightHandKey{};
		std::uint32_t swapHandsKey{};
		std::uint32_t gamepadActivationKey{};
		std::uint32_t gamepadActivationModifierKey{};
		std::uint32_t gamepadLeftHandKey{};
		std::uint32_t gamepadRightHandKey{};
		std::uint32_t gamepadSwapHandsKey{};
		bool releaseToEquip{};
		bool toggleOrbitMenu{};
		bool useRightStick{};
		Core::PerSlot<std::uint32_t> slotKeys{};
		float orbitMenuScale{};
		std::int32_t positionX{};
		std::int32_t positionY{};
		std::int32_t curve{};
		std::uint32_t backgroundColor{};
		std::uint32_t slotColor{};
		std::uint32_t slotKeyBackgroundColor{};
		std::uint32_t highlightColor{};
		std::uint32_t slotGlowColor{};
		std::uint32_t pointerColor{};
		std::uint32_t itemDescriptionBackgroundColor{};
		std::uint32_t slotNameColor{};
		std::uint32_t slotEmptyColor{};
		std::uint32_t slotKeyColor{};
		std::uint32_t handColor{};
		std::uint32_t itemDescriptionColor{};
		std::uint32_t controlsHintColor{};
		std::int32_t slotNameTextSize{};
		std::int32_t slotEmptyTextSize{};
		std::int32_t slotKeyTextSize{};
		std::int32_t handTextSize{};
		std::int32_t itemDescriptionTitleTextSize{};
		std::int32_t itemDescriptionTextSize{};
		std::int32_t controlsHintTextSize{};
		bool showSlotKeys{};
		bool showSlotKeysOnController{};
		std::int32_t slotDistance{};
		std::int32_t slotSize{};
		std::int32_t slotIconSize{};
		std::int32_t slotIconX{};
		std::int32_t slotIconY{};
		std::int32_t slotNameX{};
		std::int32_t slotNameY{};
		bool showBackground{};
		bool showSlotBackground{};
		bool showItemDescriptionBackground{};
		std::int32_t itemDescriptionBackgroundSize{};
		std::int32_t slotKeyPosition{};
		bool showHand{};
		std::int32_t handPosition{};
		bool showSlotEmpty{};
		bool showControlsHint{};
		std::int32_t controlsHintX{};
		std::int32_t controlsHintY{};
		bool showItemDescription{};
		bool showLastEquippedItem{};
		bool showItemIconDescription{};
		std::int32_t itemIconDescriptionSize{};
		bool staticItemDescription{};
		std::int32_t itemIconDescriptionX{};
		std::int32_t itemIconDescriptionY{};
		std::int32_t itemDescriptionTitleX{};
		std::int32_t itemDescriptionTitleY{};
		std::int32_t itemDescriptionTitleWidth{};
		std::int32_t itemDescriptionTextX{};
		std::int32_t itemDescriptionTextY{};
		bool itemDescriptionTextFollowsTitle{};
		std::int32_t itemDescriptionWidth{};
		float itemDescriptionLineSpacing{};
		bool showSlotGlow{};
		bool showPointer{};
		std::int32_t pointerSize{};
		std::int32_t pointerDistance{};
		std::int32_t pointerRotation{};
		float animationSpeed{};
		std::string skinPath;
		std::string fontPath;
		std::string languagePath;
		bool useI4{};
		bool showHudMessages{};
		bool enablePages{};
		std::int32_t numberOfPages{};
		std::uint32_t nextPageKey{};
		std::uint32_t previousPageKey{};
		std::uint32_t gamepadNextPageKey{};
		std::uint32_t gamepadPreviousPageKey{};
		bool showPageIndicator{};
		std::int32_t pageIndicatorX{};
		std::int32_t pageIndicatorY{};
		std::int32_t pageIndicatorRotation{};
		std::int32_t pageIndicatorColumns{};
		std::int32_t pageIndicatorSpacing{};
		std::int32_t pageIndicatorSize{};
		std::uint32_t pageIndicatorActiveColor{};
		std::uint32_t pageIndicatorInactiveColor{};
		bool enableSlowMotion{};
		float slowMotionTimeSpeed{};
		float slowMotionFadeIn{};
		float slowMotionFadeOut{};
		static constexpr float kMinSensitivity = 0.1f;
		static constexpr float kMaxSensitivity = 5.0f;
		static constexpr std::int32_t kMinMouseDPI = 200;
		static constexpr std::int32_t kMaxMouseDPI = 3200;
		static constexpr float kControllerDeadzone = 0.5f;
		static constexpr float kMinOrbitMenuScale = 0.5f;
		static constexpr float kMaxOrbitMenuScale = 1.5f;
		static constexpr std::int32_t kMinPosition = -2880;
		static constexpr std::int32_t kMaxPosition = 2880;
		static constexpr std::int32_t kMinCurve = -30;
		static constexpr std::int32_t kMaxCurve = 30;
		static constexpr std::int32_t kMinTextSize = 8;
		static constexpr std::int32_t kMaxTextSize = 32;
		static constexpr std::int32_t kMinNumberOfPages = 2;
		static constexpr std::int32_t kMaxNumberOfPages = 10;
		static constexpr std::int32_t kMaxPageIndicatorRotation = 360;
		static constexpr std::int32_t kMaxPageIndicatorSpacing = 100;
		static constexpr std::int32_t kMinPageIndicatorSize = 4;
		static constexpr std::int32_t kMaxPageIndicatorSize = 128;
		static constexpr float kMinSlowMotionTimeSpeed = 0.1f;
		static constexpr float kMaxSlowMotionTimeSpeed = 1.0f;
		static constexpr float kMaxSlowMotionFade = 1.0f;
		static constexpr std::int32_t kMinSlotDistance = 100;
		static constexpr std::int32_t kMaxSlotDistance = 800;
		static constexpr std::int32_t kMinSlotSize = 50;
		static constexpr std::int32_t kMaxSlotSize = 200;
		static constexpr std::int32_t kMinSlotIconSize = 16;
		static constexpr std::int32_t kMaxSlotIconSize = 200;
		static constexpr std::int32_t kMinSlotPosition = -600;
		static constexpr std::int32_t kMaxSlotPosition = 600;
		static constexpr std::int32_t kMinItemDescriptionBackgroundSize = 200;
		static constexpr std::int32_t kMaxItemDescriptionBackgroundSize = 1200;
		static constexpr std::int32_t kMinPointerSize = 8;
		static constexpr std::int32_t kMaxPointerSize = 96;
		static constexpr std::int32_t kMinItemIconDescriptionSize = 16;
		static constexpr std::int32_t kMaxItemIconDescriptionSize = 128;
		static constexpr std::int32_t kMinItemDescriptionPosition = -600;
		static constexpr std::int32_t kMaxItemDescriptionPosition = 600;
		static constexpr std::int32_t kMinItemDescriptionWidth = 100;
		static constexpr std::int32_t kMaxItemDescriptionWidth = 1200;
		static constexpr float kMinItemDescriptionLineSpacing = 0.8f;
		static constexpr float kMaxItemDescriptionLineSpacing = 2.0f;
		static constexpr std::int32_t kMinPointerDistance = -600;
		static constexpr std::int32_t kMaxPointerDistance = 600;
		static constexpr std::int32_t kMaxPointerRotation = 360;
		static constexpr float kMinAnimationSpeed = 0.0f;
		static constexpr float kMaxAnimationSpeed = 2.0f;
		static constexpr auto kLastSlotKeyPosition = static_cast<std::int32_t>(SlotTextPosition::kBottomRight);
		static constexpr auto kLastHandPosition = static_cast<std::int32_t>(SlotTextPosition::kNextToName);
		void Clamp(const Settings &a_valueDefault);
		void CopyDisplay(const Settings &a_from);
		void CopyPages(const Settings &a_from);
		void CopySlowMotion(const Settings &a_from);
		float SelectionDeadzone() const { return 24000.0f / static_cast<float>(mouseDPI); }
		bool operator==(const Settings &) const = default;
	};

	class SettingsStore
	{
	public:
		static SettingsStore &Get();
		Settings Snapshot() const;
		Settings Defaults() const;
		void Set(Settings a_settings);
		void Load();
		bool Save() const;
		std::uint32_t ChangeCount() const { return _changes; }
	private:
		SettingsStore() = default;
		mutable std::mutex _lock;
		Settings _defaults;
		Settings _settings;
		std::atomic<std::uint32_t> _changes{0};
	};
}
