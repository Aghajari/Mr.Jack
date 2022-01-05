//
// Created by AmirHossein Aghajari on 1/4/22.
//

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include <stdbool.h>
#include "MrJack_Map.h"
#include "GUI_init.h"
#include "GUI_gravity.h"

#define WHITECHAPEL_LEN (5 + 7 + 6 + 6 + 7 + 8 + 8 + 8 + 7 + 6 + 6 + 7 + 5)
#define mi map_items
#define DEBUG_MAP 0

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

float initialX;

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

void initializeMap(int x) {
    for (int i = 0; i < WHITECHAPEL_LEN; i++) {
        mi[i].enabled = mi[i].isHome = mi[i].isLight = mi[i].isTunnel = false;
    }

    initialX = (float) x;
    SDL_Color color = {.r = 242, .g = 255, .b = 0, .a = 255};

    int h = 73, i0 = 0;
    for (int i = 0; i < 5; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 40;
        mi[i0 + i].points[1].x = 80;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 215 + (i * h) + i;
        fix_relative(i0 + i, h, 22);
    }
    i0 += 5;

    for (int i = 0; i < 7; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 79 + 22;
        mi[i0 + i].points[1].x = 120 + 22;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 103 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 7;

    for (int i = 0; i < 6; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 162;
        mi[i0 + i].points[1].x = 162 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 140 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 6;

    for (int i = 0; i < 6; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 223;
        mi[i0 + i].points[1].x = 223 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 103 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 6;

    for (int i = 0; i < 7; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 284;
        mi[i0 + i].points[1].x = 284 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 67 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 7;

    for (int i = 0; i < 8; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 344;
        mi[i0 + i].points[1].x = 344 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 30 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 8;

    for (int i = 0; i < 8; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 404;
        mi[i0 + i].points[1].x = 404 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 67 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 8;

    for (int i = 0; i < 8; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 464;
        mi[i0 + i].points[1].x = 464 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 104 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 8;

    for (int i = 0; i < 7; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 524;
        mi[i0 + i].points[1].x = 524 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 141 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 7;

    for (int i = 0; i < 6; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 584;
        mi[i0 + i].points[1].x = 584 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 177 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 6;

    for (int i = 0; i < 6; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 645;
        mi[i0 + i].points[1].x = 645 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 141 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 6;

    for (int i = 0; i < 7; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 707;
        mi[i0 + i].points[1].x = 707 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 104 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 7;

    for (int i = 0; i < 5; ++i) {
#if DEBUG_MAP
        mi[i0 + i].enabled = true;
#endif
        mi[i0 + i].color = color;

        mi[i0 + i].points[0].x = 766;
        mi[i0 + i].points[1].x = 766 + 40;
        mi[i0 + i].points[0].y = mi[i0 + i].points[1].y = 141 + (i * h) + i;
        fix_relative(i0 + i, h, 20);
    }
    i0 += 5;

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
    int padding = 6;
    rect->y = mi[i].points[0].y;
    rect->h = mi[i].points[3].y - rect->y;
    rect->y += padding;
    rect->h -= 2 * padding;

    rect->x = (int) initialX + mi[i].points[5].x;
    int w = mi[i].points[2].x - mi[i].points[5].x;
    rect->w = rect->h;
    rect->x += w / 2 - rect->w / 2;
}

void initLights() {
    fillRectFromMap(10, &lights[1].currentRect, &lights[1]);
    fillRectFromMap(75, &lights[2].currentRect, &lights[2]);
    fillRectFromMap(12, &lights[3].currentRect, &lights[3]);
    fillRectFromMap(73, &lights[4].currentRect, &lights[4]);
    fillRectFromMap(36, &lights[0].currentRect, &lights[0]);
    fillRectFromMap(49, &lights[5].currentRect, &lights[5]);

    sprintf(lights[1].name, "light%d.png", 1);
    sprintf(lights[2].name, "light%d.png", 2);
    sprintf(lights[3].name, "light%d.png", 3);
    sprintf(lights[4].name, "light%d.png", 4);
    sprintf(lights[0].name, "light.png");
    sprintf(lights[5].name, "light.png");

    for (int i = 0; i < 6; ++i) {
        lights[i].rotation = 0;
        lights[i].visible = true;
    }
}

void initBlocks() {
    fillRectFromMap(11, &blocks[0].currentRect, &blocks[0]);
    fillRectFromMap(74, &blocks[1].currentRect, &blocks[1]);

    for (int i = 0; i < 2; ++i) {
        sprintf(blocks[i].name, "block.png");
        blocks[i].rotation = 0;
        blocks[i].visible = true;
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

        sprintf(wayBlocks[i].name, "way.png");
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
        sprintf(characters[i].name, "m%d.png", i + 1);
    }
    characters[3].rotation = -30;
    sprintf(characterLight.name, "clight.png");
}

void render_item(MapElement *element, SDL_Renderer *r) {
    IMG_Init(IMG_INIT_JPG);
    SDL_Surface *surface = IMG_Load(element->name);
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);

    if (element->rotation > 0) {
        SDL_RenderCopy(r, t, NULL, &element->currentRect);
    } else {
        SDL_RenderCopyEx(r, t, NULL, &element->currentRect,
                         element->rotation, NULL, SDL_FLIP_NONE);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);
}

void render_map(SDL_Renderer *r) {
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

    for (int i = 0; i < 8; ++i) {
        if (i < 2 && blocks[i].visible)
            render_item(&blocks[i], r);
        if (i < 4 && wayBlocks[i].visible)
            render_item(&wayBlocks[i], r);
        if (i < 6 && lights[i].visible)
            render_item(&lights[i], r);

        render_item(&characters[i], r);
    }
    characterLight.currentRect.x = characters[3].currentRect.x - characters[3].currentRect.w * 1.5;
    characterLight.currentRect.y = characters[3].currentRect.y - characters[3].currentRect.h * 1.5;
    characterLight.currentRect.w = characters[3].currentRect.w * 4;
    characterLight.currentRect.h = characters[3].currentRect.h * 4;
    characterLight.rotation = characters[3].rotation;
    render_item(&characterLight, r);
}