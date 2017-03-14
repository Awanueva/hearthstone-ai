#pragma once

namespace state {
	namespace detail {
		template <CardType CardType, CardZone CardZone>
		struct InvokeCallback {
			static void Added(state::State & state, state::CardRef card_ref, state::Cards::Card const& card) {}
			static void Removed(state::State & state, state::CardRef card_ref, state::Cards::Card const& card) {}
		};

		template <CardType CardType>
		struct InvokeCallback<CardType, kCardZonePlay> {
			static void Added(state::State & state, state::CardRef card_ref, state::Cards::Card const& card) {
				card.GetRawData().added_to_play_zone({ state, card_ref, card });
			}
			static void Removed(state::State & state, state::CardRef card_ref, state::Cards::Card const& card) {}
		};
	}
}