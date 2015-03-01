//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	%M%

	File Description	:	Converts packet files between the various formats.

	Revision History	:	2015-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4365 4514 4710 4820 4986)
#endif // #ifdef _MSC_VER

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Include necessary header files...
//	////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned int Card;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const Card CardCount    = 52;
const Card SuitCount    =  4;
const Card PerSuitCount = CardCount / SuitCount;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
enum Suit {
	Spades,
	Diamonds,
	Clubs,
	Hearts,
	SuitMin = Spades,
	SuitMax = Hearts
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *RankNames[] = {
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight",
	"Nine",
	"Ten",
	"Jack",
	"Queen",
	"King",
	"Ace"
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *SuitNames[] = {
	"Spades",
	"Diamonds",
	"Clubs",
	"Hearts"
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Card GetValidCard(Card in_card)
{
	if (in_card >= CardCount) {
		std::ostringstream o_str;
		o_str << "The specified card value (" << in_card << ") is out-of-range.";
		throw std::invalid_argument(o_str.str());
	}

	return(in_card);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Card GetValidRank(Card in_card)
{
	return(GetValidCard(in_card) % PerSuitCount);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Suit GetValidSuit(Suit in_suit)
{
	if ((in_suit < SuitMin) || (in_suit > SuitMax)) {
		std::ostringstream o_str;
		o_str << "The specified suit value (" << in_suit << ") is out-of-range.";
		throw std::invalid_argument(o_str.str());
	}

	return(in_suit);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Suit GetValidSuit(Card card)
{
	return(static_cast<Suit>(GetValidCard(card) / PerSuitCount));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetSuitName(Suit in_suit)
{
	return(SuitNames[GetValidSuit(in_suit)]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetSuitName(Card in_card)
{
	return(GetSuitName(GetValidSuit(in_card)));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetRankName(Card in_card)
{
	return(RankNames[GetValidRank(in_card)]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string GetCardName(Card in_card)
{
	std::ostringstream o_str;

	o_str << GetRankName(in_card) << " of " << GetSuitName(in_card);

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::vector<Card>          Deck;
typedef Deck::const_iterator       DeckIterC;

typedef std::vector<Deck>          DeckVector;
typedef DeckVector::const_iterator DeckVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct HandShared {
	static const std::size_t HandSharedSize = 5;

	HandShared(Card card_1, Card card_2, Card card_3, Card card_4, Card card_5)
	{
		card_list_[0] = card_1;
		card_list_[1] = card_2;
		card_list_[2] = card_3;
		card_list_[3] = card_4;
		card_list_[4] = card_5;
	}

	Card & operator [] (int card_index)
	{
		if ((card_index < 0) || (card_index > HandSharedSize))
			throw std::invalid_argument("Invalid card index specified.");

		return(card_list_[card_index]);
	}

	Card card_list_[HandSharedSize];
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::vector<HandShared>          HandSharedVector;
typedef HandSharedVector::const_iterator HandSharedVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Deck ConstructDeck()
{
	struct MyGen {
		MyGen()
			:value_(0)
		{
		}

		Card operator () ()
		{
			return(value_++);
		}

		Card value_;
	};

	Deck  out_deck(CardCount);
	MyGen tmp_gen;

	std::generate(out_deck.begin(), out_deck.end(), tmp_gen);

	return(out_deck);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void GenerateHandSharedVectorImpl(const Deck &in_deck, int shared_card_count,
	int target_element, std::size_t start_index, Deck &working_hand,
	DeckVector &out_deck)
{
	while (start_index < in_deck.size()) {
		working_hand[target_element] = in_deck[start_index++];
		if ((target_element + 1) < shared_card_count)
			GenerateHandSharedVectorImpl(in_deck, shared_card_count,
				target_element + 1, start_index, working_hand, out_deck);
		else // if ((target_element + 1) == shared_card_count)
			out_deck.push_back(working_hand);
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
DeckVector GenerateHandSharedVector(const Deck &in_deck, int shared_card_count)
{
	Deck       working_hand(shared_card_count, 0);
	DeckVector out_deck;

	GenerateHandSharedVectorImpl(in_deck, shared_card_count, 0, 0,
		working_hand, out_deck);

	return(out_deck);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_CheckDeck()
{
	Deck      my_deck(ConstructDeck());
	DeckIterC iter_b(my_deck.begin());
	DeckIterC iter_e(my_deck.end());

	for ( ; iter_b != iter_e; ++iter_b)
		std::cout << GetCardName(*iter_b) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const Deck &in_deck)
{
	DeckIterC iter_b(in_deck.begin());
	DeckIterC iter_e(in_deck.end());

	for ( ; iter_b != iter_e; ++iter_b)
		o_str << "[" << *iter_b << "]";

	return(o_str);
}
//	----------------------------------------------------------------------------
std::ostream & operator << (std::ostream &o_str, const DeckVector &in_deck)
{
	for (std::size_t count_1 = 0; count_1 < in_deck.size(); ++count_1) {
		o_str << std::setw(7) << count_1 << ": " << in_deck[count_1] << std::endl;
	}

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

namespace TEST_GenCombos {

//	////////////////////////////////////////////////////////////////////////////
const Card TEST_ComboList_1[] = { 1, 2, 3, 4, 5 };
//	----------------------------------------------------------------------------
void TEST_EmitDeck(const Deck &in_deck)
{
	DeckIterC iter_b(in_deck.begin());
	DeckIterC iter_e(in_deck.end());

	for ( ; iter_b != iter_e; ++iter_b)
		std::cout << "[" << *iter_b << "]";

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void TEST_EmitDeck(std::size_t expected_hands, const DeckVector &in_deck)
{
	std::set<Deck> unique_hands(in_deck.begin(), in_deck.end());

	std::cout << "Expected Hands: " << expected_hands << std::endl;
	std::cout << "Actual Hands  : " << in_deck.size() << " (" <<
		((expected_hands == in_deck.size()) ? "OK" : "ERROR") <<
		")" << std::endl;
	std::cout << "Unique Hands  : " << unique_hands.size() << " (" <<
		((expected_hands == unique_hands.size()) ? "OK" : "ERROR") <<
		")" << std::endl;

	std::cout << in_deck << std::endl;
}
//	----------------------------------------------------------------------------
void TEST_GenPerms_1()
{
	int       this_iteration = 0;
	Deck      short_deck(TEST_ComboList_1, TEST_ComboList_1 + 3);
//	DeckIterC iter_b(short_deck.begin());
//	DeckIterC iter_e(short_deck.end());

	do {
		std::cout << this_iteration << ": ";
		TEST_EmitDeck(short_deck);
	} while (std::next_permutation(short_deck.begin(), short_deck.end()));
}
//	----------------------------------------------------------------------------
void TEST_GenCombos_1()
{
	Deck short_deck(TEST_ComboList_1, TEST_ComboList_1 + 3);
}
//	----------------------------------------------------------------------------
void TEST_GenSharedHands_1()
{
	Deck short_deck(TEST_ComboList_1, TEST_ComboList_1 + 5);

	/*
		deck! / ((deck - cards)! * (cards!))

		48! / ((48 - 5)! * (5!)) = 1712304
	*/

	{
		// 2 cards from deck of 5 = 10 different hands
		std::size_t expected_hands = 10;
		DeckVector shared_hands(GenerateHandSharedVector(short_deck, 2));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 3 cards from deck of 5 = 10 different hands
		std::size_t expected_hands = 10;
		DeckVector shared_hands(GenerateHandSharedVector(short_deck, 3));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 4 cards from deck of 5 = 5 different hands
		std::size_t expected_hands = 5;
		DeckVector shared_hands(GenerateHandSharedVector(short_deck, 4));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 5 cards from deck of 5 = 1 different hand
		std::size_t expected_hands = 1;
		DeckVector shared_hands(GenerateHandSharedVector(short_deck, 5));
		TEST_EmitDeck(expected_hands, shared_hands);
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_GenCombos

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
//		TEST_CheckDeck();
//TEST_GenCombos::TEST_GenPerms_1();
TEST_GenCombos::TEST_GenSharedHands_1();
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

