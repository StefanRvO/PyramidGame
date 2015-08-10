#include <vector>
#include "Deck.h"
#include "structs.h"

class PyramidGame
{
  private:
    int size;
    Deck game_deck;
    int number_of_players;
    std::vector<PyramidRow> PyramidRows;
    void addPyramidRow(int cards, int flippedPercent);
    int curRow = 0;
    int curcard = -1;
    int cards_shown = 0;
  public:
    PyramidGame(int flippedPercent, int players, int size);
    ~PyramidGame();
    Card getCard();
    void giveCardBack(Card card);
    PyramidCard showNextCard();
    int remainingCards();
    Card getPlayerCard();
    std::vector<PyramidRow> &getPyramid();
};
