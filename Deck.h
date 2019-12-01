#pragma once
#include <algorithm>
#include <memory>
#include <random>
#include <vector>
#include "Card.h"

class Deck
{
public:
    // Public static methods
    static std::shared_ptr<Deck> Generate52CardDeck();

    // Public methods
    Card Draw();
    void Shuffle();
    std::string ToString();
private:
    // Private members
    std::vector<Card> cards;
    std::default_random_engine randomEngine { std::random_device()() };
};
