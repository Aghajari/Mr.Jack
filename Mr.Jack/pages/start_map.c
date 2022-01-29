//
// Created by AmirHossein Aghajari on 12/31/21.
//

#include "starter.h"
#include "../graphic/GUI_init.h"
#include "../graphic/GUI_gravity.h"
#include "../characters/characters.h"
#include "../graphic/MrJack_Map.h"
#include "../gameplay/MrJack_Gameplay.h"
#include "../audio/audio.h"

#define SCREEN_WIDTH 1300
#define SCREEN_HEIGHT 720

typedef enum {
    SELECT_CARD, NORMAL
} GameState;
GameState currentState = NORMAL;

int playerTurn = 0, turn, selectedRound = 1;
bool detective = true;
GUI_Image c[4];
char rounds_file_name[8][19];
GUI_Image rounds[8];

bool clicking = false;
int click_x1, click_y1;

bool bot_callback = false;
long long int time = -1;

GUI_FadeInImage select_card = {
        .duration = 800,
        .file = "assets/select_card.png",
        .obj = {
                .gravity = GRAVITY_FILL2
        }
};

char game_over_icon[25];
bool game_over = false;
GUI_FadeInImage game_over_image = {
        .duration = 1500,
        .file = game_over_icon,
        .obj = {
                .gravity = GRAVITY_FILL2,
                .rect = {.x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT}
        }
};
char *empty_string = " ";

void gameOver(int type) {
    // 0: Jack left the district (Jack wins)
    // 1: Jack is not caught (Jack wins)
    // 2: The detective catches Jack (Detective wins)
    // 3-10:  The detective catches innocent character :( (Jack wins)
    game_over = true;
    bot_callback = false;
    game_over_image.time = currentTime();
    sprintf(game_over_icon, "assets/game_over_%d.png", type);
    game_over_image.obj.refresh = true;
    remove(MR_JACK_DATA);

    endAudio();
    initAudio();
    playSound("assets/game_over.wav", SDL_MIX_MAXVOLUME / 2);
}

struct MapElementTmp {
    int selected, rotation;
    bool visible, isSafe;
};

void copyMapElement(struct MapElementTmp *tmp, MapElement *element) {
    tmp->isSafe = element->isSafe;
    tmp->visible = element->visible;
    tmp->selected = getFinalElementSelectedIndex(element);
    tmp->rotation = element->rotation;
}

void loadMapElement(struct MapElementTmp *tmp, MapElement *element, int i) {
    element->isSafe = tmp->isSafe;
    element->visible = tmp->visible;
    if (i >= 1) {
        if (element->isSafe) {
            strcpy(element->name, findCharacter(i)->circleSafe);
        } else {
            strcpy(element->name, findCharacter(i)->circle);
        }
        element->refresh = true;
    }

    element->rotation = tmp->rotation;
    element->selected = tmp->selected;
    if (i != -2) {
        if (i != -1) {
            fillRectFromMap2(tmp->selected, &element->currentRect, NULL);
        } else {
            fillRectFromMap(tmp->selected, &element->currentRect, NULL);
        }
    }
}

void saveTheGame(bool forZ) {
    FILE *file = fopen(forZ ? MR_JACK_Z_DATA : MR_JACK_DATA, "wb");
    if (file == NULL)
        return;

    int output[28];
    saveCharactersData(output); // 18

    for (int i = 0; i < 4; ++i)
        output[18 + i] = c[i].obj.target;
    // 22

    output[22] = getMapUISettings()->jackId;
    output[23] = getMapUISettings()->player1;
    output[24] = getMapUISettings()->player2;
    output[25] = *getMapUISettings()->seen;
    output[26] = selectedRound;
    output[27] = turn;

    struct MapElementTmp output2[20];
    MapElement *elements = get_elements(0);
    for (int i = 0; i < 8; ++i)
        copyMapElement(&output2[i], &elements[i]);

    elements = get_elements(1);
    for (int i = 0; i < 6; ++i)
        copyMapElement(&output2[i + 8], &elements[i]);

    elements = get_elements(2);
    for (int i = 0; i < 2; ++i)
        copyMapElement(&output2[i + 14], &elements[i]);

    elements = get_elements(3);
    for (int i = 0; i < 4; ++i)
        copyMapElement(&output2[i + 16], &elements[i]);

    fwrite(output, sizeof(int), 28, file);
    fwrite(output2, sizeof(struct MapElementTmp), 20, file);
    fclose(file);
}

void innerLoad() {
    playerTurn = gameplay_whosTurn(getMapUISettings()->player1,
                                   getMapUISettings()->player2,
                                   selectedRound, turn, &detective);
    getMapUISettings()->hint->text = empty_string;
    getMapUISettings()->title->text = empty_string;
    getMapUISettings()->hint->obj.refresh = true;
    getMapUISettings()->title->obj.refresh = true;
    *getMapUISettings()->ability = 2;

    currentState = SELECT_CARD;
    select_card.time = currentTime();
    select_character_map(NULL, false);

    time = currentTime() + 500;
    bot_callback = playerTurn == 0;
}

void loadTheGame(bool forZ) {
    FILE *file = fopen(forZ ? MR_JACK_Z_DATA : MR_JACK_DATA, "rb");
    if (file == NULL)
        return;

    reset_selected_items();
    int input[28];
    struct MapElementTmp input2[20];
    fread(input, sizeof(int), 28, file);
    fread(input2, sizeof(struct MapElementTmp), 20, file);
    fclose(file);

    input[1] -= 4;
    loadCharactersData(input);

    for (int i = 0; i < 4; ++i) {
        MRJACK_Character *character = pick_card();
        if (input[i + 18] == -1) {
            c[i].file = character->disabled;
            c[i].obj.target = -1;
        } else {
            c[i].file = character->character;
            c[i].obj.target = character->id;
        }
        c[i].obj.refresh = true;
    }

    getMapUISettings()->jackId = input[22];
    getMapUISettings()->player1 = input[23];
    getMapUISettings()->player2 = input[24];
    *getMapUISettings()->seen = input[25];
    selectedRound = input[26];
    turn = input[27];

    for (int i = 0; i < 8; ++i) {
        sprintf(rounds_file_name[i], "assets/round%d%s.png", i + 1, (i + 1) == selectedRound ? "s" : "");
        rounds[i].obj.refresh = true;
    }
    innerLoad();

    MapElement *elements = get_elements(0);
    for (int i = 0; i < 8; ++i)
        loadMapElement(&input2[i], &elements[i], i + 1);

    elements = get_elements(1);
    for (int i = 0; i < 6; ++i)
        loadMapElement(&input2[i + 8], &elements[i], -1);

    elements = get_elements(2);
    for (int i = 0; i < 2; ++i)
        loadMapElement(&input2[i + 14], &elements[i], -1);

    elements = get_elements(3);
    for (int i = 0; i < 4; ++i)
        loadMapElement(&input2[i + 16], &elements[i], -2);

    if (forZ)
        remove(MR_JACK_Z_DATA);
}

void selectInnerCard(int targetId) {
    if (targetId == -1)
        return;

    if (playerTurn == 1)
        saveTheGame(true);

    GUI_Image *element = NULL;
    for (int i = 0; i < 4; ++i) {
        if (c[i].obj.target == targetId) {
            element = &c[i];
            break;
        }
    }

    if (element == NULL)
        return;

    MRJACK_Character *target = findCharacter(targetId);
    element->file = target->selected;
    element->obj.refresh = true;
    select_character_map(target, detective);

    for (int i = 0; i < 4; ++i) {
        if (&c[i] != element && c[i].obj.target != -1) {
            MRJACK_Character *character = findCharacter(c[i].obj.target);
            if (c[i].file != character->character) {
                c[i].file = character->character;
                c[i].obj.refresh = true;
            }
        }
    }
    currentState = NORMAL;
}

void botApply() {
    bot_callback = false;
    if (playerTurn != 0 || game_over)
        return;

    if (get_selected_character() == NULL) {
        if (getMapUISettings()->detective) {
            selectInnerCard(c[bot_play_detective_find_best_character(c)].obj.target);
        } else {
            selectInnerCard(c[bot_play_jack_find_best_character(c)].obj.target);
        }
        bot_callback = true;
        time = currentTime() + 1000;
    } else {
        if (!has_moved()) {
            if (getMapUISettings()->detective) {
                if (bot_play_detective_move(selectedRound)) {
                    bot_callback = true;
                    time = currentTime() + 1000;
                }
            } else {
                bot_play_jack_move(selectedRound);
                bot_callback = true;
                time = currentTime() + 1000;
            }
        } else {
            if (getMapUISettings()->detective) {
                bot_play_detective_ability(selectedRound, true);
            } else {
                bot_play_jack_ability(selectedRound, true);
            }
        }
    }
}

void doneCharacter(int character) {
    if (game_over)
        return;

    if (turn == 3) {
        turn = 0;
    } else {
        turn++;
    }
    reset_selected_items();

    if (turn == 0) {
        playSound("assets/clock.wav", SDL_MIX_MAXVOLUME / 4);
        if (selectedRound == 8) {
            gameOver(1);
            return;
        }
        *getMapUISettings()->seen = isCharacterVisible(getMapUISettings()->jackId);

        MapElement *characters = get_elements(0);
        for (int i = 0; i < 8; ++i) {
            sprintf(rounds_file_name[i], "assets/round%d%s.png", i + 1, i == selectedRound ? "s" : "");
            rounds[i].obj.refresh = true;

            if (!characters[i].isSafe && isCharacterVisible(i + 1) != *getMapUISettings()->seen) {
                safe_character(i + 1);
            }
        }

        if (selectedRound >= 1 && selectedRound <= 4)
            disable_light(selectedRound);
        selectedRound++;

        for (int i = 0; i < 4; ++i) {
            MRJACK_Character *ch = pick_card();
            c[i].file = ch->character;
            c[i].obj.target = ch->id;
            c[i].obj.refresh = true;
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            if (c[i].obj.target == character) {
                c[i].file = findCharacter(c[i].obj.target)->disabled;
                c[i].obj.refresh = true;
                c[i].obj.target = -1;
                break;
            }
        }
    }

    innerLoad();
    saveTheGame(false);
}

void selectCard(const void *element) {
    if (playerTurn != 0 && (get_selected_character() == NULL || get_can_switch())) {
        int targetId = ((GUI_Image *) element)->obj.target;
        selectInnerCard(targetId);
    }
}

int start_map(int player1, int player2, int x, int y, int id, bool loadGame) {
    getMapUISettings()->player1 = player1;
    getMapUISettings()->player2 = player2;
    getMapUISettings()->jackId = id;
    getMapUISettings()->playerTurn = &playerTurn;
    game_over = false;
    selectedRound = 1;
    turn = 0;

    int quit = 0;
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
    SDL_Window *window = NULL;
    window = SDL_CreateWindow(PROJECT_NAME, x, y, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
            .file = "assets/map.jpg",
            .obj = {
                    .rect = {.x = SCREEN_HEIGHT / 4 * 267 / 411 + 4, .y = 0, .w = SCREEN_HEIGHT * 1802 /
                                                                                  1516, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&img, pnl);
    initializeMap(img.obj.rect.x);

    select_card.time = currentTime();
    select_card.obj.rect = img.obj.rect;

    GUI_Image round = {
            .file = "assets/round.png",
            .obj = {
                    .rect = {.x = img.obj.rect.x + img.obj.rect.w + 4, .y = 0, .w = SCREEN_HEIGHT * 155 /
                                                                                    1502, .h = SCREEN_HEIGHT},
                    .gravity = GRAVITY_FILL2
            },
    };
    insert_image2(&round, pnl);

    // Sum of rounds h: 1484
    int rh = (int) ((SCREEN_HEIGHT - 8) / 7.7);

    for (int i = 0; i < 8; ++i) {
        sprintf(rounds_file_name[i], "assets/round%d%s.png", i + 1, (i + 1) == selectedRound ? "s" : "");
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
        rounds[i].obj.refresh = true;
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
        c[i].obj.refresh = true;
        insert_image2(&c[i], pnl);
    }

    int x0 = round.obj.rect.x + round.obj.rect.w + 4;

    SDL_Rect players_rect = {.x = x0, .y = 10,
            .w = SCREEN_WIDTH - x0 - 4,
            .h = (SCREEN_WIDTH - x0 - 4) * 114 / 673};

    GUI_Image players[3] = {
            {
                    .file = "assets/bot.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "assets/player1.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2
                    },
            },
            {
                    .file = "assets/player2.png",
                    .obj = {
                            .rect = players_rect,
                            .gravity = GRAVITY_FILL2
                    },
            },
    };

    int seen = 1;
    getMapUISettings()->seen = &seen;
    SDL_Rect seen_rect = {.x = x0, .y = players_rect.y + players_rect.h + 10,
            .w = (SCREEN_WIDTH - x0 - 4) / 2,
            .h = (SCREEN_WIDTH - x0 - 4) / 2 * 983 / 700};
    GUI_Image seenVsUnseen[2] = {
            {
                    .file = "assets/unseen.png",
                    .obj = {
                            .rect = seen_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "assets/seen.png",
                    .obj = {
                            .rect = seen_rect,
                            .gravity = GRAVITY_FILL2
                    },
            }
    };

    GUI_Image redCard = {
            .file = "assets/red_card.png",
            .obj = {
                    .rect = {.x = seen_rect.x + seen_rect.w + 6, .y = seen_rect.y,
                            .w = (SCREEN_WIDTH - (seen_rect.x + seen_rect.w) - 20),
                            .h = (SCREEN_WIDTH - (seen_rect.x + seen_rect.w) - 20) * 247 / 157},
                    .gravity = GRAVITY_FILL2,
            },
    };

    GUI_FadeImage alibiCard = {
            .file = NULL,
            .obj = {
                    .rect = redCard.obj.rect,
                    .gravity = GRAVITY_FILL2,
                    .event = {false, false, false},
                    .clickListener = NULL
            },
            .time = 0,
            .duration = 500,
            .showDuration = 1000
    };
    getMapUISettings()->alibiCard = &alibiCard;
    insert_fade_image2(&alibiCard, pnl);

    int ability = 1;
    SDL_Rect ability_rect = {.x = x0 + 20,
            .y = SCREEN_HEIGHT - ((SCREEN_WIDTH - x0 - 44) * 135 / 350) - 10,
            .w = (SCREEN_WIDTH - x0 - 44),
            .h = (SCREEN_WIDTH - x0 - 44) * 135 / 350};
    GUI_Image ability_btn[3] = {
            {
                    .file = "assets/ability_0.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "assets/ability_1.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            },
            {
                    .file = "assets/ability_2.png",
                    .obj = {
                            .rect = ability_rect,
                            .gravity = GRAVITY_FILL2,
                    },
            }
    };

    int textTop = redCard.obj.rect.y + redCard.obj.rect.h + 20;
    GUI_Label title = {
            .font = TTF_OpenFont("assets/FreeSans.ttf", 16),
            .text = empty_string,
            .obj = {
                    .color = {.r = 242, .g = 255, .b = 0, .a = 255},
                    .rect = {.x = x0 + 10, .y = textTop, .w = SCREEN_WIDTH - x0 - 14, .h = 20},
                    .gravity = GRAVITY_LEFT | GRAVITY_TOP
            }
    };

    textTop += 25;
    GUI_Label hint = {
            .font = TTF_OpenFont("assets/FreeSans.ttf", 16),
            .text = empty_string,
            .obj = {
                    .color = {.r = 255, .g = 255, .b = 255, .a = 255},
                    .rect = {.x = title.obj.rect.x, .y = textTop, .w = title.obj.rect.w, .h = ability_rect.y - 8 -
                                                                                              textTop},
                    .gravity = GRAVITY_LEFT | GRAVITY_TOP
            }
    };

    getMapUISettings()->title = &title;
    getMapUISettings()->hint = &hint;
    getMapUISettings()->ability = &ability;
    getMapUISettings()->abilityBtn = ability_btn;
    select_character_map(NULL, false);

    playerTurn = gameplay_whosTurn(getMapUISettings()->player1,
                                   getMapUISettings()->player2,
                                   selectedRound = 1, turn = 0, &detective);
    currentState = SELECT_CARD;
    select_card.time = currentTime();
    if (loadGame)
        loadTheGame(false);

    if (playerTurn == 0)
        botApply();

    saveTheGame(false);
    if (player2 == 0 && player1 == 1)
        saveTheGame(true);

    bool playAgain = false, undo = false;
    select_card.obj.refresh = true;

    while (!quit) {
        // Why do we need this? :/
        getMapUISettings()->seen = &seen;
        getMapUISettings()->ability = &ability;

        SDL_Event evt; // no need for new/delete, stack is fine

        while (SDL_PollEvent(&evt)) {
            // quit on close, window close, or 'escape' key hit
            if (evt.type == SDL_QUIT ||
                (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
                (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }

            if (game_over && evt.type == SDL_KEYDOWN &&
                (evt.key.keysym.sym == SDLK_RETURN || evt.key.keysym.sym == SDLK_RETURN2)) {

                playAgain = true;
                quit = 1;
            }

            if (!game_over && (player2 == 0 && player1 != 0)
                && evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_z) {
                const Uint8 *keys = SDL_GetKeyboardState(NULL);
                if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]
                    || keys[SDL_SCANCODE_LGUI] || keys[SDL_SCANCODE_RGUI]) {
                    FILE *file = fopen(MR_JACK_Z_DATA, "rb");
                    if (file != NULL) {
                        undo = true;
                        fclose(file);
                        break;
                    }
                }
            }

            if (playerTurn != 0 && !game_over) {
                if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
                    clicking = true;
                    click_x1 = evt.button.x;
                    click_y1 = evt.button.y;
                } else if (evt.type == SDL_MOUSEBUTTONUP && clicking) {
                    clicking = false;
                    if (get_selected_character() != NULL)
                        get_selected_character()->click(click_x1, click_y1, evt.button.x, evt.button.y);
                }

                if (get_selected_character() != NULL && get_selected_character()->event != NULL)
                    get_selected_character()->event(&evt);

                event_for_all2(&evt, pnl->childs);
                event(&ability_btn[ability].obj, &evt);
            }
        }

        if (undo) {
            undo = false;
            loadTheGame(true);
            continue;
        }

        if (bot_callback) {
            if (time <= currentTime()) {
                botApply();
            }
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

        if (currentState == SELECT_CARD && playerTurn != 0)
            render_fadein_image(&select_card, renderer);

        if (game_over)
            render_fadein_image(&game_over_image, renderer);

        SDL_RenderPresent(renderer);
    }

    remove(MR_JACK_Z_DATA);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    return playAgain ? start() : 0;
}