//
// Created by AmirHossein Aghajari on 1/4/22.
//


#include "../characters/characters.h"
#include "GUI_init.h"
#include "GUI_gravity.h"
#include "MrJack_Map.h"
#include "../pages/starter.h"

// 5 + 7 + 6 + 6 + 7 + 8 + 8 + 8 + 7 + 6 + 6 + 7 + 5 = 86
#define WHITECHAPEL_LEN (86)
#define mi map_items
//#define DEBUG_MAP 0

#define connect(i, c1, c2, c3, c4, c5, c6) { \
mi[i].connectedTo[0] = c1;                  \
mi[i].connectedTo[1] = c2;                  \
mi[i].connectedTo[2] = c3;                  \
mi[i].connectedTo[3] = c4;                  \
mi[i].connectedTo[4] = c5;                  \
mi[i].connectedTo[5] = c6;                  \
}

Whitechapel map_items[WHITECHAPEL_LEN];
MapElement lights[6], characters[8], blocks[2], wayBlocks[4];
MRJACK_Character *selectedCharacter = NULL;

bool canSwitch, moved;
float initialX;
long long int ignoreTime = 0;

MapUISettings uiSettings;

MapUISettings *getMapUISettings() {
    return &uiSettings;
}

void initLights();

void initBlocks();

void initCharacters();

void fix_relative(int i, int h, int w2) {
    mi[i].points[2].x = mi[i].points[1].x + w2;
    mi[i].points[2].y = mi[i].points[1].y + h / 2;

    mi[i].points[3].x = mi[i].points[1].x;
    mi[i].points[3].y = mi[i].points[1].y + h;

    mi[i].points[4].x = mi[i].points[0].x;
    mi[i].points[4].y = mi[i].points[0].y + h;

    mi[i].points[5].x = mi[i].points[0].x - w2;
    mi[i].points[5].y = mi[i].points[0].y + h / 2;
}

void reset_selected_items() {
    for (int i = 0; i < WHITECHAPEL_LEN; i++) {
        mi[i].enabled = false;
    }
}

void initializeMap(int x) {
    selectedCharacter = NULL;
    for (int i = 0; i < WHITECHAPEL_LEN; i++) {
        mi[i].enabled = mi[i].isHome = mi[i].isLight = mi[i].isTunnel = false;
    }

    initialX = (float) x;

    int h = 73, i0 = 0;
    int mapInit[13][5] = {
            //ROWS, X0, X1, Y0, WIDTH
            {5, 40,  80,  215, 22},
            {7, 101, 142, 103, 20},
            {6, 162, 202, 140, 20},
            {6, 223, 263, 103, 20},
            {7, 284, 324, 67,  20},
            {8, 344, 384, 30,  20},
            {8, 404, 444, 67,  20},
            {8, 464, 504, 104, 20},
            {7, 524, 564, 141, 20},
            {6, 584, 624, 177, 20},
            {6, 645, 685, 141, 20},
            {7, 707, 747, 104, 20},
            {5, 766, 806, 141, 20},
    };

    for (int k = 0; k < 13; ++k) {
        for (int i = 0; i < mapInit[k][0]; ++i) {
            mi[i0 + i].points[0].x = mapInit[k][1];
            mi[i0 + i].points[1].x = mapInit[k][2];
            mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = mapInit[k][3] + (i * h) + i;
            fix_relative(i0 + i, h, mapInit[k][4]);
        }
        i0 += mapInit[k][0];
    }

    mi[6].isTunnel = mi[11].isTunnel
            = mi[15].isTunnel = mi[31].isTunnel
            = mi[43].isTunnel = mi[54].isTunnel
            = mi[70].isTunnel = mi[74].isTunnel
            = mi[79].isTunnel = true;

    mi[10].isLight = mi[12].isLight
            = mi[32].isLight = mi[36].isLight
            = mi[49].isLight = mi[53].isLight
            = mi[73].isLight = mi[75].isLight = true;

    mi[7].isHome = mi[8].isHome = mi[18].isHome
            = mi[14].isHome = mi[20].isHome
            = mi[22].isHome = mi[33].isHome
            = mi[35].isHome = mi[37].isHome
            = mi[48].isHome = mi[50].isHome
            = mi[52].isHome = mi[63].isHome
            = mi[65].isHome = mi[67].isHome
            = mi[71].isHome = mi[77].isHome
            = mi[78].isHome = true;

    // UP, DOWN, LU, LD, RU, RD
    connect(0, -1, 1, -1, -1, 6, 7)
    connect(1, 0, 2, -1, -1, 7, 8)
    connect(2, 1, 3, -1, -1, 8, 9)
    connect(3, 2, 4, -1, -1, 9, 10)
    connect(4, 3, -1, -1, -1, 10, 11) //1
    connect(5, -1, 6, -1, -1, -1, 12)
    connect(6, 5, 7, -1, 0, 12, 13)
    connect(7, 6, 8, 0, 1, 13, 14)
    connect(8, 7, 9, 1, 2, 14, 15)
    connect(9, 8, 10, 2, 3, 15, 16)
    connect(10, 9, 11, 3, 4, 16, 17)
    connect(11, 10, -1, 4, -1, 17, -1) //2
    connect(12, -1, 13, 5, 6, 18, 19)
    connect(13, 12, 14, 6, 7, 19, 20)
    connect(14, 13, 15, 7, 8, 20, 21)
    connect(15, 14, 16, 8, 9, 21, 22)
    connect(16, 15, 17, 9, 10, 22, 23)
    connect(17, 16, -1, 10, 11, 23, -1) //3
    connect(18, -1, 19, -1, 12, 24, 25)
    connect(19, 18, 20, 12, 13, 25, 26)
    connect(20, 19, 21, 13, 14, 26, 27)
    connect(21, 20, 22, 14, 15, 27, 28)
    connect(22, 21, 23, 15, 16, 28, 29)
    connect(23, 22, -1, 16, 17, 29, 30) //4
    connect(24, -1, 25, -1, 18, 31, 32)
    connect(25, 24, 26, 18, 19, 32, 33)
    connect(26, 25, 27, 19, 20, 33, 34)
    connect(27, 26, 28, 20, 21, 34, 35)
    connect(28, 27, 29, 21, 22, 35, 36)
    connect(29, 28, 30, 22, 23, 36, 37)
    connect(30, 29, -1, 23, -1, 37, 38) //5
    connect(31, -1, 32, -1, 24, -1, 39)
    connect(32, 31, 33, 24, 25, 39, 40)
    connect(33, 32, 34, 25, 26, 40, 41)
    connect(34, 33, 35, 26, 27, 41, 42)
    connect(35, 34, 36, 27, 28, 42, 43)
    connect(36, 35, 37, 28, 29, 43, 44)
    connect(37, 36, 38, 29, 30, 44, 45)
    connect(38, 37, -1, 30, -1, 45, 46) //6
    connect(39, -1, 40, 31, 32, -1, 47)
    connect(40, 39, 41, 32, 33, 47, 48)
    connect(41, 40, 42, 33, 34, 48, 49)
    connect(42, 41, 43, 34, 35, 49, 50)
    connect(43, 42, 44, 35, 36, 50, 51)
    connect(44, 43, 45, 36, 37, 51, 52)
    connect(45, 44, 46, 37, 38, 52, 53)
    connect(46, 45, -1, 38, -1, 53, 54) //7
    connect(47, -1, 48, 39, 40, -1, 55)
    connect(48, 47, 49, 40, 41, 55, 56)
    connect(49, 48, 50, 41, 42, 56, 57)
    connect(50, 49, 51, 42, 43, 57, 58)
    connect(51, 50, 52, 43, 44, 58, 59)
    connect(52, 51, 53, 44, 45, 59, 60)
    connect(53, 52, 54, 45, 46, 60, 61)
    connect(54, 53, -1, 46, -1, 61, -1) //8
    connect(55, -1, 56, 47, 48, -1, 62)
    connect(56, 55, 57, 48, 49, 62, 63)
    connect(57, 56, 58, 49, 50, 63, 64)
    connect(58, 57, 59, 50, 51, 64, 65)
    connect(59, 58, 60, 51, 52, 65, 66)
    connect(60, 59, 61, 52, 53, 66, 67)
    connect(61, 60, -1, 53, 54, 67, -1) //9
    connect(62, -1, 63, 55, 56, 68, 69)
    connect(63, 62, 64, 56, 57, 69, 70)
    connect(64, 63, 65, 57, 58, 70, 71)
    connect(65, 64, 66, 58, 59, 71, 72)
    connect(66, 65, 67, 59, 60, 72, 73)
    connect(67, 66, -1, 60, 61, 73, -1) //10
    connect(68, -1, 69, -1, 62, 74, 75)
    connect(69, 68, 70, 62, 63, 75, 76)
    connect(70, 69, 71, 63, 64, 76, 77)
    connect(71, 70, 72, 64, 65, 77, 78)
    connect(72, 71, 73, 65, 66, 78, 79)
    connect(73, 72, -1, 66, 67, 79, 80) //11
    connect(74, -1, 75, -1, 68, -1, 81)
    connect(75, 74, 76, 68, 69, 81, 82)
    connect(76, 75, 77, 69, 70, 82, 83)
    connect(77, 76, 78, 70, 71, 83, 84)
    connect(78, 77, 79, 71, 72, 84, 85)
    connect(79, 78, 80, 72, 73, 85, -1)
    connect(80, 79, -1, 73, -1, -1, -1) //12
    connect(81, -1, 82, 74, 75, -1, -1)
    connect(82, 81, 83, 75, 76, -1, -1)
    connect(83, 82, 84, 76, 77, -1, -1)
    connect(84, 83, 85, 77, 78, -1, -1)
    connect(85, 84, -1, 78, 79, -1, -1)
    set_can_switch(true);

    initLights();
    initBlocks();
    initCharacters();
}

void fillRectFromMap(int i, SDL_Rect *rect, MapElement *element) {
    if (element != NULL)
        element->selected = i;

    rect->x = (int) initialX + mi[i].points[5].x;
    rect->y = mi[i].points[0].y;
    rect->w = mi[i].points[2].x - mi[i].points[5].x;
    rect->h = mi[i].points[3].y - rect->y;
}

void fillRectFromMap2(int i, SDL_Rect *rect, MapElement *element) {
    if (element != NULL)
        element->selected = i;

    bool outside = i < 0;
    if (outside) {
        switch (i) {
            case -10:
                i = 5;
                break;
            case -20:
                i = 74;
                break;
            case -30:
                i = 11;
                break;
            case -40:
                i = 80;
                break;
            default:
                break;
        }
    }
    int padding = 6;
    rect->y = mi[i].points[0].y;
    rect->h = mi[i].points[3].y - rect->y;
    rect->y += padding;
    rect->h -= 2 * padding;

    rect->x = (int) initialX + mi[i].points[5].x;
    int w = mi[i].points[2].x - mi[i].points[5].x;
    rect->w = rect->h;
    rect->x += w / 2 - rect->w / 2;

    if (outside) {
        int paddingX = rect->w, paddingY = rect->h;
        switch (i) {
            case 5:
                rect->x -= paddingX;
                rect->y -= paddingY;
                break;
            case 74:
                rect->x += paddingX / 2;
                rect->y -= paddingY;
                break;
            case 11:
                rect->x -= paddingX / 2;
                rect->y += paddingY;
                break;
            case 80:
                rect->x += paddingX / 2 + 10;
                rect->y += paddingY;
                break;
            default:
                break;
        }
    }
}

void initLights() {
    fillRectFromMap(10, &lights[1].currentRect, &lights[1]);
    fillRectFromMap(75, &lights[2].currentRect, &lights[2]);
    fillRectFromMap(12, &lights[3].currentRect, &lights[3]);
    fillRectFromMap(73, &lights[4].currentRect, &lights[4]);
    fillRectFromMap(36, &lights[0].currentRect, &lights[0]);
    fillRectFromMap(49, &lights[5].currentRect, &lights[5]);

    sprintf(lights[1].name, "assets/light%d.png", 1);
    sprintf(lights[2].name, "assets/light%d.png", 2);
    sprintf(lights[3].name, "assets/light%d.png", 3);
    sprintf(lights[4].name, "assets/light%d.png", 4);
    sprintf(lights[0].name, "assets/light.png");
    sprintf(lights[5].name, "assets/light.png");

    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 5; ++j)
            lights[i].move[j] = -1;
        lights[i].rotation = 0;
        lights[i].visible = true;
        lights[i].surface = lights[i].texture = NULL;
    }
}

void initBlocks() {
    fillRectFromMap(11, &blocks[0].currentRect, &blocks[0]);
    fillRectFromMap(74, &blocks[1].currentRect, &blocks[1]);

    for (int i = 0; i < 2; ++i) {
        sprintf(blocks[i].name, "assets/block.png");
        for (int j = 0; j < 5; ++j)
            blocks[i].move[j] = -1;
        blocks[i].rotation = 0;
        blocks[i].visible = true;
        blocks[i].surface = blocks[i].texture = NULL;
    }

    wayBlocks[0].currentRect.x = (int) initialX + 25;
    wayBlocks[0].currentRect.y = 60;
    wayBlocks[0].rotation = -45;

    wayBlocks[1].currentRect.x = (int) initialX + 700;
    wayBlocks[1].currentRect.y = 40;
    wayBlocks[1].rotation = -360 + 30;

    wayBlocks[2].currentRect.x = (int) initialX + 25;
    wayBlocks[2].currentRect.y = 620;
    wayBlocks[2].rotation = -140;

    wayBlocks[3].currentRect.x = (int) initialX + 710;
    wayBlocks[3].currentRect.y = 625;
    wayBlocks[3].rotation = -215;

    for (int i = 0; i < 4; ++i) {
        wayBlocks[i].currentRect.w = 99;
        wayBlocks[i].currentRect.h = 51;
        wayBlocks[i].surface = wayBlocks[i].texture = NULL;
        sprintf(wayBlocks[i].name, "assets/way.png");
        wayBlocks[i].visible = i == 1 || i == 2;
    }
}

MapElement characterLight;

void initCharacters() {
    fillRectFromMap2(28, &characters[0].currentRect, &characters[0]);
    fillRectFromMap2(2, &characters[1].currentRect, &characters[1]);
    fillRectFromMap2(57, &characters[2].currentRect, &characters[2]);
    fillRectFromMap2(61, &characters[3].currentRect, &characters[3]);
    fillRectFromMap2(44, &characters[4].currentRect, &characters[4]);
    fillRectFromMap2(41, &characters[5].currentRect, &characters[5]);
    fillRectFromMap2(83, &characters[6].currentRect, &characters[6]);
    fillRectFromMap2(24, &characters[7].currentRect, &characters[7]);

    for (int i = 0; i < 8; ++i) {
        characters[i].rotation = 0;
        characters[i].visible = true;
        characters[i].isSafe = false;
        characters[i].surface = characters[i].texture = NULL;
        for (int j = 0; j < 5; ++j)
            characters[i].move[j] = -1;
        sprintf(characters[i].name, "assets/m%d.png", i + 1);
    }
    characters[3].rotation = -30;
    characterLight.surface = characterLight.texture = NULL;
    sprintf(characterLight.name, "assets/clight.png");
}

void render_item(MapElement *element, SDL_Renderer *r) {
    IMG_Init(IMG_INIT_JPG);
    if (element->refresh || element->texture == NULL) {
        element->refresh = false;
        if (element->texture != NULL) {
            SDL_FreeSurface(element->surface);
            SDL_DestroyTexture(element->texture);
        }
        element->surface = IMG_Load(element->name);
        element->texture = SDL_CreateTextureFromSurface(r, element->surface);
    }

    if (element->rotation > 0) {
        SDL_RenderCopy(r, element->texture, NULL, &element->currentRect);
    } else {
        SDL_RenderCopyEx(r, element->texture, NULL, &element->currentRect,
                         element->rotation, NULL, SDL_FLIP_NONE);
    }

}

double interpolator(double t) {
    return (cos((t + 1) * M_PI) / 2.0) + 0.5;
}

void fix_move_state(MapElement *element, int id) {
    if (element->move[0] != -1) {
        bool connected = false;
        for (int i = 0; i < 6; ++i) {
            if (mi[element->selected].connectedTo[i] == element->move[0]) {
                connected = true;
                break;
            }
        }
        int duration = connected ? 120 : 400;

        long long int time = currentTime();
        double scale = (double) (time - element->time) / (double) duration;
        scale = fmax(0, fmin(scale, 1));

        SDL_Rect start, end;
        if (id == -1) {
            fillRectFromMap(element->selected, &start, NULL);
            fillRectFromMap(element->move[0], &end, NULL);
        } else {
            fillRectFromMap2(element->selected, &start, NULL);
            fillRectFromMap2(element->move[0], &end, NULL);
        }

        double scale2 = interpolator(scale);
        element->currentRect.x = start.x + (int) ((end.x - start.x) * scale2);
        element->currentRect.y = start.y + (int) ((end.y - start.y) * scale2);
        ignoreTime = element->time + duration + 20;

        if (scale >= 1) {
            element->time = time;
            element->selected = element->move[0];
            for (int i = 0; i < 4; ++i)
                element->move[i] = element->move[i + 1];
        }
        if (id != -1 && element->move[0] == -1 && selectedCharacter != NULL && selectedCharacter->id == id)
            selectedCharacter->move(-1);
    }
}

void render_map(SDL_Renderer *r) {
    for (int i = 0; i < 6; ++i) {
        if (i < 2 && blocks[i].visible) {
            fix_move_state(&blocks[i], -1);
            render_item(&blocks[i], r);
        }
        if (i < 4 && wayBlocks[i].visible)
            render_item(&wayBlocks[i], r);
        if (i < 6 && lights[i].visible) {
            fix_move_state(&lights[i], -1);
            render_item(&lights[i], r);
        }
    }

    for (int i = 0; i < 8; ++i) {
        fix_move_state(&characters[i], i + 1);
        if (selectedCharacter == NULL || i + 1 != selectedCharacter->id)
            render_item(&characters[i], r);
    }

    float scale = 3;
    SDL_RenderSetScale(r, scale, scale);
    for (int i = 0; i < WHITECHAPEL_LEN; i++) {
        if (mi[i].enabled) {
            SDL_SetRenderDrawColor(r, mi[i].color.r, mi[i].color.g, mi[i].color.b, mi[i].color.a);
            for (int j = 0; j < 6; ++j) {
                SDL_RenderDrawLineF(r, initialX / scale + mi[i].points[j].x / scale,
                                    mi[i].points[j].y / scale,
                                    initialX / scale + mi[i].points[j == 5 ? 0 : j + 1].x / scale,
                                    mi[i].points[j == 5 ? 0 : j + 1].y / scale);
            }
        }

        /*char text[3];
        sprintf(text, "%d", i);
        GUI_Label label = {
                .text = text,
                .font = TTF_OpenFont("FreeSans.ttf", 19),
                .obj = {
                        .color = mi[i].color,
                        .rect = {.x = (int) (((float) initialX / scale) + mi[i].points[0].x / scale),
                                .y = mi[i].points[0].y / scale,
                                .w = (int) ((float) (mi[i].points[1].x - mi[i].points[0].x) / scale),
                                .h = (int) ((float) (mi[i].points[1].y - mi[i].points[4].y) / scale)
                        },
                        .gravity = GRAVITY_CENTER
                }
        };
        render_label(&label, r);*/
    }
    SDL_RenderSetScale(r, 1, 1);

    if (selectedCharacter != NULL) {
        render_item(&characters[selectedCharacter->id - 1], r);
        if (selectedCharacter->renderer != NULL)
            selectedCharacter->renderer(r);
    }

    characterLight.currentRect.x = characters[3].currentRect.x - characters[3].currentRect.w * 1.5;
    characterLight.currentRect.y = characters[3].currentRect.y - characters[3].currentRect.h * 1.5;
    characterLight.currentRect.w = characters[3].currentRect.w * 4;
    characterLight.currentRect.h = characters[3].currentRect.h * 4;
    characterLight.rotation = characters[3].rotation;
    render_item(&characterLight, r);
}

int find_block_selected(int index) {
    if (blocks[index].move[0] != -1)
        return blocks[index].move[0];
    else
        return blocks[index].selected;
}

void select_all(int index, int move, bool fromHome, bool detective) {
    Whitechapel *whitechapel = &mi[index];

    if (whitechapel->isHome || whitechapel->isLight) {
        if (fromHome && move > 0) {
            for (int i = 0; i < 6; ++i) {
                if (whitechapel->connectedTo[i] != -1)
                    select_all(whitechapel->connectedTo[i], move - 1, fromHome, detective);
            }
        }
        return;
    }

    if (!whitechapel->enabled) {
        bool isFilled;
        for (int i = 0; i < 8; ++i) {
            isFilled = getFinalElementSelectedIndex(&characters[i]) == index;
            if (isFilled)
                break;
        }
        if (isFilled && detective) {
            whitechapel->color.r = 255;
            whitechapel->color.g = 0;
            whitechapel->color.b = 0;
            whitechapel->color.a = 255;
            whitechapel->enabled = true;
        } else if (!isFilled) {
            whitechapel->color.r = 242;
            whitechapel->color.g = 255;
            whitechapel->color.b = 0;
            whitechapel->color.a = 255;
            whitechapel->enabled = true;
        }
    }

    if (move > 0) {
        for (int i = 0; i < 6; ++i) {
            if (whitechapel->connectedTo[i] != -1)
                select_all(whitechapel->connectedTo[i], move - 1, fromHome, detective);
        }

        if (whitechapel->isTunnel && find_block_selected(0) != index && find_block_selected(1) != index) {
            for (int i = 0; i < WHITECHAPEL_LEN; ++i) {
                if (mi[i].isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                    select_all(i, move - 1, fromHome, detective);
                }
            }
        }
    }
}

void select_whitechapel_characters(int character) {
    for (int i = 0; i < 8; ++i) {
        Whitechapel *whitechapel = &mi[getFinalElementSelectedIndex(&characters[i])];
        whitechapel->color.r = character == (i + 1) ? 0 : 242;
        whitechapel->color.g = 255;
        whitechapel->color.b = 0;
        whitechapel->color.a = 255;
        whitechapel->enabled = true;
    }
}

void select_whitechapel(int character, int move, bool fromHome, bool detective) {
    int index = getFinalElementSelectedIndex(&characters[character - 1]);
    Whitechapel *whitechapel = &mi[index];
    whitechapel->color.r = 0;
    whitechapel->color.g = 255;
    whitechapel->color.b = 0;
    whitechapel->color.a = 255;
    whitechapel->enabled = true;

    for (int i = 0; i < 6; ++i) {
        if (whitechapel->connectedTo[i] != -1)
            select_all(whitechapel->connectedTo[i], move - 1, fromHome, detective);
    }

    if (whitechapel->isTunnel && find_block_selected(0) != index && find_block_selected(1) != index) {
        for (int i = 0; i < WHITECHAPEL_LEN; ++i) {
            if (mi[i].isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                select_all(i, move - 1, fromHome, detective);
            }
        }
    }
}

void select_character_map(MRJACK_Character *character, bool detective) {
    selectedCharacter = character;
    if (selectedCharacter != NULL) {
        getMapUISettings()->detective = detective;
        selectedCharacter->init(detective);
        uiSettings.hint->text = selectedCharacter->desc;
        uiSettings.title->text = selectedCharacter->name;
        uiSettings.hint->obj.refresh = true;
        uiSettings.title->obj.refresh = true;
    }
}

MRJACK_Character *get_selected_character() {
    return selectedCharacter;
}

void set_can_switch(bool enabled) {
    canSwitch = enabled;
    if (enabled)
        moved = false;
}

bool get_can_switch() {
    return canSwitch;
}

bool has_moved() {
    return moved;
}

int getEnabledWhitechapelInArea(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < WHITECHAPEL_LEN; ++i) {
        if (isClickedOnWhitechapel(x1, y1, x2, y2, i))
            return i;
    }
    return -1;
}

int getEnabledWhitechapelInAreaOfCharacters(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 8; ++i) {
        if (isClickedOnWhitechapel(x1, y1, x2, y2, characters[i].selected))
            return characters[i].selected;
    }
    return -1;
}

bool isClickedOnWhitechapel(int x1, int y1, int x2, int y2, int index) {
    Whitechapel *w = &mi[index];
    if (w->enabled) {
        if ((w->points[0].x + (int) initialX) <= x1 && w->points[0].y <= y1 &&
            (w->points[1].x + (int) initialX) >= x1 && w->points[3].y >= y1) {
            if ((w->points[0].x + (int) initialX) <= x2 && w->points[0].y <= y2 &&
                (w->points[1].x + (int) initialX) >= x2 && w->points[3].y >= y2)
                return true;
        }
    }
    return false;
}

void find_best_move2(int move, bool fromHome, int start, int end, int index, int out[], int in[], int fill) {
    if (index == end) {
        in[fill] = end;
        int len1 = 0, len2 = fill + 1;
        for (int i = 0; i < 4; ++i) {
            if (out[i] != -1)
                len1++;
            else
                break;
        }

        if (len1 > len2 || len1 == 0)
            for (int i = 0; i < 4; ++i)
                out[i] = in[i];
        return;
    }

    if (move == 0)
        return;

    Whitechapel *whitechapel = &mi[index];
    if (move > 0) {
        if (fromHome || (!whitechapel->isHome && !whitechapel->isLight)) {
            for (int i = 0; i < 6; ++i) {
                if (whitechapel->connectedTo[i] != -1) {
                    int in_copy[4] = {in[0], in[1], in[2], in[3]};
                    in_copy[fill] = index;
                    find_best_move2(move - 1, fromHome, start, end, whitechapel->connectedTo[i], out, in_copy,
                                    fill + 1);
                }
            }
        }

        if (whitechapel->isTunnel && find_block_selected(0) != index && find_block_selected(1) != index) {
            for (int i = 0; i < WHITECHAPEL_LEN; ++i) {
                if (mi[i].isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                    int in_copy[4] = {in[0], in[1], in[2], in[3]};
                    in_copy[fill] = index;
                    find_best_move2(move - 1, fromHome, start, end, i, out, in_copy, fill + 1);
                }
            }
        }
    }
}

void find_best_move(int move, bool fromHome, int start, int end, int out[]) {
    Whitechapel *whitechapel = &mi[start];
    int in[] = {-1, -1, -1, -1};
    out[0] = out[1] = out[2] = out[3] = out[4] = -1;

    for (int i = 0; i < 6; ++i) {
        if (whitechapel->connectedTo[i] != -1)
            find_best_move2(move - 1, fromHome, start, end, whitechapel->connectedTo[i], out, in, 0);
    }

    if (whitechapel->isTunnel && find_block_selected(0) != start && find_block_selected(1) != start) {
        for (int i = 0; i < WHITECHAPEL_LEN; ++i) {
            if (mi[i].isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                find_best_move2(move - 1, fromHome, start, end, i, out, in, 0);
            }
        }
    }
}

bool isFilledWithCharacter(int w, int c) {
    for (int i = 1; i <= 8; ++i) {
        if (i == c)
            continue;
        if (getFinalCharacterSelectedIndex(i) == w)
            return true;
    }
    return false;
}

MapElement *getCharacterElement(int character) {
    return &characters[character - 1];
}

int can_jack_leave_the_district() {
    if (!getMapUISettings()->detective
        && *getMapUISettings()->seen == 0) {
        int moves = getMapUISettings()->jackId == 2 ? 4 : 3;

        int out[5];
        int startWay = getFinalCharacterSelectedIndex(getMapUISettings()->jackId);

        int ways[] = {5, 74, 11, 80};
        for (int i = 0; i < 4; ++i) {
            if (wayBlocks[i].visible)
                continue;

            int w = ways[i];
            if (w == startWay)
                return i + 1;
            else {
                find_best_move(moves, getMapUISettings()->jackId == 2, startWay, w, out);
                int len1 = 0;
                for (int j = 0; i < 5; ++j) {
                    if (out[j] != -1)
                        len1++;
                    else
                        break;
                }
                if (len1 > 0 && len1 < moves)
                    return i + 1;
            }
        }
    }
    return 0;
}

void moveCharacter(int x1, int y1, int x2, int y2, int move, bool fromHome) {
    long long int time = currentTime();
    if (ignoreTime > time)
        return;

    if (!getMapUISettings()->detective
        && *getMapUISettings()->seen == 0
        && selectedCharacter->id == getMapUISettings()->jackId) {

        int selected = -1;
        if (is_in_area(x1, y1, 810, 930, 22, 94)) {
            if (is_in_area(x2, y2, 810, 930, 22, 94))
                selected = 1;
        } else if (is_in_area(x1, y1, 135, 250, 615, 690)) {
            if (is_in_area(x2, y2, 135, 250, 615, 690))
                selected = 2;
        } else if (is_in_area(x1, y1, 130, 200, 30, 140)) {
            if (is_in_area(x2, y2, 130, 200, 30, 140))
                selected = 0;
        } else if (is_in_area(x1, y1, 820, 930, 620, 700)) {
            if (is_in_area(x2, y2, 820, 930, 620, 700))
                selected = 3;
        }

        if (selected != -1 && !wayBlocks[selected].visible) {
            int w;
            switch (selected) {
                case 1:
                    w = 74;
                    break;
                case 2:
                    w = 11;
                    break;
                case 3:
                    w = 80;
                    break;
                default:
                    w = 5;
                    break;
            }
            int *out = getCharacterElement(selectedCharacter->id)->move;
            int start = getFinalCharacterSelectedIndex(selectedCharacter->id);
            if (w != start) {
                find_best_move(move, fromHome, start, w, out);
            } else {
                out[0] = out[1] = out[2] = out[3] = out[4] = -1;
            }

            int len1 = 0;
            for (int i = 0; i < 5; ++i) {
                if (out[i] != -1)
                    len1++;
                else
                    break;
            }
            if (len1 >= move || (len1 == 0 && w != start)) {
                out[0] = out[1] = out[2] = out[3] = out[4] = -1;
                return;
            } else {
                getCharacterElement(selectedCharacter->id)->selected = start;
                out[len1] = -((selected + 1) * 10);
                gameOver(0);
            }

            getCharacterElement(selectedCharacter->id)->time = currentTime();
            ignoreTime = time + 100;
            moved = true;
            selectedCharacter->move(w);
        }
    }
    int w = getEnabledWhitechapelInArea(x1, y1, x2, y2);
    moveCharacter2(w, move, fromHome);
}

bool moveCharacter2(int w, int move, bool fromHome) {
    long long int time = currentTime();
    if (ignoreTime > time)
        return false;

    int start = getFinalCharacterSelectedIndex(selectedCharacter->id);
    if (w != -1 && start != w) {
        find_best_move(move, fromHome, start, w, getCharacterElement(selectedCharacter->id)->move);
        if (getCharacterElement(selectedCharacter->id)->move[0] == -1)
            return false;

        getCharacterElement(selectedCharacter->id)->time = currentTime();

        ignoreTime = time + 100;
        moved = true;
        selectedCharacter->move(w);

        if (getMapUISettings()->detective && isFilledWithCharacter(w, selectedCharacter->id)) {
            int id = -1;
            for (int i = 1; i <= 8; ++i) {
                if (i == selectedCharacter->id)
                    continue;
                if (getFinalCharacterSelectedIndex(i) == w) {
                    id = i;
                    break;
                }
            }
            if (id != -1) {
                if (id == getMapUISettings()->jackId) {
                    gameOver(2);
                } else {
                    gameOver(2 + id);
                }
            }
        }
        return true;
    }
    return false;
}

void moveCharacter3(int w, int character, int move, bool fromHome) {
    long long int time = currentTime();
    if (ignoreTime > time)
        return;

    int start = getFinalCharacterSelectedIndex(selectedCharacter->id);
    if (w != -1 && start != w) {
        find_best_move(move, fromHome, start, w, getCharacterElement(character)->move);
        getCharacterElement(character)->time = currentTime();
        ignoreTime = time + 100;
        moved = true;
    }
}

int fixMoveWithNoCharacter(int c) {
    int size = 0;
    MapElement *element = getCharacterElement(c);
    for (int i = 0; i < 5; ++i) {
        if (element->move[i] != -1)
            size++;
        else
            break;
    }

    int sizeTmp = size;
    int removedCount = 0;
    while (size > 0 && isFilledWithCharacter(element->move[size - 1], c)) {
        element->move[--size] = -1;
        removedCount++;
    }
    bool removeAll = false;
    for (int i = 0; i < 5; ++i) {
        if (element->move[i] != -1 && isFilledWithCharacter(element->move[i], c)) {
            removeAll = true;
            break;
        }
    }
    if (removeAll) {
        for (int i = 0; i < 5; ++i)
            element->move[i] = -1;
        return sizeTmp;
    }
    return removedCount;
}

void swapCharacters(int x1, int y1, int x2, int y2) {
    long long int time = currentTime();
    if (ignoreTime > time)
        return;

    int w = getEnabledWhitechapelInAreaOfCharacters(x1, y1, x2, y2);
    int start = getFinalCharacterSelectedIndex(selectedCharacter->id);
    if (w != -1 && start != w) {
        MapElement *targetElement = NULL;
        for (int i = 0; i < 8; ++i) {
            if (getFinalElementSelectedIndex(&characters[i]) == w) {
                targetElement = &characters[i];
                break;
            }
        }

        if (targetElement == NULL)
            return;

        getCharacterElement(selectedCharacter->id)->move[0] = w;
        getCharacterElement(selectedCharacter->id)->move[1] = -1;
        targetElement->move[0] = start;
        targetElement->move[1] = -1;

        getCharacterElement(selectedCharacter->id)->time = currentTime();
        targetElement->time = currentTime();
        ignoreTime = time + 100;
        moved = true;
        selectedCharacter->move(w);
    }
}

int get_whitechapel_len() {
    return WHITECHAPEL_LEN;
}

Whitechapel *get_whitechapel(int index) {
    return &mi[index];
}

MapElement *get_elements(int type) {
    switch (type) {
        case 0:
            return characters;
        case 1:
            return lights;
        case 2:
            return blocks;
        case 3:
            return wayBlocks;
        default:
            return NULL;
    }
}

bool isCharacterVisible(int character) {
    int selected;
    Whitechapel *target = &mi[selected = getFinalCharacterSelectedIndex(character)];
    for (int i = 0; i < 6; ++i) {
        int w = target->connectedTo[i];
        if (w != -1) {
            if (mi[w].isLight) {
                for (int j = 0; j < 6; ++j) {
                    if (lights[j].visible && getFinalElementSelectedIndex(&lights[j]) == w)
                        return true;
                }
            } else if (!mi[w].isHome) {
                for (int j = 0; j < 8; ++j) {
                    if (j == character - 1)
                        continue;

                    if (getFinalCharacterSelectedIndex(j + 1) == w)
                        return true;
                }
            }
        }
    }

    if (character != 4) {
        MapElement *johnWatson = &characters[3];
        return isVisibleForWatson(johnWatson->rotation, getFinalElementSelectedIndex(johnWatson), selected);
    }
    return false;
}

void disable_light(int index) {
    lights[index].visible = false;
}

int getFinalElementSelectedIndex(MapElement *e) {
    int index = e->selected;
    if (e->move[0] != -1) {
        for (int j = 0; j < 5; ++j) {
            if (e->move[j] != -1)
                index = e->move[j];
        }
    }
    return index;
}

int getFinalCharacterSelectedIndex(int c) {
    return getFinalElementSelectedIndex(getCharacterElement(c));
}

void safe_character(int id) {
    if (getMapUISettings()->jackId != id) {
        getCharacterElement(id)->isSafe = true;
        strcpy(getCharacterElement(id)->name, findCharacter(id)->circleSafe);
        getCharacterElement(id)->refresh = true;
    }
}

bool isVisibleForWatson(int rotation, int selected, int target) {
    if (target == selected)
        return false;

    int n;
    switch (rotation) {
        case -30:
            n = 4;
            break;
        case -90:
            n = 0;
            break;
        case -150:
            n = 2;
            break;
        case -210:
            n = 3;
            break;
        case -270:
            n = 1;
            break;
        case -330:
            n = 5;
            break;
        default:
            return false;
    }

    Whitechapel *whitechapel = &mi[selected];
    while (1) {
        int connected = whitechapel->connectedTo[n];
        if (connected == target)
            return true;
        else if (connected == -1 || mi[connected].isHome || mi[connected].isLight)
            return false;
        else
            whitechapel = &mi[connected];
    }
}