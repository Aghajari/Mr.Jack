//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../graphic/MrJack_Map.h"
#include "../characters/characters.h"

#define SCREEN_WIDTH 1300
#define SCREEN_HEIGHT 720

int start_map(int player1, int player2, int x, int y, int id) {
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
            .file = "map.jpg",
            .obj = {
                    .rect = {.x = SCREEN_HEIGHT / 4 * 267 / 411 + 4, .y = 0, .w = SCREEN_HEIGHT * 1802 / 1516, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&img, pnl);
    initializeMap(img.obj.rect.x);

    GUI_Image round = {
            .file = "round.png",
            .obj = {
                    .rect = {.x = img.obj.rect.x + img.obj.rect.w + 4, .y = 0, .w = SCREEN_HEIGHT * 155 / 1502, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&round, pnl);

    // Sum of rounds h: 1484
    char rounds_file_name[8][12];
    GUI_Image rounds[8];
    int rh = (int) ((SCREEN_HEIGHT - 8) / 7.7);
    int selected = 3;

    for (int i = 0; i < 8; ++i) {
        sprintf(rounds_file_name[i], "round%d%s.png", i + 1, i == selected ? "s" : "");
        rounds[i].file = rounds_file_name[i];
        rounds[i].obj.rect.x = round.obj.rect.x + 3;
        rounds[i].obj.rect.w = round.obj.rect.w - 7;
        if (i == 7) {
            rounds[i].obj.rect.h = (int) (rh * 0.74);
            rounds[i].obj.rect.y = rounds[i - 1].obj.rect.y - rounds[i].obj.rect.h;
        } else {
            rounds[i].obj.rect.y = SCREEN_HEIGHT - 4 - rh * (i + 1);
            rounds[i].obj.rect.h = rh;
        }
        rounds[i].obj.clickListener = NULL;
        insert_image2(&rounds[i], pnl);
    }

    int h = SCREEN_HEIGHT / 4;
    int w = img.obj.rect.x - 4;

    GUI_Image c1 = {
            .file = "c2.png",
            .obj = {
                    .rect = {.x = 0, .y = 0, .w = w, .h = h},
                    .gravity = GRAVITY_FILL
            },
    };
    insert_image2(&c1, pnl);

    GUI_Image c2 = {
            .file = "cs2.png",
            .obj = {
                    .rect = {.x = 0, .y = h, .w = w, .h = h},
                    .gravity = GRAVITY_FILL
            },
    };
    insert_image2(&c2, pnl);

    GUI_Image c3 = {
            .file = "cu2.png",
            .obj = {
                    .rect = {.x = 0, .y = h * 2, .w = w, .h = h},
                    .gravity = GRAVITY_FILL
            },
    };
    insert_image2(&c3, pnl);

    GUI_Image c4 = {
            .file = "c4.png",
            .obj = {
                    .rect = {.x = 0, .y = h * 3, .w = w, .h = h},
                    .gravity = GRAVITY_FILL
            },
    };
    insert_image2(&c4, pnl);

    int x0 = round.obj.rect.x + round.obj.rect.w + 4;
    GUI_Image characters = {
            .file = "characters.png",
            .obj = {
                    .rect = {.x = x0, .y = SCREEN_HEIGHT - ((SCREEN_WIDTH - x0 - 4) * 300 / 540),
                             .w = SCREEN_WIDTH - x0 - 4, .h = (SCREEN_WIDTH - x0 - 4) * 300 / 540},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&characters, pnl);

    while (!quit) {
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

        SDL_SetRenderDrawColor(renderer, 47, 63, 82, 255);
        SDL_RenderClear(renderer);

        draw_for_all2(renderer, pnl->childs);
        render_map(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return 0;
}