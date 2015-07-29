#pragma once
//Maintain a deck of cards and which cards is and isn't in the deck.
#include <deque>
#include <cstdint>
#define JACK 11
#define QUEEN 12
#define KING 13

#define HEARTS 0
#define TILES 1
#define CLOVERS 2
#define PIKES 3

struct Card
{
  uint8_t color;
  uint8_t value;
};


class Deck
{
  private:
    std::deque<Card> cards_in_deck;
    std::deque<Card> cards_not_in_deck;
  public:
    Deck();
    ~Deck();
    Card GetCard(); //Get next card in deck.
    void  PutCardToDeck(Card a_card); //Put card to the back of the deck.
    void PutCardToNonDeck(Card a_card); // Put card into non-deck deque.
    void shuffleDeck(); //shuffle the deck.
    void refillDeck();
    void resetDeck();
    int CardsInDeck();
    int TotalCards();

};
