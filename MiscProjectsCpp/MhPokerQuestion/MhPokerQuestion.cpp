//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	MhPokerQuestion.cpp

	File Description	:	Matt Hostetter's poker question.

	Revision History	:	2015-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning(disable:4514 4710)
# pragma warning(push)
# pragma warning(disable:4061 4350 4365 4514 4625 4626 4710 4820 4986)
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

#include <boost/io/ios_state.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//	////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning(pop)
#endif // #ifdef _MSC_VER

//	////////////////////////////////////////////////////////////////////////////
std::ostream &EmitSep(char sep_char = '=', std::streamsize sep_length = 79,
	std::ostream &o_str = std::cout)
{
	boost::io::ios_all_saver ios_saver(o_str);

	o_str << std::setfill(sep_char) << std::setw(sep_length) << "" << std::endl;

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned char Card;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const Card CardCount    = 52;
const Card SuitCount    =  4;
const Card PerSuitCount = CardCount / SuitCount;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	CODE NOTE: Using bridge order (ascending alphabetic).
enum Suit {
	Clubs,
	Diamonds,
	Hearts,
	Spades,
	SuitMin = Clubs,
	SuitMax = Spades
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	CODE NOTE: Must change if order of enum Suit changes...
const char *SuitNames[] = {
	"Clubs",
	"Diamonds",
	"Hearts",
	"Spades"
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	CODE NOTE: Must change if order of enum Suit changes...
const char SuitToCharMap[] = {
#ifdef _MSC_VER
	'\x5',
	'\x4',
	'\x3',
	'\x6'
#else
	'C',
	'D',
	'H',
	'S'
#endif // #ifdef _MSC_VER
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
const char *RankNamesShort[] = {
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"J",
	"Q",
	"K",
	"A"
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
	return(static_cast<Card>(GetValidCard(in_card) % PerSuitCount));
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
char GetSuitChar(Suit in_suit)
{
	return(SuitToCharMap[GetValidSuit(in_suit)]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
char GetSuitChar(Card in_card)
{
	return(GetSuitChar(GetValidSuit(in_card)));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetRankName(Card in_card)
{
	return(RankNames[GetValidRank(in_card)]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetRankNameShort(Card in_card)
{
	return(RankNamesShort[GetValidRank(in_card)]);
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
std::string GetCardNameShort(Card in_card, bool padded_flag = false)
{
	std::ostringstream o_str;

	if (padded_flag)
		o_str << std::right << std::setw(2) << GetRankNameShort(in_card);
	else
		o_str << GetRankNameShort(in_card);

	o_str << "-" << GetSuitChar(in_card);

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
Deck ConstructDeck(bool shuffle_flag = false)
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

	if (shuffle_flag)
		std::random_shuffle(out_deck.begin(), out_deck.end());

	return(out_deck);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream &EmitDeck(const Deck &in_deck, bool by_name = true,
	bool use_short_name = false, std::ostream &o_str = std::cout)
{
	DeckIterC iter_b(in_deck.begin());
	DeckIterC iter_e(in_deck.end());

	for ( ; iter_b != iter_e; ++iter_b) {
		if (by_name)
			o_str << ((iter_b != in_deck.begin()) ? "|" : "") <<
				((use_short_name) ? GetCardNameShort(*iter_b, true) :
				GetCardName(*iter_b));
		else
			o_str << "[" << static_cast<unsigned int>(*iter_b) << "]";
	}

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream &EmitDeck(const DeckVector &in_deck, bool by_name = true,
	bool use_short_name = false, std::ostream &o_str = std::cout)
{
	for (std::size_t count_1 = 0; count_1 < in_deck.size(); ++count_1) {
		o_str << std::setw(7) << count_1 << ": ";
		EmitDeck(in_deck[count_1], by_name, use_short_name, o_str);
		o_str << std::endl;
	}

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream &EmitDeckByLine(const Deck &in_deck,
	std::ostream &o_str = std::cout)
{
	DeckIterC iter_b(in_deck.begin());
	DeckIterC iter_e(in_deck.end());

	for (int count_1 = 0; iter_b != iter_e; ++iter_b, ++count_1)
		o_str << std::setw(2) << count_1 << " ---> " <<
			GetCardNameShort(*iter_b, true) << " ---> " << GetCardName(*iter_b) <<
			std::endl;

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const Deck &in_deck)
{
	return(EmitDeck(in_deck, true, false, o_str));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const DeckVector &in_deck)
{
	return(EmitDeck(in_deck, true, false, o_str));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t CalcSharedHandCount(std::size_t deck_size,
	std::size_t shared_card_count)
{
	if ((deck_size < 1) || (deck_size > 48))
		throw std::invalid_argument("Deck size not in range 1 to 48 inclusive.");
	else if (!shared_card_count)
		throw std::invalid_argument("Shared card count equals zero.");
	else if (shared_card_count > deck_size)
		throw std::invalid_argument("Shared card count exceeds deck size.");

	if (deck_size == shared_card_count)
		return(1);

	std::size_t size_diff = deck_size - shared_card_count;
	double fact_deck      = 1.0;
	double fact_diff      = 0.0;	//	To prevent compiler complaints only.
	double fact_cards     = 0.0;	//	To prevent compiler complaints only.

	for (std::size_t count_1 = 1; count_1 <= deck_size; ++count_1) {
		fact_deck *= count_1;
		if (count_1 == size_diff)
			fact_diff = fact_deck;
		if (count_1 == shared_card_count)
			fact_cards = fact_deck;
	}

	return(static_cast<std::size_t>(fact_deck / (fact_diff * fact_cards)));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t CalcSharedHandCount(const Deck &deck, std::size_t shared_card_count)
{
	return(CalcSharedHandCount(deck.size(), shared_card_count));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename PerHandFunctor>
	bool GenerateSharedHands(const Deck &in_deck,
		std::size_t shared_card_count, std::size_t target_element,
		std::size_t start_index, Deck &working_hand, PerHandFunctor &hand_func)
{
	while (start_index < in_deck.size()) {
		working_hand[target_element] = in_deck[start_index++];
		if ((target_element + 1) < shared_card_count)
			GenerateSharedHands(in_deck, shared_card_count, target_element + 1,
				start_index, working_hand, hand_func);
		else if (!hand_func(working_hand))
			return(false);
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename PerHandFunctor>
	bool GenerateSharedHands(const Deck &in_deck, std::size_t shared_card_count,
		PerHandFunctor &hand_func)
{
	Deck working_hand(shared_card_count, 0);

	return(GenerateSharedHands(in_deck, shared_card_count, 0, 0,
		working_hand, hand_func));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
DeckVector GenerateSharedHandsVector(const Deck &in_deck,
	std::size_t shared_card_count)
{
	struct PerHandAccumulator {
		PerHandAccumulator(const Deck &in_deck, std::size_t shared_card_count)
			:out_deck_()
		{
			out_deck_.reserve(CalcSharedHandCount(in_deck, shared_card_count));
		}

		inline bool operator () (const Deck &working_hand)
		{
			out_deck_.push_back(working_hand);

			return(true);
		}

		DeckVector out_deck_;
	};

	PerHandAccumulator my_func(in_deck, shared_card_count);

	GenerateSharedHands(in_deck, shared_card_count, my_func);

	return(my_func.out_deck_);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_CheckDeck(bool shuffle_flag = false)
{
	EmitSep('=');
	std::cout << "Check Deck (" << ((shuffle_flag) ? "" : "not ") <<
		"shuffled)" << std::endl;
	EmitSep('-');

	Deck my_deck(ConstructDeck(shuffle_flag));

	EmitDeckByLine(my_deck);

	EmitSep('=');

	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

namespace TEST_GenCombos {

//	////////////////////////////////////////////////////////////////////////////
const Card TEST_ComboList_1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//	----------------------------------------------------------------------------
void TEST_EmitDeck(std::size_t expected_hands, const DeckVector &in_deck)
{
	std::set<Deck> unique_hands(in_deck.begin(), in_deck.end());
	DeckVector     order_deck(unique_hands.begin(), unique_hands.end());

	std::cout << "Expected Hands : " << expected_hands << std::endl;
	std::cout << "Actual Hands   : " << in_deck.size() << " (" <<
		((expected_hands == in_deck.size()) ? "OK" : "ERROR") <<
		")" << std::endl;
	std::cout << "Unique Hands   : " << unique_hands.size() << " (" <<
		((expected_hands == unique_hands.size()) ? "OK" : "ERROR") <<
		")" << std::endl;
	std::cout << "Canonical Order: " << ((in_deck == order_deck) ? "TRUE" :
		"FALSE") << std::endl;

	EmitDeck(in_deck, false, false, std::cout);
}
//	----------------------------------------------------------------------------
void TEST_EmitHeader(std::size_t short_deck_size, std::size_t shared_card_count)
{
	EmitSep('-');

	std::ostringstream o_str;

	o_str << shared_card_count << " cards from a deck of " << short_deck_size;

	std::cout << o_str.str() << std::endl;

	EmitSep('-', o_str.str().size());
}
//	----------------------------------------------------------------------------
void TEST_GenSharedHands_1()
{
	EmitSep('=');

	Deck short_deck(TEST_ComboList_1, TEST_ComboList_1 + 5);

	std::cout << "Check Shared Hand Generation (deck size " <<
		short_deck.size() << ")" << std::endl;

	/*
		deck! / ((deck - cards)! * (cards!))

		48! / ((48 - 5)! * (5!)) = 1712304
	*/

	{
		// 2 cards from deck of 5 = 10 different hands
		std::size_t shared_card_count = 2;
		std::size_t expected_hands    =
			CalcSharedHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 3 cards from deck of 5 = 10 different hands
		std::size_t shared_card_count = 3;
		std::size_t expected_hands    =
			CalcSharedHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 4 cards from deck of 5 = 5 different hands
		std::size_t shared_card_count = 4;
		std::size_t expected_hands    =
			CalcSharedHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 5 cards from deck of 5 = 1 different hand
		std::size_t shared_card_count = 5;
		std::size_t expected_hands    =
			CalcSharedHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	EmitSep('=');

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void TEST_GenSharedHands_2()
{
	EmitSep('=');
	std::cout << "Check Shared Hand Generation (full deck minus two hands)" <<
		std::endl;
	EmitSep('-');

	Deck my_deck(ConstructDeck());
	my_deck.resize(48);	//	Remove two hands.

	boost::posix_time::ptime start_time(
		boost::posix_time::microsec_clock::universal_time());

	//	Generate 1712304 potential shared hands...
	DeckVector shared_hands(GenerateSharedHandsVector(my_deck, 5));

	boost::posix_time::ptime end_time(
		boost::posix_time::microsec_clock::universal_time());
	boost::posix_time::time_duration time_diff = end_time - start_time;

	std::cout << "difference    = " << time_diff << std::endl;
	std::cout << "difference ms = " << time_diff.total_milliseconds() <<
		std::endl;

	EmitSep('=');

	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_GenCombos

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
/*
std::cout << CalcSharedHandCount( 5, 2) << std::endl;
std::cout << CalcSharedHandCount( 5, 3) << std::endl;
std::cout << CalcSharedHandCount( 5, 4) << std::endl;
std::cout << CalcSharedHandCount( 5, 5) << std::endl;
std::cout << CalcSharedHandCount(48, 5) << std::endl;
*/
		TEST_CheckDeck();
		TEST_GenCombos::TEST_GenSharedHands_1();
		TEST_GenCombos::TEST_GenSharedHands_2();
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

