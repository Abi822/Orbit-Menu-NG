#pragma once

struct ImFont;

namespace RMS::UI
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
		static void UpdateFont();
		static void PreloadImages();

		static inline State _state = State::kUninitialized;
		static inline ImFont *_font = nullptr;
		static inline std::filesystem::path _fontFile;
		static inline std::uint32_t _fontGeneration = 0;
		static inline std::atomic<bool> _imagesPending{false};
	};
}
