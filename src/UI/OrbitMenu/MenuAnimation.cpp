#include "UI/OrbitMenu/MenuAnimation.h"
#include "Core/OrbitMath.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr float kOpenSeconds = 0.14f;
		constexpr float kCloseSeconds = 0.18f;
		constexpr float kHoverFollow = 16.0f;

		float EaseOutCubic(float a_t)
		{
			const float t = std::clamp(a_t, 0.0f, 1.0f);
			const float inv = 1.0f - t;
			return 1.0f - inv * inv * inv;
		}

		float AngleDelta(float a_from, float a_to)
		{
			float delta = std::fmod(a_to - a_from, Core::kTwoPi);
			if (delta > Core::kPi)
			{
				delta -= Core::kTwoPi;
			}
			else if (delta <= -Core::kPi)
			{
				delta += Core::kTwoPi;
			}
			return delta;
		}
	}

	int Motion::StrongestSlot() const
	{
		return static_cast<int>(std::ranges::max_element(hover) - hover.begin());
	}

	MenuInput MenuAnimation::Track(Input::OrbitSnapshot a_snapshot)
	{
		_now = Clock::now();
		_dt = _lastFrame == Clock::time_point{} ? 0.0f : std::clamp(std::chrono::duration<float>(_now - _lastFrame).count(), 0.0f, 0.1f);
		_lastFrame = _now;
		const bool previewing = !a_snapshot.open && _preview.load();
		const bool previewStarted = previewing && !_previewing;
		_previewEnded = !previewing && _previewing;
		_previewing = previewing;
		if (previewing)
		{
			a_snapshot.open = true;
			a_snapshot.gamepad = false;
			a_snapshot.assign = false;
		}

		const bool opened = a_snapshot.open && (a_snapshot.openCount != _seenOpenCount || previewStarted);
		if (opened)
		{
			_seenOpenCount = a_snapshot.openCount;
			_openedAt = _now;
			_closing = false;
			_motion.hover.fill(0.0f);
			_motion.pointerValid = false;
		}
		_snapshot = a_snapshot;
		return {.open = a_snapshot.open,
				.justOpened = opened,
				.previewing = previewing,
				.gamepad = a_snapshot.gamepad,
				.assign = a_snapshot.assign,
				.hoveredSlot = a_snapshot.hoveredSlot};
	}

	bool MenuAnimation::Animate(int a_hoveredSlot, float a_speed)
	{
		if (!_snapshot.open && (_snapshot.closeCount != _seenCloseCount || _previewEnded))
		{
			_seenCloseCount = _snapshot.closeCount;
			if (_visible)
			{
				_closing = true;
				_closedAt = _now;
				_closedWithChoice = !_previewEnded && _snapshot.lastCloseChoseSlot;
				_closingSlot = _snapshot.hoveredSlot;
			}
		}

		float closeProgress = 0.0f;
		if (_snapshot.open)
		{
			const float appear = EaseOutCubic(Progress(_openedAt, kOpenSeconds, a_speed));
			_motion.alpha = appear;
			_motion.scale = 0.92f + 0.08f * appear;
		}
		else if (_closing)
		{
			closeProgress = Progress(_closedAt, kCloseSeconds, a_speed);
			if (closeProgress >= 1.0f)
			{
				_closing = false;
				_visible = false;
				return false;
			}
			_motion.alpha = 1.0f - closeProgress * closeProgress;
			_motion.scale = 1.0f + 0.03f * closeProgress;
		}
		else
		{
			_visible = false;
			return false;
		}
		_visible = true;

		const int target = _snapshot.open ? a_hoveredSlot : (_closedWithChoice ? _closingSlot : -1);
		const float ease = a_speed > 0.0f ? 1.0f - std::exp(-_dt * kHoverFollow * a_speed) : 1.0f;
		for (std::size_t i = 0; i < _motion.hover.size(); ++i)
		{
			const float goal = static_cast<int>(i) == target ? 1.0f : 0.0f;
			_motion.hover[i] += (goal - _motion.hover[i]) * ease;
		}
		if (target >= 0)
		{
			const float goal = Core::SlotCenterAngle(target);
			_motion.pointerAngle = _motion.pointerValid ? _motion.pointerAngle + AngleDelta(_motion.pointerAngle, goal) * ease : goal;
			_motion.pointerValid = true;
		}
		_motion.flashSlot = _closing && _closedWithChoice ? _closingSlot : -1;
		_motion.flash = _motion.flashSlot >= 0 ? 1.0f - closeProgress : 0.0f;
		return true;
	}

	float MenuAnimation::Progress(Clock::time_point a_since, float a_seconds, float a_speed) const
	{
		return a_speed > 0.0f ? std::chrono::duration<float>(_now - a_since).count() * a_speed / a_seconds : 1.0f;
	}
}
