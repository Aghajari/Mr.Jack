//
// Created by AmirHossein Aghajari on 1/7/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"
#include "../gameplay/MrJack_Gameplay.h"

int SergentGoodley_bfs(int start, int out[]);

void SergentGoodley_select_characters(){
    MapElement *elements = get_elements(0);
    for (int i = 0; i < 8; ++i) {
        if (i == 6)
            continue;

        int index = elements[i].selected;
        if (elements[i].move[0] != -1) {
            for (int j = 0; j < 5; ++j) {
                if (elements[i].move[j] != -1)
                    index = elements[i].move[j];
            }
        }
        Whitechapel *whitechapel = get_whitechapel(index);
        whitechapel->color.r = 242;
        whitechapel->color.g = 255;
        whitechapel->color.b = 0;
        whitechapel->color.a = 255;
        whitechapel->enabled = true;
    }
}

void SergentGoodley_ability_click(const void *element) {
    /*if (*getMapUISettings()->ability == 1
        && getMapUISettings()->target != -1 && getMapUISettings()->target != 3) {
        getMapUISettings()->target = 0;

        if (has_moved()) {
            doneCharacter(7);
        } else {
            *getMapUISettings()->ability = 2;
            reset_selected_items();
            select_whitechapel(7, 3, false, getMapUISettings()->detective);
        }
    }*/
    if (!get_can_switch())
        return;

    *getMapUISettings()->ability = !*getMapUISettings()->ability;
    reset_selected_items();
    if (*getMapUISettings()->ability) {
        getMapUISettings()->target = 3;
        SergentGoodley_select_characters();
    } else {
        getMapUISettings()->target = -1;
        select_whitechapel(7, 3, false, getMapUISettings()->detective);
    }
}

void SergentGoodley_initialize(bool detective) {
    getMapUISettings()->target = getMapUISettings()->target2 = -1;
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(7, 3, false, detective);

    *getMapUISettings()->ability = 0;
    getMapUISettings()->abilityBtn[0].obj.clickListener = SergentGoodley_ability_click;
    getMapUISettings()->abilityBtn[1].obj.clickListener = SergentGoodley_ability_click;
    getMapUISettings()->abilityBtn[2].obj.clickListener = NULL;
}

void SergentGoodley_click(int x1, int y1, int x2, int y2) {
    if (getMapUISettings()->target <= 0) {
        moveCharacter(x1, y1, x2, y2, 3, false);
    } else {
        if (getMapUISettings()->target2 == -1) {
            MapElement *elements = get_elements(0);
            int i;
            for (i = 0; i < 8; ++i) {
                if (i == 6)
                    continue;

                if (isClickedOnWhitechapel(x1, y1, x2, y2, elements[i].selected))
                    break;
            }
            if (i < 8) {
                int out[get_whitechapel_len()];
                int len = SergentGoodley_bfs(elements[i].selected, out);
                if (len <= 0) {
                    return;
                }
                set_can_switch(false);
                getMapUISettings()->target2 = i;
                reset_selected_items();

                for (int j = len - 1; j > len - 1 - getMapUISettings()->target && j >= 0; j--) {
                    bool isOk = true;
                    for (int k = 0; k < 8; ++k) {
                        if (elements[k].selected == out[j]) {
                            isOk = false;
                            break;
                        }
                    }
                    if (!isOk)
                        continue;

                    Whitechapel *whitechapel = get_whitechapel(out[j]);
                    whitechapel->color.r = 242;
                    whitechapel->color.g = 255;
                    whitechapel->color.b = 0;
                    whitechapel->color.a = 255;
                    whitechapel->enabled = true;
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
            MapElement *element = &get_elements(0)[getMapUISettings()->target2];
            if (index != -1 && index != element->selected) {
                int out[get_whitechapel_len()];
                int len = SergentGoodley_bfs(element->selected, out);
                int size = 0;
                for (int j = len - 1; j >= 0; j--) {
                    element->move[size] = out[j];
                    size++;
                    if (out[j] == index)
                        break;
                }
                element->move[size] = -1;
                element->time = currentTime();
                element->move_len = size;
                getMapUISettings()->target -= size;
                getMapUISettings()->target2 = -1;

                if (getMapUISettings()->target == 0) {
                    if (has_moved()) {
                        doneCharacter(7);
                    } else {
                        *getMapUISettings()->ability = 2;
                        reset_selected_items();
                        select_whitechapel(7, 3, false, getMapUISettings()->detective);
                    }
                } else {
                    reset_selected_items();
                    SergentGoodley_select_characters();
                }
            } else if (index == element->selected) {
                getMapUISettings()->target2 = -1;
                reset_selected_items();
                SergentGoodley_select_characters();
            }
        }
    }
}

void SergentGoodley_moved(int target) {
    if (target == -1 && *getMapUISettings()->ability == 2) {
        doneCharacter(7);
    } else if (target != -1) {
        for (int i = 0; i < 3; ++i)
            getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;

        if (*getMapUISettings()->ability != 2) {
            SergentGoodley_ability_click(NULL);
            *getMapUISettings()->ability = 1;
        }
        set_can_switch(false);
    }
}

int SergentGoodley_bfs(int start, int out[]) {
    return bfs(start, getFinalCharacterSelectedIndex(7), out, false, false);
}