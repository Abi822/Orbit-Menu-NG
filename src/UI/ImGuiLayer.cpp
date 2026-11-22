#include "UI/ImGuiLayer.h"
#include "Game/SlowMotion.h"
#include "Input/InputManager.h"
#include "UI/Graphics/Font.h"
#include "UI/Graphics/GpuTexture.h"
#include "UI/OrbitMenu/OrbitRenderer.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>

namespace RMS::UI
{
	namespace
	{
		[[nodiscard]] ImVec2 ScreenSize()
		{
			const auto screen = RE::BSGraphics::Renderer::GetScreenSize();
			return {static_cast<float>(screen.width), static_cast<float>(screen.height)};
		}
	}

	void ImGuiLayer::Setup()
	{
		OrbitMenu::OrbitRenderer::Get().LoadSkin();
		Initialize();
	}

	void ImGuiLayer::Preload()
	{
		Initialize();
		if (_state != State::kReady)
		{
			return;
		}
		OrbitMenu::OrbitRenderer::Get().PreloadContent(*_font);
		_imagesPending.store(true);
	}

	void ImGuiLayer::OnLoadingScreenDrawn()
	{
		PreloadImages();
	}

	void ImGuiLayer::OnMenuDrawn(bool a_itemMenu)
	{
		PreloadImages();

		auto &input = Input::InputManager::Get();
		auto &renderer = OrbitMenu::OrbitRenderer::Get();
		Game::SlowMotion::Get().Update(input.IsOrbitMenuOpen() && !input.IsAssignMode());

		if (input.IsAssignMode() != a_itemMenu)
		{
			return;
		}
		if ((!input.IsOrbitMenuOpen() && !renderer.IsVisible() && !renderer.IsPreviewing()) || _state != State::kReady)
		{
			return;
		}

		const ImVec2 screen = ScreenSize();
		if (screen.x < 1.0f || screen.y < 1.0f)
		{
			return;
		}

		auto &io = ImGui::GetIO();
		io.DisplaySize = screen;

		float delta = 1.0f / 60.0f;
		if (const auto *timer = RE::BSTimer::GetSingleton(); timer && timer->realTimeDelta > 0.0f)
		{
			delta = timer->realTimeDelta;
		}
		io.DeltaTime = std::clamp(delta, 1.0f / 1000.0f, 0.25f);

		UpdateFont();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		renderer.Draw(*ImGui::GetForegroundDrawList(), *_font, io.DisplaySize);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::Initialize()
	{
		if (_state != State::kUninitialized)
		{
			return;
		}
		auto *device = Graphics::GameDevice();
		auto *context = Graphics::GameContext();
		if (!device || !context)
		{
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto &io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
		io.MouseDrawCursor = false;

		const auto &renderer = OrbitMenu::OrbitRenderer::Get();
		_fontFile = renderer.SkinFont();
		_fontGeneration = renderer.SkinGeneration();
		_font = Graphics::LoadFont(*io.Fonts, _fontFile);

		if (!_font || !ImGui_ImplDX11_Init(device, context))
		{
			logger::error("ImGui: Initialisation failed for Orbit Menu!");
			ImGui::DestroyContext();
			_state = State::kFailed;
			return;
		}
		ImGui_ImplDX11_CreateDeviceObjects();

		_state = State::kReady;
		logger::info("ImGui: {} initialised for Orbit Menu", IMGUI_VERSION);
	}

	void ImGuiLayer::UpdateFont()
	{
		const auto &renderer = OrbitMenu::OrbitRenderer::Get();
		if (renderer.SkinGeneration() == _fontGeneration)
		{
			return;
		}
		_fontGeneration = renderer.SkinGeneration();
		if (renderer.SkinFont() == _fontFile)
		{
			return;
		}
		_fontFile = renderer.SkinFont();
		auto &atlas = *ImGui::GetIO().Fonts;
		if (ImFont *font = Graphics::LoadFont(atlas, _fontFile))
		{
			atlas.RemoveFont(_font);
			_font = font;
		}
	}

	void ImGuiLayer::PreloadImages()
	{
		if (!_imagesPending.load(std::memory_order_relaxed) || !_imagesPending.exchange(false))
		{
			return;
		}
		const ImVec2 screen = ScreenSize();
		if (screen.x < 1.0f || screen.y < 1.0f)
		{
			return;
		}
		OrbitMenu::OrbitRenderer::Get().UpdateImages(screen);
		ImGui_ImplDX11_UpdateTexture(ImGui::GetIO().Fonts->TexData);
	}
}
