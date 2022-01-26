//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

void JohnSmith_select_items(){
    MapElement *elements = get_elements(1);
    for (int i = 0; i < 6; ++i) {
        if (elements[i].visible) {
            Whitechapel *whitechapel = get_whitechapel(elements[i].selected);
            whitechapel->color.r = 255;
            whitechapel->color.g = 255;
            whitechapel->color.b = 0;
            whitechapel->color.a = 255;
            whitechapel->enabled = true;
        }
    }
}

void JohnSmith_ability_click(const void *element) {
    if (!get_can_switch())
        return;

    *getMapUISettings()->ability = !*getMapUISettings()->ability;
    reset_selected_items();
    if (*getMapUISettings()->ability) {
        JohnSmith_select_items();
    } else {
        select_whitechapel(6, 3, false, getMapUISettings()->detective);
    }
}

void JohnSmith_initialize(bool detective){
    getMapUISettings()->target = -1;
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(6, 3, false, detective);
    *getMapUISettings()->ability = 0;

    getMapUISettings()->abilityBtn[0].obj.clickListener = JohnSmith_ability_click;
    getMapUISettings()->abilityBtn[1].obj.clickListener = JohnSmith_ability_click;
    getMapUISettings()->abilityBtn[2].obj.clickListener = NULL;
}

void JohnSmith_click(int x1, int y1, int x2, int y2){
    if (*getMapUISettings()->ability == 1) {
        if (getMapUISettings()->target == -1) {
            MapElement *elements = get_elements(1);
            int i;
            for (i = 0; i < 6; ++i) {
                if (isClickedOnWhitechapel(x1, y1, x2, y2, elements[i].selected))
                    break;
            }
            if (i < 6) {
                set_can_switch(false);
                getMapUISettings()->target = i;
                reset_selected_items();

                for (int j = 0; j < get_whitechapel_len(); ++j) {
                    Whitechapel *whitechapel = get_whitechapel(j);
                    if (whitechapel->isLight) {
                        bool isOk = true;
                        for (int k = 0; k < 6; ++k) {
                            if (elements[k].visible && elements[k].selected == j) {
                                isOk = false;
                                break;
                            }
                        }
                        if (isOk) {
                            whitechapel->color.r = 242;
                            whitechapel->color.g = 255;
                            whitechapel->color.b = 0;
                            whitechapel->color.a = 255;
                            whitechapel->enabled = true;
                        }
                    }
                }

                Whitechapel *whitechapel = get_whitechapel(elements[i].selected);
                whitechapel->color.r = 0;
                whitechapel->color.g = 255;
                whitechapel->color.b = 0;
                whitechapel->color.a = 255;
                whitechapel->enabled = true;
            }
        } else {
            int index = getEnabledWhitechapelInArea(x1, y1, x2, y2);
            MapElement *element = &get_elements(1)[getMapUISettings()->target];
            if (index != -1 && index != element->selected) {
                element->move[0] = index;
                element->move[1] = -1;
                element->time = currentTime();
                *getMapUISettings()->ability = 2;

                if (has_moved()) {
                    doneCharacter(6);
                } else {
                    reset_selected_items();
                    select_whitechapel(6, 3, false, getMapUISettings()->detective);
                }
            } else if (index == element->selected) {
                getMapUISettings()->target = -1;
                reset_selected_items();
                JohnSmith_select_items();
            }
        }
    } else {
        moveCharacter(x1, y1, x2, y2, 3, false);
    }
}

void JohnSmith_moved(int target){
    if (target == -1 && *getMapUISettings()->ability == 2) {
        doneCharacter(6);
    } else if (target != -1) {
        for (int i = 0; i < 3; ++i)
            getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;

        if (*getMapUISettings()->ability != 2) {
            JohnSmith_ability_click(NULL);
            *getMapUISettings()->ability = 1;
        }
        set_can_switch(false);
    }
}