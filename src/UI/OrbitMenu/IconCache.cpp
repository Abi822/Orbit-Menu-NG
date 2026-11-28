#include "UI/OrbitMenu/IconCache.h"
#include "UI/Graphics/SwfIcon.h"
#include "UI/Skins/SvgImage.h"

namespace Orbit::UI::OrbitMenu
{
	namespace
	{
		constexpr int kGap = 2;
		constexpr int kColumns = 8;
		constexpr int kRows = 2;
		constexpr int kCapacity = kColumns * kRows;
	}

	void IconCache::Update(const Core::PerSlot<IconRequest> &a_requests, std::uint32_t a_revision, int a_slotPixels, int a_itemDescriptionPixels)
	{
		if (a_revision == _revision && a_slotPixels == _slotPixels && a_itemDescriptionPixels == _itemDescriptionPixels)
		{
			return;
		}
		_revision = a_revision;
		if (a_slotPixels != _slotPixels || a_itemDescriptionPixels != _itemDescriptionPixels)
		{
			_slotPixels = a_slotPixels;
			_itemDescriptionPixels = a_itemDescriptionPixels;
			_cell = std::max(a_slotPixels, a_itemDescriptionPixels) + kGap;
			_icons.clear();
			if (_texture.Create(_cell * kColumns, _cell * 2 * kRows))
			{
				_texture.Clear();
			}
		}
		_slotIcons.fill(-1);
		if (!_texture.Valid())
		{
			return;
		}

		for (std::size_t slot = 0; slot < a_requests.size(); ++slot)
		{
			_slotIcons[slot] = IndexOf(a_requests[slot]);
		}

		const Graphics::GpuTexture::RenderTarget target{_texture};
		if (!target.Bound())
		{
			return;
		}
		for (std::size_t slot = 0; slot < a_requests.size(); ++slot)
		{
			const auto &request = a_requests[slot];
			if (request.Empty() || _slotIcons[slot] >= 0)
			{
				continue;
			}
			int index = IndexOf(request);
			if (index < 0)
			{
				index = FreeIndex();
				if (index < 0)
				{
					continue;
				}
				Store(index, request);
			}
			_slotIcons[slot] = index;
		}
	}

	std::optional<Graphics::CachedImage> IconCache::Find(int a_slot, IconSize a_size) const
	{
		if (!Core::IsValidSlot(a_slot) || !_texture.Valid())
		{
			return std::nullopt;
		}
		const int index = _slotIcons[static_cast<std::size_t>(a_slot)];
		if (index < 0)
		{
			return std::nullopt;
		}
		return _texture.Image(CellX(index), CellY(index, a_size), Pixels(a_size));
	}

	int IconCache::IndexOf(const IconRequest &a_request) const
	{
		const auto icon = std::ranges::find(_icons, a_request);
		return icon != _icons.end() ? static_cast<int>(icon - _icons.begin()) : -1;
	}

	int IconCache::FreeIndex() const
	{
		if (static_cast<int>(_icons.size()) < kCapacity)
		{
			return static_cast<int>(_icons.size());
		}
		for (int index = 0; index < kCapacity; ++index)
		{
			if (std::ranges::find(_slotIcons, index) == _slotIcons.end())
			{
				return index;
			}
		}
		return -1;
	}

	void IconCache::Store(int a_index, const IconRequest &a_request)
	{
		const auto index = static_cast<std::size_t>(a_index);
		if (index == _icons.size())
		{
			_icons.push_back(a_request);
		}
		else
		{
			const std::vector<std::uint8_t> blank(static_cast<std::size_t>(_cell) * static_cast<std::size_t>(_cell) * 4);
			_texture.Upload(CellX(a_index), CellY(a_index, IconSize::kSlot), _slotPixels, blank);
			_texture.Upload(CellX(a_index), CellY(a_index, IconSize::kItemDescription), _itemDescriptionPixels, blank);
			_icons[index] = a_request;
		}
		Draw(a_request, a_index);
	}

	void IconCache::Draw(const IconRequest &a_request, int a_index)
	{
		const int x = CellX(a_index);
		if (!a_request.skinFile.empty())
		{
			const Skins::SvgImage image{a_request.skinFile};
			const auto slotIcon = image.Rasterize(_slotPixels);
			const auto itemDescriptionIcon = image.Rasterize(_itemDescriptionPixels);
			if (slotIcon.empty() || itemDescriptionIcon.empty())
			{
				return;
			}
			_texture.Upload(x, CellY(a_index, IconSize::kSlot), _slotPixels, slotIcon);
			_texture.Upload(x, CellY(a_index, IconSize::kItemDescription), _itemDescriptionPixels, itemDescriptionIcon);
			return;
		}
		Graphics::SwfIcon icon{a_request.source, a_request.label, a_request.tint};
		if (!icon.Valid())
		{
			return;
		}
		icon.Render(x, CellY(a_index, IconSize::kSlot), _slotPixels, _texture.Width(), _texture.Height());
		icon.Render(x, CellY(a_index, IconSize::kItemDescription), _itemDescriptionPixels, _texture.Width(), _texture.Height());
	}

	int IconCache::Pixels(IconSize a_size) const
	{
		return a_size == IconSize::kSlot ? _slotPixels : _itemDescriptionPixels;
	}

	int IconCache::CellX(int a_index) const
	{
		return a_index % kColumns * _cell + kGap / 2;
	}

	int IconCache::CellY(int a_index, IconSize a_size) const
	{
		return (a_index / kColumns * 2 + (a_size == IconSize::kSlot ? 0 : 1)) * _cell + kGap / 2;
	}
}
