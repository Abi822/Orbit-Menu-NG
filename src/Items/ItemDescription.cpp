#include "Items/ItemDescription.h"
#include "Config/Translation.h"

namespace Orbit::Items
{
	namespace
	{
		[[nodiscard]] std::string Upper(std::string_view a_text)
		{
			std::string text{a_text};
			std::ranges::transform(text, text.begin(), [](unsigned char a_char)
								   { return static_cast<char>(std::toupper(a_char)); });
			return text;
		}

		[[nodiscard]] std::string Whole(float a_value)
		{
			return std::to_string(std::lround(a_value));
		}

		[[nodiscard]] std::string Decimal(float a_value)
		{
			const float rounded = std::round(a_value * 10.0f) / 10.0f;
			return rounded == std::floor(rounded) ? Whole(rounded) : std::format("{:.1f}", rounded);
		}

		[[nodiscard]] std::string MagicDescription(RE::MagicItem *a_magic)
		{
			if (!a_magic)
			{
				return {};
			}
			RE::BSString text;
			RE::MagicSystem::GetMagicItemDescription(text, a_magic, "", "");
			return text.c_str() ? text.c_str() : "";
		}

		[[nodiscard]] std::string FormDescription(RE::TESForm &a_form)
		{
			auto *description = a_form.As<RE::TESDescription>();
			if (!description)
			{
				return {};
			}
			RE::BSString text;
			description->GetDescription(text, &a_form);
			return text.c_str() ? text.c_str() : "";
		}

		[[nodiscard]] std::string KnownEffects(RE::IngredientItem &a_ingredient)
		{
			std::string text;
			for (std::uint32_t i = 0; i < a_ingredient.effects.size(); ++i)
			{
				const auto *effect = a_ingredient.effects[i];
				if ((a_ingredient.gamedata.knownEffectFlags & (1u << i)) == 0 || !effect || !effect->baseEffect)
				{
					continue;
				}
				if (!text.empty())
				{
					text += ", ";
				}
				text += effect->baseEffect->GetFullName();
			}
			return text;
		}

		[[nodiscard]] std::string SpellLevel(const RE::MagicItem &a_spell)
		{
			const auto *base = CostliestEffect(a_spell);
			if (!base)
			{
				return {};
			}
			const auto skill = base->GetMinimumSkillLevel();
			if (skill >= 100)
			{
				return Config::Translate("Master");
			}
			if (skill >= 75)
			{
				return Config::Translate("Expert");
			}
			if (skill >= 50)
			{
				return Config::Translate("Adept");
			}
			if (skill >= 25)
			{
				return Config::Translate("Apprentice");
			}
			return Config::Translate("Novice");
		}
	}

	Core::ItemDescription DescribeItem(RE::PlayerCharacter &a_player, RE::TESForm &a_form, std::string_view a_title, ItemKind a_kind, RE::ExtraDataList *a_extraList)
	{
		Core::ItemDescription item{Upper(a_title)};

		if (a_kind == ItemKind::kSpell || a_kind == ItemKind::kPower)
		{
			auto *spell = a_form.As<RE::MagicItem>();
			item.text = MagicDescription(spell);
			if (a_kind == ItemKind::kSpell && spell)
			{
				const bool perSecond = spell->GetCastingType() == RE::MagicSystem::CastingType::kConcentration;
				item.footer.push_back({Config::Translate("lvl"), SpellLevel(*spell)});
				item.footer.push_back({Config::Translate("COST"), Whole(spell->CalculateMagickaCost(&a_player)), perSecond ? Config::Translate("per sec") : ""});
			}
			return item;
		}
		if (a_kind == ItemKind::kShout)
		{
			item.text = FormDescription(a_form);
			return item;
		}

		auto *object = a_form.As<RE::TESBoundObject>();
		if (!object)
		{
			return item;
		}
		RE::InventoryEntryData entry{object, 1};
		entry.AddExtraList(a_extraList);

		std::string line;
		switch (a_kind)
		{
		case ItemKind::kWeapon:
			line = Config::Translate("Deals {} Damage", Whole(a_player.GetDamage(&entry)));
			break;
		case ItemKind::kAmmo:
			line = Config::Translate("Deals {} Damage", Decimal(a_form.As<RE::TESAmmo>()->GetRuntimeData().data.damage));
			break;
		case ItemKind::kShield:
		case ItemKind::kArmor:
			if (const auto armor = std::lround(a_player.GetArmorValue(&entry)); armor > 0)
			{
				line = Config::Translate("Gives {} Armor", armor);
			}
			break;
		default:
			break;
		}

		if (auto *ingredient = a_form.As<RE::IngredientItem>())
		{
			item.text = KnownEffects(*ingredient);
		}
		else if (auto *magic = a_form.As<RE::MagicItem>())
		{
			item.text = MagicDescription(magic);
		}
		else if (auto *enchantment = entry.GetEnchantment())
		{
			item.text = MagicDescription(enchantment);
		}
		else
		{
			item.text = FormDescription(a_form);
		}

		if (!line.empty())
		{
			item.text = item.text.empty() ? line : line + '\n' + item.text;
		}
		return item;
	}

	Core::ItemDescription DescribeMissingItem(std::string_view a_title, std::string_view a_status)
	{
		Core::ItemDescription item{Upper(a_title)};
		item.text = a_status;
		return item;
	}
}
