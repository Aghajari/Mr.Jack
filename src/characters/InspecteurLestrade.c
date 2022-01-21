//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

void InspecteurLestrade_initialize(bool detective){
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(1, 3, false, detective);
    *getMapUISettings()->ability = 0;
}

void InspecteurLestrade_click(int x1, int y1, int x2, int y2) {
    moveCharacter(x1, y1, x2, y2, 3, false);
}