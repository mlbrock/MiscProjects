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
const Card CardCount = 52;
const Card SuitCount =  4;
const Card RankCount = CardCount / SuitCount;
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
	return(static_cast<Card>(GetValidCard(in_card) % RankCount));
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
	return(static_cast<Suit>(GetValidCard(card) / RankCount));
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
class MhPokerGame {
public:
	static const std::size_t SharedCardCount = 5;	//	Less than CardCount
	static const std::size_t PlayerCardCount = 2;	// As it's in a std::pair<>

	MhPokerGame(std::size_t player_count, bool shuffle_flag = true);

	Card RevealOneSharedCard();

	std::vector<double> RankPlayerHands(unsigned int max_usecs = 0);

	std::string GetPlayerHandString(std::size_t player_index) const;
	void EmitPlayerHands(std::ostream &o_str = std::cout) const;

private:
	Deck             deck_;
	Deck             revealed_cards_;
	std::size_t      player_count_;
	HandPlayerVector player_hands_;
	DeckVector       player_deck_;
	DeckVector       player_scratchpad_;

	bool EvaluatePlayerHands(std::size_t target_element,
			std::size_t start_index, DeckVector &working_hands);
	bool EvaluatePlayerHands(DeckVector &working_hands);
};
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
	if ((player_count_ < 2) ||
		(player_count_ > ((CardCount - SharedCardCount) / PlayerCardCount)))
		throw std::invalid_argument("Invalid player count.");

	revealed_cards_.reserve(SharedCardCount);
	player_hands_.reserve(player_count_);
	player_deck_.resize(player_count_);
	player_scratchpad_.resize(player_count_);

	//	Cheesy deal doesn't rotate among players, but is nice in testing...
	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		player_hands_.push_back(std::make_pair(deck_[deck_.size() - 1],
			deck_[deck_.size() - 2]));
		deck_.resize(deck_.size() - 2);
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

//	////////////////////////////////////////////////////////////////////////////
std::vector<double> MhPokerGame::RankPlayerHands(unsigned int max_usecs)
{
/*
	struct PerHandAssessor {
		PerHandAssessor(const HandPlayerVector &player_hands,
			unsigned int max_usecs)
			:player_hands_(player_hands)
			,player_ranking_(player_hands.size())
			,hands_evaluated_(0)
			,max_usecs_(max_usecs)
			,end_time_(boost::posix_time::microsec_clock::universal_time() +
				boost::posix_time::microsec(max_usecs))
		{
		}

		inline bool operator () (const Deck &working_hand)
		{

			++hands_evaluated_;

			return((!max_usecs_) ||
				(boost::posix_time::microsec_clock::universal_time() < end_time_));
		}

		const HandPlayerVector   &player_hands_;
		std::vector<double>       player_ranking_;
		std::size_t               hands_evaluated_;
		unsigned int              max_usecs_;
		boost::posix_time::ptime  end_time_;

	private:
		PerHandAssessor & operator = (const PerHandAssessor &);	//	Not defined.
	};

	PerHandAssessor my_func(player_hands_, max_usecs);

	GenerateSharedHands(deck_, SharedCardCount, my_func);

	return(my_func.player_ranking_);
*/
	if (revealed_cards_.empty()) {
	}

	return(std::vector<double>());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string MhPokerGame::GetPlayerHandString(std::size_t player_index) const
{
	if (player_index >= player_hands_.size())
		throw std::invalid_argument("Invalid player index.");

	std::ostringstream o_str;

	o_str << "Player-" << std::setfill('0') << std::setw(2) << player_index <<
		": " << GetCardNameShort(player_hands_[player_index].first, true) <<
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
bool MhPokerGame::EvaluatePlayerHands(std::size_t target_element,
	std::size_t start_index, DeckVector &working_hands)
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

//	////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
typedef unsigned __int16 BitsType;
typedef BitsType         BitsRank;
typedef BitsType         BitsSuit;
inline BitsType MyPopCount(BitsType src_bits)
{
	return(__popcnt16(src_bits));
}
//# pragma warning(push)
//# pragma warning(disable:4061 4350 4365 4514 4625 4626 4710 4820 4986)
//	Returns position of lowest set bit + 1. Zero if not bits are set.
inline BitsType MyLowestBitIndex(BitsType src_bits)
{
   unsigned long Index;
   unsigned long Mask = src_bits;

	return(static_cast<BitsType>((!_BitScanForward(&Index, Mask)) ?
		0 : (Mask + 1)));
}
//	Returns position of highest set bit + 1. Zero if not bits are set.
inline BitsType MyHighestBitIndex(BitsType src_bits)
{
   unsigned long Index;
   unsigned long Mask = src_bits;

	return(static_cast<BitsType>((!_BitScanReverse(&Index, Mask)) ?
		0 : (Mask + 1)));
}
#else	//	Assumes Linux...
typedef u_int32_t BitsType;
typedef BitsType  BitsRank;
typedef BitsType  BitsSuit;
inline BitsType MyPopCount(BitsType src_bits)
{
	return(static_cast<BitsType>(__builtin_popcount(src_bits)));
}
//	Returns position of lowest set bit + 1. Zero if not bits are set.
inline BitsType MyLowestBitIndex(BitsType src_bits)
{
	return((!src_bits) ? 0 : static_cast<BitsType>(__builtin_ffs(src)));
}
//	Returns position of highest set bit + 1. Zero if not bits are set.
inline BitsType MyHighestBitIndex(BitsType src_bits)
{
	return((!src_bits) ? 0 : static_cast<BitsType>(
		((sizeof(src_bits) * CHAR_BIT) - __builtin_clz(src_bits))));
}
#endif // #ifdef _MSC_VER
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
struct HandFull {
	HandFull()
	{
		::memset(cards_, '\0', sizeof(cards_));
	}

	std::size_t size() const
	{
		return(MhPokerGame::SharedCardCount);
	}

	const Card & operator [] (std::size_t card_index) const
	{
		if (card_index >= MhPokerGame::SharedCardCount)
			throw std::invalid_argument("Invalid card index specified.");

		return(cards_[card_index]);
	}

	Card & operator [] (std::size_t card_index)
	{
		return(const_cast<Card &>(
			const_cast<const HandFull * const>(this)->operator[](card_index)));
	}

	Card cards_[MhPokerGame::SharedCardCount];
};
*/
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

//MhPokerGame::SharedCardCount
//	////////////////////////////////////////////////////////////////////////////
typedef CardArray<MhPokerGame::SharedCardCount> HandFull;
typedef CardArray<RankCount>                    HandRank;
//typedef std::vector<HandFull>          HandFullVector;
//typedef HandFullVector::const_iterator HandFullVectorIterC;
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
	HandHighCard
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool MhPokerGame::EvaluatePlayerHands(DeckVector &working_hands)
{
	HandType    best_hand_type  = HandHighCard;
	std::size_t best_hand_hint  = 0;
	std::size_t best_hand_index = 0;
	BitsRank    last_bits_rank  = 0;
	BitsSuit    last_bits_suit  = 0;

	for (std::size_t count_1 = 0; count_1 < player_count_; ++count_1) {
		const Deck &player_hand = working_hands[count_1];
		HandRank    hand_rank;
/*
		BitsRank bits_rank =
			static_cast<BitsRank>(
			1 << GetValidRank(player_hand[0]) |
			1 << GetValidRank(player_hand[1]) |
			1 << GetValidRank(player_hand[2]) |
			1 << GetValidRank(player_hand[3]) |
			1 << GetValidRank(player_hand[4]));
		BitsSuit bits_suit =
			static_cast<BitsSuit>(
			1 << GetValidSuit(player_hand[0]) |
			1 << GetValidSuit(player_hand[1]) |
			1 << GetValidSuit(player_hand[2]) |
			1 << GetValidSuit(player_hand[3]) |
			1 << GetValidSuit(player_hand[4]));
*/
		BitsRank bits_rank = 0;
		BitsSuit bits_suit = 0;
		for (std::size_t count_2 = 0; count_2 < player_hand.size(); ++count_2) {
			bits_rank          |=
				static_cast<BitsRank>(1 << GetValidRank(player_hand[count_2]));
			bits_suit          |=
				static_cast<BitsSuit>(1 << GetValidSuit(player_hand[count_2]));
			hand_rank[GetValidRank(player_hand[count_2])]++;
		}
		BitsRank    count_ranks = MyPopCount(bits_rank);
		BitsSuit    count_suits = MyPopCount(bits_suit);
		HandType    hand_type;
		std::size_t hand_hint;
		/*
			Ace low straight:
				1000000001111
				1   0   0   F
		*/
		if ((count_suits == 1)           &&
			 ((bits_rank == 0x31)         ||
			  (bits_rank == (0x31 <<  1)) ||
			  (bits_rank == (0x31 <<  2)) ||
			  (bits_rank == (0x31 <<  3)) ||
			  (bits_rank == (0x31 <<  4)) ||
			  (bits_rank == (0x31 <<  5)) ||
			  (bits_rank == (0x31 <<  6)) ||
			  (bits_rank == (0x31 <<  7)) ||
			  (bits_rank == (0x31 <<  8)) ||
			  (bits_rank == 0x100f))) {
			hand_type = HandStraightFlush;
			hand_hint = 0;
		}
		else ((count_ranks == 2) && (count_suits == 4) &&
			((hand_rank[bit_low = (MyLowestBitIndex(bits_rank) - 1)] == 4) ||
			 (hand_rank[bit_high = (MyHighestBitIndex(bits_rank) - 1)] == 4))) {
			hand_type = HandFourOfAKind;
			hand_hint = (hand_rank[bit_low] == 4) ? bit_low : bit_high;
		}
		if (!count_1) {
			last_bits_rank = bits_rank;
			last_bits_suit = bits_suit;
		}
		else {
		}
	}

/*
	++hands_evaluated_;
*/

/*
	return((!max_usecs_) ||
		(boost::posix_time::microsec_clock::universal_time() < end_time_));
*/
	return(true);
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

namespace TEST_GameCycle {

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunGameCycle()
{
	MhPokerGame my_game(2, false);

	my_game.EmitPlayerHands();

	my_game.RevealOneSharedCard();

	std::vector<double> player_rank(my_game.RankPlayerHands(250000));
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_GameCycle

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
      HandFull hand_full_m;
const HandFull hand_full_c;
Card card_m = hand_full_m[3];
Card card_c = hand_full_c[3];
/*
		TEST_CheckDeck();
		TEST_GenCombos::TEST_GenSharedHands_1();
		TEST_GenCombos::TEST_GenSharedHands_2();
*/
		TEST_GameCycle::TEST_RunGameCycle();
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

