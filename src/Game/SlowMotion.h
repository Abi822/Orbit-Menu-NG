#pragma once

namespace Orbit::Game
{
	class SlowMotion
	{
	public:
		static SlowMotion &Get();

		void Update(bool a_menuOpen);
		void Reset();

	private:
		using Clock = std::chrono::steady_clock;

		SlowMotion() = default;

		void FadeTo(float a_factor, float a_seconds);
		void Apply(float a_factor);
		float FadeProgress() const;

		bool _menuOpen = false;
		bool _active = false;
		bool _fading = false;
		float _factor = 1.0f;
		float _fadeFrom = 1.0f;
		float _fadeTo = 1.0f;
		float _fadeSeconds = 0.0f;
		Clock::time_point _fadeStart{};
		float _baseSpeed = 1.0f;
		float _lastSpeed = 1.0f;
		float _maxBaseSpeed = 1.0f;
	};
}
