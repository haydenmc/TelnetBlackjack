#pragma once
#include <string>
#include <map>

enum Suit
{
    Diamond,
    Club,
    Heart,
    Spade
};

enum Rank
{
    Ace,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    Jack,
    Queen,
    King
};

class Card
{
public:
    // Public static methods
    static std::string ToString(Rank rank);
    static std::string ToString(Suit suit);

    // Public instance methods
    Card(Rank rank, Suit suit);
    std::string ToString();

private:
    // Static data
    static const std::map<Suit, std::string> suitStrings;
    static const std::map<Rank, std::string> rankStrings;

    // Private members
    Rank rank;
    Suit suit;
};
