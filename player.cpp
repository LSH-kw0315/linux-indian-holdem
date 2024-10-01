#include "player.h"

void Player::checkRanking(Card *sharedHand[2]) {
    std::string s0_pattern = sharedHand[0]->getPattern();
    std::string s1_pattern = sharedHand[1]->getPattern();
    int s0_num = sharedHand[0]->getNum();
    int s1_num = sharedHand[1]->getNum();

    std::string op_pattern = opponentCard->getPattern();
    int op_num = opponentCard->getNum();

    std::string my_pattern = myCard->getPattern();
    int my_num = myCard->getNum();
    int op_numrel;
    int op_patternrel;
    int my_numrel;
    int my_patternrel;
    // opponent
    if ((s0_num + 2 == s1_num + 1 == op_num) ||
        (s0_num + 2 == s1_num == op_num + 1) ||
        (s0_num + 1 == s1_num == op_num + 2) ||
        (s0_num + 1 == s1_num + 2 == op_num) ||
        (s0_num == s1_num + 1 == op_num + 2) ||
        (s0_num == s1_num + 2 == op_num + 1)) {
        op_numrel = STRAIGHT;
        // normal straight

    } else if ((s0_num == 1 && s1_num == 12 && op_num == 13) ||
               (s0_num == 1 && s1_num == 13 && op_num == 12) ||
               (s1_num == 1 && s0_num == 13 && op_num == 12) ||
               (s1_num == 1 && s0_num == 12 && op_num == 13) ||
               (op_num == 1 && s0_num == 13 && s1_num == 12) ||
               (op_num == 1 && s0_num == 12 && s1_num == 13)) {
        op_numrel = ROYALSTRAIGHT;
        // royal straight
    } else if (s0_num == s1_num && s1_num == op_num) {
        // tripple
        op_numrel = TRIPPLE;
    } else if (s0_num == s1_num || s0_num == op_num || s1_num == op_num) {
        op_numrel = ONEPAIR;
        if (s0_num == s1_num || s0_num == op_num) {
            op_same_cardnum = s0_num;
        } else if (s1_num == op_num) {
            op_same_cardnum = s1_num;
        }
    } else {
        op_numrel = HIGHCARD;
    }
    if (s0_pattern.compare(s1_pattern) == 0 &&
        s0_pattern.compare(op_pattern) == 0) {
        op_patternrel = FLUSH;
    } else {
        op_patternrel = NONE;
    }
    // my
    if ((s0_num + 2 == s1_num + 1 == my_num) ||
        (s0_num + 2 == s1_num == my_num + 1) ||
        (s0_num + 1 == s1_num == my_num + 2) ||
        (s0_num + 1 == s1_num + 2 == my_num) ||
        (s0_num == s1_num + 1 == my_num + 2) ||
        (s0_num == s1_num + 2 == my_num + 1)) {
        my_numrel = STRAIGHT;
        // normal straight

    } else if ((s0_num == 1 && s1_num == 12 && my_num == 13) ||
               (s0_num == 1 && s1_num == 13 && my_num == 12) ||
               (s1_num == 1 && s0_num == 13 && my_num == 12) ||
               (s1_num == 1 && s0_num == 12 && my_num == 13) ||
               (my_num == 1 && s0_num == 13 && s1_num == 12) ||
               (my_num == 1 && s0_num == 12 && s1_num == 13)) {
        my_numrel = ROYALSTRAIGHT;
        // royal straight
    } else if (s0_num == s1_num && s1_num == my_num) {
        // tripple
        my_numrel = TRIPPLE;
    } else if (s0_num == s1_num || s0_num == my_num || s1_num == my_num) {
        my_numrel = ONEPAIR;
        if (s0_num == s1_num || s0_num == my_num) {
            my_same_cardnum = s0_num;
        } else if (s1_num == my_num) {
            my_same_cardnum = s1_num;
        }
    } else {
        my_numrel = HIGHCARD;
    }
    if (s0_pattern.compare(s1_pattern) == 0 &&
        s1_pattern.compare(my_pattern) == 0) {
        my_patternrel = FLUSH;
    } else {
        my_patternrel = NONE;
    }

    if (op_patternrel == FLUSH && op_numrel == ROYALSTRAIGHT) {
        opponent_ranking = RTF;
    } else if (op_patternrel == FLUSH && op_numrel == STRAIGHT) {
        opponent_ranking = STRAIGHTFLUSH;
    } else if (op_patternrel == FLUSH) {
        opponent_ranking = FLUSH;
    } else {
        opponent_ranking = op_numrel;
    }

    if (my_patternrel == FLUSH && my_numrel == ROYALSTRAIGHT) {
        my_ranking = RTF;
    } else if (my_patternrel == FLUSH && my_numrel == STRAIGHT) {
        my_ranking = STRAIGHTFLUSH;
    } else if (my_patternrel == FLUSH) {
        my_ranking = FLUSH;
    } else {
        my_ranking = my_numrel;
    }
}

std::string Player::judging(bool first_bet, Player user, Card *sharedHand[2]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);
    int posb = dis(gen);
    bool canBet = user.getBet() - bet > 0 && user.getBet() - bet < remainder;
    bool canCall = user.getBet() - bet > 0 && user.getBet() - bet <= remainder;
    bool canCheck = user.getBet() - bet == 0;
    bool canCheckInAllin = user.getBet() - bet <= 0;
    int shared_high_num;
    int my_high_num;
    int op_high_num;

    if (sharedHand[0]->getNum() == 1 || sharedHand[1]->getNum() == 1) {
        shared_high_num = 1;
    } else {
        shared_high_num = (sharedHand[0]->getNum() > sharedHand[1]->getNum())
                              ? (sharedHand[0]->getNum())
                              : (sharedHand[1]->getNum());
    }

    if (myCard->getNum() == 1 || shared_high_num == 1) {
        my_high_num = 1;
    } else {
        my_high_num = (myCard->getNum() > shared_high_num) ? (myCard->getNum())
                                                           : (shared_high_num);
    }

    if (opponentCard->getNum() == 1 || shared_high_num == 1) {
        op_high_num = 1;
    } else {
        op_high_num = (opponentCard->getNum() > shared_high_num)
                          ? (opponentCard->getNum())
                          : (shared_high_num);
    }

    bool userAllined = user.getAll_ined();
    if (userAllined) {
        canPlay = false;
        if (opponent_ranking > my_ranking) {
            // lose
            if (posb > 10) {
                cpuJudge = "fold";
                return cpuJudge;
            } else if (posb <= 10 && canCheckInAllin) {
                cpuJudge = "check";
                return cpuJudge;
            } else if (posb <= 10 && !canCheckInAllin && canCall) {
                cpuJudge = "call";
                return cpuJudge;
            } else {
                cpuJudge = "all-in";
                return cpuJudge;
            }
        } else if (opponent_ranking == my_ranking) {
            // equal
            if (my_ranking == ONEPAIR) {
                if (op_same_cardnum == my_same_cardnum) {
                    if (posb > 10 && canCheckInAllin) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else if (posb > 10 && !canCheckInAllin && canCall) {
                        cpuJudge = "call";
                        return cpuJudge;

                    } else if (posb > 10 && !canCheckInAllin && !canCall) {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    } else {
                        cpuJudge = "fold";
                        return cpuJudge;
                    }
                } else if (op_same_cardnum == 1 ||
                           op_same_cardnum > my_same_cardnum) {
                    if (posb > 10) {
                        cpuJudge = "fold";
                        return cpuJudge;
                    } else if (posb <= 10 && canCheckInAllin) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else if (posb <= 10 && !canCheckInAllin && canCall) {
                        cpuJudge = "call";
                        return cpuJudge;
                    } else if (posb <= 10 && !canCheckInAllin && !canCall) {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }

                } else if (my_same_cardnum == 1 ||
                           my_same_cardnum > op_same_cardnum) {
                    if (posb > 10 && canCheckInAllin) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else if (posb > 10 && !canCheckInAllin && canCall) {
                        cpuJudge = "call";
                        return cpuJudge;
                    } else if (posb > 10 && !canCheckInAllin && !canCall) {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    } else {
                        cpuJudge = "fold";
                        return cpuJudge;
                    }
                }
                //---all-in one pair end
            } else if (my_high_num == op_high_num) {
                if (posb > 10 && canCheckInAllin) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else if (posb > 10 && !canCheckInAllin && canCall) {
                    cpuJudge = "call";
                    return cpuJudge;

                } else if (posb > 10 && !canCheckInAllin && !canCall) {
                    cpuJudge = "all-in";
                    return cpuJudge;
                } else {
                    cpuJudge = "fold";
                    return cpuJudge;
                }
            } else if (my_high_num == 1 || my_high_num > op_high_num) {
                if (posb > 10 && canCheckInAllin) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else if (posb > 10 && !canCheckInAllin && canCall) {
                    cpuJudge = "call";
                    return cpuJudge;
                } else if (posb > 10 && !canCheckInAllin && !canCall) {
                    cpuJudge = "all-in";
                    return cpuJudge;
                } else {
                    cpuJudge = "fold";
                    return cpuJudge;
                }
            } else if (op_high_num == 1 || my_high_num < op_high_num) {
                if (posb > 10) {
                    cpuJudge = "fold";
                    return cpuJudge;
                } else if (posb <= 10 && canCheckInAllin) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else if (posb <= 10 && !canCheckInAllin && canCall) {
                    cpuJudge = "call";
                    return cpuJudge;
                } else if (posb <= 10 && !canCheckInAllin && !canCall) {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            }

        } else {
            // win
            if (posb > 10 && canCall) {
                cpuJudge = "call";
                return cpuJudge;
            } else if (posb > 10 && !canCall) {
                cpuJudge = "all-in";
                return cpuJudge;
            } else {
                cpuJudge = "fold";
                return cpuJudge;
            }
        }
    }

    if (first_bet) {
        // first_bet = true; in other words, user bet = cpu bet.
        if (opponent_ranking > my_ranking) {
            // opponents is higher than me
            if (posb > 30) {
                cpuJudge = "fold";
                return cpuJudge;
            } else if (posb <= 30 && posb > 10) {
                cpuJudge = "check";
                return cpuJudge;
            } else if (posb <= 10 && posb < 5) {
                cpuJudge = "bet";
                return cpuJudge;
            } else {
                cpuJudge = "all-in";
                return cpuJudge;
            }
        } else if (opponent_ranking == my_ranking) {
            // opponent is equal to me.
            if (my_ranking == ONEPAIR) {
                if (op_same_cardnum == my_same_cardnum) {
                    if (posb > 30) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else if (posb <= 30 && posb > 5) {
                        cpuJudge = "bet";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                } else if (op_same_cardnum == 1 ||
                           op_same_cardnum > my_same_cardnum) {
                    if (posb > 30) {
                        cpuJudge = "fold";
                        return cpuJudge;
                    } else if (posb <= 30 && posb > 10) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else if (posb <= 10 && posb < 5) {
                        cpuJudge = "bet";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }

                } else if (my_same_cardnum == 1 ||
                           my_same_cardnum > op_same_cardnum) {
                    if (posb > 20) {
                        cpuJudge = "bet";
                        return cpuJudge;
                    } else if (posb <= 20 && posb > 10) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                }
            } else if (my_high_num == op_high_num) {
                // draw game.
                if (posb > 30) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else if (posb <= 30 && posb > 5) {
                    cpuJudge = "bet";
                    return cpuJudge;
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            } else if (my_high_num == 1 || my_high_num > op_high_num) {
                // I have higher number
                if (posb > 20) {
                    cpuJudge = "bet";
                    return cpuJudge;
                } else if (posb <= 20 && posb > 10) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            } else if (op_high_num == 1 || my_high_num < op_high_num) {
                // I lose.
                if (posb > 30) {
                    cpuJudge = "fold";
                    return cpuJudge;
                } else if (posb <= 30 && posb > 10) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else if (posb <= 10 && posb < 5) {
                    cpuJudge = "bet";
                    return cpuJudge;
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            }

        } else {
            // I am higher than opponent
            if (posb > 20) {
                cpuJudge = "bet";
                return cpuJudge;
            } else if (posb <= 20 && posb > 10) {
                cpuJudge = "check";
                return cpuJudge;
            } else if (posb <= 10) {
                cpuJudge = "all-in";
                return cpuJudge;
            }
        }
    } else {
        // first_bet = false
        if (opponent_ranking > my_ranking) {
            if (posb > 30) {
                cpuJudge = "fold";
                return cpuJudge;
            } else if (posb <= 30 && posb > 10) {
                if (canCheck) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else {
                    if (canCall) {
                        cpuJudge = "call";
                        return cpuJudge;
                    } else {
                        cpuJudge = "fold";
                        return cpuJudge;
                    }
                }
            } else if (posb <= 10 && posb < 5) {
                if (canBet) {
                    cpuJudge = "raise";
                    return cpuJudge;
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            } else {
                cpuJudge = "all-in";
                return cpuJudge;
            }

        } else if (opponent_ranking == my_ranking) {
            if (my_ranking == ONEPAIR) {
                if (op_same_cardnum == my_same_cardnum) {
                    if (posb > 30) {
                        if (canCheck) {
                            cpuJudge = "check";
                            return cpuJudge;
                        } else {
                            if (canCall) {
                                cpuJudge = "call";
                                return cpuJudge;
                            } else {
                                cpuJudge = "all-in";
                                return cpuJudge;
                            }
                        }
                    } else if (posb <= 30 && posb > 5) {
                        if (canBet) {
                            cpuJudge = "raise";
                            return cpuJudge;
                        } else {
                            cpuJudge = "all-in";
                            return cpuJudge;
                        }
                    } else if (posb <= 5 && (op_high_num == 1 ||
                                             op_high_num > my_high_num)) {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }

                } else if (op_same_cardnum == 1 ||
                           op_same_cardnum > my_same_cardnum) {
                    if (posb > 30) {
                        cpuJudge = "fold";
                        return cpuJudge;
                    } else if (posb <= 30 && posb > 10) {
                        if (canCheck) {
                            cpuJudge = "check";
                            return cpuJudge;
                        } else {
                            if (canCall) {
                                cpuJudge = "call";
                                return cpuJudge;
                            } else {
                                cpuJudge = "fold";
                                return cpuJudge;
                            }
                        }
                    } else if (posb <= 10 && posb < 5) {
                        if (canBet) {
                            cpuJudge = "raise";
                            return cpuJudge;
                        } else {
                            cpuJudge = "all-in";
                            return cpuJudge;
                        }
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }

                } else if (my_same_cardnum == 1 ||
                           my_same_cardnum > op_same_cardnum) {
                    if (posb > 20) {
                        if (canBet) {
                            cpuJudge = "raise";
                            return cpuJudge;
                        } else {
                            cpuJudge = "all-in";
                            return cpuJudge;
                        }
                    } else if (posb <= 20 && posb > 10) {
                        if (canCheck) {
                            cpuJudge = "check";
                            return cpuJudge;
                        } else {
                            if (canCall) {
                                cpuJudge = "call";
                                return cpuJudge;
                            } else {
                                cpuJudge = "all-in";
                                return cpuJudge;
                            }
                        }
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                }
            } else if (my_high_num == op_high_num) {
                // draw game.
                if (posb > 30) {
                    if (canCheck) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else {
                        if (canCall) {
                            cpuJudge = "call";
                            return cpuJudge;
                        } else {
                            cpuJudge = "all-in";
                            return cpuJudge;
                        }
                    }
                } else if (posb <= 30) {
                    if (canBet) {
                        cpuJudge = "raise";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                }

            } else if (my_high_num == 1 || my_high_num > op_high_num) {
                // I have higher number
                if (posb > 20) {
                    if (canBet) {
                        cpuJudge = "raise";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                } else if (posb <= 20 && posb > 10) {
                    if (canCheck) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else {
                        if (canCall) {
                            cpuJudge = "call";
                            return cpuJudge;
                        } else {
                            cpuJudge = "all-in";
                            return cpuJudge;
                        }
                    }
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            } else if (op_high_num == 1 || op_high_num > my_high_num) {
                if (posb > 30) {
                    cpuJudge = "fold";
                    return cpuJudge;
                } else if (posb <= 30 && posb > 10) {
                    if (canCheck) {
                        cpuJudge = "check";
                        return cpuJudge;
                    } else {
                        if (canCall) {
                            cpuJudge = "call";
                            return cpuJudge;
                        } else {
                            cpuJudge = "fold";
                            return cpuJudge;
                        }
                    }
                } else if (posb <= 10 && posb < 5) {
                    if (canBet) {
                        cpuJudge = "raise";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                } else {
                    cpuJudge = "all-in";
                    return cpuJudge;
                }
            }

        } else {
            // I am higher than opponent
            if (posb > 20) {
                if (canBet) {
                    cpuJudge = "raise";
                    return cpuJudge;
                }
            } else if (posb <= 20 && posb > 10) {
                if (canCheck) {
                    cpuJudge = "check";
                    return cpuJudge;
                } else {
                    if (canCall) {
                        cpuJudge = "call";
                        return cpuJudge;
                    } else {
                        cpuJudge = "all-in";
                        return cpuJudge;
                    }
                }
            } else if (posb <= 10) {
                cpuJudge = "all-in";
                return cpuJudge;
            }
        }
    }
    cpuJudge = "none";
    return cpuJudge;
}

void Player::betChipsForComputer(Player user) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int betDiffer = user.getBet() - bet;
    int ran;
    std::uniform_int_distribution<int> dis1(betDiffer + 1, remainder / 5);
    std::uniform_int_distribution<int> dis2(betDiffer + 1, remainder / 4);
    std::uniform_int_distribution<int> dis3(betDiffer + 1, remainder - 1);
    if (cpuJudge.compare("bet") == 0) {
        if (betDiffer + 1 > remainder / 5) {
            ran = dis3(gen);
        } else if (remainder - 1 > betDiffer + 1) {
            ran = dis1(gen);
        } else {
            ran = remainder;
        }
        while (ran > remainder) {

            if (betDiffer + 1 > remainder / 5) {
                ran = dis3(gen);
            } else if (remainder - 1 > betDiffer + 1) {
                ran = dis1(gen);
            } else {
                ran = 1;
                break;
            }
        }
        bet += ran;
        remainder -= ran;
        return;
    } else if (cpuJudge.compare("check") == 0) {
        checked = true;
        return;
    } else if (cpuJudge.compare("call") == 0) {
        bet += betDiffer;
        remainder -= betDiffer;
        return;
    } else if (cpuJudge.compare("raise") == 0) {
        if (betDiffer + 1 > remainder / 5) {
            ran = dis3(gen);
        } else if (remainder - 1 > betDiffer + 1) {
            ran = dis2(gen);
        } else {
            ran = remainder;
        }
        while (ran > remainder) {

            if (betDiffer + 1 > remainder / 5) {
                ran = dis3(gen);
            } else if (remainder - 1 > betDiffer + 1) {
                ran = dis2(gen);
            } else {
                ran = remainder;
                break;
            }
        }
        bet += ran;
        remainder -= ran;
        return;
    } else if (cpuJudge.compare("fold") == 0) {
        folded = true;
        canPlay = false;
        return;
    } else if (cpuJudge.compare("all-in") == 0) {
        canPlay = false;
        all_ined = true;
        bet += remainder;
        remainder = 0;
        return;
    } else {
        return;
    }
};

void Player::betChips(int amount) {
    bet += amount;
    remainder -= amount;
}

void Player::getChips(int chips) {
    bet = 0;
    remainder += chips;
}
