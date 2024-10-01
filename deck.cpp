#include "deck.h"
Deck::Deck() {
    head = NULL;
    makeDeck();
}
void Deck::makeDeck() {
    if (head != NULL) {
        return;
    }

    std::string list[4] = {"space", "heart", "clover", "diamond"};
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 13; j++) {
                head = new Card(j + 1, list[i], head);
            }
        }
    }
}

bool Deck::isEmpty() const { return head == NULL; }

void Deck::clear() {
    while (head != NULL) {
        Card *tmp = head;
        head = head->getNext();
        tmp->setNext(NULL);
        delete tmp;
    }
}

Card *Deck::drawCard() {
    if (isEmpty()) {
        makeDeck();
    }
    std::string list[4] = {"◆", "♣", "♥", "♠"};
    Card *cur;
    Card *prev;
    Card *res = NULL;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis1(1, 13);
    std::uniform_int_distribution<int> dis2(0, 3);
    while (true) {
        int num = dis1(gen);
        std::string pattern = list[dis2(gen)];
        cur = head;
        if (cur != NULL && cur->getNum() == num &&
            cur->getPattern().compare(pattern)) {
            res = new Card(cur->getNum(), cur->getPattern(), NULL);
            head = head->getNext();
            cur->setNext(NULL);
            delete cur;
        } else {
            cur = cur->getNext();
            for (prev = head; cur != NULL;
                 prev = prev->getNext(), cur = cur->getNext()) {
                if (cur->getNum() == num &&
                    cur->getPattern().compare(pattern)) {
                    res = new Card(cur->getNum(), cur->getPattern(), NULL);
                    prev->setNext(cur->getNext());
                    cur->setNext(NULL);
                    delete cur;
                    break;
                }
            }
        }

        if (res != NULL) {
            break;
        }
    }
    return res;
}
