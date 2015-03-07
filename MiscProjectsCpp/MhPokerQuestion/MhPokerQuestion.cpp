//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	MhPokerQuestion.cpp

	File Description	:	An interesting poker question.

	Revision History	:	2015-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning(disable:4514 4710 4820)
# pragma warning(push)
# pragma warning(disable:4061 4350 4365 4514 4625 4626 4710 4986)
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
/*
	Approaches to hand-ranking from "List of poker hands"
	( https://en.wikipedia.org/wiki/List_of_poker_hands ).

		1)	Straight flush (10 ---> 4 * 10 = 40):
			a)	Highest card of the straight.

		2)	Four of a kind (13 * 12 = 156 ---> 4 * 156 = 624):
			a)	Card rank of the four of a kind cards.
			b)	Rank of fifth card.

		3)	Full house ( 3744):
			a)	Card rank of the three of a kind cards.
			b) Card rank of the pair cards.

		4)	Flush (5148):
			a) Highest card of the flush.
			b) Second-highest card of the flush.
			c) Third-highest card of the flush.
			d) Fourth-highest card of the flush.
			e) Fifth-highest card of the flush.
			f) Or tie splits the pot.

		5) Straight (10240):
			a)	Highest card of the straight.
			b) Or tie splits the pot.

		6) Three of a kind (13 * 4 * 65 = 3380):
			a)	Card rank of the three of a kind cards.
			b) Card rank of the remaining two cards.

		7) Two pair:
			a) Highest pair of the two pair.
			b) Lowest pair of the two pair.
			c) Or card rank of the single remaining card.

		8) One pair:
			a) Card rank of the pair cards.
			b) Card rank of highest of the three remaining cards.
			c) Card rank of second-highest of the three remaining cards.
			d) Card rank of third-highest of the three remaining cards.

		9) High card:
			a) Highest card of the hand.
			b) Second-highest card of the hand.
			c) Third-highest card of the hand.
			d) Fourth-highest card of the hand.
			e) Fifth-highest card of the hand.

		Suit is not used in hand ranking, per my reading of "High card by suit"
		( https://en.wikipedia.org/wiki/High_card_by_suit ).
*/
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
std::ostream &EmitSep(char sep_char = '=', std::streamsize sep_length = 79,
	std::ostream &o_str = std::cout)
{
	boost::io::ios_all_saver ios_saver(o_str);

	o_str << std::setfill(sep_char) << std::setw(sep_length) << "" << std::endl;

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned char Card;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const Card CardCount = 52;
const Card SuitCount =  4;
const Card RankCount = CardCount / SuitCount;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const std::size_t HandSizeCount = 5;
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
inline Card GetRank(Card in_card)
{
	return(static_cast<Card>(in_card % RankCount));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
inline Suit GetSuit(Card card)
{
	return(static_cast<Suit>(card / RankCount));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <std::size_t CardCount> class CardArray {
public:
	static const std::size_t CardArraySize = CardCount;

	CardArray()
	{
		clear();
	}

	std::size_t size() const
	{
		return(CardArraySize);
	}

	void clear()
	{
		::memset(cards_, '\0', sizeof(cards_));
	}

	const Card & operator [] (std::size_t card_index) const
	{
		if (card_index >= CardArraySize)
			throw std::invalid_argument("Invalid card index specified.");

		return(cards_[card_index]);
	}

	Card & operator [] (std::size_t card_index)
	{
		return(const_cast<Card &>(
			const_cast<const CardArray * const>(this)->operator[](card_index)));
	}

private:
	Card cards_[CardArraySize];
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef CardArray<HandSizeCount>       HandFull;
typedef std::vector<HandFull>          HandFullVector;
typedef HandFullVector::const_iterator HandFullVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef CardArray<RankCount>                    HandRank;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::pair<Card, Card>            HandPlayer;
typedef std::vector<HandPlayer>          HandPlayerVector;
typedef HandPlayerVector::const_iterator HandPlayerVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::vector<Card>          Deck;
typedef Deck::const_iterator       DeckIterC;

typedef std::vector<Deck>          DeckVector;
typedef DeckVector::const_iterator DeckVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
enum HandType {
	HandStraightFlush,
	HandFourOfAKind,
	HandFullHouse,
	HandFlush,
	HandStraight,
	HandThreeOfAKind,
	HandTwoPair,
	HandOnePair,
	HandHighCard,
	HandNone,					//	All hands are better than this.
	HandValidMin = HandStraightFlush,
	HandValidMax = HandHighCard
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
typedef unsigned __int16 BitsType;
typedef BitsType         BitsRank;
typedef BitsType         BitsSuit;
inline BitsType MyPopCount(BitsType src_bits)
{
	return(__popcnt16(src_bits));
}
/*
inline BitsType MyPopCount(unsigned __int32 src_bits)
{
	return(__popcnt(src_bits));
}
*/
//	Returns position of lowest set bit + 1. Zero if no bits are set.
inline BitsType MyLowestBitIndex(BitsType src_bits)
{
   unsigned long Index;
   unsigned long Mask = src_bits;

	return(static_cast<BitsType>((!_BitScanForward(&Index, Mask)) ?
		0 : (Index + 1)));
}
//	Returns position of highest set bit + 1. Zero if no bits are set.
inline BitsType MyHighestBitIndex(BitsType src_bits)
{
   unsigned long Index;
   unsigned long Mask = src_bits;

	return(static_cast<BitsType>((!_BitScanReverse(&Index, Mask)) ?
		0 : (Index + 1)));
}
#else	//	Assumes Linux...
typedef u_int32_t BitsType;
typedef BitsType  BitsRank;
typedef BitsType  BitsSuit;
inline BitsType MyPopCount(BitsType src_bits)
{
	return(static_cast<BitsType>(__builtin_popcount(src_bits)));
}
//	Returns position of lowest set bit + 1. Zero if no bits are set.
inline BitsType MyLowestBitIndex(BitsType src_bits)
{
	return((!src_bits) ? 0 : static_cast<BitsType>(__builtin_ffs(src_bits)));
}
//	Returns position of highest set bit + 1. Zero if no bits are set.
inline BitsType MyHighestBitIndex(BitsType src_bits)
{
	return((!src_bits) ? 0 : static_cast<BitsType>(
		((sizeof(src_bits) * CHAR_BIT) - __builtin_clz(src_bits))));
}
#endif // #ifdef _MSC_VER
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct HandEval {
	HandEval()
		:hand_type_(HandNone)
		,hand_rank_()
		,bits_rank_(0)
		,bits_suit_(0)
		,count_ranks_(0)
		,count_suits_(0)
		,hand_hint_1_(0)
		,hand_hint_2_(0)
	{
	}

	bool EvaluatePlayerHand(const HandFull &player_hand,
		HandType min_hand_type = HandNone);

	HandType    hand_type_;
	HandRank    hand_rank_;
	BitsRank    bits_rank_;
	BitsSuit    bits_suit_;
	BitsRank    count_ranks_;
	BitsSuit    count_suits_;
	std::size_t hand_hint_1_;
	std::size_t hand_hint_2_;

	inline static int CompareHint(std::size_t hand_hint_1_,
		std::size_t hand_hint_2_)
	{
		return((hand_hint_1_ < hand_hint_2_) ? -1 :
			((hand_hint_1_ > hand_hint_2_) ? 1 : 0));
	}

	inline static int CompareHint1(const HandEval &hand_eval_1,
		const HandEval &hand_eval_2)
	{
		return(CompareHint(hand_eval_1.hand_hint_1_, hand_eval_2.hand_hint_1_));
	}

	inline static int CompareHint2(const HandEval &hand_eval_1,
		const HandEval &hand_eval_2)
	{
		return(CompareHint(hand_eval_1.hand_hint_2_, hand_eval_2.hand_hint_2_));
	}

	inline static int CompareHints1And2(const HandEval &hand_eval_1,
		const HandEval &hand_eval_2)
	{
		int cmp = CompareHint1(hand_eval_1, hand_eval_2);

		return((cmp) ? cmp : CompareHint2(hand_eval_1, hand_eval_2));
	}

	//	If #1 is a better hand than #2, return will be 1.
	static int Compare(const HandEval &hand_eval_1, const HandEval &hand_eval_2,
		const HandFull &hand_full_1, const HandFull &hand_full_2);
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::vector<HandEval>          HandEvalVector;
typedef HandEvalVector::const_iterator HandEvalVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct HandRankResults {
	HandRankResults(std::size_t player_count)
		:best_hands_(player_count)
		,best_evals_(player_count)
		,best_counts_(player_count)
		,best_hand_index_(0)
		,hands_possible_(0)
		,hands_ranked_(0)
	{
	}

	double GetPercentRanked() const
	{
		return(((!hands_possible_) || (!hands_ranked_)) ? 0.0 :
			((static_cast<double>(hands_ranked_) /
			static_cast<double>(hands_possible_)) * 100.0));
	}

	double GetPercentBest(std::size_t player_index) const
	{
/*
		return((!hands_ranked_) ? 0.0 :
			((static_cast<double>(best_counts_[player_index]) /
			static_cast<double>(hands_ranked_)) * 100.0));
*/
		std::size_t total_count = 0;

		for (std::size_t count_1 = 0; count_1 < best_counts_.size(); ++count_1)
			total_count += best_counts_[count_1];

		return((!total_count) ? 0.0 :
			((static_cast<double>(best_counts_[player_index]) /
			static_cast<double>(total_count)) * 100.0));
	}

	HandFullVector           best_hands_;
	HandEvalVector           best_evals_;
	std::vector<std::size_t> best_counts_;
	std::size_t              best_hand_index_;
	std::size_t              hands_possible_;
	std::size_t              hands_ranked_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned long long             DeckMask;
typedef std::vector<DeckMask>          DeckMaskVector;
typedef DeckMaskVector::const_iterator DeckMaskVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class MhPokerGame {
public:
	static const std::size_t SharedCardCount = HandSizeCount;	//	Less than CardCount
	static const std::size_t PlayerCardCount = 2;	// As it's in a std::pair<>
	static const std::size_t MaxPlayerCount  =
		(CardCount - SharedCardCount) / PlayerCardCount;

	MhPokerGame(std::size_t player_count, bool shuffle_flag = true);

	std::size_t GetPlayerCount() const
	{
		return(player_count_);
	}

	Card RevealOneSharedCard();

	HandRankResults RankPlayerHands(bool randomize_decks = true,
		unsigned int max_usecs = 0);
	HandFullVector GetFinalPlayerHands(unsigned int &best_hand_bitset) const;
	HandFullVector GetFinalPlayerHands() const;

	std::string GetPlayerName(std::size_t player_index) const;
	std::string GetPlayerHandString(std::size_t player_index) const;
	void EmitPlayerHands(std::ostream &o_str = std::cout) const;
	void EmitPlayerHands(const HandRankResults &rank_results,
		std::ostream &o_str = std::cout) const;


/*
	CODE NOTE: To be removed.
	bool EvaluatePlayerHands(std::size_t target_element,
		std::size_t start_index, HandFullVector &working_hands);
*/
	template <typename PerHandFunctor> 
		bool RankPlayerHands(const DeckVector &player_decks,
			std::size_t target_element, std::size_t start_index,
			HandFullVector &working_hands, PerHandFunctor &hand_func) const;

	std::size_t EvaluatePlayerHands(const HandFullVector &working_hands,
		bool short_circuit_flag = false);
	std::size_t EvaluatePlayerHands(const HandFullVector &working_hands,
		HandEvalVector *hand_evals, std::vector<std::size_t> *hand_counts,
		bool short_circuit_flag = false);

	DeckMaskVector GetDeckMasks() const
	{
		return(deck_masks_);
	}

	DeckMaskVector GetDefaultDeckMasks() const
	{
		return(default_deck_masks_);
	}

private:
	Deck             deck_;
	Deck             revealed_cards_;
	std::size_t      player_count_;
	HandPlayerVector player_hands_;
	DeckVector       player_deck_;
	DeckVector       player_scratchpad_;
	DeckMaskVector   deck_masks_;
	DeckMaskVector   default_deck_masks_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
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
const char *SuitBriefNameChars = "CDHS";
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	CODE NOTE: Must change if order of enum Suit changes...
//	Un-comment the line below to never use the IBM-PC character set.
//#define DO_NOT_USE_IBM_CHARS
const char SuitToCharMap[] = {
#if defined(_MSC_VER) && (!defined(DO_NOT_USE_IBM_CHARS))
	'\x5',
	'\x4',
	'\x3',
	'\x6'
#else
	'C',
	'D',
	'H',
	'S'
#endif // #if defined(_MSC_VER) && (!defined(DO_NOT_USE_IBM_CHARS))
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
const char *RankBriefNameChars = "234567890JQKA";
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *HandTypeNameList[] = {
	"Straight flush",
	"Four-of-a-kind",
	"Full house",
	"Flush",
	"Straight",
	"Three-of-a-kind",
	"Two pair",
	"One pair",
	"High card"
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
	return(GetRank(GetValidCard(in_card)));
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
	return(GetSuit(GetValidCard(card)));
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
const char *GetHandTypeName(HandType hand_type)
{
	if ((hand_type < HandValidMin) || (hand_type > HandValidMax))
		throw std::invalid_argument("Invalid hand type.");

	return(HandTypeNameList[hand_type]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string GetHandTypeName(HandEval hand_eval, bool with_hints_flag = false)
{
	std::ostringstream o_str;

	if (!with_hints_flag)
		return(GetHandTypeName(hand_eval.hand_type_));

	switch (hand_eval.hand_type_) {
		case HandStraightFlush	:
			//	Hint 1 is the rank of high card of the straight flush.
			o_str << GetHandTypeName(hand_eval.hand_type_) << ", " <<
				GetRankName(hand_eval.hand_hint_1_) << " high";
			break;
		case HandFourOfAKind		:
			/*
				Hint 1 contains the four-of-a-kind rank.
				Hint 2 contains the 'kicker' rank.
			*/	
			o_str << "Four " << GetRankName(hand_eval.hand_hint_1_) <<
				"s, " << GetRankName(hand_eval.hand_hint_2_) << " kicker";
			break;
		case HandFullHouse		:
			/*
				Hint 1 contains the three-of-a-kind rank.
				Hint 2 contains the pair rank.
			*/	
			o_str << GetHandTypeName(hand_eval.hand_type_) << ", " <<
				GetRankName(hand_eval.hand_hint_1_) << "s and " <<
				GetRankName(hand_eval.hand_hint_2_) << "s";
			break;
		case HandFlush				:
			//	No hints provided for flushes...
			o_str << GetHandTypeName(hand_eval.hand_type_) << " in "
				<< GetSuitName(static_cast<Suit>(
					MyLowestBitIndex(hand_eval.bits_suit_) - 1));
			break;
		case HandStraight			:
			//	Hint 1 is the rank of high card of the straight.
			o_str << GetHandTypeName(hand_eval.hand_type_) << ", " <<
				GetRankName(hand_eval.hand_hint_1_) << " high";
			break;
		case HandThreeOfAKind	:
			/*
				Hint 1 contains the three-of-a-kind rank.
				Hint 2 contains the highest rank of the remaining two cards.
			*/
			o_str << "Three " << GetRankName(hand_eval.hand_hint_1_) << "s";
			break;
		case HandTwoPair			:
			/*
				Hints 1 and 2 contain the ranks of the two pairs, with
				hint 1 being the rank of the high pair.
			*/
			o_str << GetHandTypeName(hand_eval.hand_type_) << ", " <<
				GetRankName(hand_eval.hand_hint_1_) << "s and " <<
				GetRankName(hand_eval.hand_hint_2_) << "s";
			break;
		case HandOnePair			:
			//	No hints provided for single pair hands...
			o_str << GetHandTypeName(hand_eval.hand_type_);
			break;
		case HandHighCard			:
			//	No hints provided (or needed) for high card hands...
/*
			cmp = static_cast<int>(hand_eval_1.bits_rank_) -
				static_cast<int>(hand_eval_2.bits_rank_);
*/
			break;
		case HandNone				:	//	Shouldn't happen.
			throw std::logic_error("Unexpected HandType::HandNone encountered.");
			break;
	}

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Card ParseCard(const char *in_string)
{
	if ((!in_string) || (::strlen(in_string) < 2))
		throw std::invalid_argument("Invalid card parse source string.");

	const char *rank_ptr = ::strchr(RankBriefNameChars,
		::toupper(*in_string));

	if (!rank_ptr)
		throw std::invalid_argument("Invalid card parse source string rank.");

	const char *suit_ptr = ::strchr(SuitBriefNameChars,
		::toupper(in_string[1]));

	if (!suit_ptr)
		throw std::invalid_argument("Invalid card parse source string suit.");

	return(static_cast<Card>((rank_ptr - RankBriefNameChars) +
		((suit_ptr - SuitBriefNameChars) * RankCount)));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
HandFull ParseHand(const char *in_string)
{
	HandFull out_cards;

	if ((!in_string) || (::strlen(in_string) < (HandFull::CardArraySize * 2)))
		throw std::invalid_argument("Invalid hand parse source string.");

	for (std::size_t count_1 = 0; count_1 < HandFull::CardArraySize;
		++count_1, in_string += 2)
		out_cards[count_1] = ParseCard(in_string);

	return(out_cards);
}
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
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
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
Deck ConstructDeck(bool shuffle_flag = false)
{
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
std::ostream & operator << (std::ostream &o_str, const HandFull &datum)
{
	for (std::size_t count_1 = 0; count_1 < HandFull::CardArraySize; ++count_1)
		o_str << ((count_1) ? " " : "") << GetCardNameShort(datum[count_1]);

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const DeckVector &in_deck)
{
	return(EmitDeck(in_deck, true, false, o_str));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t CalcPossibleHandCount(std::size_t deck_size,
	std::size_t hand_card_count)
{
	if ((deck_size < 1) || (deck_size > 52))
		throw std::invalid_argument("Deck size not in range 1 to 52 inclusive.");
	else if (!hand_card_count)
		throw std::invalid_argument("Hand count equals zero.");
	else if (hand_card_count > deck_size)
		throw std::invalid_argument("Hand card count exceeds deck size.");

	if (deck_size == hand_card_count)
		return(1);

	std::size_t size_diff = deck_size - hand_card_count;
	double fact_deck      = 1.0;
	double fact_diff      = 0.0;	//	To prevent compiler complaints only.
	double fact_cards     = 0.0;	//	To prevent compiler complaints only.

	for (std::size_t count_1 = 1; count_1 <= deck_size; ++count_1) {
		fact_deck *= count_1;
		if (count_1 == size_diff)
			fact_diff = fact_deck;
		if (count_1 == hand_card_count)
			fact_cards = fact_deck;
	}

	return(static_cast<std::size_t>(fact_deck / (fact_diff * fact_cards)));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t CalcPossibleHandCount(const Deck &deck, std::size_t hand_card_count)
{
	return(CalcPossibleHandCount(deck.size(), hand_card_count));
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
		if ((target_element + 1) < shared_card_count) {
			if (!GenerateSharedHands(in_deck, shared_card_count,
				target_element + 1, start_index, working_hand, hand_func))
				return(false);
		}
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

namespace {

//	////////////////////////////////////////////////////////////////////////////
struct PerHandAccumulator {
	PerHandAccumulator(const Deck &in_deck, std::size_t shared_card_count)
		:out_deck_()
	{
		out_deck_.reserve(CalcPossibleHandCount(in_deck, shared_card_count));
	}

	inline bool operator () (const Deck &working_hand)
	{
		out_deck_.push_back(working_hand);

		return(true);
	}

	DeckVector out_deck_;
};
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
DeckVector GenerateSharedHandsVector(const Deck &in_deck,
	std::size_t shared_card_count)
{
	PerHandAccumulator my_func(in_deck, shared_card_count);

	GenerateSharedHands(in_deck, shared_card_count, my_func);

	return(my_func.out_deck_);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
MhPokerGame::MhPokerGame(std::size_t player_count, bool shuffle_flag)
	:deck_(ConstructDeck(shuffle_flag))
	,revealed_cards_()
	,player_count_(player_count)
	,player_hands_()
	,player_deck_()
	,player_scratchpad_()
{
	if ((player_count_ < 2) || (player_count_ > MaxPlayerCount))
		throw std::invalid_argument("Invalid player count.");

	revealed_cards_.reserve(SharedCardCount);
	player_hands_.reserve(player_count_);
	player_deck_.resize(player_count_);
	player_scratchpad_.resize(player_count_);
	deck_masks_.resize(player_count_);
	default_deck_masks_.resize(player_count_);

	//	Cheesy deal doesn't rotate among players, but is nice in testing...
	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		player_hands_.push_back(std::make_pair(deck_[deck_.size() - 1],
			deck_[deck_.size() - 2]));
		deck_.resize(deck_.size() - 2);
		deck_masks_[count_1]         = std::numeric_limits<DeckMask>::max();
		default_deck_masks_[count_1] = std::numeric_limits<DeckMask>::max();
		player_scratchpad_[count_1].push_back(player_hands_.back().first);
		player_scratchpad_[count_1].push_back(player_hands_.back().second);
		std::sort(player_scratchpad_[count_1].begin(),
			player_scratchpad_[count_1].end());
	}

	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		player_deck_[count_1] = deck_;
		player_deck_[count_1].push_back(player_hands_[count_1].first);
		player_deck_[count_1].push_back(player_hands_[count_1].second);
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
Card MhPokerGame::RevealOneSharedCard()
{
	if (revealed_cards_.size() == SharedCardCount)
		throw std::logic_error("Unable to reveal a new shared card as all "
			"cards have been revealed.");

	revealed_cards_.push_back(deck_.back());

	deck_.pop_back();

	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		player_deck_[count_1] = deck_;
		player_deck_[count_1].push_back(player_hands_[count_1].first);
		player_deck_[count_1].push_back(player_hands_[count_1].second);
		if (revealed_cards_.size() == 1)
			deck_masks_[count_1] = static_cast<DeckMask>(
				(1 << player_hands_[count_1].first) |
				(1 << player_hands_[count_1].second) |
				(1 << revealed_cards_.back()));
		else
			deck_masks_[count_1] |=
				static_cast<DeckMask>(1 << revealed_cards_.back());
		player_scratchpad_[count_1].push_back(revealed_cards_.back());
		std::sort(player_scratchpad_[count_1].begin(),
			player_scratchpad_[count_1].end());
	}

	return(revealed_cards_.back());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
	Hand evaluation population:
		0 shared reveals:
			General card population: all cards not held by any opponent:
				52 - ((PC - 1) * 2)
			Hands can be any combination of 5 from the card pop.

		1 shared reveal:
			General card population: all cards not held by any opponent:
				52 - ((PC - 1) * 2) - 1
			Hands must include at least one of cards known to the player,
			those cards being their own two held cards plus the single
			revealed card.

		2 shared reveals:
			General card population: all cards not held by any opponent:
				52 - ((PC - 1) * 2) - 2
			Hands must include at least two of cards known to the player,
			those cards being their own two held cards plus the two revealed
			cards.

		3 shared reveals:
			General card population: all cards not held by any opponent:
				52 - ((PC - 1) * 2) - 3
			Hands must include at least three of cards known to the player,
			those cards being (that set being their own two held cards plus
			the three revealed cards.

		4 shared reveals:
			General card population: all cards not held by any opponent:
				52 - ((PC - 1) * 2) - 4
			Hands must include at least four of cards known to the player,
			those cards being (that set being their own two held cards plus
			the four revealed cards.
*/
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
struct PerHandAssessor {
	PerHandAssessor(MhPokerGame *game_ptr, const DeckVector &player_decks,
		bool randomize_decks,
		unsigned int max_usecs)
		:game_ptr_(game_ptr)
		,player_decks_ptr_(&player_decks)
		,player_decks_()
		,rank_results_(player_decks.size())
		,max_usecs_(max_usecs)
		,end_time_(boost::posix_time::microsec_clock::universal_time() +
			boost::posix_time::microsec(max_usecs))
	{
		if (randomize_decks) {
			player_decks_ = player_decks;
			//	CODE NOTE: TO-DO: Randomize the copy of the decks...
			player_decks_ptr_ = &player_decks_;
		}

		rank_results_.hands_possible_ =
			CalcPossibleHandCount(player_decks[0].size(), 5);
	}

	inline bool operator () (const HandFullVector &working_hands)
	{
		++rank_results_.hands_ranked_;

		game_ptr_->EvaluatePlayerHands(working_hands,
			&rank_results_.best_evals_, &rank_results_.best_counts_, false);

		return((!max_usecs_) ||
			(boost::posix_time::microsec_clock::universal_time() < end_time_));
	}

	MhPokerGame              *game_ptr_;
	const DeckVector         *player_decks_ptr_;
	DeckVector                player_decks_;
	HandRankResults           rank_results_;
	unsigned int              max_usecs_;
	boost::posix_time::ptime  end_time_;
};
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
HandRankResults MhPokerGame::RankPlayerHands(bool randomize_decks,
	unsigned int max_usecs)
{
	PerHandAssessor my_func(this, player_deck_, randomize_decks, max_usecs);

	HandFullVector working_hands(player_count_);

	RankPlayerHands(*my_func.player_decks_ptr_, 0, 0, working_hands, my_func);

	return(my_func.rank_results_);
}
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
struct FinalHandAssessor {
	FinalHandAssessor(const MhPokerGame &poker_game)
		:player_count_(poker_game.GetPlayerCount())
		,best_hands_(player_count_)
		,best_evals_(player_count_)
		,first_pass_done_(false)
	{
	}

	inline bool operator () (const HandFullVector &working_hands)
	{
		for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
			const HandFull &player_hand = working_hands[count_1];
			HandEval        hand_eval;
			if (hand_eval.EvaluatePlayerHand(player_hand, (!first_pass_done_) ?
				HandNone : best_evals_[count_1].hand_type_)) {
				if (!first_pass_done_) {
					best_hands_[count_1] = player_hand;
					best_evals_[count_1] = hand_eval;
				}
				else if (hand_eval.hand_type_ < best_evals_[count_1].hand_type_) {
					best_hands_[count_1] = player_hand;
					best_evals_[count_1] = hand_eval;
				}
				else if (hand_eval.hand_type_ == best_evals_[count_1].hand_type_) {
					int cmp = HandEval::Compare(hand_eval, best_evals_[count_1],
						working_hands[count_1], best_hands_[count_1]);
					if (cmp > 0) {
						//	New best hand found...
						best_hands_[count_1] = player_hand;
						best_evals_[count_1] = hand_eval;
					}
				}
			}
		}

		first_pass_done_ = true;

		return(true);
	}

	unsigned int GetBestHandBitSet() const
	{
		std::size_t  best_hand_index  = 0;
		unsigned int best_hand_bitset = 1;

		for (std::size_t count_1 = 1; count_1 < player_count_; ++count_1) {
			int cmp = HandEval::Compare(best_evals_[best_hand_index],
				best_evals_[count_1], best_hands_[best_hand_index],
				best_hands_[count_1]);
			if (cmp < 0) {
				best_hand_index  = count_1;
				best_hand_bitset = 1 << count_1;
			}
			else if (!cmp)
				best_hand_bitset |= 1 << count_1;
		}

		return(best_hand_bitset);
	}

	std::size_t    player_count_;
	HandFullVector best_hands_;
	HandEvalVector best_evals_;
	bool           first_pass_done_;
};
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
HandFullVector MhPokerGame::GetFinalPlayerHands(unsigned int &best_hand_bitset)
	const
{
	FinalHandAssessor my_func(*this);

	HandFullVector working_hands(player_count_);

	RankPlayerHands(player_scratchpad_, 0, 0, working_hands, my_func);

	best_hand_bitset = my_func.GetBestHandBitSet();

	return(my_func.best_hands_);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
HandFullVector MhPokerGame::GetFinalPlayerHands() const
{
	unsigned int best_hand_bitset;

	return(GetFinalPlayerHands(best_hand_bitset));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string MhPokerGame::GetPlayerName(std::size_t player_index) const
{
	if (player_index >= player_hands_.size())
		throw std::invalid_argument("Invalid player index.");

	std::ostringstream o_str;

	o_str << "Player-" << std::setfill('0') << std::setw(2) << player_index;

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string MhPokerGame::GetPlayerHandString(std::size_t player_index) const
{
	std::ostringstream o_str;

	o_str << GetPlayerName(player_index) << ": " <<
		GetCardNameShort(player_hands_[player_index].first, true) <<
		" | " << GetCardNameShort(player_hands_[player_index].second, true);

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void MhPokerGame::EmitPlayerHands(std::ostream &o_str) const
{
	for (std::size_t count_1 = 0; count_1 < player_hands_.size(); ++count_1)
		o_str << GetPlayerHandString(count_1) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void MhPokerGame::EmitPlayerHands(const HandRankResults &rank_results,
	std::ostream &o_str) const
{
	for (std::size_t count_1 = 0; count_1 < player_hands_.size(); ++count_1) {
		o_str << GetPlayerHandString(count_1) << " ---> " <<
			rank_results.GetPercentBest(count_1) << "% of " <<
			rank_results.hands_ranked_ <<
			" (" << rank_results.GetPercentRanked() << "%) hands ranked." <<
			std::endl;
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename PerHandFunctor>
	bool MhPokerGame::RankPlayerHands(const DeckVector &player_decks,
		std::size_t target_element, std::size_t start_index,
		HandFullVector &working_hands, PerHandFunctor &hand_func) const
{
	while (start_index < player_decks[0].size()) {
		for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1)
			working_hands[count_1][target_element] =
				player_decks[count_1][start_index];
		++start_index;
		if ((target_element + 1) < HandFull::CardArraySize) {
			if (!RankPlayerHands(player_decks, target_element + 1,
				start_index, working_hands, hand_func))
				return(false);
		}
		else if (!hand_func(working_hands))
			return(false);
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

/*
	CODE NOTE: To be removed.
//	////////////////////////////////////////////////////////////////////////////
bool MhPokerGame::EvaluatePlayerHands(std::size_t target_element,
	std::size_t start_index, HandFullVector &working_hands)
{
	while (start_index < player_deck_[0].size()) {
		for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1)
			working_hands[count_1][target_element] =
				player_deck_[count_1][start_index];
		++start_index;
		if ((target_element + 1) < SharedCardCount)
			EvaluatePlayerHands(target_element + 1, start_index, working_hands);
		else if (!EvaluatePlayerHands(working_hands))
			return(false);
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////
*/

//	////////////////////////////////////////////////////////////////////////////
std::size_t MhPokerGame::EvaluatePlayerHands(
	const HandFullVector &working_hands, HandEvalVector *hand_evals,
	std::vector<std::size_t> *hand_counts, bool short_circuit_flag)
{
	HandEval     best_hand_eval;
	std::size_t  best_hand_index = 0;
	std::size_t  best_hand_count = 0;
	unsigned int best_hand_bit   = 0;

	assert((sizeof(best_hand_bit) * CHAR_BIT) >= MaxPlayerCount);

	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		const HandFull &player_hand = working_hands[count_1];
		HandEval        hand_eval;
		if (hand_eval.EvaluatePlayerHand(player_hand,
			((!short_circuit_flag) || (!count_1)) ? HandNone :
			best_hand_eval.hand_type_)) {
			if (!count_1) {
				best_hand_eval  = hand_eval;
				best_hand_index = count_1;
				best_hand_count = 1;
				best_hand_bit   = 1 << count_1;
			}
			else if (hand_eval.hand_type_ < best_hand_eval.hand_type_) {
				best_hand_eval  = hand_eval;
				best_hand_index = count_1;
				best_hand_count = 1;
				best_hand_bit   = 1 << count_1;
			}
			else if (hand_eval.hand_type_ == best_hand_eval.hand_type_) {
				int cmp = HandEval::Compare(hand_eval, best_hand_eval,
					working_hands[count_1], working_hands[best_hand_index]);
				if (cmp > 0) {
					//	New best eval found...
					best_hand_eval  = hand_eval;
					best_hand_index = count_1;
					best_hand_count = 1;
					best_hand_bit   = 1 << count_1;
				}
				else if (!cmp) {
					//	Exactly equivalent, split pot...
					best_hand_count++;
					best_hand_bit |= 1 << count_1;
				}
				//	If neither, no change.
			}
		}
	}

	if (hand_counts) {
		if (best_hand_count == 1)
			(*hand_counts)[best_hand_index]++;
		else {
			int best_index_lo = MyLowestBitIndex(best_hand_bit) - 1;
			int best_index_hi = MyHighestBitIndex(best_hand_bit) - 1;
			for (int count_1 = best_index_lo; count_1 <= best_index_hi;
				++count_1) {
				if (best_hand_bit & (1 << count_1))
					(*hand_counts)[count_1]++;
			}
		}
	}

	return(best_hand_count);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t MhPokerGame::EvaluatePlayerHands(
	const HandFullVector &working_hands, bool short_circuit_flag)
{
	return(EvaluatePlayerHands(working_hands, NULL, NULL, short_circuit_flag));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool HandEval::EvaluatePlayerHand(const HandFull &player_hand,
	HandType min_hand_type)
{
	bits_rank_ = 0;
	bits_suit_ = 0;

	for (std::size_t count_1 = 0; count_1 < player_hand.size(); ++count_1) {
		Card this_rank = GetRank(player_hand[count_1]);
		bits_rank_ |= static_cast<BitsRank>(1 << this_rank);
		bits_suit_ |= static_cast<BitsSuit>(1 << GetSuit(player_hand[count_1]));
		hand_rank_[this_rank]++;
	}

	count_ranks_ = MyPopCount(bits_rank_);
	count_suits_ = MyPopCount(bits_suit_);

	/*
		Ace low straight:
			1000000001111
			1   0   0   F
	*/

	//	//////////////////////////////////////////////////////////////////////
	/*
		If a single suit and the rank bits are contiguous (or in the ace-low
		pattern 0b1000000001111 = 0x100F), it's a straight flush.

		Only hint 1 is provided for straight flushes (high card of the straight).
	*/
	//	----------------------------------------------------------------------
	if ((count_suits_ == 1)           &&
		 ((bits_rank_ == 0x1f)         ||
		  (bits_rank_ == (0x1f <<  1)) ||
		  (bits_rank_ == (0x1f <<  2)) ||
		  (bits_rank_ == (0x1f <<  3)) ||
		  (bits_rank_ == (0x1f <<  4)) ||
		  (bits_rank_ == (0x1f <<  5)) ||
		  (bits_rank_ == (0x1f <<  6)) ||
		  (bits_rank_ == (0x1f <<  7)) ||
		  (bits_rank_ == (0x1f <<  8)) ||
		  (bits_rank_ == 0x100f))) {
		hand_type_   = HandStraightFlush;
		hand_hint_1_ = (bits_rank_ == 0x100f) ? 3 :
			(MyHighestBitIndex(bits_rank_) - 1);
		return(true);
	}
	else if (min_hand_type == HandStraightFlush)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		If all four suits are represented and only two different card ranks
		are present, check to see if either of the card ranks has four
		instances. If so, it's four-of-a-kind.

		Otherwise, it's a full house.
	*/
	//	----------------------------------------------------------------------
	if ((count_suits_ == 4) && (count_ranks_ == 2)) {
		BitsType bit_lo = MyLowestBitIndex(bits_rank_) - 1;
		BitsType bit_hi = MyHighestBitIndex(bits_rank_) - 1;
		if ((hand_rank_[bit_lo] == 4) || (hand_rank_[bit_hi] == 4)) {
			hand_type_ = HandFourOfAKind;
			/*
				Hint 1 contains the four-of-a-kind rank.
				Hint 2 contains the 'kicker' rank.
			*/	
			if (hand_rank_[bit_lo] == 4) {
				hand_hint_1_ = bit_lo;
				hand_hint_2_ = bit_hi;
			}
			else {
				hand_hint_1_ = bit_hi;
				hand_hint_2_ = bit_lo;
			}
			return(true);
		}
		else if (min_hand_type == HandFourOfAKind)
			return(false);
		hand_type_ = HandFullHouse;
		/*
			Hint 1 contains the three-of-a-kind rank.
			Hint 2 contains the pair rank.
		*/	
		if (hand_rank_[bit_lo] == 3) {
			hand_hint_1_ = bit_lo;
			hand_hint_2_ = bit_hi;
		}
		else {
			hand_hint_1_ = bit_hi;
			hand_hint_2_ = bit_lo;
		}
		return(true);
	}
	else if (min_hand_type == HandFourOfAKind)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		If we have only two ranks in the hand and the hand wasn't processed
		by the four-of-a-kind logic, the hand is a full house.
	*/
	//	----------------------------------------------------------------------
	if (count_ranks_ == 2) {
		BitsType bit_lo = MyLowestBitIndex(bits_rank_) - 1;
		BitsType bit_hi = MyHighestBitIndex(bits_rank_) - 1;
		hand_type_ = HandFullHouse;
		/*
			Hint 1 contains the three-of-a-kind rank.
			Hint 2 contains the pair rank.
		*/	
		if (hand_rank_[bit_lo] == 3) {
			hand_hint_1_ = bit_lo;
			hand_hint_2_ = bit_hi;
		}
		else {
			hand_hint_1_ = bit_hi;
			hand_hint_2_ = bit_lo;
		}
		return(true);
	}
	else if (min_hand_type == HandFullHouse)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		If the hand has a single suit and isn't a straight flush, it's a flush.

		Note that no hints are provided for a flush as they aren't necessary
		for hand type detection.
	*/
	//	----------------------------------------------------------------------
	if (count_suits_ == 1) {
		hand_type_ = HandFlush;
		return(true);
	}
	else if (min_hand_type == HandFlush)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		if the rank bits are contiguous (or are in the ace-low pattern
		0b1000000001111 = 0x100F), and the hand wasn't processed by the
		straight flush logic, it's a straight.

		Only hint 1 is provided for straights (high card of the straight).
	*/
	//	----------------------------------------------------------------------
	if ((bits_rank_ == 0x1f)         ||
		 (bits_rank_ == (0x1f <<  1)) ||
		 (bits_rank_ == (0x1f <<  2)) ||
		 (bits_rank_ == (0x1f <<  3)) ||
		 (bits_rank_ == (0x1f <<  4)) ||
		 (bits_rank_ == (0x1f <<  5)) ||
		 (bits_rank_ == (0x1f <<  6)) ||
		 (bits_rank_ == (0x1f <<  7)) ||
		 (bits_rank_ == (0x1f <<  8)) ||
		 (bits_rank_ == 0x100f)) {
		hand_type_   = HandStraight;
		hand_hint_1_ = (bits_rank_ == 0x100f) ? 3 :
			(MyHighestBitIndex(bits_rank_) - 1);
		return(true);
	}
	else if (min_hand_type == HandStraight)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		If the hand has three ranks, it could qualify as three-of-a-kind or
		as two pair.

		If the hand is two pair than for any two ranks in the hand, at least
		one must be a pair. So if examination of two ranks does not result
		in a pair, it's a three-of-kind.

		The logic here is a little backwards in order to support the short-
		circuiting of hand evaluation.
	*/
	//	----------------------------------------------------------------------
	if (count_ranks_ == 3) {
		BitsType bit_lo = MyLowestBitIndex(bits_rank_) - 1;
		BitsType bit_hi = MyHighestBitIndex(bits_rank_) - 1;
		BitsType bit_mi = MyLowestBitIndex(static_cast<BitsType>(
			bits_rank_ & (~((1 << bit_lo) | (1 << bit_hi))))) - 1;
		if ((hand_rank_[bit_lo] != 2) && (hand_rank_[bit_hi] != 2)) {
			//	Hint 1 contains the three-of-a-kind rank.
			//	Hint 2 contains the highest rank of the remaining cards.
			hand_type_   = HandThreeOfAKind;
			hand_hint_1_ = (hand_rank_[bit_lo] == 3) ? bit_lo :
				((hand_rank_[bit_hi] == 3) ? bit_hi : bit_mi);
			if (hand_rank_[bit_lo] == 3) {
				hand_hint_1_ = bit_lo;
				hand_hint_2_ = std::max(bit_hi, bit_mi);
			}
			else if (hand_rank_[bit_hi] == 3) {
				hand_hint_1_ = bit_hi;
				hand_hint_2_ = std::max(bit_lo, bit_mi);
			}
			else {
				hand_hint_1_ = bit_mi;
				hand_hint_2_ = std::max(bit_lo, bit_hi);
			}
			return(true);
		}
		else if (min_hand_type == HandThreeOfAKind)
			return(false);
		/*
			Two pair logic.

			Hints 1 and 2 contain the ranks of the two pairs. Hint 1
			is the rank of the highest pair.
		*/
		if (hand_rank_[bit_hi] == 2) {
			hand_hint_1_ = bit_hi;
			hand_hint_2_ = (hand_rank_[bit_lo] == 2) ? bit_lo : bit_mi;
		}
		else {
			hand_hint_1_ = bit_mi;
			hand_hint_2_ = bit_lo;
		}
		hand_type_ = HandTwoPair;
		return(true);
	}
	else if ((min_hand_type == HandThreeOfAKind) ||
		(min_hand_type == HandTwoPair))
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		At this point the only hand in which there are four different ranks
		is one pair.

		Note that no hints are provided for a pair as they aren't necessary
		for hand type detection.
	*/
	//	----------------------------------------------------------------------
	if (count_ranks_ == 4) {
		hand_type_ = HandOnePair;
		return(true);
	}
	else if (min_hand_type == HandOnePair)
		return(false);
	//	//////////////////////////////////////////////////////////////////////

	//	//////////////////////////////////////////////////////////////////////
	/*
		Otherwise it's just a high card hand.

		Note that no hints are provided for a high card hand as they aren't
		necessary for hand type detection.
	*/
	//	----------------------------------------------------------------------
	hand_type_ = HandHighCard;
	//	//////////////////////////////////////////////////////////////////////

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int HandEval::Compare(const HandEval &hand_eval_1, const HandEval &hand_eval_2,
	const HandFull &hand_full_1, const HandFull &hand_full_2)
{
	//	Sense reversed because lower HandType values are better.
	if (hand_eval_1.hand_type_ > hand_eval_2.hand_type_)
		return(-1);
	else if (hand_eval_1.hand_type_ < hand_eval_2.hand_type_)
		return(1);

	int cmp = 0;

	switch (hand_eval_1.hand_type_) {
		case HandStraightFlush	:
			//	Hint 1 is the rank of high card of the straight flush.
			cmp = CompareHint1(hand_eval_1, hand_eval_2);
			break;
		case HandFourOfAKind		:
			/*
				Hint 1 contains the four-of-a-kind rank.
				Hint 2 contains the 'kicker' rank.
			*/	
			if ((cmp = CompareHint1(hand_eval_1, hand_eval_2)) == 0)
				cmp = CompareHint2(hand_eval_1, hand_eval_2);
			break;
		case HandFullHouse		:
			/*
				Hint 1 contains the three-of-a-kind rank.
				Hint 2 contains the pair rank.
			*/	
			if ((cmp = CompareHint1(hand_eval_1, hand_eval_2)) == 0)
				cmp = CompareHint2(hand_eval_1, hand_eval_2);
			break;
		case HandFlush				:
			//	No hints provided for flushes...
			{
				/*
					Have to test whether calling My{High|Low}estBitIndex() four
					times and using the high-max and low-min as loop boundaries
					is a performance win over just iterating from 12 to 0.

					The answer to which should also inform the loop in the
					HandOnePair case.
				*/
				BitsType bit_lo_1 = MyLowestBitIndex(hand_eval_1.bits_rank_) - 1;
				BitsType bit_hi_1 = MyHighestBitIndex(hand_eval_1.bits_rank_) - 1;
				BitsType bit_lo_2 = MyLowestBitIndex(hand_eval_2.bits_rank_) - 1;
				BitsType bit_hi_2 = MyHighestBitIndex(hand_eval_2.bits_rank_) - 1;
				for (BitsType count_1 = std::max(bit_hi_1, bit_hi_2);
					count_1 <= std::min(bit_lo_1, bit_lo_2); --count_1) {
					cmp = static_cast<int>(hand_eval_1.hand_rank_[count_1]) -
						static_cast<int>(hand_eval_2.hand_rank_[count_1]);
					if (cmp)
						break;
				}
			}
			break;
		case HandStraight			:
			//	Hint 1 is the rank of high card of the straight.
			cmp = CompareHint1(hand_eval_1, hand_eval_2);
			break;
		case HandThreeOfAKind	:
			/*
				Hint 1 contains the three-of-a-kind rank.
				Hint 2 contains the highest rank of the remaining two cards.

				If hint 2 compares the same, compare the single remaining rank.

				Note that this logic is identical with that for HandTwoPair.
				They should probably be combined by way of case fall-through.
			*/
			if ((cmp = CompareHint1(hand_eval_1, hand_eval_2)) == 0) {
				if ((cmp = CompareHint2(hand_eval_1, hand_eval_2)) == 0)
					cmp = static_cast<int>(hand_eval_1.bits_rank_) -
						static_cast<int>(hand_eval_2.bits_rank_);
/*
	CODE NOTE: Obsoleted. To be removed.
					cmp = static_cast<int>(hand_eval_1.bits_rank_ &
						(~((1 << hand_eval_1.hand_hint_1_) |
						   (1 << hand_eval_1.hand_hint_2_)))) -
							static_cast<int>(hand_eval_2.bits_rank_ &
						(~((1 << hand_eval_2.hand_hint_1_) |
						   (1 << hand_eval_2.hand_hint_2_))));
*/
			}
			break;
		case HandTwoPair			:
			/*
				Hints 1 and 2 contain the ranks of the two pairs, with
				hint 1 being the rank of the high pair.

				Use the single remaining rank as a tie-breaker.
			*/
			if ((cmp = CompareHint1(hand_eval_1, hand_eval_2)) == 0) {
				if ((cmp = CompareHint2(hand_eval_1, hand_eval_2)) == 0)
					cmp = static_cast<int>(hand_eval_1.bits_rank_) -
						static_cast<int>(hand_eval_2.bits_rank_);
/*
	CODE NOTE: Obsoleted. To be removed.
					cmp = static_cast<int>(hand_eval_1.bits_rank_ &
						(~((1 << hand_eval_1.hand_hint_1_) |
						   (1 << hand_eval_1.hand_hint_2_)))) -
							static_cast<int>(hand_eval_2.bits_rank_ &
						(~((1 << hand_eval_2.hand_hint_1_) |
						   (1 << hand_eval_2.hand_hint_2_))));
*/
			}
			break;
		case HandOnePair			:
			/*
				No hints provided for single pair hands...

				So we examine the ranks in both hands from high-to-low. If
				a rank of either hand is a pair, we compare the number of
				cards of that rank in the two hands.

				If the result of that comparison not zero, we've completed the
				comparison. Otherwise, the same rank comprises the pair in
				both hands. So we compare the bit set of all ranks in the
				two hands
			*/
			for (std::size_t count_1 = (RankCount - 1); count_1 <= 0; --count_1) {
				if ((hand_eval_1.hand_rank_[count_1] == 2) ||
					 (hand_eval_2.hand_rank_[count_1] == 2)) {
					cmp = static_cast<int>(hand_eval_1.hand_rank_[count_1]) -
						static_cast<int>(hand_eval_2.hand_rank_[count_1]);
					break;
				 }
			}
			if (!cmp)
				cmp = static_cast<int>(hand_eval_1.bits_rank_) -
					static_cast<int>(hand_eval_2.bits_rank_);
			break;
		case HandHighCard			:
			//	No hints provided (or needed) for high card hands...
			cmp = static_cast<int>(hand_eval_1.bits_rank_) -
				static_cast<int>(hand_eval_2.bits_rank_);
			break;
		case HandNone				:	//	Shouldn't happen.
			throw std::logic_error("Unexpected HandType::HandNone encountered.");
			break;
	}

	return(cmp);
}
//	////////////////////////////////////////////////////////////////////////////

namespace TEST_CheckComboCalc {

//	////////////////////////////////////////////////////////////////////////////
void TEST_DoTest(std::size_t deck_size, std::size_t hand_card_count)
{
	std::cout << "Deck Size: " << std::setw(2) << deck_size <<
		", hand size: " << std::setw(2) << hand_card_count << " ---> " <<
		std::setw(10) << CalcPossibleHandCount(deck_size, hand_card_count) <<
		std::endl;
}
//	----------------------------------------------------------------------------
void RunTest()
{
	EmitSep('=');
	std::cout << "Check CalcPossibleHandCount()" << std::endl;
	EmitSep('-');

	TEST_DoTest( 5, 2);
	TEST_DoTest( 5, 3);
	TEST_DoTest( 5, 4);
	TEST_DoTest( 5, 5);
	TEST_DoTest(48, 5);
	TEST_DoTest(50, 5);
	TEST_DoTest(52, 5);

	EmitSep('=');

	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // TEST_CheckComboCalc

namespace TEST_DeckGeneration {

//	////////////////////////////////////////////////////////////////////////////
void RunTest(bool shuffle_flag = false)
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

} // namespace TEST_DeckGeneration

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
void RunTest_GenSharedHands_1()
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
			CalcPossibleHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 3 cards from deck of 5 = 10 different hands
		std::size_t shared_card_count = 3;
		std::size_t expected_hands    =
			CalcPossibleHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 4 cards from deck of 5 = 5 different hands
		std::size_t shared_card_count = 4;
		std::size_t expected_hands    =
			CalcPossibleHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	{
		// 5 cards from deck of 5 = 1 different hand
		std::size_t shared_card_count = 5;
		std::size_t expected_hands    =
			CalcPossibleHandCount(short_deck, shared_card_count);
		TEST_EmitHeader(short_deck.size(), shared_card_count);
		DeckVector shared_hands(
			GenerateSharedHandsVector(short_deck, shared_card_count));
		TEST_EmitDeck(expected_hands, shared_hands);
	}

	EmitSep('=');

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void RunTest_GenSharedHands_2()
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

namespace TEST_GameCycle {

//	////////////////////////////////////////////////////////////////////////////
void RunTest_GameCycle(std::size_t game_cycle_number = 1,
	std::size_t player_count = 2, bool shuffle_flag = true)
{
	EmitSep('=');
	std::cout << "Game Cycle " << game_cycle_number << " (" << player_count <<
		" players, " << ((shuffle_flag) ? "" : "not ") << "shuffled)" <<
		std::endl;
	EmitSep('-');

	MhPokerGame my_game(player_count, shuffle_flag);

	{
		std::cout << "Initial player deal" << std::endl;
		HandRankResults rank_result(my_game.RankPlayerHands(false, 250000));
		my_game.EmitPlayerHands(rank_result);
	}

	for (std::size_t count_1 = 0; count_1 < HandSizeCount; ++count_1) {
		EmitSep('-');
		std::cout << "Revealed shared card " << (count_1 + 1) << " of " <<
			HandSizeCount << ": " <<
			GetCardNameShort(my_game.RevealOneSharedCard()) << std::endl;
		{
			HandRankResults rank_result(my_game.RankPlayerHands(false, 250000));
			my_game.EmitPlayerHands(rank_result);
		}
	}

	EmitSep('-');

	unsigned int best_hand_bitset;
	HandFullVector final_hands(my_game.GetFinalPlayerHands(best_hand_bitset));
	std::cout << "Final player hands:" << std::endl;
	for (std::size_t count_1 = 0; count_1 < final_hands.size(); ++count_1) {
		std::cout << my_game.GetPlayerName(count_1) << ": " <<
			final_hands[count_1];
		HandEval hand_eval;
		hand_eval.EvaluatePlayerHand(final_hands[count_1]);
		std::cout << " (" << GetHandTypeName(hand_eval, true) << ")" <<
			((best_hand_bitset & (1 << count_1)) ? " WINNING HAND" : "") <<
			std::endl;
	}

	EmitSep('=');

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void RunTest()
{
	for (std::size_t count_1 = 0; count_1 < 5; ++count_1) {
		RunTest_GameCycle(count_1 + 1, 2, true);
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_GameCycle

namespace TEST_CmdLineHands {

//	////////////////////////////////////////////////////////////////////////////
void RunTest(int argc, char **argv)
{
	EmitSep('=');
	std::cout << "Command Line Hand Test (" << (argc - 1) << " hands)" <<
		std::endl;
	EmitSep('-');

	for (int count_1 = 1; count_1 < argc; ++count_1) {
		try {
			std::cout << "Source Hand: " << argv[count_1];
			HandFull this_hand(ParseHand(argv[count_1]));
			std::cout << " ---> OK" << std::endl;
			std::cout << "Parsed Hand: " << this_hand;
			HandEval hand_eval;
			hand_eval.EvaluatePlayerHand(this_hand);
			std::cout << " (" << GetHandTypeName(hand_eval, true) << ")" <<
				std::endl;
		}
		catch (const std::exception &except) {
			std::cout << " ---> ERROR: " << except.what() << std::endl;
		}
	}

	EmitSep('=');

	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_CmdLineHands

namespace {

//	////////////////////////////////////////////////////////////////////////////
/*
	Windows documentation states that use of the popcnt() intrinsics are
	supported only for some CPU families/revisions and that if not supported,
	the results are "unpredictable" --- whatever that means. Upshot is, we test.
*/
void EnsurePlatformSupport()
{
#ifdef _MSC_VER
	int cpu_info_array[4];

	__cpuid(cpu_info_array, 0);
	if (cpu_info_array[0] < 2)
		throw std::runtime_error("CPU does not support CPUID info type 2.");

	__cpuid(cpu_info_array, 2);
	if (!(cpu_info_array[1] & (1 << 23)))
		throw std::runtime_error("CPU does not support popcnt instruction.");
#endif // #ifdef _MSC_VER
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
		EnsurePlatformSupport();
		TEST_CmdLineHands::RunTest(argc, argv);
		TEST_CheckComboCalc::RunTest();
		TEST_DeckGeneration::RunTest();
		TEST_GenCombos::RunTest_GenSharedHands_1();
//		TEST_GenCombos::RunTest_GenSharedHands_2();
		TEST_GameCycle::RunTest();
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
template <typename PerHandFunctor>
	bool MhPokerGame::EvaluatePlayerHands(PerHandFunctor &hand_func)
{
	DeckVector working_hand(player_count_);

	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1)
		working_hands.resize(SharedCardCount, 0);

	return(EvaluatePlayerHands(0, 0, working_hand, hand_func));
}
*/
//	////////////////////////////////////////////////////////////////////////////

