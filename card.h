#ifndef __CARD__H__
#define __CARD__H__
#include <cstdlib>
#include <string>
class Card {
  private:
    int num;
    std::string pattern;
    Card *next;

  public:
    Card(int num, std::string pattern, Card *card);
    void setNum(int num);
    int getNum() const;

    void setPattern(std::string pattern);
    std::string getPattern() const;

    void setNext(Card *card);
    Card *&getNext();
    const Card *getNext() const;
};

#endif
