//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../characters/characters.h"


#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700

bool reveal = false;

void nextClick2(const void *target) {

}

void revealClick(const void *target) {
    reveal = true;
}

int start_jack(int player1, int player2, int x, int y) {
    int quit = 0;
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    SDL_Window *window = NULL;
    window = SDL_CreateWindow("Mr. Jack", x, y, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "create window failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = NULL;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {   // renderer creation may fail too
        fprintf(stderr, "create renderer failed: %s\n", SDL_GetError());
        return 2;
    }
    initialize();

    GUI_Panel *pnl = create_panel();
    GUI_Image img = {
            .file = "bg.png",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_CENTER
            },
    };
    insert_image2(&img, pnl);

    GUI_Label lbl = {
            .text = "See Mr. Jack's identity by clicking on the red card",
            .font = TTF_OpenFont("FreeSans.ttf", 30),
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 100, .h = SCREEN_HEIGHT, .w = SCREEN_WIDTH},
                    .gravity = GRAVITY_TOP | GRAVITY_CENTER_H
            }
    };
    insert_label2(&lbl, pnl);

    GUI_FadeInImage red_card = {
            .duration = 800,
            .file = "red_card.png",
            .obj = {
                    .rect = {.x = SCREEN_WIDTH / 2 - 78,
                            .y = SCREEN_HEIGHT / 2 - 123 + 50,
                            .w = 78 * 2, .h = 123 * 2},
                    .gravity = GRAVITY_FILL2,
                    .clickListener = revealClick
            },
    };
    insert_fadein_image2(&red_card, pnl);

    GUI_FadeInImage next = {
            .duration = 500,
            .file = "next.png",
            .hoverFile = NULL,
            .selectedFile = NULL,
            .obj = {
                    .rect = {.x = SCREEN_WIDTH - 122 - 40, .y = SCREEN_HEIGHT - 62 - 40, .w = 122, .h = 62},
                    .clickListener = nextClick2,
                    .gravity = GRAVITY_CENTER
            },
            .time = -1
    };

    MRJACK_Character c;
    GUI_FadeInImage character = {
            .duration = 500,
            .obj = {
                    .rect = {.x = SCREEN_WIDTH / 2 - (int) (78 * 1.5),
                            .y = SCREEN_HEIGHT / 2 - (int) (123 * 1.5) + 50,
                            .w = 78 * 3, .h = 123 * 3},
                    .gravity = GRAVITY_FILL2,
                    .clickListener = revealClick
            }
    };

    while (!quit) {
        if (reveal && next.time == -1) {
            c = findCharacter(rand() % CHARACTERS_COUNT + 1);
            character.file = c.innocent;
            lbl.text = c.name;

            insert_fadein_image2(&next, pnl);
            insert_fadein_image2(&character, pnl);
        }

        SDL_Event evt; // no need for new/delete, stack is fine

        while (SDL_PollEvent(&evt)) {
            // quit on close, window close, or 'escape' key hit
            if (evt.type == SDL_QUIT ||
                (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
                (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }

            event_for_all2(&evt, pnl->childs);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_for_all2(renderer, pnl->childs);
        SDL_RenderPresent(renderer);
    }

    int x2, y2;
    SDL_GetWindowPosition(window, &x2, &y2);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return start_map(player1, player2, x2, y2, c.id);
}