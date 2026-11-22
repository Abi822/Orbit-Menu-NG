#pragma once

#include <d3d11.h>
#include <imgui.h>
#include <wrl/client.h>

namespace RMS::UI::Graphics
{
	[[nodiscard]] ID3D11Device *GameDevice();

	[[nodiscard]] ID3D11DeviceContext *GameContext();

	struct CachedImage
	{
		ImTextureRef texture;
		ImVec2 uv0{};
		ImVec2 uv1{};
	};

	class GpuTexture
	{
	public:
		class RenderTarget
		{
		public:
			explicit RenderTarget(GpuTexture &a_texture);
			~RenderTarget();

			RenderTarget(const RenderTarget &) = delete;
			RenderTarget &operator=(const RenderTarget &) = delete;

			[[nodiscard]] bool Bound() const noexcept { return _context != nullptr; }

		private:
			ID3D11DeviceContext *_context = nullptr;
			std::array<ID3D11RenderTargetView *, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> _targets{};
			ID3D11DepthStencilView *_depth = nullptr;
			std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> _viewports{};
			UINT _viewportCount = 0;
		};

		bool Create(int a_width, int a_height);
		void Reset();
		void Upload(int a_x, int a_y, int a_size, const std::vector<std::uint8_t> &a_rgba);

		[[nodiscard]] bool Valid() const noexcept { return _view != nullptr; }
		[[nodiscard]] int Width() const noexcept { return _width; }
		[[nodiscard]] int Height() const noexcept { return _height; }
		[[nodiscard]] CachedImage Image(int a_x, int a_y, int a_size) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _view;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _target;
		int _width = 0;
		int _height = 0;
	};

	class PremultipliedDraw
	{
	public:
		explicit PremultipliedDraw(ImDrawList &a_drawList);
		~PremultipliedDraw();

		PremultipliedDraw(const PremultipliedDraw &) = delete;
		PremultipliedDraw &operator=(const PremultipliedDraw &) = delete;

	private:
		ImDrawList &_drawList;
	};

	[[nodiscard]] ImU32 PremultipliedColor(float a_alpha, float a_brightness = 1.0f);
}
