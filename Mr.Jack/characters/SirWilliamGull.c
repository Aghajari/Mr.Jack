//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

void SirWilliamGull_ability_click(const void *element) {
    *getMapUISettings()->ability = !*getMapUISettings()->ability;
    reset_selected_items();
    if (*getMapUISettings()->ability) {
        select_whitechapel_characters(8);
    } else {
        select_whitechapel(8, 3, false, getMapUISettings()->detective);
    }
}

void SirWilliamGull_initialize(bool detective) {
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(8, 3, false, detective);

    *getMapUISettings()->ability = 0;
    for (int i = 0; i < 3; ++i)
        getMapUISettings()->abilityBtn[i].obj.clickListener = SirWilliamGull_ability_click;
}

void SirWilliamGull_click(int x1, int y1, int x2, int y2) {
    if (*getMapUISettings()->ability == 0)
        moveCharacter(x1, y1, x2, y2, 3, false);
    else
        swapCharacters(x1, y1, x2, y2);
}

void SirWilliamGull_moved(int target) {
    if (target == -1) {
        doneCharacter(8);
    } else {
        for (int i = 0; i < 3; ++i)
            getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;
    }
}