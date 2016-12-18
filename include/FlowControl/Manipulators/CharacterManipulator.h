#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeMinion ||
					card.GetCardType() == state::kCardTypeHero);
			}

			void Cost(int new_cost) { card_.SetCost(new_cost); }
			void Attack(int new_attack) { card_.SetAttack(new_attack); }
			void MaxHP(int new_max_hp) { card_.SetMaxHP(new_max_hp); }

			void Taunt(bool v) { card_.SetTaunt(v); }
			void Shield(bool v) { card_.SetShield(v); }
			void Charge(bool v) { card_.SetCharge(v); }

			detail::DamageSetter Internal_SetDamage() { return detail::DamageSetter(card_); }

			void AfterAttack()
			{
				card_.IncreaseNumAttacksThisTurn();
			}
		};
	}
}