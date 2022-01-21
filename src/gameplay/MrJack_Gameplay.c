//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "MrJack_Gameplay.h"

int gameplay_whosTurn(int player1, int player2, int round, int turn) {
    int side = (round % 2) ? 1 : -1;

    if (player1 == side) {
        if (player2 == side) {
            return turn == 0 ? 1 : 2;
        } else {
            return 1;
        }
    } else if (player2 == side) {
        return 2;
    } else {
        return 0;
    }
}