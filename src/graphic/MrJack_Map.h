//
// Created by AmirHossein Aghajari on 1/4/22.
//

#ifndef MRJACK_MAP_H
#define MRJACK_MAP_H
#include "../characters/characters.h"

typedef struct {
    SDL_Point points[6];
    SDL_Color color;
    bool enabled, isLight, isTunnel, isHome;
    int connectedTo[6];
} Whitechapel;

typedef struct {
    SDL_Rect currentRect;
    char name[12];
    int rotation;
    int selected;
    bool visible;

    int move[5];
    int move_len;
    long long int time;
} MapElement;

typedef struct {
    GUI_Label *hint, *title;
    int *ability;
} MapUISettings;

void initializeMap(int x);
void render_map(SDL_Renderer *r);
void reset_selected_items();
void select_whitechapel(int character, int move, bool fromHome, bool detective);

void select_character_map(MRJACK_Character *character, bool detective);
MRJACK_Character *get_selected_character();
void set_can_switch(bool enabled);
bool get_can_switch();

void set_ability_enabled(bool enabled);
bool get_ability_enabled();

int getEnabledWhitechapelInArea(int x1, int y1, int x2, int y2);
void find_best_move(int move, bool fromHome, int start, int end, int out[]);
MapElement *getCharacterElement(int character);
void moveCharacter(int x1, int y1, int x2, int y2, int move, bool fromHome);

MapUISettings * getMapUISettings();
#endif //MRJACK_MAP_H

