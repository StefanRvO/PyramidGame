#pragma once
//Maintain a deck of cards and which cards is and isn't in the deck.
#include <deque>
#include <cstdint>
#include "structs.h"





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
