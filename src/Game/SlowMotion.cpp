#include "Game/SlowMotion.h"
#include "Config/Settings.h"

namespace Orbit::Game
{
	namespace
	{
		constexpr float kChangeTolerance = 1e-4f;
		constexpr float kLowestSpeed = 1e-6f;
	}

	SlowMotion &SlowMotion::Get() noexcept
	{
		static SlowMotion singleton;
		return singleton;
	}

	void SlowMotion::Update(bool a_menuOpen)
	{
		if (a_menuOpen != _menuOpen)
		{
			_menuOpen = a_menuOpen;
			const auto settings = Config::SettingsStore::Get().Snapshot();
			if (a_menuOpen && settings.enableSlowMotion)
			{
				FadeTo(settings.slowMotionTimeSpeed, settings.slowMotionFadeIn);
			}
			else if (!a_menuOpen && _active)
			{
				FadeTo(1.0f, settings.slowMotionFadeOut);
			}
		}
		if (!_fading)
		{
			return;
		}
		const float progress = FadeProgress();
		if (progress >= 1.0f)
		{
			Apply(_fadeTo);
			_fading = false;
			_active = _factor < 1.0f;
			return;
		}
		const float eased = progress * progress * (3.0f - 2.0f * progress);
		Apply(_fadeFrom * std::pow(_fadeTo / _fadeFrom, eased));
	}

	void SlowMotion::Reset()
	{
		if (_active)
		{
			Apply(1.0f);
		}
		_menuOpen = false;
		_active = false;
		_fading = false;
	}

	void SlowMotion::FadeTo(float a_factor, float a_seconds)
	{
		if (!_active)
		{
			_others = RE::BSTimer::QGlobalTimeMultiplierTarget();
			_written = _others;
			_ceiling = std::max(1.0f, _others);
			_factor = 1.0f;
			_active = true;
		}
		_fadeFrom = _factor;
		_fadeTo = a_factor;
		_fadeSeconds = a_seconds;
		_fadeStart = Clock::now();
		_fading = true;
	}

	void SlowMotion::Apply(float a_factor)
	{
		auto *timer = RE::BSTimer::GetSingleton();
		if (!timer)
		{
			return;
		}
		if (const float target = RE::BSTimer::QGlobalTimeMultiplierTarget(); std::abs(target - _written) > kChangeTolerance * _written)
		{
			_others = std::min(target / _factor, _ceiling);
		}
		const float speed = std::max(_others * a_factor, kLowestSpeed);
		timer->SetGlobalTimeMultiplier(speed, true);
		_written = speed;
		_factor = a_factor;
	}

	float SlowMotion::FadeProgress() const
	{
		if (_fadeSeconds <= 0.0f)
		{
			return 1.0f;
		}
		return std::min(std::chrono::duration<float>(Clock::now() - _fadeStart).count() / _fadeSeconds, 1.0f);
	}
}
