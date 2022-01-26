//
// Created by AmirHossein Aghajari on 1/6/22.
//

#ifndef MRJACK_GAMEPLAY_H
#define MRJACK_GAMEPLAY_H

int gameplay_whosTurn(int player1, int player2, int round, int turn, bool *detective);
int bfs(int start, int target, int out[], bool tunnel, bool home);

int bot_play_detective_find_best_character(GUI_Image c[4]);
bool bot_play_detective_move(int round);
void bot_play_detective_ability(int round, bool done);

void bot_play_jack_ability(int round, bool done);
void bot_play_jack_move(int round);
int bot_play_jack_find_best_character(GUI_Image c[4]);
#endif //MRJACK_GAMEPLAY_H
