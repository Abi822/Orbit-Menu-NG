#pragma once

#include <map>
#include <unordered_map>

namespace Orbit::UI::Skins
{
	enum class Part
	{
		kItemDescriptionBackground,
		kSlot,
		kSlotHovered,
		kSlotEquipped,
		kSlotGlow,
		kPointer,
		kSlotKeyBackground,
		kPageIndicatorActive,
		kPageIndicatorInactive
	};

	inline constexpr std::size_t kPartCount = 9;

	class Skin
	{
	public:
		void Load(const std::string &a_path);
		std::uint32_t Generation() const { return _generation; }
		const std::filesystem::path &PartFile(Part a_part) const;
		std::filesystem::path IconFile(const RE::TESForm &a_form) const;

	private:
		void Index(const std::filesystem::path &a_folder);
		std::string _path;
		bool _loaded = false;
		std::uint32_t _generation = 0;
		std::array<std::filesystem::path, kPartCount> _parts{};
		std::map<std::string, std::filesystem::path, std::less<>> _typeIcons;
		std::map<std::string, std::filesystem::path, std::less<>> _keywordIcons;
		std::unordered_map<RE::FormID, std::filesystem::path> _itemIcons;
	};
}
