#include "Items/EquipController.h"
#include "Items/Inventory.h"
#include "Items/ItemKind.h"
#include "Items/SlotStore.h"

#undef GetObject

namespace Orbit::Items
{
	namespace
	{
		const RE::BGSEquipSlot *HandSlot(bool a_left)
		{
			const auto *objects = RE::BGSDefaultObjectManager::GetSingleton();
			return objects ? objects->GetObject<RE::BGSEquipSlot>(a_left ? RE::DEFAULT_OBJECT::kLeftHandEquip : RE::DEFAULT_OBJECT::kRightHandEquip) : nullptr;
		}

		// Stops player from holding ghost spells
		void UnequipSpell(RE::ActorEquipManager &a_equipManager, RE::Actor &a_actor, RE::SpellItem &a_spell, const RE::BGSEquipSlot *a_slot)
		{
			if (!a_slot)
			{
				return;
			}
			using func_t = void (*)(RE::ActorEquipManager *, RE::Actor *, RE::SpellItem *, const RE::BGSEquipSlot *);
			static REL::Relocation<func_t> func{RELOCATION_ID(37946, 38902)};
			func(&a_equipManager, &a_actor, &a_spell, a_slot);
		}

		void UnequipShout(RE::ActorEquipManager &a_equipManager, RE::Actor &a_actor, RE::TESShout &a_shout)
		{
			using func_t = void (*)(RE::ActorEquipManager *, RE::Actor *, RE::TESShout *);
			static REL::Relocation<func_t> func{RELOCATION_ID(37948, 38904)};
			func(&a_equipManager, &a_actor, &a_shout);
		}

		constexpr std::uint32_t kVoiceSource = 2;

		void UnequipPower(RE::ActorEquipManager &a_equipManager, RE::Actor &a_actor, RE::SpellItem &a_power)
		{
			using func_t = void (*)(RE::ActorEquipManager *, RE::Actor *, RE::SpellItem *, std::uint32_t);
			static REL::Relocation<func_t> func{RELOCATION_ID(37947, 38903)};
			func(&a_equipManager, &a_actor, &a_power, kVoiceSource);
		}

		bool CanChangeHands(const RE::TESForm &a_form)
		{
			return a_form.Is(RE::FormType::Weapon, RE::FormType::Spell, RE::FormType::Scroll) && !IsTwoHanded(a_form);
		}

		bool IsHeldByPressedHand(Input::Hand a_hand, bool a_inLeft, bool a_inRight)
		{
			switch (a_hand)
			{
			case Input::Hand::kLeft:
				return a_inLeft;
			case Input::Hand::kRight:
				return a_inRight;
			case Input::Hand::kBoth:
				return a_inLeft || a_inRight;
			default:
				return false;
			}
		}

		// Handles equipping and unequipping items for player.
		class Equipper
		{
		public:
			static std::optional<Equipper> Create()
			{
				auto *player = RE::PlayerCharacter::GetSingleton();
				auto *equipManager = RE::ActorEquipManager::GetSingleton();
				if (!player || !equipManager)
				{
					return std::nullopt;
				}
				return Equipper{*player, *equipManager};
			}

			void Use(RE::TESForm &a_form, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				switch (GetItemKind(&a_form))
				{
				case ItemKind::kWeapon:
				case ItemKind::kScroll:
				case ItemKind::kShield:
				case ItemKind::kTorch:
					EquipHeld(*a_form.As<RE::TESBoundObject>(), a_slot, a_hand);
					break;
				case ItemKind::kArmor:
					EquipArmor(*a_form.As<RE::TESObjectARMO>(), a_slot, a_hand);
					break;
				case ItemKind::kSpell:
					EquipSpell(*a_form.As<RE::SpellItem>(), a_slot, a_hand);
					break;
				case ItemKind::kPower:
				case ItemKind::kShout:
					EquipPowerOrShout(a_form, a_slot, a_hand);
					break;
				case ItemKind::kAmmo:
					EquipAmmo(*a_form.As<RE::TESAmmo>(), a_slot, a_hand);
					break;
				case ItemKind::kConsumable:
				case ItemKind::kMisc:
					Consume(*a_form.As<RE::TESBoundObject>(), a_slot);
					break;
				default:
					break;
				}
			}

			void SwapHands()
			{
				auto *right = _inventory.HeldItem(kRightHand);
				auto *left = _inventory.HeldItem(kLeftHand);
				if (right == left || (right && !CanChangeHands(*right)) || (left && !CanChangeHands(*left)))
				{
					return;
				}

				auto *rightList = right ? ListForMove(*right, kRightHand) : nullptr;
				auto *leftList = left ? ListForMove(*left, kLeftHand) : nullptr;
				if (!left)
				{
					UnequipItemFromHand(*right, kRightHand);
				}
				if (!right)
				{
					UnequipItemFromHand(*left, kLeftHand);
				}
				if (right)
				{
					EquipItemInHand(*right, rightList, kLeftHand);
				}
				if (left)
				{
					EquipItemInHand(*left, leftList, kRightHand);
				}
				logger::info("Equip Controller: Swapped the items in the left and right hands");
			}

		private:
			Equipper(RE::PlayerCharacter &a_player, RE::ActorEquipManager &a_equipManager) : _player(a_player),
																						_equipManager(a_equipManager),
																						_inventory(a_player)
			{
			}

			void EquipHeld(RE::TESBoundObject &a_object, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				const auto copy = _inventory.FindCarriedCopy(a_object, a_slot.name);
				if (!copy)
				{
					logger::info("Equip Controller: {} is not in inventory", a_slot.name);
					return;
				}

				const bool inRight = _inventory.IsInHand(a_object, a_slot.name, kRightHand);
				const bool inLeft = _inventory.IsInHand(a_object, a_slot.name, kLeftHand);
				const auto equip = [&](const RE::BGSEquipSlot *a_where)
				{ _equipManager.EquipObject(&_player, &a_object, copy->extraList, 1, a_where); };

				const bool leftOnly = a_object.Is(RE::FormType::Armor, RE::FormType::Light);
				if (leftOnly || IsTwoHanded(a_object))
				{
					if (!(leftOnly ? inLeft : inRight))
					{
						equip(nullptr);
					}
					else if (IsHeldByPressedHand(a_hand, inLeft, inRight))
					{
						_equipManager.UnequipObject(&_player, &a_object, _inventory.HeldExtraList(leftOnly));
					}
					return;
				}

				if (a_hand == Input::Hand::kDefault)
				{
					if (!inRight)
					{
						equip(a_object.Is(RE::FormType::Scroll) ? HandSlot(kRightHand) : nullptr);
					}
					return;
				}

				if (a_hand == Input::Hand::kBoth)
				{
					if (inRight && inLeft)
					{
						UnequipFromHand(a_object, kRightHand);
						UnequipFromHand(a_object, kLeftHand);
						return;
					}
					if (copy->total < 2)
					{
						if (!inRight)
						{
							equip(HandSlot(kRightHand));
						}
						return;
					}
					const auto other = inRight ? copy : _inventory.FindCarriedCopy(a_object, a_slot.name, &*copy);
					if (!inRight)
					{
						equip(HandSlot(kRightHand));
					}
					if (!inLeft && other)
					{
						_equipManager.EquipObject(&_player, &a_object, other->extraList, 1, HandSlot(kLeftHand));
					}
					return;
				}

				const bool left = a_hand == Input::Hand::kLeft;
				if (left ? inLeft : inRight)
				{
					UnequipFromHand(a_object, left);
				}
				else
				{
					equip(HandSlot(left));
				}
			}

			void EquipSpell(RE::SpellItem &a_spell, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				if (!_inventory.Knows(a_spell))
				{
					logger::info("Equip Controller: Player doesn't know {}", a_slot.name);
					return;
				}

				const bool inLeft = _inventory.IsInHand(a_spell, a_slot.name, kLeftHand);
				const bool inRight = _inventory.IsInHand(a_spell, a_slot.name, kRightHand);
				const auto equip = [&](bool a_left)
				{ _equipManager.EquipSpell(&_player, &a_spell, HandSlot(a_left)); };

				if (a_spell.IsTwoHanded())
				{
					if (IsHeldByPressedHand(a_hand, inLeft, inRight))
					{
						if (inLeft)
						{
							UnequipSpellFromHand(a_spell, Input::Hand::kLeft);
						}
						if (inRight)
						{
							UnequipSpellFromHand(a_spell, Input::Hand::kRight);
						}
					}
					else
					{
						_equipManager.EquipSpell(&_player, &a_spell, a_spell.GetEquipSlot());
					}
					return;
				}

				if (a_hand == Input::Hand::kDefault)
				{
					if (!inRight)
					{
						equip(kRightHand);
					}
					return;
				}

				const bool useLeft = a_hand != Input::Hand::kRight;
				const bool useRight = a_hand != Input::Hand::kLeft;
				const bool unequip = a_hand == Input::Hand::kBoth ? inLeft && inRight : (useLeft ? inLeft : inRight);
				if (unequip)
				{
					UnequipSpellFromHand(a_spell, a_hand);
					return;
				}
				if (useLeft && !inLeft)
				{
					equip(kLeftHand);
				}
				if (useRight && !inRight)
				{
					equip(kRightHand);
				}
			}

			void EquipPowerOrShout(RE::TESForm &a_form, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				if (!_inventory.Knows(a_form))
				{
					logger::info("Equip Controller: Player doesn't know {}", a_slot.name);
					return;
				}
				const bool unequip = _inventory.IsEquipped(a_form, a_slot.name);
				if (unequip && a_hand == Input::Hand::kDefault)
				{
					return;
				}
				if (auto *shout = a_form.As<RE::TESShout>())
				{
					if (unequip)
					{
						UnequipShout(_equipManager, _player, *shout);
					}
					else
					{
						_equipManager.EquipShout(&_player, shout);
					}
				}
				else if (auto *power = a_form.As<RE::SpellItem>())
				{
					if (unequip)
					{
						UnequipPower(_equipManager, _player, *power);
					}
					else
					{
						_equipManager.EquipSpell(&_player, power, power->GetEquipSlot());
					}
				}
			}

			void EquipArmor(RE::TESObjectARMO &a_armor, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				if (auto *worn = _inventory.WornExtraList(a_armor))
				{
					if (a_hand != Input::Hand::kDefault)
					{
						_equipManager.UnequipObject(&_player, &a_armor, worn);
					}
					return;
				}
				const auto copy = _inventory.FindCarriedCopy(a_armor, a_slot.name);
				if (!copy)
				{
					logger::info("Equip Controller: {} is not in inventory", a_slot.name);
					return;
				}
				_equipManager.EquipObject(&_player, &a_armor, copy->extraList);
			}

			void EquipAmmo(RE::TESAmmo &a_ammo, const AssignedItem &a_slot, Input::Hand a_hand)
			{
				const auto copy = _inventory.FindCarriedCopy(a_ammo, a_slot.name);
				if (!copy)
				{
					logger::info("Equip Controller: {} is not in inventory", a_slot.name);
					return;
				}
				if (_inventory.IsEquipped(a_ammo, a_slot.name))
				{
					if (a_hand != Input::Hand::kDefault)
					{
						auto *worn = _inventory.WornExtraList(a_ammo);
						const auto count = worn ? worn->GetCount() : copy->total;
						_equipManager.UnequipObject(&_player, &a_ammo, worn, static_cast<std::uint32_t>(count));
					}
					return;
				}
				const auto count = copy->extraList ? copy->extraList->GetCount() : copy->total;
				_equipManager.EquipObject(&_player, &a_ammo, copy->extraList, static_cast<std::uint32_t>(count));
			}

			void Consume(RE::TESBoundObject &a_item, const AssignedItem &a_slot)
			{
				const auto copy = _inventory.FindCarriedCopy(a_item, a_slot.name);
				if (!copy)
				{
					logger::info("Equip Controller: Player has no {} left", a_slot.name);
					return;
				}
				const bool misc = a_item.Is(RE::FormType::Misc); //Checks Misc items, so they can equip right away for their scripts run, mods like Skyrim's Got Talent
				_equipManager.EquipObject(&_player, &a_item, copy->extraList, 1, nullptr, !misc, misc); 
			}

			void UnequipFromHand(RE::TESBoundObject &a_object, bool a_leftHand)
			{
				_equipManager.UnequipObject(&_player, &a_object, _inventory.HeldExtraList(a_leftHand), 1, HandSlot(a_leftHand));
			}

			void UnequipSpellFromHand(RE::SpellItem &a_spell, Input::Hand a_hand)
			{
				if (a_hand == Input::Hand::kLeft || a_hand == Input::Hand::kBoth)
				{
					UnequipSpell(_equipManager, _player, a_spell, HandSlot(kLeftHand));
				}
				if (a_hand == Input::Hand::kRight || a_hand == Input::Hand::kBoth)
				{
					UnequipSpell(_equipManager, _player, a_spell, HandSlot(kRightHand));
				}
			}

			RE::ExtraDataList *ListForMove(const RE::TESForm &a_item, bool a_leftHand)
			{
				auto *entry = _player.GetEquippedEntryData(a_leftHand);
				const char *name = entry ? entry->GetDisplayName() : nullptr;
				if (!name || std::string_view{name} == a_item.GetName())
				{
					return nullptr;
				}
				return _inventory.HeldExtraList(a_leftHand);
			}

			void UnequipItemFromHand(RE::TESForm &a_item, bool a_leftHand)
			{
				if (a_item.Is(RE::FormType::Spell))
				{
					UnequipSpellFromHand(*a_item.As<RE::SpellItem>(), a_leftHand ? Input::Hand::kLeft : Input::Hand::kRight);
				}
				else if (auto *object = a_item.As<RE::TESBoundObject>())
				{
					UnequipFromHand(*object, a_leftHand);
				}
			}

			void EquipItemInHand(RE::TESForm &a_item, RE::ExtraDataList *a_extra, bool a_leftHand)
			{
				if (a_item.Is(RE::FormType::Spell))
				{
					_equipManager.EquipSpell(&_player, a_item.As<RE::SpellItem>(), HandSlot(a_leftHand));
				}
				else if (auto *object = a_item.As<RE::TESBoundObject>())
				{
					_equipManager.EquipObject(&_player, object, a_extra, 1, HandSlot(a_leftHand));
				}
			}

			RE::PlayerCharacter &_player;
			RE::ActorEquipManager &_equipManager;
			Inventory _inventory;
		};
	}

	EquipController &EquipController::Get()
	{
		static EquipController instance;
		return instance;
	}

	void EquipController::Register()
	{
		if (auto *events = RE::ScriptEventSourceHolder::GetSingleton())
		{
			events->AddEventSink<RE::TESEquipEvent>(this);
		}
	}

	void EquipController::EquipSlot(int a_slot, Input::Hand a_hand)
	{
		const auto slot = SlotStore::Get().Slot(a_slot);
		auto *form = slot.IsEmpty() ? nullptr : RE::TESForm::LookupByID(slot.formID);
		auto equipper = Equipper::Create();
		if (!form || !equipper)
		{
			return;
		}
		logger::info("Equip Controller: Slot {}: {}", a_slot + 1, slot.name);
		equipper->Use(*form, slot, a_hand);
		++_changes;
	}

	void EquipController::SwapHands()
	{
		if (auto equipper = Equipper::Create())
		{
			equipper->SwapHands();
			++_changes;
		}
	}

	RE::BSEventNotifyControl EquipController::ProcessEvent(const RE::TESEquipEvent *a_event, RE::BSTEventSource<RE::TESEquipEvent> *)
	{
		if (a_event && a_event->actor.get() == RE::PlayerCharacter::GetSingleton())
		{
			++_changes;
		}
		return RE::BSEventNotifyControl::kContinue;
	}
}
