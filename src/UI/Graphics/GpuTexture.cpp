#include "UI/Graphics/GpuTexture.h"
#include <imgui_impl_dx11.h>

namespace Orbit::UI::Graphics
{
	ID3D11Device *GameDevice()
	{
		auto *renderer = RE::BSGraphics::Renderer::GetSingleton();
		return renderer ? reinterpret_cast<ID3D11Device *>(renderer->GetRuntimeData().forwarder) : nullptr;
	}

	ID3D11DeviceContext *GameContext()
	{
		auto *renderer = RE::BSGraphics::Renderer::GetSingleton();
		return renderer ? reinterpret_cast<ID3D11DeviceContext *>(renderer->GetRuntimeData().context) : nullptr;
	}

	namespace
	{
		[[nodiscard]] ID3D11BlendState *PremultipliedBlend()
		{
			static ID3D11BlendState *blend = nullptr;
			if (!blend)
			{
				if (auto *device = GameDevice())
				{
					D3D11_BLEND_DESC desc{};
					auto &target = desc.RenderTarget[0];
					target.BlendEnable = TRUE;
					target.SrcBlend = D3D11_BLEND_ONE;
					target.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
					target.BlendOp = D3D11_BLEND_OP_ADD;
					target.SrcBlendAlpha = D3D11_BLEND_ONE;
					target.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
					target.BlendOpAlpha = D3D11_BLEND_OP_ADD;
					target.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					device->CreateBlendState(&desc, &blend);
				}
			}
			return blend;
		}

		void SetBlend(const ImDrawList *, const ImDrawCmd *a_command)
		{
			auto *state = static_cast<ImGui_ImplDX11_RenderState *>(ImGui::GetPlatformIO().Renderer_RenderState);
			if (state && state->DeviceContext && a_command->UserCallbackData)
			{
				state->DeviceContext->OMSetBlendState(static_cast<ID3D11BlendState *>(a_command->UserCallbackData), nullptr, 0xFFFFFFFF);
			}
		}
	}

	GpuTexture::RenderTarget::RenderTarget(GpuTexture &a_texture)
	{
		auto *context = GameContext();
		if (!context || !a_texture._target)
		{
			return;
		}
		_context = context;
		_viewportCount = static_cast<UINT>(_viewports.size());
		_context->RSGetViewports(&_viewportCount, _viewports.data());
		_context->OMGetRenderTargets(static_cast<UINT>(_targets.size()), _targets.data(), &_depth);

		ID3D11RenderTargetView *target = a_texture._target.Get();
		_context->OMSetRenderTargets(1, &target, nullptr);
		constexpr float kClear[4]{0.0f, 0.0f, 0.0f, 0.0f};
		_context->ClearRenderTargetView(target, kClear);
		const D3D11_VIEWPORT viewport{0.0f, 0.0f, static_cast<float>(a_texture._width), static_cast<float>(a_texture._height), 0.0f, 1.0f};
		_context->RSSetViewports(1, &viewport);
	}

	GpuTexture::RenderTarget::~RenderTarget()
	{
		if (!_context)
		{
			return;
		}
		_context->OMSetRenderTargets(static_cast<UINT>(_targets.size()), _targets.data(), _depth);
		_context->RSSetViewports(_viewportCount, _viewports.data());
		for (auto *target : _targets)
		{
			if (target)
			{
				target->Release();
			}
		}
		if (_depth)
		{
			_depth->Release();
		}
	}

	bool GpuTexture::Create(int a_width, int a_height)
	{
		if (Valid() && a_width == _width && a_height == _height)
		{
			return true;
		}
		Reset();
		auto *device = GameDevice();
		if (!device || a_width < 1 || a_height < 1)
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = static_cast<UINT>(a_width);
		desc.Height = static_cast<UINT>(a_height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		if (FAILED(device->CreateTexture2D(&desc, nullptr, &_texture)) ||
			FAILED(device->CreateShaderResourceView(_texture.Get(), nullptr, &_view)) ||
			FAILED(device->CreateRenderTargetView(_texture.Get(), nullptr, &_target)))
		{
			logger::error("Gpu Texture: Could not create a {}x{} texture for Orbit Menu images!", a_width, a_height);
			Reset();
			return false;
		}
		_width = a_width;
		_height = a_height;
		return true;
	}

	void GpuTexture::Reset()
	{
		_target.Reset();
		_view.Reset();
		_texture.Reset();
		_width = 0;
		_height = 0;
	}

	void GpuTexture::Upload(int a_x, int a_y, int a_size, const std::vector<std::uint8_t> &a_rgba)
	{
		auto *context = GameContext();
		const auto bytes = static_cast<std::size_t>(a_size) * static_cast<std::size_t>(a_size) * 4;
		if (!context || !_texture || a_size < 1 || a_rgba.size() < bytes || a_x + a_size > _width || a_y + a_size > _height)
		{
			return;
		}
		const D3D11_BOX box{static_cast<UINT>(a_x), static_cast<UINT>(a_y), 0, static_cast<UINT>(a_x + a_size), static_cast<UINT>(a_y + a_size), 1};
		context->UpdateSubresource(_texture.Get(), 0, &box, a_rgba.data(), static_cast<UINT>(a_size * 4), 0);
	}

	CachedImage GpuTexture::Image(int a_x, int a_y, int a_size) const
	{
		const float width = static_cast<float>(std::max(_width, 1));
		const float height = static_cast<float>(std::max(_height, 1));
		const auto id = static_cast<ImTextureID>(reinterpret_cast<std::uintptr_t>(_view.Get()));
		return {ImTextureRef{id}, {static_cast<float>(a_x) / width, static_cast<float>(a_y) / height}, {static_cast<float>(a_x + a_size) / width, static_cast<float>(a_y + a_size) / height}};
	}

	PremultipliedDraw::PremultipliedDraw(ImDrawList &a_drawList) : _drawList(a_drawList)
	{
		_drawList.AddCallback(SetBlend, PremultipliedBlend());
	}

	PremultipliedDraw::~PremultipliedDraw()
	{
		_drawList.AddCallback(ImGui::GetPlatformIO().DrawCallback_ResetRenderState, nullptr);
	}

	ImU32 PremultipliedColor(float a_alpha, float a_brightness)
	{
		const float alpha = std::clamp(a_alpha, 0.0f, 1.0f);
		const float value = std::clamp(a_brightness, 0.0f, 1.0f) * alpha;
		return ImGui::ColorConvertFloat4ToU32({value, value, value, alpha});
	}
}
