#include "UI/OrbitMenu/IconCache.h"
#include "UI/Graphics/SwfIcon.h"
#include "UI/Skins/SvgImage.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kGap = 2;
		constexpr int kSlots = Core::kSlotCount;
	}

	void IconCache::Update(const Core::PerSlot<IconRequest> &a_requests, std::uint32_t a_revision, int a_slotPixels, int a_centrePixels)
	{
		if (a_revision == _revision && a_slotPixels == _slotPixels && a_centrePixels == _centrePixels)
		{
			return;
		}
		_revision = a_revision;
		_slotPixels = a_slotPixels;
		_centrePixels = a_centrePixels;
		_cell = std::max(a_slotPixels, a_centrePixels) + kGap;
		_drawn.fill(false);

		if (!_texture.Create(_cell * kSlots, _cell * 2))
		{
			return;
		}
		const Graphics::GpuTexture::RenderTarget target{_texture};
		if (!target.Bound())
		{
			return;
		}
		for (int slot = 0; slot < kSlots; ++slot)
		{
			_drawn[static_cast<std::size_t>(slot)] = Draw(a_requests[static_cast<std::size_t>(slot)], slot);
		}
	}

	std::optional<Graphics::CachedImage> IconCache::Find(int a_slot, IconSize a_size) const
	{
		if (!Core::IsValidSlot(a_slot) || !_drawn[static_cast<std::size_t>(a_slot)] || !_texture.Valid())
		{
			return std::nullopt;
		}
		return _texture.Image(CellX(a_slot), CellY(a_size), Pixels(a_size));
	}

	bool IconCache::Draw(const IconRequest &a_request, int a_slot)
	{
		if (a_request.Empty())
		{
			return false;
		}
		const int x = CellX(a_slot);
		if (!a_request.skinFile.empty())
		{
			const Skins::SvgImage image{a_request.skinFile};
			const auto slotIcon = image.Rasterize(_slotPixels);
			const auto centreIcon = image.Rasterize(_centrePixels);
			if (slotIcon.empty() || centreIcon.empty())
			{
				return false;
			}
			_texture.Upload(x, CellY(IconSize::kSlot), _slotPixels, slotIcon);
			_texture.Upload(x, CellY(IconSize::kCentre), _centrePixels, centreIcon);
			return true;
		}
		Graphics::SwfIcon icon{a_request.source, a_request.label, a_request.tint};
		if (!icon.Valid())
		{
			return false;
		}
		icon.Render(x, CellY(IconSize::kSlot), _slotPixels, _texture.Width(), _texture.Height());
		icon.Render(x, CellY(IconSize::kCentre), _centrePixels, _texture.Width(), _texture.Height());
		return true;
	}

	int IconCache::Pixels(IconSize a_size) const
	{
		return a_size == IconSize::kSlot ? _slotPixels : _centrePixels;
	}

	int IconCache::CellX(int a_slot) const
	{
		return a_slot * _cell + kGap / 2;
	}

	int IconCache::CellY(IconSize a_size) const
	{
		return (a_size == IconSize::kSlot ? 0 : _cell) + kGap / 2;
	}
}
