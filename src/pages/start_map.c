//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../characters/characters.h"
#include "../graphic/MrJack_Map.h"
#include "../gameplay/MrJack_Gameplay.h"

#define SCREEN_WIDTH 1300
#define SCREEN_HEIGHT 720

typedef enum {
    SELECT_CARD, NORMAL
} GameState;
GameState currentState = NORMAL;

int playerTurn = 0;
bool detective = true;
GUI_Image c[4];

bool clicking = false;
int click_x1, click_y1;

void selectCard(const void *element) {
    if (playerTurn != 0 && (get_selected_character() == NULL || get_can_switch())) {
        MRJACK_Character *target = findCharacter(((GUI_Image *) element)->obj.target);
        ((GUI_Image *) element)->file = target->selected;
        ((GUI_Image *) element)->obj.refresh = true;
        select_character_map(target, detective);

        for (int i = 0; i < 4; ++i) {
            if (&c[i] != element) {
                MRJACK_Character *character = findCharacter(c[i].obj.target);
                if (c[i].file != character->character) {
                    c[i].file = character->character;
                    c[i].obj.refresh = true;
                }
            }
        }
    }
}

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
                    .rect = {.x = SCREEN_HEIGHT / 4 * 267 / 411 + 4, .y = 0, .w = SCREEN_HEIGHT * 1802 /
                                                                                  1516, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&img, pnl);
    initializeMap(img.obj.rect.x);

    GUI_Image select_card = {
            .file = "select_card.png",
            .obj = {
                    .rect = img.obj.rect,
                    .gravity = GRAVITY_FILL2
            },
    };

    GUI_Image round = {
            .file = "round.png",
            .obj = {
                    .rect = {.x = img.obj.rect.x + img.obj.rect.w + 4, .y = 0, .w = SCREEN_HEIGHT * 155 /
                                                                                    1502, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&round, pnl);

    // Sum of rounds h: 1484
    char rounds_file_name[8][12];
    GUI_Image rounds[8];
    int rh = (int) ((SCREEN_HEIGHT - 8) / 7.7);
    int selectedRound = 1;

    for (int i = 0; i < 8; ++i) {
        sprintf(rounds_file_name[i], "round%d%s.png", i + 1, (i + 1) == selectedRound ? "s" : "");
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

    for (int i = 0; i < 4; ++i) {
        MRJACK_Character *character = pick_card();
        c[i].file = character->character;
        c[i].obj.gravity = GRAVITY_FILL;
        c[i].obj.target = character->id;
        c[i].obj.clickListener = selectCard;
        c[i].obj.event.pressed = false;
        c[i].obj.event.pressing = false;
        c[i].obj.rect.x = 0;
        c[i].obj.rect.w = w;
        c[i].obj.rect.h = h;
        c[i].obj.rect.y = i * h;
        insert_image2(&c[i], pnl);
    }

    int x0 = round.obj.rect.x + round.obj.rect.w + 4;

    SDL_Rect players_rect = {.x = x0, .y = 10,
            .w = SCREEN_WIDTH - x0 - 4,
            .h = (SCREEN_WIDTH - x0 - 4) * 114 / 673};

    GUI_Image players[3] = {
            {
                    .file = "bot.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "player1.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2
                    },
            },
            {
                    .file = "player2.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2
                    },
            },
    };

    int seen = 1;
    SDL_Rect seen_rect = {.x = x0, .y = players_rect.y + players_rect.h + 10,
            .w = (SCREEN_WIDTH - x0 - 4) / 2,
            .h = (SCREEN_WIDTH - x0 - 4) / 2 * 983 / 700};
    GUI_Image seenVsUnseen[2] = {
            {
                    .file = "unseen.png",
                    .obj = {
                            .rect = seen_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "seen.png",
                    .obj = {
                            .rect = seen_rect,
                            .gravity = GRAVITY_FILL2
                    },
            }
    };

    GUI_Image redCard = {
            .file = "red_card.png",
            .obj = {
                    .rect = {.x = seen_rect.x + seen_rect.w + 6, .y = seen_rect.y,
                            .w = (SCREEN_WIDTH - (seen_rect.x + seen_rect.w) - 20),
                            .h = (SCREEN_WIDTH - (seen_rect.x + seen_rect.w) - 20) * 247 / 157},
                    .gravity = GRAVITY_FILL2,
            },
    };

    int ability = 1;
    SDL_Rect ability_rect = {.x = x0 + 20,
            .y = SCREEN_HEIGHT - ((SCREEN_WIDTH - x0 - 44) * 135 / 350) - 10,
            .w = (SCREEN_WIDTH - x0 - 44),
            .h = (SCREEN_WIDTH - x0 - 44) * 135 / 350};
    GUI_Image ability_btn[3] = {
            {
                    .file = "ability_0.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "ability_1.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "ability_2.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            }
    };


    int textTop = redCard.obj.rect.y + redCard.obj.rect.h + 20;
    GUI_Label title = {
            .font = TTF_OpenFont("FreeSans.ttf", 16),
            .text = "Hello World :)",
            .obj = {
                    .color = {.r = 242, .g = 255, .b = 0, .a = 255},
                    .rect = {.x = x0 + 10, .y = textTop, .w = SCREEN_WIDTH - x0 - 14, .h = 20},
                    .gravity = GRAVITY_LEFT | GRAVITY_TOP
            }
    };

    textTop += 25;
    GUI_Label hint = {
            .font = TTF_OpenFont("FreeSans.ttf", 16),
            .text = "Hello World :)",
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = title.obj.rect.x, .y = textTop, .w = title.obj.rect.w, .h = ability_rect.y - 8 - textTop},
                    .gravity = GRAVITY_LEFT | GRAVITY_TOP
            }
    };

    getMapUISettings()->title = &title;
    getMapUISettings()->hint = &hint;
    getMapUISettings()->ability = &ability;

    playerTurn = gameplay_whosTurn(player1, player2, selectedRound, 0);

    while (!quit) {
        SDL_Event evt; // no need for new/delete, stack is fine

        while (SDL_PollEvent(&evt)) {
            // quit on close, window close, or 'escape' key hit
            if (evt.type == SDL_QUIT ||
                (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
                (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }

            if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
                clicking = true;
                click_x1 = evt.button.x;
                click_y1 = evt.button.y;
            } else if (evt.type == SDL_MOUSEBUTTONUP && clicking) {
                clicking = false;
                if (get_selected_character() != NULL)
                    get_selected_character()->click(click_x1, click_y1, evt.button.x, evt.button.y);
            }
            event_for_all2(&evt, pnl->childs);
        }

        SDL_SetRenderDrawColor(renderer, 47, 63, 82, 255);
        SDL_RenderClear(renderer);

        render_image(&players[playerTurn], renderer);
        render_image(&seenVsUnseen[seen], renderer);
        render_image(&ability_btn[ability], renderer);
        render_image(&redCard, renderer);
        render_label(&hint, renderer);
        render_label(&title, renderer);

        draw_for_all2(renderer, pnl->childs);
        render_map(renderer);

        if (currentState == SELECT_CARD)
            render_image(&select_card, renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return 0;
}