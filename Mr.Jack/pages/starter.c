//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../audio/audio.h"

#define INITIAL_X 200
#define INITIAL_Y 200
#define SPLASH_WIDTH 490
#define SPLASH_HEIGHT 500

enum State {
    MENU, START, START2
};

enum State state = MENU;

void click(const void *element) {
    state = START;
}

void click2(const void *element) {
    state = START2;
}

int start() {
    state = MENU;
    int quit = 0;
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
    SDL_Window *window = NULL;
    window = SDL_CreateWindow(PROJECT_NAME, INITIAL_X, INITIAL_Y, SPLASH_WIDTH, SPLASH_HEIGHT, SDL_WINDOW_SHOWN);
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
    initAudio();

    GUI_Panel *pnl = create_panel();
    GUI_Image img = {
            .file = "assets/cover.jpg",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SPLASH_WIDTH, .h = SPLASH_HEIGHT},
                    .gravity = GRAVITY_CENTER
            },
    };
    insert_image2(&img, pnl);

    GUI_Label label = {
            .font = TTF_OpenFont("assets/FreeSans.ttf", 19),
            .text = CREATOR,
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SPLASH_WIDTH - 20, .h = SPLASH_HEIGHT - 20},
                    .gravity = GRAVITY_BOTTOM | GRAVITY_RIGHT,
            },
    };
    insert_label2(&label, pnl);

    bool continueGame = false;
    FILE *file = fopen(MR_JACK_DATA, "rb");
    if (file != NULL) {
        continueGame = true;
        fclose(file);
    }

    GUI_Panel *start = create_panel2((GUI_OBJECT) {
            .color = {.r = 0, .g = 0, .b = 0, .a = 130},
            .hoverColor = {.r = 0, .g = 0, .b = 0, .a = 160},
            .pressingColor = {.r = 0, .g = 0, .b = 0, .a = 200},
            .rect = {.x = SPLASH_WIDTH / 3, .y = SPLASH_HEIGHT / 2 - (continueGame ? 55 : 25), .w = SPLASH_WIDTH / 3, .h = 50},
            .clickListener = click,
    });
    GUI_Label start_txt = {
            .font = TTF_OpenFont("assets/FreeSans.ttf", 19),
            .text = continueGame ? "NEW" : "START",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = start->obj.rect,
                    .gravity = GRAVITY_CENTER
            },
    };
    insert_label2(&start_txt, start);
    insert_panel2(start, pnl);

    if (continueGame) {
        GUI_Panel *continue_btn = create_panel2((GUI_OBJECT) {
                .color = {.r = 0, .g = 0, .b = 0, .a = 130},
                .hoverColor = {.r = 0, .g = 0, .b = 0, .a = 160},
                .pressingColor = {.r = 0, .g = 0, .b = 0, .a = 200},
                .rect = {.x = SPLASH_WIDTH / 3, .y = SPLASH_HEIGHT / 2 + 5, .w = SPLASH_WIDTH / 3, .h = 50},
                .clickListener = click2,
        });
        GUI_Label continue_txt = {
                .font = TTF_OpenFont("assets/FreeSans.ttf", 19),
                .text = "CONTINUE",
                .obj = {
                        .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                        .rect = continue_btn->obj.rect,
                        .gravity = GRAVITY_CENTER
                },
        };
        insert_label2(&continue_txt, continue_btn);
        insert_panel2(continue_btn, pnl);
    }

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_for_all2(renderer, pnl->childs);
        SDL_RenderPresent(renderer);
        if (state == START || state == START2)
            break;
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    if (state == START) {
        return start_game();
        //return start_map(0, 0, 50, 50, 2, false);
        //return start_jack(-1, 1, 200, 200);
    } else if (state == START2) {
        return start_map(0, 0, 50, 50, 0, true);
    }

    return 0;
}