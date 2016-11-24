#pragma warning( disable : 4100 4267)

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include "State/State.h"

#include "Manipulators/Manipulators.h"

using namespace State::Cards;

static State::Cards::RawCard GetHero(State::PlayerIdentifier player)
{
	State::Cards::RawCard raw_card;
	raw_card.card_id = 8;
	raw_card.card_type = State::kCardTypeHero;
	raw_card.enchanted_states.zone = State::kCardZonePlay;
	raw_card.enchanted_states.max_hp = 30;
	raw_card.enchanted_states.player = player;
	return raw_card;
}

static void CheckZoneAndPosition(const State::State & state, CardRef ref, State::PlayerIdentifier player, State::CardZone zone, int pos)
{
	auto & item = state.mgr.Get(ref);
	assert(item.GetPlayerIdentifier() == player);
	assert(item.GetZone() == zone);
	assert(item.GetZonePosition() == pos);
}

struct Enchantment1
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier1;

	Enchantments::ApplyFunctor apply_functor;
};
struct Enchantment2
{
	static constexpr EnchantmentTiers tier = kEnchantmentTier2;

	Enchantments::ApplyFunctor apply_functor;
};

static void test1()
{
	State::State state;

	State::Cards::RawCard c1;
	c1.card_type = State::kCardTypeMinion;
	c1.card_id = 1;
	c1.enchanted_states.player = State::kPlayerFirst;
	c1.enchanted_states.zone = State::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, State::Cards::Card(c1));
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZoneDeck, 0);

	c1.enchanted_states.zone = State::kCardZoneGraveyard;
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZoneDeck, 0);

	Manipulators::StateManipulator(state).Minion(r1).GetZoneChanger().ChangeTo<State::kCardZoneHand>(state, State::kPlayerFirst);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZoneHand, 0);

	auto state2 = state;
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZoneHand, 0);

	Manipulators::StateManipulator(state2).Minion(r1).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerFirst, 0);
	CheckZoneAndPosition(state, r1, State::kPlayerFirst, State::kCardZoneHand, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZonePlay, 0);

	Manipulators::StateManipulator(state2).Minion(r1).SetCost(9);
	assert(state.mgr.Get(r1).GetCost() == 5);
	assert(state2.mgr.Get(r1).GetCost() == 9);

	CardRef r2 = state2.mgr.PushBack(state2, State::Cards::Card(c1));
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZoneGraveyard, 0);
	Manipulators::StateManipulator(state2).Minion(r2).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerFirst, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZonePlay, 1);

	CardRef r3 = state2.mgr.PushBack(state2, State::Cards::Card(c1));
	Manipulators::StateManipulator(state2).Minion(r3).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerFirst, 2);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, State::kCardZonePlay, 2);

	CardRef r4 = state2.mgr.PushBack(state2, State::Cards::Card(c1));
	Manipulators::StateManipulator(state2).Minion(r4).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerFirst, 1);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, State::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZonePlay, 2);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, State::kCardZonePlay, 3);

	// steal minion
	Manipulators::StateManipulator(state2).Minion(r1).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerSecond, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, State::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerFirst, State::kCardZonePlay, 2);
	CheckZoneAndPosition(state2, r1, State::kPlayerSecond, State::kCardZonePlay, 0);

	// steal minion
	Manipulators::StateManipulator(state2).Minion(r3).GetZoneChanger().ChangeTo<State::kCardZonePlay>(state2, State::kPlayerSecond, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, State::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerSecond, State::kCardZonePlay, 1);

	// send to graveyard
	Manipulators::StateManipulator(state2).Minion(r1).GetZoneChanger().ChangeTo<State::kCardZoneGraveyard>(state2, State::kPlayerFirst);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, State::kCardZonePlay, 1);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZoneGraveyard, 0);

	// send to another player's graveyard
	Manipulators::StateManipulator(state2).Minion(r2).GetZoneChanger().ChangeTo<State::kCardZoneGraveyard>(state2, State::kPlayerFirst);
	CheckZoneAndPosition(state2, r4, State::kPlayerFirst, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r3, State::kPlayerSecond, State::kCardZonePlay, 0);
	CheckZoneAndPosition(state2, r1, State::kPlayerFirst, State::kCardZoneGraveyard, 0);
	CheckZoneAndPosition(state2, r2, State::kPlayerFirst, State::kCardZoneGraveyard, 1);

	Enchantment1 enchant1{ [](State::Cards::Card & card) {
		card.SetCost(card.GetCost() + 1);
	} };
	Enchantment2 enchant2{ [](State::Cards::Card & card) {
		card.SetCost(card.GetCost() * 2);
	} };

	auto manipulator = Manipulators::StateManipulator(state).Minion(r1);
	auto ref1 = manipulator.Enchant().Add(enchant1);
	auto ref2 = manipulator.Enchant().Add(enchant2);
	auto ref3 = manipulator.Enchant().Add(enchant1);
	auto ref4 = manipulator.Enchant().Add(enchant2);

	auto state3 = state;

	Manipulators::StateManipulator(state3).Minion(r1).Enchant().Remove<Enchantment2>(ref2);
}

class AuraHelper
{
public:
	struct EnchantmentType
	{
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		Enchantments::ApplyFunctor apply_functor;
	};

	AuraHelper(CardRef eligible1, CardRef eligible2) : eligible1_(eligible1), eligible2_(eligible2) {}

	template <typename T>
	bool IsEligible(T&& target)
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");

		if (target == eligible1_) return true;
		if (target == eligible2_) return true;
		return false;
	}

	template <typename Functor>
	void IterateEligibles(Functor&& op)
	{
		op(eligible2_);
		op(eligible1_);
	}

	template <typename T>
	EnchantmentType CreateEnchantmentFor(T&& target)
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");

		return EnchantmentType{ [](State::Cards::Card & card) {
			card.SetCost(card.GetCost() - 1);
		} };
	}

private:
	CardRef eligible1_;
	CardRef eligible2_;
};

static void test2()
{
	State::State state;

	State::Cards::RawCard c1;
	c1.card_type = State::kCardTypeMinion;
	c1.card_id = 1;
	c1.enchanted_states.zone = State::kCardZoneDeck;
	c1.enchanted_states.cost = 5;
	CardRef r1 = state.mgr.PushBack(state, State::Cards::Card(c1));

	State::Cards::RawCard c2;
	c2.card_type = State::kCardTypeMinion;
	c2.card_id = 2;
	c2.enchanted_states.zone = State::kCardZoneDeck;
	c2.enchanted_states.cost = 5;
	CardRef r2 = state.mgr.PushBack(state, State::Cards::Card(c2));

	State::Cards::RawCard c3;
	c3.card_type = State::kCardTypeMinion;
	c3.card_id = 3;
	c3.enchanted_states.zone = State::kCardZoneDeck;
	c3.enchanted_states.cost = 9;
	CardRef r3 = state.mgr.PushBack(state, State::Cards::Card(c3));

	typedef AuraHelper ClientAuraHelper;
	ClientAuraHelper client_aura_helper(r1, r2);
	Manipulators::StateManipulator(state).Minion(r3).Aura().Update(client_aura_helper);

	Manipulators::StateManipulator(state).Minion(r3).Aura().Update(client_aura_helper);

	auto state2 = state;

	ClientAuraHelper client_aura_helper2(r1, r3);
	Manipulators::StateManipulator(state2).Minion(r3).Aura().Update(client_aura_helper2);
}

static void test3()
{
	State::Cards::Card hero1(GetHero(State::kPlayerFirst));
	State::Cards::Card hero2(GetHero(State::kPlayerSecond));

	State::State state;
	
	CardRef r1 = state.mgr.PushBack(state, hero1);
	CardRef r2 = state.mgr.PushBack(state, hero2);
}

int main()
{
	test1();
	test2();
	test3();

	return 0;
}