#include "card.h"
#include "deck.h"
#include "player.h"
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

WINDOW *menu;
WINDOW *table;
WINDOW *play;
WINDOW *gameres;
bool first_bet = true;

bool game_enter_inputted = false;
int mainmenuCursor = 0;
int playmenuCursor = 2;
string mainmenuList[4] = {"START", "WINRATE", "RULE", "QUIT"};
string playmenuList[6] = {"Bet", "Check", "Call", "Raise", "Fold", "All-In"};
void mainmenuset();
void tableset();
void playset();
void mainmenu();

void rulemenu();

void winratemenu();

void gamedisplay();
void tabledisplay();
void playdisplay();
void cpuPlay();
void gameEnterAction(int selected);
void roundEnd();

void endFunc();

int read_file();
int write_file();

string state;

typedef struct info {
    int userWin;
    int userLose;
    int userTotalGame;
    double userWinRate;
} INFO;

INFO *gameInfo = (INFO *)malloc(sizeof(INFO));
int *userInfo = (int *)malloc(sizeof(int));

void signalHandler(int signum);

Player user;
Player cpu;

Deck deck;

Card *sharedHand[2] = {NULL, NULL};

int main(void) {
    atexit(endFunc);
    *userInfo = open("userInfo.bin", O_CREAT | O_RDWR,
                     0644); // not only O_CREAT, but also O_RDWR is needed.
    gameInfo->userLose = 0;
    gameInfo->userWin = 0;
    gameInfo->userTotalGame = 0;
    gameInfo->userWinRate = 0;

    if (*userInfo < 0) {
        perror("file open error:");
        return -1;
    }
    if (read_file() < 0) {
        perror("read error");
    }
    signal(SIGWINCH, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);
    int x, y = 0;
    int input;
    initscr();
    // first
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_CYAN);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
    keypad(stdscr, true);
    noecho();
    cbreak();
    curs_set(0);
    refresh();
    mainmenu();
    while (1) {
        input = getch();
        if (state.compare("main") == 0) {
            switch (input) {
            case KEY_RIGHT:
                mainmenuCursor++;
                delwin(menu);
                mainmenu();
                break;
            case KEY_LEFT:
                mainmenuCursor--;
                if (mainmenuCursor < 0) {
                    mainmenuCursor = 3;
                }
                delwin(menu);
                endwin();
                mainmenu();
                break;
            case KEY_UP:
                /* code */
                mainmenuCursor += 2;
                delwin(menu);
                endwin();
                mainmenu();
                break;
            case KEY_DOWN:
                /* code */
                mainmenuCursor -= 2;
                if (mainmenuCursor < 0) {
                    mainmenuCursor = 4 + mainmenuCursor;
                }
                delwin(menu);
                endwin();
                mainmenu();
                break;
            case KEY_ENTER:
            case '\n':
                /* code */
                delwin(menu);
                endwin();
                int condition = mainmenuCursor % 4;
                switch (condition) {
                case 0: {
                    Card *cpu_card = deck.drawCard();
                    Card *user_card = deck.drawCard();
                    sharedHand[0] = deck.drawCard();
                    sharedHand[1] = deck.drawCard();
                    user.init();
                    cpu.init();
                    user.setCard(cpu_card, user_card);
                    cpu.setCard(user_card, cpu_card);
                    user.betChips(1);
                    cpu.betChips(1);
                    gamedisplay();
                    break;
                }
                case 1: {
                    winratemenu();
                    break;
                }
                case 2: {
                    rulemenu();
                    break;
                }
                case 3: {
                    delwin(menu);
                    endwin();
                    return 0;
                }
                }
            }
        } else if (state.compare("rule") == 0) {
            if (input == '\n' || input == KEY_ENTER) {
                delwin(menu);
                endwin();
                mainmenu();
            }
        } else if (state.compare("winrate") == 0) {
            if (input == '\n' || input == KEY_ENTER) {
                delwin(menu);
                endwin();
                mainmenu();
            }
        } else if (state.compare("gameplay_choose") == 0) {

            //--first bet initialize
            //---one round end condition

            switch (input) {
            case KEY_UP:
                playmenuCursor--;
                delwin(play);
                delwin(table);
                endwin();
                gamedisplay();
                break;
            case KEY_DOWN:
                playmenuCursor++;
                if (playmenuCursor < 0) {
                    playmenuCursor = 5;
                }
                delwin(play);
                delwin(table);
                endwin();
                gamedisplay();
                break;
            case KEY_ENTER:
            case '\n':
                int selected = playmenuCursor % 6;
                bool canCheckInAllin = cpu.getBet() - user.getBet() <= 0;
                bool canCheck = cpu.getBet() - user.getBet() == 0;
                bool canCall =
                    cpu.getBet() - user.getBet() > 0 &&
                    user.getRemainder() >= cpu.getBet() - user.getBet();
                bool canBet =
                    cpu.getBet() - user.getBet() > 0 &&
                    user.getRemainder() > cpu.getBet() - user.getBet();
                if (cpu.getAll_ined()) {
                    if (selected == 0 || selected == 3) {
                        continue;
                    } else if ((selected == 1 && !canCheckInAllin)) {
                        // check fail.
                        continue;
                    } else if (selected == 2 && !canCall) {
                        // call fail
                        continue;
                    }
                } else {
                    if (selected == 0 && !first_bet && !canBet) {
                        // bet fail
                        continue;
                    } else if (selected == 1 && !canCheck) {
                        // check fail.
                        continue;
                    } else if (selected == 2 && !canCall) {
                        // call fail
                        continue;
                    } else if (selected == 3 && !canBet) {
                        // raise fail
                        continue;
                    }
                }
                delwin(play);
                gameEnterAction(selected);
                break;
            }
        }
    }
}
void mainmenu() {
    state = "main";
    keypad(stdscr, true);
    noecho();
    cbreak();
    mainmenuset();
    int spotindex = mainmenuCursor % 4;
    string origin = mainmenuList[spotindex];
    mainmenuList[spotindex].insert(0, 1, '>');
    mvwprintw(menu, 1, 42, "Indian Holdem");
    mvwprintw(menu, 20, 22, mainmenuList[0].c_str());
    mvwprintw(menu, 20, 56, mainmenuList[1].c_str());
    mvwprintw(menu, 22, 22, mainmenuList[2].c_str());
    mvwprintw(menu, 22, 56, mainmenuList[3].c_str());
    wrefresh(menu);
    mainmenuList[spotindex] = origin;
}
void rulemenu() {
    state = "rule";
    mainmenuset();
    mvwprintw(menu, 1, 32, "Rule of Indian Holdem");
    mvwprintw(menu, 5, 0, "1. how to bet");
    mvwprintw(menu, 6, 2,
              "check :you can do this when amount of chips each bets is same.");
    mvwprintw(menu, 7, 9, "just skip your turn");
    mvwprintw(menu, 8, 2, "bet :you can do this when nobody bets chips.");
    mvwprintw(menu, 9, 7, "you can bet chips as you want");
    mvwprintw(menu, 10, 2, "call :you can do this when someone bets chips.");
    mvwprintw(menu, 11, 8, "you bet your chips as much as opponent bets.");
    mvwprintw(menu, 12, 2, "raise :you can do this when someone bets chips.");
    mvwprintw(menu, 13, 9, "you can bet your chips more than opponent.");
    mvwprintw(menu, 14, 2, "fold :you can do this anytime.");
    mvwprintw(menu, 15, 9,
              "give chips that you bet to "
              "opponent and give up that game.");
    mvwprintw(menu, 16, 2,
              "all-in :you can do this anytime. you bet your every chip.");
    mvwprintw(menu, 17, 10,
              "if you don't have chips enough to call, you can only choose "
              "fold or this.");
    mvwprintw(menu, 20, 0, "2. game flow");
    mvwprintw(menu, 21, 2, "i)each player gets one card.");
    mvwprintw(menu, 22, 4, "each player can check opponent's card only.");
    mvwprintw(menu, 23, 2, "ii)there are two card on the table.");
    mvwprintw(menu, 24, 5,
              "this two card are shared hand each player can uses.");
    mvwprintw(menu, 25, 2,
              "iii)bets chips according to opponent's card and shared hand. ");
    mvwprintw(menu, 26, 2,
              "iv)after end of bet, each player open own card to everyone. ");
    mvwprintw(menu, 27, 5, "player who has higher hand-ranking wins.");
    mvwprintw(menu, 28, 2,
              "v)hand-ranking: straight flush>flush>straight>tripple>one "
              "pair>high card ");
    mvwprintw(
        menu, 29, 4,
        "if each has same hand-ranking, player who has higer number wins.");
    mvwprintw(menu, 31, 0, "3.else");
    mvwprintw(menu, 32, 2, "i)each player get 20 chips at start of game.");
    mvwprintw(menu, 33, 2,
              "ii)entire game proceed until one player lose all chips. player "
              "who lose all chips defeat.");
    mvwprintw(
        menu, 34, 2,
        "iii)deck contain 104 cards. if there is no card in the deck, deck "
        "will be filled with 104 cards.");
    mvwprintw(menu, 35, 2,
              "iv)in this software, cpu always plays their turn first. ");
    mvwprintw(menu, 36, 2,
              "v)player who folds when he get equal to or higer ranking than "
              "TRIPPLE ");
    mvwprintw(menu, 37, 4,
              "will pay 10 more chips as penalty.(if he has less than 10 chip, "
              "he lose that game!)");
    mvwprintw(menu, 39, 32, "press enter to return to main menu");
    wrefresh(menu);
}
void winratemenu() {
    state = "winrate";
    mainmenuset();
    if (gameInfo->userTotalGame == 0) {
        gameInfo->userWinRate = 0;
    } else {
        gameInfo->userWinRate =
            (double)gameInfo->userWin / gameInfo->userTotalGame;
    }
    string numberofwin = "number of wins: " + to_string(gameInfo->userWin);
    string numberoflose = "number of defeat: " + to_string(gameInfo->userLose);
    string numberofgame =
        "number of games you've played: " + to_string(gameInfo->userTotalGame);
    string winrate_str =
        "win rate: " + to_string(gameInfo->userWinRate * 100) + "%%";

    mvwprintw(menu, 1, 32, "your game statistics");
    mvwprintw(menu, 10, 2, numberofgame.c_str());
    mvwprintw(menu, 11, 2, numberofwin.c_str());
    mvwprintw(menu, 12, 2, numberoflose.c_str());
    mvwprintw(menu, 13, 2, winrate_str.c_str());
    mvwprintw(menu, 25, 32, "press enter to return to main menu");
    wrefresh(menu);
}
void mainmenuset() {
    menu = newwin(50, 150, 0, 0);
    wbkgd(menu, COLOR_PAIR(1));
}
void tableset() {
    table = newwin(30, 150, 0, 0);
    wbkgd(table, COLOR_PAIR(2));
}
void playset() {
    play = newwin(20, 150, 30, 0);
    wbkgd(play, COLOR_PAIR(3));
}
void endset() {
    gameres = newwin(50, 150, 0, 0);
    wbkgd(gameres, COLOR_PAIR(4));
}
void gamedisplay() {
    int c;
    state = "gameplay_choose";
    // first_bet is global, so I take care of it.
    if (user.getBet() - cpu.getBet() == 0) {
        first_bet = true;
    }
    if ((!user.getCanPlay() && !cpu.getCanPlay()) ||
        (user.getChecked() && cpu.getChecked()) ||
        (user.getFolded() || cpu.getFolded())) {
        delwin(play);
        delwin(table);
        endwin();
        roundEnd();
    }
    int spotindex = 0;

    spotindex = playmenuCursor % 6;

    string origin = playmenuList[spotindex];
    playmenuList[spotindex].insert(0, 1, '>');
    tableset();
    playset();
    cpuPlay();
    tabledisplay();
    playdisplay();
    wrefresh(table);
    wrefresh(play);
    cbreak();
    keypad(stdscr, true);
    if (cpu.getJudge().compare("fold") == 0) {
        sleep(3);
    }
    playmenuList[spotindex] = origin;
    if ((!user.getCanPlay() && !cpu.getCanPlay()) ||
        (user.getChecked() && cpu.getChecked()) ||
        (user.getFolded() || cpu.getFolded())) {
        delwin(play);
        delwin(table);
        endwin();
        roundEnd();
    }
}
void tabledisplay() {
    mvwprintw(table, 0, 0, "shared hand");
    int s0_num = sharedHand[0]->getNum();
    int s1_num = sharedHand[1]->getNum();
    string s0_card;
    string s1_card;
    switch (s0_num) {
    case 1:
        s0_card = "A\n " + sharedHand[0]->getPattern();
        break;
    case 11:
        s0_card = "J\n " + sharedHand[0]->getPattern();
        break;
    case 12:
        s0_card = "Q\n " + sharedHand[0]->getPattern();
        break;
    case 13:
        s0_card = "K\n " + sharedHand[0]->getPattern();
        break;
    default:
        s0_card = to_string(s0_num) + "\n " + sharedHand[0]->getPattern();
        break;
    }
    switch (s1_num) {
    case 1:
        s1_card = "A\n " + sharedHand[1]->getPattern();
        break;
    case 11:
        s1_card = "J\n " + sharedHand[1]->getPattern();
        break;
    case 12:
        s1_card = "Q\n " + sharedHand[1]->getPattern();
        break;
    case 13:
        s1_card = "K\n " + sharedHand[1]->getPattern();
        break;
    default:
        s1_card = to_string(s1_num) + "\n " + sharedHand[1]->getPattern();
        break;
    }

    mvwprintw(table, 2, 0, "-----------");
    mvwprintw(table, 3, 2, s0_card.c_str());

    mvwprintw(table, 11, 0, "-----------");
    mvwprintw(table, 12, 2, s1_card.c_str());
    // mvwprintw(table, 3, 1, );
    for (int i = 0; i < 6; i++) {
        mvwprintw(table, 3 + i, 0, "|");
        mvwprintw(table, 3 + i, 10, "|");

        mvwprintw(table, 12 + i, 0, "|");
        mvwprintw(table, 12 + i, 10, "|");
    }
    mvwprintw(table, 8, 0, "-----------");
    mvwprintw(table, 17, 0, "-----------");

    // shared hand part
    for (int i = 0; i < 150; i++) {
        mvwprintw(table, 19, i, "-");
    }
    mvwprintw(table, 20, 0, "opponent's card");
    // opponent's card part
    int cpu_num = cpu.getMyCard()->getNum();
    string cpu_card;
    switch (cpu_num) {
    case 1:
        cpu_card = "A\n " + cpu.getMyCard()->getPattern();
        break;
    case 11:
        cpu_card = "J\n " + cpu.getMyCard()->getPattern();
        break;
    case 12:
        cpu_card = "Q\n " + cpu.getMyCard()->getPattern();
        break;
    case 13:
        cpu_card = "K\n " + cpu.getMyCard()->getPattern();
        break;
    default:
        cpu_card = to_string(cpu_num) + "\n " + cpu.getMyCard()->getPattern();
        break;
    }
    mvwprintw(table, 23, 2, cpu_card.c_str());
    mvwprintw(table, 22, 0, "-----------");
    for (int i = 0; i < 6; i++) {
        mvwprintw(table, 23 + i, 0, "|");
        mvwprintw(table, 23 + i, 10, "|");
    }
    mvwprintw(table, 28, 0, "-----------");
}
void playdisplay() {
    string whosturn;
    string opponentRanking;

    switch (cpu.getMyRanking()) {
    case HIGHCARD:
        opponentRanking = "opponent : HIGH-CARD";
        break;
    case ONEPAIR:
        opponentRanking = "opponent : ONE PAIR";
        break;
    case TRIPPLE:
        opponentRanking = "opponent : TRIPPLE";
        break;
    case STRAIGHT:
        opponentRanking = "opponent : STRAIGHT";
        break;
    case FLUSH:
        opponentRanking = "opponent : FLUSH";
        break;
    case STRAIGHTFLUSH:
        opponentRanking = "opponent : STRAIGHT FLUSH";
        break;
    case RTF:
        opponentRanking = "opponent : ROYAL STARIGHT FLUSH";
        break;
    default:
        opponentRanking = "none";
        break;
    }
    string user_remainder = "chips you own: " + to_string(user.getRemainder());
    string cpu_remainder = "chips cpu own: " + to_string(cpu.getRemainder());
    string user_bet = "chips you bet: " + to_string(user.getBet());
    string cpu_bet = "chips cpu bet: " + to_string(cpu.getBet());
    mvwprintw(play, 8, 0, cpu_remainder.c_str());
    mvwprintw(play, 9, 0, cpu_bet.c_str());
    mvwprintw(play, 10, 0, user_remainder.c_str());
    mvwprintw(play, 11, 0, user_bet.c_str());

    mvwprintw(play, 0, 17, opponentRanking.c_str());
    for (int i = 0; i < 6; i++) {
        mvwprintw(play, i + 2, 0, playmenuList[i].c_str());
    }
}
void cpuPlay() {
    if (cpu.getCanPlay() && cpu.getHasTurn()) {
        cpu.checkRanking(sharedHand);
        string cpu_thinking = cpu.judging(first_bet, user, sharedHand);
        if (cpu_thinking.compare("fold") == 0) {
            mvwprintw(play, 2, 10, "cpu: fold");
            cpu.betChipsForComputer(user);
            first_bet = false;
        } else if (cpu_thinking.compare("check") == 0) {
            cpu.betChipsForComputer(user);
            mvwprintw(play, 2, 10, "cpu: check");
        } else if (cpu_thinking.compare("call") == 0) {

            cpu.betChipsForComputer(user);
            mvwprintw(play, 2, 10, "cpu: call");
        } else if (cpu_thinking.compare("raise") == 0) {
            cpu.betChipsForComputer(user);
            mvwprintw(play, 2, 10, "cpu: raise ");
        } else if (cpu_thinking.compare("all-in") == 0) {
            cpu.betChipsForComputer(user);
            mvwprintw(play, 2, 10, "cpu: all-in");
            first_bet = false;
        } else if (cpu_thinking.compare("bet") == 0) {
            first_bet = false;
            cpu.betChipsForComputer(user);
            mvwprintw(play, 2, 10, "cpu: bet");
        }
        cpu.TurnPlayed();
    } else {
        string showCpuJudge = "cpu: " + cpu.getJudge();
        mvwprintw(play, 2, 10, showCpuJudge.c_str());
        return;
    }
}
void gameEnterAction(int selected) {
    nocbreak();
    keypad(stdscr, false);
    state = "gameplay_bet";
    playset();
    cpuPlay();
    int spotindex = 0;
    int ran;
    spotindex = playmenuCursor % 6;
    random_device rd;
    mt19937 gen(rd());
    int betDiffer = cpu.getBet() - user.getBet();
    uniform_int_distribution<int> dis1(betDiffer + 1, user.getRemainder() / 3);
    uniform_int_distribution<int> dis2(betDiffer + 1, user.getRemainder() - 1);

    string origin = playmenuList[spotindex];
    playmenuList[spotindex].insert(0, 1, '>');
    playdisplay();
    if (cpu.getAll_ined()) {
        switch (selected) {
        case 1:
            mvwprintw(play, 5, 10, "you selected check.");

            break;
        case 2:
            mvwprintw(play, 5, 10, "you selected call.");
            user.betChips(cpu.getBet() - user.getBet());
            user.cantPlay();

            break;
        case 4:
            mvwprintw(play, 5, 10, "you selected fold.");
            user.haveFolded();
            user.cantPlay();

            break;
        case 5:
            mvwprintw(play, 5, 10, "you selected all-in.");
            user.betChips(user.getRemainder());
            user.haveAll_ined();
            user.cantPlay();

            break;
        }
    } else {
        switch (selected) {
        case 0:
            // bet
            mvwprintw(play, 5, 10, "you selected bet.");
            mvwprintw(play, 6, 10, "you bet random chips. ");
            if (betDiffer + 1 > user.getRemainder() / 3) {
                ran = dis2(gen);
            } else if (betDiffer + 1 < user.getRemainder() - 1) {
                ran = dis1(gen);
            } else {
                ran = user.getRemainder();
            }
            while (ran > user.getRemainder()) {

                if (betDiffer + 1 > user.getRemainder() / 3) {
                    ran = dis2(gen);
                } else if (betDiffer + 1 < user.getRemainder() - 1) {
                    ran = dis1(gen);
                } else {
                    ran = user.getRemainder();
                }
            }
            user.betChips(ran);
            first_bet = false;
            break;
        case 1:
            // check
            mvwprintw(play, 5, 10, "you selected check.");
            user.haveChecked();

            break;
        case 2:
            // call
            mvwprintw(play, 5, 10, "you selected call.");
            user.betChips(cpu.getBet() - user.getBet());

            break;
        case 3:
            // raise
            mvwprintw(play, 5, 10, "you selected raise.");
            mvwprintw(play, 6, 10, "you bet random chips. ");
            if (betDiffer + 1 > user.getRemainder() / 3) {
                ran = dis2(gen);
            } else if (user.getRemainder() - 1 > betDiffer + 1) {
                ran = dis1(gen);
            } else {
                ran = user.getRemainder();
            }
            while (ran > user.getRemainder()) {

                if (betDiffer + 1 > user.getRemainder() / 3) {
                    ran = dis2(gen);
                } else if (betDiffer + 1 < user.getRemainder() - 1) {
                    ran = dis1(gen);
                } else {
                    ran = user.getRemainder();
                }
            }
            user.betChips(ran);
            break;
        case 4:
            // fold
            mvwprintw(play, 5, 10, "you selected fold.");
            user.haveFolded();
            user.cantPlay();
            first_bet = false;

            break;
        case 5:
            // all-in
            mvwprintw(play, 5, 10, "you selected all-in.");
            user.betChips(user.getRemainder());
            user.haveAll_ined();
            user.cantPlay();

            first_bet = false;
            break;
        }
    }
    wrefresh(play);

    sleep(3);
    playmenuList[spotindex] = origin;
    cpu.unlockTurn();
    delwin(play);
    delwin(table);
    endwin();
    gamedisplay();
}
void roundEnd() {
    nocbreak();
    keypad(stdscr, false);
    state = "round_end";
    endset();
    int shared_highnum;
    if (sharedHand[0]->getNum() == 1 || sharedHand[1]->getNum() == 1) {
        shared_highnum = 1;
    } else {
        shared_highnum = (sharedHand[0]->getNum() > sharedHand[1]->getNum())
                             ? (sharedHand[0]->getNum())
                             : (sharedHand[1]->getNum());
    }
    int player_handranking = cpu.getOpponentRanking();
    int cpu_handranking = cpu.getMyRanking();
    int player_highnum;
    int cpu_highnum;

    if (user.getMyCard()->getNum() == 1 || shared_highnum == 1) {
        player_highnum = 1;
    } else {
        player_highnum = (user.getMyCard()->getNum() > shared_highnum)
                             ? (user.getMyCard()->getNum())
                             : (shared_highnum);
    }
    if (cpu.getMyCard()->getNum() == 1 || shared_highnum == 1) {
        cpu_highnum = 1;
    } else {
        cpu_highnum = (cpu.getMyCard()->getNum() > shared_highnum)
                          ? (cpu.getMyCard()->getNum())
                          : (shared_highnum);
    }
    string player_cardRes;
    string cpu_cardRes;
    string player_chipRes;
    string cpu_chipRes;
    string player_card;
    string cpu_card;
    switch (user.getMyCard()->getNum()) {
    case 1:
        /* code */
        player_card = "Player's Card : A " + user.getMyCard()->getPattern();
        break;
    case 11:
        /* code */
        player_card = "Player's Card : J " + user.getMyCard()->getPattern();
        break;
    case 12:
        /* code */
        player_card = "Player's Card : Q " + user.getMyCard()->getPattern();
        break;
    case 13:
        /* code */
        player_card = "Player's Card : K " + user.getMyCard()->getPattern();
        break;

    default:
        player_card =
            "Player's Card : " + to_string(user.getMyCard()->getNum()) + " " +
            user.getMyCard()->getPattern();
        break;
    }
    switch (cpu.getMyCard()->getNum()) {
    case 1:
        /* code */
        cpu_card = "CPU's Card : A " + cpu.getMyCard()->getPattern();
        break;
    case 11:
        /* code */
        cpu_card = "CPU's Card : J " + cpu.getMyCard()->getPattern();
        break;
    case 12:
        /* code */
        cpu_card = "CPU's Card : Q " + cpu.getMyCard()->getPattern();
        break;
    case 13:
        /* code */
        cpu_card = "CPU's Card : K " + cpu.getMyCard()->getPattern();
        break;

    default:
        cpu_card = "CPU's Card : " + to_string(cpu.getMyCard()->getNum()) +
                   " " + cpu.getMyCard()->getPattern();
        break;
    }
    player_chipRes = "chips you own: " + to_string(user.getRemainder()) +
                     ", chips you bet: " + to_string(user.getBet());
    cpu_chipRes = "chips cpu own: " + to_string(cpu.getRemainder()) +
                  ", chips cpu bet: " + to_string(cpu.getBet());
    switch (player_handranking) {
    case HIGHCARD:
        switch (player_highnum) {
        case 1:
            player_cardRes = "Your Hand ranking: Ace HIGH";
            break;
        case 11:
            player_cardRes = "Your Hand ranking: JACK HIGH";
            break;
        case 12:
            player_cardRes = "Your Hand ranking: QUEEN HIGH";
            break;
        case 13:
            player_cardRes = "Your Hand ranking: KING HIGH";
            break;
        default:
            player_cardRes =
                "Your Hand ranking: " + to_string(player_highnum) + " HIGH";
            break;
        }
        break;
    case ONEPAIR:
        switch (cpu.getOP_Same_Card_Num()) {
        case 1:
            player_cardRes = "Your Hand ranking: Ace ONE PAIR";
            break;
        case 11:
            player_cardRes = "Your Hand ranking: JACK ONE PAIR";
            break;
        case 12:
            player_cardRes = "Your Hand ranking: QUEEN ONE PAIR";
            break;
        case 13:
            player_cardRes = "Your Hand ranking: KING ONE PAIR";
            break;
        default:
            player_cardRes =
                "Your Hand ranking: " + to_string(cpu.getOP_Same_Card_Num()) +
                " ONE PAIR";
            break;
        }
        break;

    case TRIPPLE:
        switch (player_highnum) {
        case 1:
            player_cardRes = "Your Hand ranking: A TRIPPLE";
            break;
        case 11:
            player_cardRes = "Your Hand ranking: J TRIPPLE";
            break;
        case 12:
            player_cardRes = "Your Hand ranking: Q TRIPPLE";
            break;
        case 13:
            player_cardRes = "Your Hand ranking: K TRIPPLE";
            break;

        default:
            player_cardRes =
                "Your Hand ranking: " + to_string(player_highnum) + " TRIPPLE";
            break;
        }
        break;
    case STRAIGHT:
        player_cardRes = "Your Hand ranking: STARIGHT";
        break;
    case FLUSH:
        player_cardRes = "Your Hand ranking: FLUSH";
        break;
    case STRAIGHTFLUSH:
        player_cardRes = "Your Hand ranking: STRAIGHT FLUSH";
        break;
    case RTF:
        player_cardRes = "Your Hand ranking: ROYAL STRAIGHT FLUSH";
        break;
    }

    switch (cpu_handranking) {
    case HIGHCARD:
        switch (cpu_highnum) {
        case 1:
            cpu_cardRes = "CPU's Hand ranking: Ace HIGH";
            break;
        case 11:
            cpu_cardRes = "CPU's Hand ranking: JACK HIGH";
            break;
        case 12:
            cpu_cardRes = "CPU's Hand ranking: QUEEN HIGH";
            break;
        case 13:
            cpu_cardRes = "CPU's Hand ranking: KING HIGH";
            break;
        default:
            cpu_cardRes =
                "CPU's Hand ranking: " + to_string(cpu_highnum) + " HIGH";
            break;
        }
        break;
    case ONEPAIR:
        switch (cpu.getMy_Same_Card_Num()) {
        case 1:
            cpu_cardRes = "CPU's Hand ranking: Ace ONE PAIR";
            break;
        case 11:
            cpu_cardRes = "CPU's Hand ranking: JACK ONE PAIR";
            break;
        case 12:
            cpu_cardRes = "CPU's Hand ranking: QUEEN ONE PAIR";
            break;
        case 13:
            cpu_cardRes = "CPU's Hand ranking: KING ONE PAIR";
            break;
        default:
            cpu_cardRes =
                "CPU's Hand ranking: " + to_string(cpu.getMy_Same_Card_Num()) +
                " ONE PAIR";
            break;
        }
        break;

    case TRIPPLE:
        switch (cpu_highnum) {
        case 1:
            cpu_cardRes = "CPU's Hand ranking: A TRIPPLE";
            break;
        case 11:
            cpu_cardRes = "CPU's Hand ranking: J TRIPPLE";
            break;
        case 12:
            cpu_cardRes = "CPU's Hand ranking: Q TRIPPLE";
            break;
        case 13:
            cpu_cardRes = "CPU's Hand ranking: K TRIPPLE";
            break;

        default:
            cpu_cardRes =
                "CPU's Hand ranking: " + to_string(cpu_highnum) + " TRIPPLE";
            break;
        }
        break;
    case STRAIGHT:
        cpu_cardRes = "CPU's Hand ranking: STARIGHT";
        break;
    case FLUSH:
        cpu_cardRes = "CPU's Hand ranking: FLUSH";
        break;
    case STRAIGHTFLUSH:
        cpu_cardRes = "CPU's Hand ranking: STRAIGHT FLUSH";
        break;
    case RTF:
        cpu_cardRes = "CPU's Hand ranking: ROYAL STRAIGHT FLUSH";
        break;
    }
    string cpu_highcard;
    switch (cpu_highnum) {
    case 1:
        cpu_highcard = "CPU highest: A";
        break;
    case 11:
        cpu_highcard = "CPU highest: J";
        break;
    case 12:
        cpu_highcard = "CPU highest: Q";
        break;
    case 13:
        cpu_highcard = "CPU highest: K";
        break;
    default:
        cpu_highcard = "CPU highest: " + to_string(cpu_highnum);
        break;
    }
    string user_highcard;
    switch (player_highnum) {
    case 1:
        user_highcard = "Player highest: A";
        break;
    case 11:
        user_highcard = "Player highest: J";
        break;
    case 12:
        user_highcard = "Player highest: Q";
        break;
    case 13:
        user_highcard = "Player highest: K";
        break;
    default:
        user_highcard = "Player highest: " + to_string(player_highnum);
        break;
    }
    mvwprintw(gameres, 0, 0, player_cardRes.c_str());
    mvwprintw(gameres, 1, 0, player_chipRes.c_str());
    mvwprintw(gameres, 2, 0, player_card.c_str());
    mvwprintw(gameres, 3, 0, user_highcard.c_str());

    mvwprintw(gameres, 5, 0, cpu_cardRes.c_str());
    mvwprintw(gameres, 6, 0, cpu_chipRes.c_str());
    mvwprintw(gameres, 7, 0, cpu_card.c_str());
    mvwprintw(gameres, 8, 0, cpu_highcard.c_str());
    if (user.getFolded()) {
        if (player_handranking >= TRIPPLE) {
            mvwprintw(gameres, 15, 30,
                      "you folded when you have ranking equal to or higer than "
                      "TRIPPLE!");
            mvwprintw(gameres, 16, 30,
                      "you get penalty!! give CPU to 10 chips in addtion!");
            if (user.getRemainder() < 10) {
                user.betChips(user.getRemainder());
            } else {
                user.betChips(10);
            }
        }
        mvwprintw(gameres, 20, 50, "CPU Get chips!");
        cpu.getChips(user.getBet() + cpu.getBet());
    } else if (cpu.getFolded()) {
        if (cpu_handranking >= TRIPPLE) {
            mvwprintw(gameres, 15, 30,
                      "CPU folded when CPU has ranking equal to or higer than "
                      "TRIPPLE!");
            mvwprintw(gameres, 16, 30,
                      "CPU get penalty!! give you to 10 chips in addtion!");
            if (cpu.getRemainder() < 10) {
                cpu.betChips(cpu.getRemainder());
            } else {
                cpu.betChips(10);
            }
        }
        mvwprintw(gameres, 20, 50, "Player Get chips!");
        user.getChips(user.getBet() + cpu.getBet());
    } else if (player_handranking > cpu_handranking) {
        mvwprintw(gameres, 20, 50, "Player Get chips!");
        user.getChips(user.getBet() + cpu.getBet());
    } else if (player_handranking == cpu_handranking) {
        if (player_handranking == ONEPAIR) {
            if (cpu.getOP_Same_Card_Num() == cpu.getMy_Same_Card_Num()) {
                mvwprintw(gameres, 20, 50, "Draw!");
                cpu.getChips(cpu.getBet());
                user.getChips(user.getBet());
            } else if (cpu.getOP_Same_Card_Num() == 1 ||
                       cpu.getOP_Same_Card_Num() > cpu.getMy_Same_Card_Num()) {
                mvwprintw(gameres, 20, 50, "Player Get chips!");
                user.getChips(user.getBet() + cpu.getBet());
            } else if (cpu.getMy_Same_Card_Num() == 1 ||
                       cpu.getMy_Same_Card_Num() > cpu.getOP_Same_Card_Num()) {
                mvwprintw(gameres, 20, 50, "CPU Get chips!");
                cpu.getChips(user.getBet() + cpu.getBet());
            }
        } else {
            if (player_highnum == cpu_highnum) {
                mvwprintw(gameres, 20, 50, "Draw!");
                cpu.getChips(cpu.getBet());
                user.getChips(user.getBet());
            } else if (player_highnum == 1 || player_highnum > cpu_highnum) {
                mvwprintw(gameres, 20, 50, "Player Get chips!");
                user.getChips(user.getBet() + cpu.getBet());
            } else if (cpu_highnum == 1 || cpu_highnum > player_highnum) {
                mvwprintw(gameres, 20, 50, "CPU Get chips!");
                cpu.getChips(user.getBet() + cpu.getBet());
            }
        }
    } else {
        mvwprintw(gameres, 20, 50, "CPU Get chips!");
        cpu.getChips(user.getBet() + cpu.getBet());
    }
    if (cpu.getRemainder() <= 0 || user.getRemainder() <= 0) {
        if (cpu.getRemainder() <= 0) {
            mvwprintw(gameres, 30, 50, "Player WIN!!");
            gameInfo->userTotalGame++;
            gameInfo->userWin++;
        } else if (user.getRemainder() <= 0) {
            mvwprintw(gameres, 30, 50, "CPU WIN!!");
            gameInfo->userTotalGame++;
            gameInfo->userLose++;
        }
        wrefresh(gameres);
        sleep(10);
        delwin(gameres);
        endwin();
        keypad(stdscr, false);
        mainmenu();
    } else {
        wrefresh(gameres);
        sleep(10);
        user.initBet();
        cpu.initBet();

        user.betChips(1);
        cpu.betChips(1);

        user.CanPlay();
        cpu.CanPlay();

        user.notFolded();
        cpu.notFolded();

        cpu.notChecked();
        user.notChecked();

        cpu.notAll_ined();
        user.notAll_ined();

        cpu.unlockTurn();

        Card *user_card = deck.drawCard();
        Card *cpu_card = deck.drawCard();
        sharedHand[0] = deck.drawCard();
        sharedHand[1] = deck.drawCard();
        user.setCard(cpu_card, user_card);
        cpu.setCard(user_card, cpu_card);
        delwin(gameres);
        endwin();

        gamedisplay();
    }
}
void signalHandler(int signum) {
    if (signum == SIGWINCH) {
        if (state.compare("main") == 0) {
            delwin(menu);
            endwin();
            mainmenu();
        } else if (state.compare("rule") == 0) {
            delwin(menu);
            endwin();
            rulemenu();
        } else if (state.compare("winrate") == 0) {
            delwin(menu);
            endwin();
            winratemenu();
        } else if (state.compare("gameplay_choose") == 0 ||
                   state.compare("gameplay_bet") == 0) {
            delwin(table);
            delwin(play);
            endwin();
            gamedisplay();
        } else if (state.compare("round_end") == 0) {
            delwin(gameres);
            endwin();
            roundEnd();
        }
    }
    if (signum == SIGINT || signum == SIGTERM || signum == SIGSEGV) {
        if (state.compare("main") == 0 || state.compare("rule") == 0 ||
            state.compare("winrate") == 0) {
            delwin(menu);
            endwin();
        } else if (state.compare("gameplay_choose") == 0 ||
                   state.compare("gameplay_bet") == 0) {
            delwin(play);
            delwin(table);
            endwin();
        } else if (state.compare("round_end") == 0) {
            delwin(gameres);
            endwin();
        }
        exit(0);
    }
}
void endFunc() {
    write_file();
    close(*userInfo);
    free(userInfo);
    free(gameInfo);
}
int read_file() {
    lseek(*userInfo, 0, SEEK_SET);
    if (read(*userInfo, (INFO *)gameInfo, sizeof(INFO)) < 0) {
        return -1;
    }
    return 1;
}
int write_file() {
    lseek(*userInfo, 0, SEEK_SET);
    if (gameInfo->userTotalGame == 0) {
        gameInfo->userWinRate = 0;
    } else {
        gameInfo->userWinRate = gameInfo->userWin / gameInfo->userTotalGame;
    }
    if (write(*userInfo, (INFO *)gameInfo, sizeof(INFO)) < 0) {
        return -1;
    }
    return 1;
}
