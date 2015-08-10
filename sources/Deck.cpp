#include "../headers/Deck.h"
#include <algorithm>
Deck::Deck()
{
  resetDeck();
}

Deck::~Deck()
{
}

void Deck::resetDeck()
{
  cards_in_deck.clear();
  for(uint8_t i = 0; i < 4; i++)
  {
    for(int8_t j = 1; j < 14; j++)
    {
      Card this_card;
      this_card.color = i;
      this_card.value = j;
      cards_in_deck.push_back(this_card);
    }
  }
  cards_not_in_deck.clear();
}
Card Deck::GetCard()
{
  Card return_card = cards_in_deck.front();
  cards_in_deck.pop_front();
  return return_card;
}

void Deck::PutCardToDeck(Card a_card)
{ //Put card to the back of the deck.
  cards_in_deck.push_back(a_card);
}

void Deck::PutCardToNonDeck(Card a_card)
{ //Put card to the back of the non-deck deque.
  cards_not_in_deck.push_back(a_card);
}

void Deck::shuffleDeck() //shuffle the deck.
{
  std::random_shuffle(cards_in_deck.begin(), cards_in_deck.end());
}

void Deck::refillDeck()
{ //refillDeck from cards not in Deck.
  cards_in_deck = cards_not_in_deck;
  cards_not_in_deck.clear();
}

int Deck::CardsInDeck()
{
  return cards_in_deck.size();
}

int Deck::TotalCards()
{
  return cards_in_deck.size() + cards_not_in_deck.size();
}
