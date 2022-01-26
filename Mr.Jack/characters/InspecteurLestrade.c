//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include "../graphic/GUI_init.h"
#include "inner_characters.h"
#include "../graphic/MrJack_Map.h"

GUI_FadeInImage InspecteurLestrade_yellow = {
        .time = 0,
        .duration = 0,
        .file = "assets/yellow_cover.png",
        .obj = {
                .texture = NULL,
                .surface = NULL,
                .clickListener = NULL,
        }
}, InspecteurLestrade_green = {
        .time = 0,
        .duration = 0,
        .file = "assets/green_cover.png",
        .obj = {
                .texture = NULL,
                .surface = NULL,
                .clickListener = NULL,
        }
};

int InspecteurLestrade_clickX, InspecteurLestrade_clickY;

void InspecteurLestrade_ability_click(const void *element) {
    if (!get_can_switch())
        return;

    InspecteurLestrade_clickX = -1;
    *getMapUISettings()->ability = !*getMapUISettings()->ability;
    reset_selected_items();
    if (!*getMapUISettings()->ability) {
        select_whitechapel(1, 3, false, getMapUISettings()->detective);
    }

    InspecteurLestrade_yellow.obj.refresh = true;
    InspecteurLestrade_green.obj.refresh = true;
}

void InspecteurLestrade_initialize(bool detective) {
    set_can_switch(true);
    reset_selected_items();
    select_whitechapel(1, 3, false, detective);
    *getMapUISettings()->ability = 0;
    getMapUISettings()->target = -1;

    getMapUISettings()->abilityBtn[0].obj.clickListener = InspecteurLestrade_ability_click;
    getMapUISettings()->abilityBtn[1].obj.clickListener = InspecteurLestrade_ability_click;
    getMapUISettings()->abilityBtn[2].obj.clickListener = NULL;
}

void InspecteurLestrade_click(int x1, int y1, int x2, int y2) {
    moveCharacter(x1, y1, x2, y2, 3, false);
}

void InspecteurLestrade_render(SDL_Renderer *r) {
    if (*getMapUISettings()->ability == 1) {
        MapElement *elements = get_elements(3);

        if (getMapUISettings()->target == -1) {
            for (int i = 0; i < 4; ++i) {
                if (elements[i].visible) {
                    InspecteurLestrade_yellow.obj.rect = elements[i].currentRect;
                    render_fadein_image_rotation(&InspecteurLestrade_yellow, r, elements[i].rotation);
                }
            }
        } else {
            for (int i = 0; i < 4; ++i) {
                if (i == getMapUISettings()->target && elements[i].visible) {
                    InspecteurLestrade_green.obj.rect = elements[i].currentRect;
                    render_fadein_image_rotation(&InspecteurLestrade_green, r, elements[i].rotation);
                } else if (!elements[i].visible) {
                    InspecteurLestrade_yellow.obj.rect = elements[i].currentRect;
                    render_fadein_image_rotation(&InspecteurLestrade_yellow, r, elements[i].rotation);
                }
            }
        }
    }
}

void InspecteurLestrade_event(SDL_Event *ev) {
    if (*getMapUISettings()->ability == 1) {
        if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
            InspecteurLestrade_clickX = ev->button.x;
            InspecteurLestrade_clickY = ev->button.y;

        } else if (ev->type == SDL_MOUSEBUTTONUP && InspecteurLestrade_clickX != -1) {
            int selected = -1;

            if (is_in_area(InspecteurLestrade_clickX, InspecteurLestrade_clickY, 810, 930, 22, 94)) {
                if (is_in_area(ev->button.x, ev->button.y, 810, 930, 22, 94))
                    selected = 1;
            } else if (is_in_area(InspecteurLestrade_clickX, InspecteurLestrade_clickY, 135, 250, 615, 690)) {
                if (is_in_area(ev->button.x, ev->button.y, 135, 250, 615, 690))
                    selected = 2;
            } else if (is_in_area(InspecteurLestrade_clickX, InspecteurLestrade_clickY, 130, 200, 30, 140)) {
                if (is_in_area(ev->button.x, ev->button.y, 130, 200, 30, 140))
                    selected = 0;
            } else if (is_in_area(InspecteurLestrade_clickX, InspecteurLestrade_clickY, 820, 930, 620, 700)) {
                if (is_in_area(ev->button.x, ev->button.y, 820, 930, 620, 700))
                    selected = 3;
            }

            if (selected != -1) {
                MapElement *elements = get_elements(3);

                if (getMapUISettings()->target == -1) {
                    if (elements[selected].visible) {
                        set_can_switch(false);
                        getMapUISettings()->target = selected;
                    }
                } else {
                    if (!elements[selected].visible) {
                        elements[selected].visible = true;
                        elements[getMapUISettings()->target].visible = false;
                        if (has_moved()) {
                            doneCharacter(1);
                        } else {
                            *getMapUISettings()->ability = 2;
                            reset_selected_items();
                            select_whitechapel(1, 3, false, getMapUISettings()->detective);
                        }
                    } else if (selected == getMapUISettings()->target) {
                        getMapUISettings()->target = -1;
                    }
                }
            }
        }
    }
}

void InspecteurLestrade_moved(int target) {
    if (target == -1 && *getMapUISettings()->ability == 2) {
        doneCharacter(1);
    } else if (target != -1) {
        for (int i = 0; i < 3; ++i)
            getMapUISettings()->abilityBtn[i].obj.clickListener = NULL;

        if (*getMapUISettings()->ability != 2) {
            InspecteurLestrade_ability_click(NULL);
            *getMapUISettings()->ability = 1;
        }
        set_can_switch(false);
    }
}