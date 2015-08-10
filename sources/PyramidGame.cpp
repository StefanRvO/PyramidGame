#include <ctime>
#include "../headers/PyramidGame.h"
#include <cstdlib>
PyramidGame::PyramidGame(int flippedPercent, int players, int size_)
{
  srand(time(NULL));
  number_of_players = players;
  //generate the Pyramid
  size = size_;
  for(int i = size; i > 0; i--)
  {
    addPyramidRow(i, flippedPercent);
  }
  game_deck.shuffleDeck();
}

PyramidGame::~PyramidGame()
{
  for(auto row : PyramidRows)
  {
    delete[] row.cards;
  }
}

void PyramidGame::addPyramidRow(int cards, int flippedPercent)
{
  PyramidRow row;
  row.cards = new PyramidCard[cards];
  row.number_of_cards = cards;
  for(int i = 0; i < cards; i++)
  {
    row.cards[i].card = game_deck.GetCard();
    row.cards[i].shown = false;
    if(rand()%100 + 1 <= flippedPercent) row.cards[i].flipped = true;
    else row.cards[i].flipped = false;

  }
  PyramidRows.push_back(row);
}

Card PyramidGame::getCard()
{
  if(!game_deck.CardsInDeck())
  {
    game_deck.refillDeck();
    game_deck.shuffleDeck();
  }
  return game_deck.GetCard();
}

void PyramidGame::giveCardBack(Card card)
{
  game_deck.PutCardToNonDeck(card);
}

PyramidCard PyramidGame::showNextCard()
{
  curcard++;
  if(curcard > PyramidRows[curRow].number_of_cards)
  {
    curRow++;
    curcard = 0;
  }
  PyramidRows[curRow].cards[curcard].shown = true;
  cards_shown++;
  return PyramidRows[curRow].cards[curcard];
}

int PyramidGame::remainingCards()
{
  int sum = 0;
  for(int i = 1; i <= size; i++) sum += i;
  return sum;
}
Card PyramidGame::getPlayerCard()
{
  return game_deck.GetCard();
}

std::vector<PyramidRow> &PyramidGame::getPyramid()
{
  return PyramidRows;
}
