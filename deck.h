#ifndef __DECK__H__
#define __DECK__H__
#include "card.h"
#include <iostream>
#include <random>
#include <unistd.h>
class Deck {
  private:
    Card *head;

  public:
    Deck();

    void clear();
    bool isEmpty() const;
    void makeDeck();
    Card *drawCard();
};

#endif
