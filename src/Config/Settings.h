#pragma once

#include "Core/Slots.h"

namespace Orbit::Config
{
	enum class BadgePosition : std::int32_t
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
		// Defaults come from OrbitMenu_Default.ini, then overwritten by user OrbitMenu.ini.
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
		float backgroundOpacity{};
		std::uint32_t backgroundColor{};
		std::uint32_t slotColor{};
		std::uint32_t highlightColor{};
		std::uint32_t slotNameColor{};
		std::uint32_t slotEmptyColor{};
		std::uint32_t slotKeyColor{};
		std::uint32_t handColor{};
		std::uint32_t itemDescriptionColor{};
		std::uint32_t hintColor{};
		std::int32_t slotNameTextSize{};
		std::int32_t slotEmptyTextSize{};
		std::int32_t slotKeyTextSize{};
		std::int32_t handTextSize{};
		std::int32_t itemDescriptionTitleTextSize{};
		std::int32_t itemDescriptionTextSize{};
		std::int32_t hintTextSize{};
		bool showSlotKeys{};
		bool showSlotKeysOnController{};
		std::int32_t slotDistance{};
		std::int32_t slotSize{};
		std::int32_t slotIconSize{};
		bool showSlotBackground{};
		bool showCenterBackground{};
		std::int32_t centerBackgroundSize{};
		std::int32_t slotKeyPosition{};
		bool showHand{};
		std::int32_t handPosition{};
		bool showSlotEmpty{};
		bool showHint{};
		bool showItemDescription{};
		bool showItemIconDescription{};
		std::int32_t itemIconDescriptionSize{};
		bool staticItemDescription{};
		std::int32_t itemIconDescriptionX{};
		std::int32_t itemIconDescriptionY{};
		std::int32_t itemDescriptionTitleX{};
		std::int32_t itemDescriptionTitleY{};
		std::int32_t itemDescriptionTextX{};
		std::int32_t itemDescriptionTextY{};
		std::int32_t itemDescriptionWidth{};
		bool showSlotGlow{};
		bool showPointer{};
		std::int32_t pointerSize{};
		std::int32_t pointerDistance{};
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
		static constexpr std::int32_t kMinPosition = -2000;
		static constexpr std::int32_t kMaxPosition = 2000;
		static constexpr std::int32_t kMinCurve = -30;
		static constexpr std::int32_t kMaxCurve = 30;
		static constexpr float kMinOpacity = 0.0f;
		static constexpr float kMaxOpacity = 0.95f;
		static constexpr std::int32_t kMinTextSize = 8;
		static constexpr std::int32_t kMaxTextSize = 32;
		static constexpr std::int32_t kMinNumberOfPages = 2;
		static constexpr std::int32_t kMaxNumberOfPages = 10;
		static constexpr float kMinSlowMotionTimeSpeed = 0.1f;
		static constexpr float kMaxSlowMotionTimeSpeed = 1.0f;
		static constexpr float kMaxSlowMotionFade = 1.0f;
		static constexpr std::int32_t kMinSlotDistance = 200;
		static constexpr std::int32_t kMaxSlotDistance = 450;
		static constexpr std::int32_t kMinSlotSize = 60;
		static constexpr std::int32_t kMaxSlotSize = 200;
		static constexpr std::int32_t kMinSlotIconSize = 16;
		static constexpr std::int32_t kMaxSlotIconSize = 200;
		static constexpr std::int32_t kMinCenterBackgroundSize = 200;
		static constexpr std::int32_t kMaxCenterBackgroundSize = 1200;
		static constexpr std::int32_t kMinPointerSize = 8;
		static constexpr std::int32_t kMaxPointerSize = 96;
		static constexpr std::int32_t kMinItemIconDescriptionSize = 16;
		static constexpr std::int32_t kMaxItemIconDescriptionSize = 128;
		static constexpr std::int32_t kMinItemDescriptionPosition = -600;
		static constexpr std::int32_t kMaxItemDescriptionPosition = 600;
		static constexpr std::int32_t kMinItemDescriptionWidth = 100;
		static constexpr std::int32_t kMaxItemDescriptionWidth = 1200;
		static constexpr std::int32_t kMinPointerDistance = -50;
		static constexpr std::int32_t kMaxPointerDistance = 150;
		static constexpr float kMinAnimationSpeed = 0.0f;
		static constexpr float kMaxAnimationSpeed = 2.0f;
		static constexpr auto kLastSlotKeyPosition = static_cast<std::int32_t>(BadgePosition::kBottomRight);
		static constexpr auto kLastHandPosition = static_cast<std::int32_t>(BadgePosition::kNextToName);
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
