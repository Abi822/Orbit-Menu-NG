#pragma once

#include "Core/Slots.h"
#include "UI/Graphics/GpuTexture.h"

namespace Orbit::UI::OrbitMenu
{
	struct IconRequest
	{
		std::filesystem::path skinFile;
		std::string source;
		std::string label;
		std::optional<std::uint32_t> tint;
		bool Empty() const { return skinFile.empty() && label.empty(); }
		bool operator==(const IconRequest &) const = default;
	};

	enum class IconSize
	{
		kSlot,
		kItemDescription
	};

	class IconCache
	{
	public:
		void Update(const Core::PerSlot<IconRequest> &a_requests, std::uint32_t a_revision, int a_slotPixels, int a_itemDescriptionPixels);
		std::optional<Graphics::CachedImage> Find(int a_slot, IconSize a_size) const;

	private:
		int IndexOf(const IconRequest &a_request) const;
		int FreeIndex() const;
		void Store(int a_index, const IconRequest &a_request);
		void Draw(const IconRequest &a_request, int a_index);
		int Pixels(IconSize a_size) const;
		int CellX(int a_index) const;
		int CellY(int a_index, IconSize a_size) const;
		Graphics::GpuTexture _texture;
		std::vector<IconRequest> _icons;
		Core::PerSlot<int> _slotIcons{};
		std::uint32_t _revision = 0;
		int _slotPixels = 0;
		int _itemDescriptionPixels = 0;
		int _cell = 0;
	};
}
