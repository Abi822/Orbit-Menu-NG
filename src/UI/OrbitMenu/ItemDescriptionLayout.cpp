#include "UI/OrbitMenu/ItemDescriptionLayout.h"
#include "UI/OrbitMenu/MenuLayout.h"
#include <cfloat>
#include <ranges>
#include <span>

namespace RMS::UI::OrbitMenu
{
	namespace
	{
		using TextPiece = ItemDescriptionLayout::TextPiece;
		using TextRole = ItemDescriptionLayout::TextRole;

		constexpr float kEdgeInset = 16.0f;
		constexpr float kIconInset = 22.0f;
		constexpr float kIconGap = 10.0f;
		constexpr float kGap = 8.0f;

		constexpr float kNameSize = 22.0f;
		constexpr float kNameLineHeight = 1.1f;
		constexpr std::size_t kNameLines = 2;
		constexpr float kMinNameRatio = 0.7f;

		constexpr float kValueSize = 20.0f;
		constexpr float kLabelRatio = 0.6f;
		constexpr float kLabelSpacingRatio = 0.2f;
		constexpr float kStatSpacingRatio = 0.7f;
		constexpr float kStatRowGapRatio = 0.25f;
		constexpr float kMinRowScale = 0.85f;

		constexpr float kTextSize = 17.0f;
		constexpr float kMinTextSize = 12.0f;
		constexpr float kTextLineHeight = 1.2f;
		constexpr std::size_t kNoLineLimit = SIZE_MAX;

		constexpr std::array kSmallerHeaders{0.9f, 0.8f, 0.7f};
		constexpr int kEvenLineSteps = 8;
		constexpr int kCentringSteps = 3;
		constexpr float kCentringTolerance = 1.0f;

		constexpr std::string_view kSpaces = " \t\r";

		struct Sizes
		{
			float header = 1.0f;
			float text = 0.0f;
		};

		struct Block
		{
			std::vector<TextPiece> pieces;
			float y = 0.0f;
			float bottom = 0.0f;
			bool even = false;
			bool fits = true;

			void Gap(float a_gap)
			{
				if (!pieces.empty())
				{
					y += a_gap;
				}
			}
		};

		struct Words
		{
			std::vector<std::string_view> text;
			std::vector<float> widths;
			float space = 0.0f;
		};

		struct Lines
		{
			std::vector<std::string> text;
			std::vector<float> widths;
			bool fits = true;
		};

		[[nodiscard]] float Apothem(float a_radius) noexcept
		{
			return a_radius * std::cos(Core::kSliceAngle * 0.5f);
		}

		[[nodiscard]] float HalfWidthAt(float a_radius, float a_y) noexcept
		{
			const float half = Core::kSliceAngle * 0.5f;
			const float side = Apothem(a_radius) - kEdgeInset;
			const float diagonal = a_radius * (std::cos(half) + std::sin(half)) - kEdgeInset * std::numbers::sqrt2_v<float> - std::abs(a_y);
			return std::max(0.0f, std::min(side, diagonal));
		}

		[[nodiscard]] float WidthAt(float a_radius, float a_top, float a_height) noexcept
		{
			return 2.0f * std::min(HalfWidthAt(a_radius, a_top), HalfWidthAt(a_radius, a_top + a_height));
		}

		class Placer
		{
		public:
			Placer(const Graphics::TextMeasure &a_text, float a_radius) noexcept : _text(a_text), _radius(a_radius) {}

			[[nodiscard]] Block Place(const Core::ItemDescription &a_description, const Sizes &a_sizes, float a_top, float a_bottom, bool a_even) const
			{
				Block block{.y = a_top, .bottom = a_bottom, .even = a_even};
				const float gap = kGap * a_sizes.header;
				AddName(block, a_description.name, kNameSize * a_sizes.header);
				if (!a_description.footer.empty())
				{
					block.Gap(gap);
					AddStats(block, a_description.footer, kValueSize * a_sizes.header);
				}
				if (!a_description.text.empty())
				{
					block.Gap(gap);
					AddLines(block, a_description.text, a_sizes.text, a_sizes.text * kTextLineHeight, kNoLineLimit, TextRole::kText);
				}
				return block;
			}

		private:
			[[nodiscard]] Words Measure(float a_size, std::string_view a_text) const
			{
				Words words;
				for (const auto line : std::views::split(a_text, '\n'))
				{
					const std::string_view text{line.begin(), line.end()};
					auto start = text.find_first_not_of(kSpaces);
					if (start != std::string_view::npos && !words.text.empty())
					{
						words.text.emplace_back();
					}
					while (start != std::string_view::npos)
					{
						const auto end = text.find_first_of(kSpaces, start);
						words.text.push_back(text.substr(start, end - start));
						start = text.find_first_not_of(kSpaces, end);
					}
				}
				for (const auto word : words.text)
				{
					words.widths.push_back(_text.Width(a_size, word));
				}
				words.space = _text.Width(a_size, " ");
				return words;
			}

			[[nodiscard]] Lines Wrap(const Words &a_words, float a_size, float a_lineHeight, std::size_t a_maxLines, const Block &a_block, float a_maxWidth) const
			{
				Lines lines;
				const auto &words = a_words.text;
				std::size_t next = 0;
				float top = a_block.y;
				while (next < words.size())
				{
					if (top + a_size > a_block.bottom)
					{
						lines.fits = false;
						break;
					}
					const float width = std::min(WidthAt(_radius, top, a_size), a_maxWidth);
					const bool last = lines.text.size() + 1 == a_maxLines || top + a_lineHeight + a_size > a_block.bottom;
					std::string text{words[next]};
					float textWidth = a_words.widths[next];
					for (++next; next < words.size() && !words[next].empty(); ++next)
					{
						const float longer = textWidth + a_words.space + a_words.widths[next];
						if (longer > width)
						{
							break;
						}
						text += ' ';
						text += words[next];
						textWidth = longer;
					}
					if (next < words.size() && words[next].empty())
					{
						++next;
					}
					if ((last && next < words.size()) || textWidth > width)
					{
						_text.Shorten(a_size, text, width);
						textWidth = _text.Width(a_size, text);
						lines.fits = false;
					}
					lines.text.push_back(std::move(text));
					lines.widths.push_back(textWidth);
					if (last)
					{
						break;
					}
					top += a_lineHeight;
				}
				return lines;
			}

			[[nodiscard]] Lines WrapEvenly(const Words &a_words, float a_size, float a_lineHeight, std::size_t a_maxLines, const Block &a_block) const
			{
				Lines lines = Wrap(a_words, a_size, a_lineHeight, a_maxLines, a_block, FLT_MAX);
				if (!lines.fits || lines.text.size() < 2)
				{
					return lines;
				}
				float narrow = std::ranges::max(a_words.widths);
				float wide = std::ranges::max(lines.widths);
				for (int step = 0; step < kEvenLineSteps; ++step)
				{
					const float maxWidth = (narrow + wide) * 0.5f;
					Lines tighter = Wrap(a_words, a_size, a_lineHeight, a_maxLines, a_block, maxWidth);
					if (tighter.fits && tighter.text.size() <= lines.text.size())
					{
						wide = maxWidth;
						lines = std::move(tighter);
					}
					else
					{
						narrow = maxWidth;
					}
				}
				return lines;
			}

			bool AddLines(Block &a_block, std::string_view a_text, float a_size, float a_lineHeight, std::size_t a_maxLines, TextRole a_role) const
			{
				const Words words = Measure(a_size, a_text);
				const Lines lines = a_block.even ? WrapEvenly(words, a_size, a_lineHeight, a_maxLines, a_block)
												 : Wrap(words, a_size, a_lineHeight, a_maxLines, a_block, FLT_MAX);
				float top = a_block.y;
				for (std::size_t i = 0; i < lines.text.size(); ++i)
				{
					a_block.pieces.push_back({lines.text[i], a_size, {-lines.widths[i] * 0.5f, top}, a_role});
					a_block.y = top + a_size;
					top += a_lineHeight;
				}
				a_block.fits = a_block.fits && lines.fits;
				return lines.fits;
			}

			void AddName(Block &a_block, std::string_view a_name, float a_size) const
			{
				const float smallest = a_size * kMinNameRatio;
				for (float size = a_size;; size -= 1.0f)
				{
					Block trial = a_block;
					if (AddLines(trial, a_name, size, size * kNameLineHeight, kNameLines, TextRole::kName) || size - 1.0f < smallest)
					{
						a_block = std::move(trial);
						return;
					}
				}
			}

			[[nodiscard]] float StatWidth(const Core::ItemStat &a_stat, float a_size) const
			{
				const float labelSize = a_size * kLabelRatio;
				float width = _text.Width(a_size, a_stat.value);
				if (!a_stat.label.empty())
				{
					width += _text.Width(labelSize, a_stat.label) + a_size * kLabelSpacingRatio;
				}
				if (!a_stat.suffix.empty())
				{
					width += _text.Width(labelSize, a_stat.suffix) + a_size * kLabelSpacingRatio;
				}
				return width;
			}

			[[nodiscard]] float RowWidth(std::span<const Core::ItemStat> a_row, float a_size) const
			{
				float width = a_size * kStatSpacingRatio * static_cast<float>(a_row.size() - 1);
				for (const auto &stat : a_row)
				{
					width += StatWidth(stat, a_size);
				}
				return width;
			}

			void AddStats(Block &a_block, const std::vector<Core::ItemStat> &a_stats, float a_size) const
			{
				const std::span<const Core::ItemStat> stats{a_stats};
				const float rowGap = a_size * kStatRowGapRatio;

				std::vector<std::span<const Core::ItemStat>> rows;
				float scale = 1.0f;
				float top = a_block.y;
				for (std::size_t next = 0; next < stats.size(); top += a_size + rowGap)
				{
					const float available = WidthAt(_radius, top, a_size);
					std::size_t count = 1;
					while (next + count < stats.size() && RowWidth(stats.subspan(next, count + 1), a_size) * kMinRowScale <= available)
					{
						++count;
					}
					rows.push_back(stats.subspan(next, count));
					scale = std::min(scale, available / RowWidth(rows.back(), a_size));
					next += count;
				}

				const float size = a_size * scale;
				const float labelSize = size * kLabelRatio;
				for (std::size_t i = 0; i < rows.size(); ++i)
				{
					if (i > 0)
					{
						a_block.y += rowGap * scale;
					}
					const float rowTop = a_block.y;
					if (rowTop + size > a_block.bottom)
					{
						a_block.fits = false;
						return;
					}
					const float labelTop = rowTop + (size - labelSize) * 0.8f;
					float x = -RowWidth(rows[i], size) * 0.5f;
					const auto add = [&](const std::string &a_text, float a_textSize, float a_y, TextRole a_role)
					{
						a_block.pieces.push_back({a_text, a_textSize, {x, a_y}, a_role});
						x += _text.Width(a_textSize, a_text);
					};
					for (const auto &stat : rows[i])
					{
						if (!stat.label.empty())
						{
							add(stat.label, labelSize, labelTop, TextRole::kLabel);
							x += size * kLabelSpacingRatio;
						}
						add(stat.value, size, rowTop, TextRole::kValue);
						if (!stat.suffix.empty())
						{
							x += size * kLabelSpacingRatio;
							add(stat.suffix, labelSize, labelTop, TextRole::kLabel);
						}
						x += size * kStatSpacingRatio;
					}
					a_block.y = rowTop + size;
				}
			}

			Graphics::TextMeasure _text;
			float _radius;
		};
	}

	ItemDescriptionLayout ItemDescriptionLayout::Compute(const Graphics::TextMeasure &a_text, const Core::ItemDescription &a_description, float a_textSize, float a_iconSize, float a_centerBackgroundRadius)
	{
		ItemDescriptionLayout layout;
		layout.iconTop = -a_iconSize * 0.5f;
		if (a_description.name.empty())
		{
			return layout;
		}

		const bool withIcon = a_iconSize > 0.0f;
		const float apothem = Apothem(a_centerBackgroundRadius);
		const float lead = withIcon ? a_iconSize + kIconGap : 0.0f;
		const float top = withIcon ? kIconInset + lead - apothem : kEdgeInset - apothem;
		const float bottom = apothem - kEdgeInset;

		const float header = a_textSize / kTextSize;
		const float smallestText = std::min(a_textSize, kMinTextSize);
		std::vector<Sizes> tries;
		for (float size = a_textSize; size > smallestText; size -= 1.0f)
		{
			tries.push_back({header, size});
		}
		tries.push_back({header, smallestText});
		for (const float factor : kSmallerHeaders)
		{
			tries.push_back({header * factor, smallestText * factor});
		}

		const Placer placer{a_text, a_centerBackgroundRadius};
		Sizes sizes;
		for (const auto &tried : tries)
		{
			sizes = tried;
			if (placer.Place(a_description, sizes, top, bottom, false).fits)
			{
				break;
			}
		}

		float blockTop = top;
		Block block = placer.Place(a_description, sizes, top, bottom, true);
		float height = block.y - top;
		for (int step = 0; block.fits && step < kCentringSteps; ++step)
		{
			const float centred = (lead - height) * 0.5f;
			if (centred <= top)
			{
				break;
			}
			Block moved = placer.Place(a_description, sizes, centred, bottom, true);
			if (!moved.fits)
			{
				break;
			}
			const float movedHeight = moved.y - centred;
			if (std::abs(movedHeight - height) < kCentringTolerance)
			{
				blockTop = centred;
				block = std::move(moved);
				break;
			}
			height = movedHeight;
		}

		layout.pieces = std::move(block.pieces);
		layout.iconTop = blockTop - lead;
		return layout;
	}
}
