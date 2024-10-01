#ifndef __PLAYER__H__
#define __PLAYER__H__
const int NONE = -1;
const int HIGHCARD = 0;
const int ONEPAIR = 1;
const int TRIPPLE = 2;
const int STRAIGHT = 3;
const int FLUSH = 4;
const int STRAIGHTFLUSH = 5;
const int RTF = 6;
const int ROYALSTRAIGHT = 10;
#include "card.h"
#include <random>
class Player {
  private:
    int remainder;
    int bet;
    int opponent_ranking;
    int my_ranking;
    bool checked;
    bool canPlay;
    bool hasTurn;
    bool folded;
    bool all_ined;
    Card *myCard;
    Card *opponentCard;
    std::string cpuJudge;
    int op_same_cardnum;
    int my_same_cardnum;

  public:
    Player() { init(); }
    void betChips(int amount);
    void getChips(int chip);

    void init() {
        remainder = 20;
        bet = 0;
        opponent_ranking = NONE;
        my_ranking = NONE;
        myCard = NULL;
        opponentCard = NULL;
        checked = false;
        canPlay = true;
        hasTurn = true;
        folded = false;
        all_ined = false;
        cpuJudge = "";
        op_same_cardnum = 0;
        my_same_cardnum = 0;
    };

    void checkRanking(Card *sharedHand[2]);

    void setCard(Card *oppo_card, Card *my_card) {
        opponentCard = oppo_card;
        myCard = my_card;
    };
    void initBet() { this->bet = 0; }

    int getOpponentRanking() const { return opponent_ranking; }

    int getMyRanking() const { return my_ranking; }

    int getRemainder() const { return remainder; }

    int getBet() const { return bet; }

    int getOP_Same_Card_Num() const { return op_same_cardnum; }
    int getMy_Same_Card_Num() const { return my_same_cardnum; }
    Card *getMyCard() { return myCard; }

    void cantPlay() { canPlay = false; }
    void CanPlay() { canPlay = true; }

    void TurnPlayed() { hasTurn = false; }
    void unlockTurn() { hasTurn = true; }

    void haveChecked() { checked = true; }
    void notChecked() { checked = false; }

    void haveFolded() { folded = true; }
    void notFolded() { folded = false; }

    void haveAll_ined() { all_ined = true; }
    void notAll_ined() { all_ined = false; }

    bool getCanPlay() const { return canPlay; }
    bool getHasTurn() const { return hasTurn; }
    bool getChecked() const { return checked; }
    bool getFolded() const { return folded; }
    bool getAll_ined() const { return all_ined; }

    std::string getJudge() const { return cpuJudge; }
    // for computer
    std::string judging(bool first_bet, Player user, Card *sharedHand[2]);
    void betChipsForComputer(Player user);
};

#endif
