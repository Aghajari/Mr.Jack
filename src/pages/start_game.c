//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../audio/audio.h"

#define INITIAL_X 200
#define INITIAL_Y 200
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700

GUI_Panel *pnl, *rain_pnl;
GUI_FadeInImage detective, mr_jack,
        next, bg, player1, player2;

SDL_Rect gp1_center, gp2_center,
        gp1_jack, gp2_jack,
        gp1_detective, gp2_detective;

enum GameState {
    STARTING,
    CHOICE,
};

enum GameState gameState = STARTING;
long long lastRainTime;

void createRain();

bool isShowingNext = false;
int statePlayer1 = 0, statePlayer2 = 0;
bool nextClicked = false;

void nextClick(const void *target){
    nextClicked = true;
}

void updateNext(){
    player2.obj.rect = statePlayer2 == 0 ? gp2_center : (statePlayer2 == -1 ? gp2_jack : gp2_detective);
    player1.obj.rect = statePlayer1 == 0 ? gp1_center : (statePlayer1 == -1 ? gp1_jack : gp1_detective);
    mr_jack.selected = statePlayer2 == -1 || statePlayer1 == -1;
    detective.selected = statePlayer2 == 1 || statePlayer1 == 1;

    if (statePlayer1 != 0 && !isShowingNext) {
        isShowingNext = true;
        insert_fadein_image2(&next, pnl);
    } else if (isShowingNext && statePlayer1 == 0){
        isShowingNext = false;
        remove_element(&next);
    }
}
void keyPlayer2(int value) {
    statePlayer2 = (int) fmax(-1, fmin(1, statePlayer2 + value));
    updateNext();
}

void keyPlayer1(int value) {
    statePlayer1 = (int) fmax(-1, fmin(1, statePlayer1 + value));
    updateNext();
}

void choiceListener(const void *target) {
    statePlayer1 = target == &mr_jack ? -1 : 1;
    updateNext();
}

void listener() {
    playSound("thunder.wav", SDL_MIX_MAXVOLUME / 2);

    GUI_FadeRect *rect1 = malloc(sizeof(GUI_FadeRect));
    GUI_FadeRect *rect2 = malloc(sizeof(GUI_FadeRect));
    if (rect1 == NULL || rect2 == NULL) {
        if (rect1 != NULL) free(rect1);
        if (rect2 != NULL) free(rect2);
        return;
    }

    rect1->showDuration = rect2->showDuration = 50;
    rect1->duration = rect2->duration = 50;
    rect1->obj.rect = rect2->obj.rect = (SDL_Rect) {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT};
    rect1->obj.color.r = rect1->obj.color.g = rect1->obj.color.b = 255;
    rect2->obj.color.r = rect2->obj.color.g = rect2->obj.color.b = 255;
    rect1->delay = 150;
    rect2->delay = 350;
    insert_fade_rect2(rect1, rain_pnl);
    insert_fade_rect2(rect2, rain_pnl);
}

void listener2() {
    gameState = CHOICE;
    playSound("theme.wav", SDL_MIX_MAXVOLUME);

    gp1_center.w = 55;
    gp1_center.h = 38;
    gp1_center.x = SCREEN_WIDTH / 2 - gp1_center.w / 2;

    int h = gp1_center.h * 2 + 25;

    next = (GUI_FadeInImage) {
            .duration = 500,
            .file = "next.png",
            .hoverFile = NULL,
            .selectedFile = NULL,
            .obj = {
                    .rect = {.x = SCREEN_WIDTH - 122 - 40, .y = SCREEN_HEIGHT - 62 - 40, .w = 122, .h = 62},
                    .clickListener = nextClick
            }
    };

    bg = (GUI_FadeInImage) {
            .duration = 800,
            .file = "bg.png",
            .hoverFile = NULL,
            .selectedFile = NULL,
            .obj = {
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL,
                    .clickListener = choiceListener
            },
            .time = currentTime()
    };

    mr_jack = (GUI_FadeInImage) {
            .duration = 1500,
            .file = "mr_jack.png",
            .hoverFile = "mr_jack_hover.png",
            .selectedFile = "mr_jack_selected.png",
            .obj = {
                    .rect = {.x = SCREEN_WIDTH / 2 - 306 - 60, .y = SCREEN_HEIGHT / 2 - 306 / 2 +
                                                                    30 - h / 2, .h = 306, .w = 306},
                    .gravity = GRAVITY_CENTER,
                    .clickListener = choiceListener
            }
    };

    detective = (GUI_FadeInImage) {
            .duration = 1500,
            .file = "detective.png",
            .hoverFile = "detective_hover.png",
            .selectedFile = "detective_selected.png",
            .obj = {
                    .rect = {.x = SCREEN_WIDTH / 2 + 60, .y = SCREEN_HEIGHT / 2 - 306 / 2 + 30 - h / 2, .h = 306, .w = 306},
                    .gravity = GRAVITY_CENTER,
                    .clickListener = choiceListener
            }
    };

    gp1_center.y = detective.obj.rect.y + detective.obj.rect.h;
    gp2_center.x = gp1_center.x;
    gp2_center.w = gp1_center.w;
    gp2_center.h = gp1_center.h;
    gp2_center.y = gp1_center.y + 25 + gp1_center.h;

    gp1_jack.y = gp1_center.y;
    gp1_jack.w = gp1_center.w;
    gp1_jack.h = gp1_center.h;
    gp2_jack.y = gp2_center.y;
    gp2_jack.w = gp2_center.w;
    gp2_jack.h = gp2_center.h;
    gp1_jack.x = gp2_jack.x = mr_jack.obj.rect.x + mr_jack.obj.rect.w / 2 - 20;

    gp1_detective.y = gp1_center.y;
    gp1_detective.w = gp1_center.w;
    gp1_detective.h = gp1_center.h;
    gp2_detective.y = gp2_center.y;
    gp2_detective.w = gp2_center.w;
    gp2_detective.h = gp2_center.h;
    gp1_detective.x = gp2_detective.x = detective.obj.rect.x + detective.obj.rect.w / 2 - 20;

    player1 = (GUI_FadeInImage) {
            .duration = 1500,
            .file = "gamepad.png",
            .hoverFile = NULL,
            .selectedFile = NULL,
            .obj = {
                    .rect = gp1_center,
                    .gravity = GRAVITY_CENTER,
                    .clickListener = NULL
            }
    };

    player2 = (GUI_FadeInImage) {
            .duration = 1500,
            .file = "gamepad.png",
            .hoverFile = NULL,
            .selectedFile = NULL,
            .obj = {
                    .rect = gp2_center,
                    .gravity = GRAVITY_CENTER,
                    .clickListener = NULL
            }
    };

    insert_fadein_image2(&mr_jack, pnl);
    insert_fadein_image2(&detective, pnl);
    insert_fadein_image2(&player1, pnl);
    insert_fadein_image2(&player2, pnl);
}

int start_game() {
    srand(lastRainTime = currentTime() + 3000);

    int quit = 0;
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
    SDL_Window *window = NULL;
    window = SDL_CreateWindow("Mr. Jack", INITIAL_X, INITIAL_Y, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    initAudio();
    playSound("rain.wav", SDL_MIX_MAXVOLUME / 3);

    pnl = create_panel();
    rain_pnl = create_panel();

    GUI_FadeLabel label = {
            .font = TTF_OpenFont("FreeSans.ttf", 50),
            .text = "1888 East End, London",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_CENTER,
            },
            .showDuration = 2500,
            .duration = 1000,
            .delay = 1000,
            .listener = NULL
    };
    insert_fade_label2(&label, pnl);

    GUI_FadeLabel label2 = {
            .font = TTF_OpenFont("FreeSans.ttf", 50),
            .text = "Hello Jack?",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_CENTER,
            },
            .showDuration = 1500,
            .duration = 1500,
            .delay = 6000,
            .listener = NULL
    };
    insert_fade_label2(&label2, pnl);

    GUI_FadeLabel label3 = {
            .font = TTF_OpenFont("FreeSans.ttf", 50),
            .text = "Where are you?",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_CENTER,
            },
            .showDuration = 1500,
            .duration = 1500,
            .delay = 10500,
            .listener = NULL
    };
    insert_fade_label2(&label3, pnl);

    GUI_FadeLabel label4 = {
            .font = TTF_OpenFont("FreeSans.ttf", 50),
            .text = "I'm gonna find you...",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_CENTER,
            },
            .showDuration = 1500,
            .duration = 1500,
            .delay = 15000,
            .listener = listener
    };
    insert_fade_label2(&label4, pnl);

    GUI_FadeLabel label5 = {
            .font = TTF_OpenFont("FreeSans.ttf", 35),
            .text = "Choose what character you want to play with",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = 0, .y = 70, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_TOP | GRAVITY_CENTER_H,
            },
            .showDuration = -1,
            .duration = 2000,
            .delay = 20000,
            .listener = listener2
    };
    insert_fade_label2(&label5, pnl);

    insert_panel(rain_pnl);
    insert_panel(pnl);

    while (!quit) {
        SDL_Event evt; // no need for new/delete, stack is fine

        while (SDL_PollEvent(&evt)) {
            // quit on close, window close, or 'escape' key hit
            if (evt.type == SDL_QUIT ||
                (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
                (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }
            if (gameState == CHOICE && evt.type == SDL_KEYDOWN) {
                if (evt.key.keysym.sym == SDLK_LEFT)
                    keyPlayer1(-1);
                else if (evt.key.keysym.sym == SDLK_RIGHT)
                    keyPlayer1(1);
                else if (evt.key.keysym.sym == SDLK_UP)
                    keyPlayer2(-1);
                else if (evt.key.keysym.sym == SDLK_DOWN)
                    keyPlayer2(1);
            }

            event_for_all2(&evt, pnl->childs);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameState == STARTING)
            createRain();
        else
            render_fadein_image(&bg, renderer);

        draw_rain(renderer, rain_pnl->childs);
        draw_for_all2(renderer, pnl->childs);
        SDL_RenderPresent(renderer);

        if (nextClicked)
            break;
    }

    remove_element(pnl);
    remove_element(rain_pnl);
    free_panel(pnl);
    free_panel(rain_pnl);

    int x, y;
    SDL_GetWindowPosition(window, &x, &y);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    //endAudio();
    return start_jack(statePlayer1, statePlayer2, x, y);
}

int rnd(int bound) {
    return rand() % bound;
}

void createRain() {
    long duration = 80, time = currentTime();
    if (time - lastRainTime > duration) {
        lastRainTime = time;

        GUI_RainImage *img = malloc(sizeof(GUI_RainImage));
        if (img == NULL)
            return;
        img->obj.rect = (SDL_Rect) {.x = rnd(SCREEN_WIDTH - 50),
                .y = rnd(SCREEN_HEIGHT - 50),
                .w = 147 / (rnd(4) + 3),
                .h = 143 / (rnd(4) + 3)};
        img->duration = 500;
        img->showDuration = 1000;
        img->type = rnd(13);
        insert_rain2(img, rain_pnl);
    }
}