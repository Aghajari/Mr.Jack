//
// Created by AmirHossein Aghajari on 12/31/21.
//

#ifndef STARTER_H
#define STARTER_H

#define PROJECT_NAME "Mr. Jack"
#define CREATOR "AmirHossein Aghajari"
#define MR_JACK_DATA "mr_jack_game.data"
#define MR_JACK_Z_DATA "mr_jack_game_z.data"

int start();
int start_jack(int player1, int player2, int x, int y);
int start_game();
int start_map(int player1, int player2, int x, int y, int jack, _Bool loadGame);

void gameOver(int type);
#endif //STARTER_H
