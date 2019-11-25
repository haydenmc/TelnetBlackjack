#include "Card.h"

#pragma region Static Methods
std::string Card::ToString(Rank rank)
{
    return Card::rankStrings.at(rank);
}

std::string Card::ToString(Suit suit)
{
    return Card::suitStrings.at(suit);
}
#pragma endregion

#pragma region Static Data
const std::map<Suit, std::string> Card::suitStrings =
{
    { Suit::Diamond, "Diamonds" },
    { Suit::Club,    "Clubs" },
    { Suit::Heart,   "Hearts" },
    { Suit::Spade,   "Spades" },
};

const std::map<Rank, std::string> Card::rankStrings =
{
    { Rank::Ace,   "Ace" },
    { Rank::R2,    "2" },
    { Rank::R3,    "3" },
    { Rank::R4,    "4" },
    { Rank::R5,    "5" },
    { Rank::R6,    "6" },
    { Rank::R7,    "7" },
    { Rank::R8,    "8" },
    { Rank::R9,    "9" },
    { Rank::R10,   "10" },
    { Rank::Jack,  "Jack" },
    { Rank::Queen, "Queen" },
    { Rank::King,  "King" },
};
#pragma endregion

#pragma region Public Methods
Card::Card(Rank rank, Suit suit):
    rank(rank),
    suit(suit)
{ }

std::string Card::ToString()
{
    return Card::ToString(this->rank) + " of " + Card::ToString(this->suit);
}
#pragma endregion