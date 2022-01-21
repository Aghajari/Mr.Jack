//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

void MissStealthy_initialize(bool detective){
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(2, 4, true, detective);

    *getMapUISettings()->ability = 1;
}

void MissStealthy_click(int x1, int y1, int x2, int y2){
    moveCharacter(x1, y1, x2, y2, 4, true);
}