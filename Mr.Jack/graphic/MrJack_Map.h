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
    char name[20];
    int rotation;
    int selected;
    bool isSafe;
    bool visible;

    int move[5];
    int move_len;
    long long int time;

    void *texture, *surface;
    bool refresh;
} MapElement;

typedef struct {
    GUI_Label *hint, *title;
    GUI_Image *abilityBtn;
    GUI_FadeImage *alibiCard;
    int *ability, *seen, *playerTurn;
    int target, target2;
    int player1, player2, jackId;
    bool detective;
} MapUISettings;

void initializeMap(int x);
void render_map(SDL_Renderer *r);
void reset_selected_items();
void select_whitechapel(int character, int move, bool fromHome, bool detective);
void select_whitechapel_characters(int character);
bool isCharacterVisible(int character);
void disable_light(int index);
int find_block_selected(int index);

void select_character_map(MRJACK_Character *character, bool detective);
MRJACK_Character *get_selected_character();
void set_can_switch(bool enabled);
bool get_can_switch();
bool has_moved();

bool isClickedOnWhitechapel(int x1, int y1, int x2, int y2, int index);
int getEnabledWhitechapelInArea(int x1, int y1, int x2, int y2);
void find_best_move(int move, bool fromHome, int start, int end, int out[]);
MapElement *getCharacterElement(int character);
void moveCharacter(int x1, int y1, int x2, int y2, int move, bool fromHome);
bool moveCharacter2(int w, int move, bool fromHome);
void moveCharacter3(int w, int character, int move, bool fromHome);
void swapCharacters(int x1, int y1, int x2, int y2);
int fixMoveWithNoCharacter(int c);

MapUISettings * getMapUISettings();
int get_whitechapel_len();
MapElement *get_elements(int type);
Whitechapel *get_whitechapel(int index);
int getFinalCharacterSelectedIndex(int c);
int getFinalElementSelectedIndex(MapElement *e);

void fillRectFromMap(int i, SDL_Rect *rect, MapElement *element);
void fillRectFromMap2(int i, SDL_Rect *rect, MapElement *element);
int can_jack_leave_the_district();
void safe_character(int id);
bool isFilledWithCharacter(int w, int c);
bool isVisibleForWatson(int rotation, int selected, int target);

#endif //MRJACK_MAP_H

