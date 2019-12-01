#include "Deck.h"

#pragma region Public static methods
std::shared_ptr<Deck> Deck::Generate52CardDeck()
{
    std::shared_ptr<Deck> deck = std::make_shared<Deck>();
    for (unsigned int s = 0; s < Suit::MAX_SUIT; ++s)
    {
        for (unsigned int r = 0; r < Rank::MAX_RANK; ++r)
        {
            deck->cards.emplace_back<Rank, Suit>(static_cast<Rank>(r), static_cast<Suit>(s));
        }
    }
    return deck;
}
#pragma endregion

#pragma region Public methods
Card Deck::Draw()
{
    Card returnVal = this->cards.back();
    this->cards.pop_back();
    return returnVal;
}

void Deck::Shuffle()
{
    std::shuffle(std::begin(this->cards), std::end(this->cards), randomEngine);
}

std::string Deck::ToString()
{
    std::string returnVal;
    for (unsigned int i = 0; i < this->cards.size(); ++i)
    {
        if (i != 0)
        {
            returnVal += "\n";
        }
        returnVal += this->cards.at(i).ToString();
    }
    return returnVal;
}
#pragma endregion