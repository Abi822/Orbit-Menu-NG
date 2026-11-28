#pragma once

#include "Core/Slots.h"
#include "Input/InputManager.h"

namespace Orbit::UI::OrbitMenu
{
	struct MenuInput
	{
		bool open = false;
		bool justOpened = false;
		bool previewing = false;
		bool gamepad = false;
		bool assign = false;
		int hoveredSlot = -1;
		int lastEquippedSlot = -1;
	};

	struct Motion
	{
		float alpha = 1.0f;
		float scale = 1.0f;
		Core::PerSlot<float> hover{};
		float pointerAngle = 0.0f;
		bool pointerValid = false;
		int flashSlot = -1;
		float flash = 0.0f;
		int StrongestSlot() const;
	};

	class MenuAnimation
	{
	public:
		MenuInput Track(Input::OrbitSnapshot a_snapshot);
		bool Animate(int a_hoveredSlot, float a_speed);
		const Motion &GetMotion() const { return _motion; }
		bool IsVisible() const { return _visible; }
		bool IsPreviewing() const { return _preview.load(); }
		void SetPreview(bool a_preview) { _preview.store(a_preview); }

	private:
		using Clock = std::chrono::steady_clock;
		float Progress(Clock::time_point a_since, float a_seconds, float a_speed) const;
		std::atomic<bool> _preview{false};
		Input::OrbitSnapshot _snapshot{};
		Clock::time_point _now{};
		Clock::time_point _lastFrame{};
		Clock::time_point _openedAt{};
		Clock::time_point _closedAt{};
		float _dt = 0.0f;
		std::uint32_t _seenOpenCount = 0;
		std::uint32_t _seenCloseCount = 0;
		bool _visible = false;
		bool _previewing = false;
		bool _previewEnded = false;
		bool _closing = false;
		bool _closedWithChoice = false;
		int _closingSlot = -1;
		Motion _motion;
	};
}
