#include "UI/Skins/Skin.h"
#include "UI/Skins/SkinIconNames.h"
#include <charconv>

namespace Orbit::UI::Skins
{
	namespace
	{
		struct PartName
		{
			std::string_view name;
			Part part;
		};

		constexpr std::array kPartNames{PartName{"orbit_background", Part::kCentre}, PartName{"wheel_background", Part::kCentre},
										PartName{"slot_background", Part::kSlot}, PartName{"slot_highlighted_background", Part::kSlotHovered},
										PartName{"slot_active_background", Part::kSlotEquipped}, PartName{"orbit_slot_glow", Part::kSlotGlow},
										PartName{"orbit_pointer", Part::kPointer}, PartName{"slot_key_background", Part::kSlotKeyBadge},
										PartName{"orbit_indicator_active", Part::kPageIndicatorActive}, PartName{"wheel_indicator_active", Part::kPageIndicatorActive},
										PartName{"orbit_indicator_inactive", Part::kPageIndicatorInactive}, PartName{"wheel_indicator_inactive", Part::kPageIndicatorInactive}};

		std::filesystem::path SkinFolder(std::string_view a_path)
		{
			while (!a_path.empty() && (a_path.front() == '\\' || a_path.front() == '/'))
			{
				a_path.remove_prefix(1);
			}
			const std::filesystem::path path{std::u8string{a_path.begin(), a_path.end()}};
			if (path.empty() || path.has_root_name())
			{
				return {};
			}
			return std::filesystem::path{"Data"} / path;
		}

		template <class Svg>
		void ForEachSkinFile(const std::filesystem::path &a_folder, Svg a_svg)
		{
			std::error_code error;
			for (std::filesystem::recursive_directory_iterator it{a_folder, error}, end; !error && it != end; it.increment(error))
			{
				std::error_code fileError;
				if (!it->is_regular_file(fileError))
				{
					continue;
				}
				const auto extension = it->path().extension();
				if (_wcsicmp(extension.c_str(), L".svg") == 0)
				{
					a_svg(it->path(), SKSE::stl::utf16_to_utf8(it->path().stem().native()).value_or(std::string{}));
				}
			}
		}

		RE::FormID ItemFormID(RE::TESDataHandler &a_data, std::string_view a_name)
		{
			auto separator = a_name.rfind("_0x");
			if (separator == std::string_view::npos)
			{
				separator = a_name.rfind("_0X");
			}
			if (separator == std::string_view::npos)
			{
				return 0;
			}
			const auto digits = a_name.substr(separator + 3);
			RE::FormID localID = 0;
			const auto [end, error] = std::from_chars(digits.data(), digits.data() + digits.size(), localID, 16);
			if (error != std::errc{} || end != digits.data() + digits.size())
			{
				return 0;
			}
			const auto *form = a_data.LookupForm(localID, a_name.substr(0, separator));
			return form ? form->GetFormID() : 0;
		}
	}

	void Skin::Load(const std::string &a_path)
	{
		if (_loaded && a_path == _path)
		{
			return;
		}
		_path = a_path;
		_loaded = true;
		++_generation;
		_parts = {};
		_typeIcons.clear();
		_keywordIcons.clear();
		_itemIcons.clear();

		const auto folder = SkinFolder(a_path);
		if (folder.empty())
		{
			logger::info("Skin: {} is not a folder inside Data, using the built-in look", a_path);
			return;
		}
		Index(folder);

		const auto where = SKSE::stl::utf16_to_utf8(folder.native()).value_or(std::string{});
		const auto parts = std::ranges::count_if(_parts, [](const std::filesystem::path &a_file)
												 { return !a_file.empty(); });
		if (parts == 0 && _typeIcons.empty() && _keywordIcons.empty() && _itemIcons.empty())
		{
			logger::info("Skin: Using the built-in look (no skin files in {})", where);
			return;
		}
		logger::info("Skin: Found {}: {} menu image(s), {} type icon(s), {} item icon(s), {} keyword icon(s)", where, parts,
					 _typeIcons.size(), _itemIcons.size(), _keywordIcons.size());
	}

	const std::filesystem::path &Skin::PartFile(Part a_part) const
	{
		return _parts[static_cast<std::size_t>(a_part)];
	}

	std::filesystem::path Skin::IconFile(const RE::TESForm &a_form) const
	{
		if (const auto item = _itemIcons.find(a_form.GetFormID()); item != _itemIcons.end())
		{
			return item->second;
		}
		if (const auto *keywords = a_form.As<RE::BGSKeywordForm>(); keywords && !_keywordIcons.empty())
		{
			const std::pair<const std::string, std::filesystem::path> *best = nullptr;
			for (std::uint32_t i = 0; i < keywords->numKeywords; ++i)
			{
				const auto *keyword = keywords->keywords[i];
				const char *name = keyword ? keyword->GetFormEditorID() : nullptr;
				if (!name)
				{
					continue;
				}
				const auto found = _keywordIcons.find(std::string_view{name});
				if (found != _keywordIcons.end() && (!best || found->first < best->first))
				{
					best = &*found;
				}
			}
			if (best)
			{
				return best->second;
			}
		}
		if (const auto type = _typeIcons.find(TypeIconName(a_form)); type != _typeIcons.end())
		{
			return type->second;
		}
		return {};
	}

	void Skin::Index(const std::filesystem::path &a_folder)
	{
		auto *data = RE::TESDataHandler::GetSingleton();
		std::array<std::size_t, kPartCount> partRanks{};
		partRanks.fill(kPartNames.size());
		ForEachSkinFile(
			a_folder, [this, data, &partRanks](const std::filesystem::path &a_file, const std::string &a_name)
			{
				const std::string_view name{a_name};
				if (const auto entry = std::ranges::find(kPartNames, name, &PartName::name); entry != kPartNames.end())
				{
					const auto rank = static_cast<std::size_t>(entry - kPartNames.begin());
					const auto part = static_cast<std::size_t>(entry->part);
					if (rank < partRanks[part])
					{
						partRanks[part] = rank;
						_parts[part] = a_file;
					}
				}
				else if (name.starts_with("KWD_"))
				{
					_keywordIcons.emplace(name.substr(4), a_file);
				}
				else if (name.starts_with("FID_"))
				{
					if (const auto formID = data ? ItemFormID(*data, name.substr(4)) : 0; formID != 0)
					{
						_itemIcons.emplace(formID, a_file);
					}
				}
				else
				{
					_typeIcons.emplace(a_name, a_file);
				} });
	}
}
