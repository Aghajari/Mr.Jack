//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

GUI_FadeInImage JohnWatson_ability_parent = {
        .duration = 550,
        .file = "assets/ability_light_parent.png",
        .hoverFile = NULL,
        .selectedFile = NULL,
        .obj = {
                .gravity = GRAVITY_CENTER,
                .surface = NULL,
                .texture = NULL,
        }
}, JohnWatson_ability = {
        .duration = 550,
        .file = "assets/ability_light_selected.png",
        .hoverFile = NULL,
        .selectedFile = NULL,
        .obj = {
                .gravity = GRAVITY_CENTER,
                .surface = NULL,
                .texture = NULL,
        }
};

int JohnWatson_clickX, JohnWatson_clickY;

void JohnWatson_initialize(bool detective) {
    set_can_switch(true);
    JohnWatson_clickX = -1;

    reset_selected_items();
    select_whitechapel(4, 3, false, detective);
    *getMapUISettings()->ability = 2;

    for (int i = 0; i < 3; ++i)
        getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;

    JohnWatson_ability_parent.obj.refresh = true;
    JohnWatson_ability.obj.refresh = true;
}

void JohnWatson_click(int x1, int y1, int x2, int y2) {
    moveCharacter(x1, y1, x2, y2, 3, false);
}

void JohnWatson_render(SDL_Renderer *r) {
    if (has_moved() && *getMapUISettings()->playerTurn != 0) {
        render_fadein_image(&JohnWatson_ability_parent, r);
        render_fadein_image_rotation(&JohnWatson_ability, r, getCharacterElement(4)->rotation);
    }
}

void JohnWatson_event(SDL_Event *ev) {
    if (has_moved()) {
        if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
            JohnWatson_clickX = ev->button.x;
            JohnWatson_clickY = ev->button.y;

        } else if (ev->type == SDL_MOUSEBUTTONUP && JohnWatson_clickX != -1) {
            int oldRotation = getCharacterElement(4)->rotation;

            if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1155, 1200, 366, 415)) {
                if (is_in_area(ev->button.x, ev->button.y, 1155, 1200, 366, 415))
                    getCharacterElement(4)->rotation = -90;

            } else if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1106, 1155, 386, 442)) {
                if (is_in_area(ev->button.x, ev->button.y, 1106, 1155, 386, 442))
                    getCharacterElement(4)->rotation = -150;

            } else if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1103, 1155, 454, 506)) {
                if (is_in_area(ev->button.x, ev->button.y, 1103, 1155, 454, 506))
                    getCharacterElement(4)->rotation = -210;

            } else if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1155, 1200, 475, 524)) {
                if (is_in_area(ev->button.x, ev->button.y, 1155, 1200, 475, 524))
                    getCharacterElement(4)->rotation = -270;

            } else if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1203, 1252, 386, 442)) {
                if (is_in_area(ev->button.x, ev->button.y, 1203, 1252, 386, 442))
                    getCharacterElement(4)->rotation = -30;

            } else if (is_in_area(JohnWatson_clickX, JohnWatson_clickY, 1203, 1252, 454, 506)) {
                if (is_in_area(ev->button.x, ev->button.y, 1203, 1252, 454, 506))
                    getCharacterElement(4)->rotation = -330;
            }
            JohnWatson_clickX = -1;

            if (oldRotation != getCharacterElement(4)->rotation)
                doneCharacter(4);
        }
    }
}

void JohnWatson_moved(int target) {
    if (target == -1)
        return;

    set_can_switch(false);
    reset_selected_items();
    getMapUISettings()->title->text = getMapUISettings()->hint->text = NULL;
    *getMapUISettings()->ability = 1;

    JohnWatson_ability_parent.obj.rect = getMapUISettings()->hint->obj.rect;
    JohnWatson_ability_parent.time = currentTime();
    JohnWatson_ability.obj.rect = JohnWatson_ability_parent.obj.rect;
    JohnWatson_ability.time = currentTime();
}