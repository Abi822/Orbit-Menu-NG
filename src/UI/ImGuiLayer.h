#pragma once

struct ImFont;

namespace Orbit::UI
{
	class ImGuiLayer
	{
	public:
		static void Setup();
		static void Preload();
		static void OnLoadingScreenDrawn();
		static void OnMenuDrawn(bool a_itemMenu);

	private:
		enum class State
		{
			kUninitialized,
			kReady,
			kFailed
		};

		static void Initialize();
		static void PreloadImages();
		static inline State _state = State::kUninitialized;
		static inline ImFont *_font = nullptr;
		static inline std::atomic<bool> _imagesPending{false};
	};
}
