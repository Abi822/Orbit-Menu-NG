#pragma once

#include "Core/Slots.h"
#include "UI/Graphics/GpuTexture.h"

namespace RMS::UI::OrbitMenu
{
	struct IconRequest
	{
		std::filesystem::path skinFile;
		std::string source;
		std::string label;
		std::optional<std::uint32_t> tint;

		[[nodiscard]] bool Empty() const noexcept { return skinFile.empty() && label.empty(); }

		bool operator==(const IconRequest &) const = default;
	};

	enum class IconSize
	{
		kSlot,
		kCentre
	};

	class IconCache
	{
	public:
		void Update(const Core::PerSlot<IconRequest> &a_requests, std::uint32_t a_revision, int a_slotPixels, int a_centrePixels);

		[[nodiscard]] std::optional<Graphics::CachedImage> Find(int a_slot, IconSize a_size) const;

	private:
		[[nodiscard]] bool Draw(const IconRequest &a_request, int a_slot);
		[[nodiscard]] int Pixels(IconSize a_size) const noexcept;
		[[nodiscard]] int CellX(int a_slot) const noexcept;
		[[nodiscard]] int CellY(IconSize a_size) const noexcept;

		Graphics::GpuTexture _texture;
		Core::PerSlot<bool> _drawn{};
		std::uint32_t _revision = 0;
		int _slotPixels = 0;
		int _centrePixels = 0;
		int _cell = 0;
	};
}
