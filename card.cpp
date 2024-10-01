#include "card.h"
Card::Card(int num = 0, std::string pattern = " ", Card *card = NULL) {
    this->num = num;
    this->pattern = pattern;
    next = card;
};

void Card::setNum(int num) { this->num = num; };
int Card::getNum() const { return num; }
void Card::setPattern(std::string pattern) { this->pattern = pattern; }
std::string Card::getPattern() const { return pattern; }
void Card::setNext(Card *card) { next = card; }
Card *&Card::getNext() { return next; }
const Card *Card::getNext() const { return next; }
