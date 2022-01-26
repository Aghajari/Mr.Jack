//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

char *SherlockHolmes_ability_text = "Click on alibi cards (the red card) to draw a card...";

void SherlockHolmes_initialize(bool detective){
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(5, 3, false, detective);

    *getMapUISettings()->ability = 2;
    getMapUISettings()->target = -1;
    for (int i = 0; i < 3; ++i)
        getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;
}

void SherlockHolmes_click(int x1, int y1, int x2, int y2){
    moveCharacter(x1, y1, x2, y2, 3, false);
}

void SherlockHolmes_card_click(const void *element){
    if (getMapUISettings()->target != -1)
        return;

    MRJACK_Character *character = pick_character();
    getMapUISettings()->target = 1;
    getMapUISettings()->alibiCard->obj.clickListener = NULL;
    getMapUISettings()->alibiCard->time = currentTime();
    getMapUISettings()->alibiCard->file = character->innocent;
    getMapUISettings()->alibiCard->obj.refresh = true;

    if (getMapUISettings()->detective)
        safe_character(character->id);
    doneCharacter(5);
}

void SherlockHolmes_moved(int target){
    if (target == -1)
        return;

    set_can_switch(false);
    reset_selected_items();
    getMapUISettings()->target = -1;
    *getMapUISettings()->ability = 1;

    if (*getMapUISettings()->playerTurn != 0) {
        getMapUISettings()->hint->text = SherlockHolmes_ability_text;
        getMapUISettings()->hint->obj.refresh = true;
    }
    getMapUISettings()->alibiCard->obj.clickListener = SherlockHolmes_card_click;
}