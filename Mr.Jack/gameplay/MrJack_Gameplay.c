//
// Created by AmirHossein Aghajari on 1/6/22.
//

#include <stdbool.h>
#include "../graphic/GUI_init.h"
#include "../graphic/MrJack_Map.h"
#include "MrJack_Gameplay.h"
#include "../linked_list/linked_list.h"

int gameplay_whosTurn(int player1, int player2, int round, int turn, bool *detective) {
    int side = (round % 2) ? 1 : -1;

    if (turn == 1 || turn == 2) {
        if (side == 1) {
            side = -1;
        } else {
            side = 1;
        }
    }
    *detective = side == 1;
    getMapUISettings()->detective = *detective;

    if (player1 == side) {
        if (player2 == side) {
            return turn <= 1 ? 1 : 2;
        } else {
            return 1;
        }
    } else if (player2 == side) {
        return 2;
    } else {
        return 0;
    }
}

struct QueueItem {
    int parent, value;
    bool visited;
};

int bfs(int start, int target, int *out, bool tunnel, bool home) {
    // enqueue => append
    // dequeue => pop

    int len;
    struct QueueItem board[len = get_whitechapel_len()];
    for (int i = 0; i < len; ++i) {
        board[i].parent = -1;
        board[i].value = i;
        board[i].visited = false;
    }

    struct LinkedListNode *queue = linked_list_create();

    board[start].visited = true;
    linked_list_append(linked_list_new(&board[start]), queue);
    int size = 1;

    bool findPath = false;

    while (size > 0 && !findPath) {
        struct QueueItem *item = linked_list_findByIndex(queue, 1)->data;
        linked_list_removeAt(queue, 1);
        size--;
        Whitechapel *whitechapel = get_whitechapel(item->value);

        for (int i = 0; i < 6; ++i) {
            int w = whitechapel->connectedTo[i];
            if (w != -1 && !board[w].visited) {
                Whitechapel *next = get_whitechapel(w);
                if (home || (!next->isHome && !next->isLight)) {
                    board[w].parent = item->value;
                    board[w].visited = true;
                    if (w == target) {
                        findPath = true;
                        break;
                    }
                    linked_list_append(linked_list_new(&board[w]), queue);
                    size++;
                }
            }
        }

        if (tunnel && !findPath) {
            if (whitechapel->isTunnel && find_block_selected(0) != item->value &&
                find_block_selected(1) != item->value) {
                for (int i = 0; i < get_whitechapel_len(); ++i) {
                    if (get_whitechapel(i)->isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                        if (!board[i].visited) {
                            Whitechapel *next = get_whitechapel(i);
                            if (home || (!next->isHome && !next->isLight)) {
                                board[i].parent = item->value;
                                board[i].visited = true;
                                if (i == target) {
                                    findPath = true;
                                    break;
                                }
                                linked_list_append(linked_list_new(&board[i]), queue);
                                size++;
                            }
                        }
                    }
                }
            }
        }
    }

    if (findPath) {
        len = 0;
        int w = board[target].parent;
        while (w != start) {
            out[len++] = w;
            w = board[w].parent;
        }
    } else {
        len = -1;
    }

    linked_list_freeDataEnabled(false);
    while (queue->next != NULL)
        linked_list_pop(queue);
    free(queue);
    linked_list_freeDataEnabled(true);

    return len;
}

struct Strategy {
    int characters[8], lights[6];
};

int bot_play_detective_find_best_character(GUI_Image c[4]) {
    int visibleCount = 0, invisibleCount = 0;

    MapElement *characters = get_elements(0);
    bool visibility[8];
    for (int i = 0; i < 8; ++i) {
        if (!characters[i].isSafe) {
            if ((visibility[i] = isCharacterVisible(i + 1)))
                visibleCount++;
            else
                invisibleCount++;
        }
    }

    if (visibleCount + invisibleCount == 1) {
        int out[get_whitechapel_len()];
        int min = 100, target = -1;

        for (int i = 0; i < 4; ++i) {
            if (c[i].obj.target != -1) {
                if (c[i].obj.target == getMapUISettings()->jackId)
                    continue;

                int len = bfs(getFinalCharacterSelectedIndex(c[i].obj.target),
                              getFinalCharacterSelectedIndex(getMapUISettings()->jackId), out, true,
                              c[i].obj.target == 2);
                if (target == -1 || min > len) {
                    target = i;
                    min = len;
                }
            }
        }

        if (min <= 3 || (min == 4 && c[target].obj.target == 2)
            || (min <= 6 && c[target].obj.target == 7)) {
            return target;
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            if (c[i].obj.target == 6) {
                return i;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if (c[i].obj.target == 7) {
                return i;
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (c[i].obj.target != -1) {
            return i;
        }
    }

    return -1;
}

int sti = 0;

void fill_strategy(int index, int move, bool fromHome, struct Strategy *strategies, int *visited) {
    Whitechapel *whitechapel = get_whitechapel(index);

    if (whitechapel->isHome || whitechapel->isLight) {
        if (fromHome && move > 0) {
            for (int i = 0; i < 6; ++i) {
                if (whitechapel->connectedTo[i] != -1)
                    fill_strategy(whitechapel->connectedTo[i], move - 1, fromHome, strategies, visited);
            }
        }
        return;
    }

    if (!visited[index] && !isFilledWithCharacter(index, 0)) {
        strategies[sti++].characters[get_selected_character()->id - 1] = index;
        visited[index] = 1;
    }

    if (move > 0) {
        for (int i = 0; i < 6; ++i) {
            if (whitechapel->connectedTo[i] != -1)
                fill_strategy(whitechapel->connectedTo[i], move - 1, fromHome, strategies, visited);
        }

        if (whitechapel->isTunnel && find_block_selected(0) != index && find_block_selected(1) != index) {
            for (int i = 0; i < get_whitechapel_len(); ++i) {
                if (get_whitechapel(i)->isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                    fill_strategy(i, move - 1, fromHome, strategies, visited);
                }
            }
        }
    }
}

void find_strategy(int move, bool fromHome, struct Strategy *strategies) {
    int index = getFinalCharacterSelectedIndex(get_selected_character()->id);
    Whitechapel *whitechapel = get_whitechapel(index);
    sti = 0;
    int visited[get_whitechapel_len()];
    for (int i = 0; i < get_whitechapel_len(); ++i) {
        visited[i] = 0;
    }

    for (int i = 0; i < 6; ++i) {
        if (whitechapel->connectedTo[i] != -1)
            fill_strategy(whitechapel->connectedTo[i], move - 1, fromHome, strategies, visited);
    }

    if (whitechapel->isTunnel && find_block_selected(0) != index && find_block_selected(1) != index) {
        for (int i = 0; i < get_whitechapel_len(); ++i) {
            if (get_whitechapel(i)->isTunnel && find_block_selected(0) != i && find_block_selected(1) != i) {
                fill_strategy(i, move - 1, fromHome, strategies, visited);
            }
        }
    }
}

bool isCharacterVisibleForJohn(int character, int rotation) {
    int selected = getFinalCharacterSelectedIndex(character);

    if (character != 4) {
        MapElement *johnWatson = getCharacterElement(4);
        return isVisibleForWatson(rotation, getFinalElementSelectedIndex(johnWatson), selected);
    }
    return false;
}

bool isCharacterVisibleInStrategy(int character, struct Strategy *strategy) {
    int selected;
    Whitechapel *target = get_whitechapel(selected = strategy->characters[character - 1]);
    for (int i = 0; i < 6; ++i) {
        int w = target->connectedTo[i];
        if (w != -1) {
            if (get_whitechapel(w)->isLight) {
                for (int j = 0; j < 6; ++j) {
                    if (strategy->lights[j] == w)
                        return true;
                }
            } else {
                for (int j = 0; j < 8; ++j) {
                    if (j == character - 1)
                        continue;

                    if (strategy->characters[j] == w)
                        return true;
                }
            }
        }
    }

    if (character != 4) {
        MapElement *johnWatson = getCharacterElement(4);
        return isVisibleForWatson(johnWatson->rotation, getFinalElementSelectedIndex(johnWatson), selected);
    }
    return false;
}

void fill_def_strategy(int len, struct Strategy *strategy) {
    MapElement *elements = get_elements(0);
    for (int j = 0; j < 8; ++j) {
        strategy[0].characters[j] = getFinalElementSelectedIndex(&elements[j]);
    }
    elements = get_elements(1);
    for (int j = 0; j < 6; ++j) {
        strategy[0].lights[j] = elements[j].visible ? getFinalElementSelectedIndex(&elements[j]) : -1;
    }
    for (int i = 1; i < len; ++i) {
        for (int j = 0; j < 8; ++j) {
            strategy[i].characters[j] = strategy[0].characters[j];
        }
        for (int j = 0; j < 6; ++j) {
            strategy[i].lights[j] = strategy[0].lights[j];
        }
    }
}

bool bot_play_detective_move(int round) {
    int visibleCount = 0, invisibleCount = 0;

    MapElement *characters = get_elements(0);
    bool visibility[8];
    for (int i = 0; i < 8; ++i) {
        if (!characters[i].isSafe) {
            if ((visibility[i] = isCharacterVisible(i + 1)))
                visibleCount++;
            else
                invisibleCount++;
        }
    }

    bool visibleImportant = !(*getMapUISettings()->seen) || round == 1;
    bool output = true;

    if (visibleCount + invisibleCount <= 1 &&
        get_selected_character()->id != getMapUISettings()->jackId) {
        if (get_selected_character()->id == 7) {
            bot_play_detective_ability(round, false);
            output = false;
        }

        int target = getFinalCharacterSelectedIndex(getMapUISettings()->jackId);
        int start = getFinalCharacterSelectedIndex(get_selected_character()->id);
        int out[100];
        int moves = get_selected_character()->id == 2 ? 4 : 3;
        int len = bfs(start, target, out, true, false);
        out[len++] = target;
        if (len > 0) {
            MapElement *element = getCharacterElement(get_selected_character()->id);
            int size = 0;
            for (int j = len - 1; j >= 0; j--) {
                element->move[size] = out[j];
                if (out[j] == target) {
                    break;
                }
                size++;
                if (size == moves) {
                    size -= fixMoveWithNoCharacter(get_selected_character()->id);
                    break;
                }
            }
            int t = start;
            for (int i = 0; i < 5; ++i) {
                if (element->move[i] != -1) {
                    t = element->move[i];
                    element->move[i] = -1;
                }
            }
            if (moveCharacter2(t, moves, get_selected_character()->id == 2))
                return output;
        }
    }
    if (round == 8 && visibleCount + invisibleCount == 2) {
        int guessJackId = getMapUISettings()->jackId;
        if (rand() % 2) {
            for (int i = 1; i <= 8; ++i) {
                if (i != getMapUISettings()->jackId && !getCharacterElement(i)->isSafe) {
                    guessJackId = i;
                    break;
                }
            }
        }
        if (get_selected_character()->id == 7) {
            bot_play_detective_ability(round, false);
            output = false;
        }

        int target = getFinalCharacterSelectedIndex(guessJackId);
        int start = getFinalCharacterSelectedIndex(get_selected_character()->id);
        int out[100];
        int moves = get_selected_character()->id == 2 ? 4 : 3;
        int len = bfs(start, target, out, true, false);
        out[len++] = target;
        if (len > 0) {
            MapElement *element = getCharacterElement(get_selected_character()->id);
            int size = 0;
            for (int j = len - 1; j >= 0; j--) {
                element->move[size] = out[j];
                if (out[j] == target) {
                    break;
                }
                size++;
                if (size == moves) {
                    size -= fixMoveWithNoCharacter(get_selected_character()->id);
                    break;
                }
            }
            int t = start;
            for (int i = 0; i < 5; ++i) {
                if (element->move[i] != -1) {
                    t = element->move[i];
                    element->move[i] = -1;
                }
            }
            if (moveCharacter2(t, moves, get_selected_character()->id == 2))
                return output;
        }
    }

    int len = get_whitechapel_len();
    struct Strategy strategy[len];
    fill_def_strategy(len, strategy);

    MRJACK_Character *selected = get_selected_character();
    find_strategy(selected->id == 2 ? 4 : 3, selected->id == 2, strategy);

    int target = -1;
    int value = 0;
    for (int i = 0; i < len; ++i) {
        if (strategy[i].characters[selected->id - 1] == getFinalCharacterSelectedIndex(selected->id))
            continue;

        int mv = 0;
        for (int j = 1; j <= 8; ++j) {
            if (getCharacterElement(j)->isSafe)
                continue;
            mv += isCharacterVisibleInStrategy(j, &strategy[i]) == visibleImportant;
        }

        if (target == -1 || mv > value) {
            value = mv;
            target = i;
        }
    }

    int index = strategy[target].characters[selected->id - 1];
    moveCharacter2(index, selected->id == 2 ? 4 : 3, selected->id == 2);
    if (!output)
        doneCharacter(7);
    return output;
}

void bot_play_detective_ability(int round, bool done) {
    bool visibleImportant = !(*getMapUISettings()->seen) || round == 1;

    MRJACK_Character *selected = get_selected_character();
    switch (selected->id) {
        case 1: {
            int goodWays[4] = {6, 74, 11, 79};
            MapElement *elements = get_elements(3);
            int target = -1;
            for (int i = 0; i < 4; ++i) {
                if (!elements[i].visible
                    && find_block_selected(0) != goodWays[i]
                    && find_block_selected(1) != goodWays[i]) {
                    target = i;
                    break;
                }
            }
            if (target == -1) {
                for (int i = 0; i < 4; ++i) {
                    if (!elements[i].visible) {
                        target = i;
                        break;
                    }
                }
            }

            int w = -1;
            for (int i = 0; i < 4; ++i) {
                if (elements[i].visible
                    && (find_block_selected(0) == goodWays[i]
                        || find_block_selected(1) == goodWays[i])) {
                    w = i;
                    break;
                }
            }
            if (w == -1) {
                for (int i = 0; i < 4; ++i) {
                    if (elements[i].visible) {
                        w = i;
                        break;
                    }
                }
            }

            elements[target].visible = true;
            elements[w].visible = false;
            if (done)
                doneCharacter(1);
            break;
        }
        case 2:
            break;
        case 3: {
            int goodWays[4] = {6, 74, 11, 79};
            MapElement *elements = get_elements(3);

            int target = 0;
            for (int i = 0; i < 4; ++i) {
                if (find_block_selected(0) == goodWays[i]) {
                    target = -1;
                    break;
                }
            }
            if (target == -1) {
                target = 1;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(1) == goodWays[i]) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 0;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(0) == goodWays[i] && !elements[i].visible) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 1;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(1) == goodWays[i] && !elements[i].visible) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 0;
            }

            int w = -1;
            for (int i = 0; i < 4; ++i) {
                if (!elements[i].visible
                    && find_block_selected(0) != goodWays[i]
                    && find_block_selected(1) != goodWays[i]) {
                    w = goodWays[i];
                    break;
                }
            }
            if (w == -1 || (find_block_selected(0) == w || find_block_selected(1) == w)) {
                int i = 0;
                do {
                    w = goodWays[i++];
                } while (find_block_selected(0) == w || find_block_selected(1) == w);
            }
            MapElement *blocks = get_elements(2);
            blocks[target].move[0] = w;
            blocks[target].move[1] = -1;
            blocks[target].time = currentTime();
            if (done)
                doneCharacter(3);
            break;
        }
        case 4: {
            int rotations[] = {-90, -270, -150, -210, -30, -330};
            int target = -1;
            int value = 0;
            for (int i = 0; i < 6; ++i) {
                if (rotations[i] == getCharacterElement(4)->rotation)
                    continue;

                int mv = 0;
                for (int j = 1; j <= 8; ++j) {
                    if (getCharacterElement(j)->isSafe)
                        continue;
                    mv += isCharacterVisibleForJohn(j, rotations[i]) == visibleImportant;
                }

                if (target == -1 || mv >= value) {
                    value = mv;
                    target = i;
                }
            }

            getCharacterElement(4)->rotation = rotations[target];
            if (done)
                doneCharacter(4);
            break;
        }
        case 5: {
            safe_character(pick_character()->id);

            if (done)
                doneCharacter(5);
            break;
        }
        case 6: {
            int lights = 0;
            MapElement *elements = get_elements(1);
            for (int i = 0; i < 6; ++i)
                lights += elements[i].visible;
            int len = lights * (8 - lights);
            struct Strategy strategy[len];
            fill_def_strategy(len, strategy);

            int i = 0;
            for (int k = 0; k < 6; ++k) {
                if (!elements[k].visible)
                    continue;

                for (int j = 1; j <= (8 - lights); ++j) {
                    int rich = 0, l;
                    for (l = 0; l < get_whitechapel_len(); ++l) {
                        if (get_whitechapel(l)->isLight) {
                            bool isFilled = false;
                            for (int m = 0; m < 6; ++m) {
                                if (getFinalElementSelectedIndex(&elements[m]) == l) {
                                    isFilled = true;
                                    break;
                                }
                            }
                            if (!isFilled)
                                rich++;
                            if (rich == j)
                                break;
                        }
                    }
                    strategy[i++].lights[k] = l;
                }
            }
            len = i;

            int target = -1;
            int value = 0;
            for (i = 0; i < len; ++i) {
                int mv = 0;
                for (int j = 1; j <= 8; ++j) {
                    if (getCharacterElement(j)->isSafe)
                        continue;
                    mv += isCharacterVisibleInStrategy(j, &strategy[i]) == visibleImportant;
                }

                if (target == -1 || mv > value) {
                    value = mv;
                    target = i;
                }
            }

            for (int j = 0; j < 6; ++j) {
                if (elements[j].visible && getFinalElementSelectedIndex(&elements[j]) != strategy[target].lights[j]) {
                    elements[j].move[0] = strategy[target].lights[j];
                    elements[j].move[1] = -1;
                    elements[j].time = currentTime();
                }
            }

            if (done)
                doneCharacter(6);
            break;
        }
        case 7: {
            int target = getFinalCharacterSelectedIndex(7);
            int out[100];
            int moves = 3;
            if (getMapUISettings()->jackId != 7) {
                int len = bfs(getFinalCharacterSelectedIndex(getMapUISettings()->jackId), target, out, false, false);
                if (len > 0) {
                    int size = 0;
                    for (int j = len - 1; j >= 0; j--) {
                        getCharacterElement(getMapUISettings()->jackId)->move[size] = out[j];
                        size++;
                        if (size == moves)
                            break;
                    }
                    size -= fixMoveWithNoCharacter(getMapUISettings()->jackId);
                    getCharacterElement(getMapUISettings()->jackId)->move[size] = -1;
                    getCharacterElement(getMapUISettings()->jackId)->time = currentTime();
                    getCharacterElement(getMapUISettings()->jackId)->move_len = size;
                    moves -= size;
                }
            }
            if (moves > 0) {
                for (int i = 0; i < 8; ++i) {
                    if (moves <= 0)
                        break;
                    if (i == 6 || getCharacterElement(i + 1)->isSafe)
                        continue;

                    int len = bfs(getFinalCharacterSelectedIndex(i + 1), target, out, false, false);
                    if (len > 0) {
                        int size = 0;
                        for (int j = len - 1; j >= 0; j--) {
                            getCharacterElement(i + 1)->move[size] = out[j];
                            size++;
                            if (size == moves)
                                break;
                        }
                        size -= fixMoveWithNoCharacter(i + 1);
                        getCharacterElement(i + 1)->move[size] = -1;
                        getCharacterElement(i + 1)->time = currentTime();
                        getCharacterElement(i + 1)->move_len = size;
                        moves -= size;
                    }
                }
            }
            if (moves > 0) {
                for (int i = 0; i < 8; ++i) {
                    if (moves <= 0)
                        break;
                    if (i == 6)
                        continue;

                    int len = bfs(getFinalCharacterSelectedIndex(i + 1), target, out, false, false);
                    if (len > 0) {
                        int size = 0;
                        for (int j = len - 1; j >= 0; j--) {
                            getCharacterElement(i + 1)->move[size] = out[j];
                            size++;
                            if (size == moves)
                                break;
                        }
                        size -= fixMoveWithNoCharacter(i + 1);
                        getCharacterElement(i + 1)->move[size] = -1;
                        getCharacterElement(i + 1)->time = currentTime();
                        getCharacterElement(i + 1)->move_len = size;
                        moves -= size;
                    }
                }
            }
            if (done)
                doneCharacter(7);
            break;
        }
        default:
            break;
    }
}

int bot_play_jack_find_best_character(GUI_Image c[4]) {
    if (can_jack_leave_the_district()) {
        for (int i = 0; i < 4; ++i) {
            if (c[i].obj.target == getMapUISettings()->jackId) {
                return i;
            }
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (c[i].obj.target == 6) {
            return i;
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (c[i].obj.target == 2) {
            return i;
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (c[i].obj.target != -1) {
            return i;
        }
    }

    return -1;
}

void bot_play_jack_move(int round) {
    if (get_selected_character()->id == getMapUISettings()->jackId) {
        int id = can_jack_leave_the_district();
        if (id) {
            int move = getMapUISettings()->jackId == 2 ? 4 : 3;
            bool fromHome = getMapUISettings()->jackId == 2 ? true : false;
            switch (id) {
                case 1:
                    moveCharacter(130, 30, 130, 30, move, fromHome);
                    return;
                case 2:
                    moveCharacter(810, 30, 810, 30, move, fromHome);
                    return;
                case 3:
                    moveCharacter(135, 630, 135, 630, move, fromHome);
                    return;
                case 4:
                    moveCharacter(820, 630, 820, 630, move, fromHome);
                    return;
                default:
                    break;
            }
        }
    }
    bool visibleImportant = !(!(*getMapUISettings()->seen) || round == 1);

    int len = get_whitechapel_len();
    struct Strategy strategy[len];
    fill_def_strategy(len, strategy);

    MRJACK_Character *selected = get_selected_character();
    find_strategy(selected->id == 2 ? 4 : 3, selected->id == 2, strategy);

    int target = -1;
    int value = 0;
    for (int i = 0; i < len; ++i) {
        if (strategy[i].characters[selected->id - 1] == getFinalCharacterSelectedIndex(selected->id))
            continue;

        int mv = 0;
        for (int j = 1; j <= 8; ++j) {
            if (getCharacterElement(j)->isSafe)
                continue;
            mv += isCharacterVisibleInStrategy(j, &strategy[i]) == visibleImportant;
        }

        if (target == -1 || mv >= value) {
            value = mv;
            target = i;
        }
    }
    if (target == -1)
        target = 0;

    int index = strategy[target].characters[selected->id - 1];
    moveCharacter2(index, selected->id == 2 ? 4 : 3, selected->id == 2);
}

void bot_play_jack_ability(int round, bool done) {
    bool visibleImportant = !(!(*getMapUISettings()->seen) || round == 1);

    MRJACK_Character *selected = get_selected_character();
    switch (selected->id) {
        case 1: {
            int goodWays[4] = {6, 74, 11, 79};
            MapElement *elements = get_elements(3);
            int target = -1;
            for (int i = 0; i < 4; ++i) {
                if (!elements[i].visible
                    && find_block_selected(0) != goodWays[i]
                    && find_block_selected(1) != goodWays[i]) {
                    target = i;
                    break;
                }
            }
            if (target == -1) {
                for (int i = 0; i < 4; ++i) {
                    if (!elements[i].visible) {
                        target = i;
                        break;
                    }
                }
            }

            int w = -1;
            for (int i = 0; i < 4; ++i) {
                if (elements[i].visible
                    && (find_block_selected(0) == goodWays[i]
                        || find_block_selected(1) == goodWays[i])) {
                    w = i;
                    break;
                }
            }
            if (w == -1) {
                for (int i = 0; i < 4; ++i) {
                    if (elements[i].visible) {
                        w = i;
                        break;
                    }
                }
            }

            elements[target].visible = true;
            elements[w].visible = false;
            if (done)
                doneCharacter(1);
            break;
        }
        case 2:
            break;
        case 3: {
            int goodWays[4] = {6, 74, 11, 79};
            MapElement *elements = get_elements(3);

            int target = 0;
            for (int i = 0; i < 4; ++i) {
                if (find_block_selected(0) == goodWays[i]) {
                    target = -1;
                    break;
                }
            }
            if (target == -1) {
                target = 1;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(1) == goodWays[i]) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 0;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(0) == goodWays[i] && !elements[i].visible) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 1;
                for (int i = 0; i < 4; ++i) {
                    if (find_block_selected(1) == goodWays[i] && !elements[i].visible) {
                        target = -1;
                        break;
                    }
                }
            }
            if (target == -1) {
                target = 0;
            }

            int w = -1;
            for (int i = 0; i < 4; ++i) {
                if (!elements[i].visible
                    && find_block_selected(0) != goodWays[i]
                    && find_block_selected(1) != goodWays[i]) {
                    w = goodWays[i];
                    break;
                }
            }
            if (w == -1 || (find_block_selected(0) == w || find_block_selected(1) == w)) {
                int i = 0;
                do {
                    w = goodWays[i++];
                } while (find_block_selected(0) == w || find_block_selected(1) == w);
            }
            MapElement *blocks = get_elements(2);
            blocks[target].move[0] = w;
            blocks[target].move[1] = -1;
            blocks[target].time = currentTime();
            if (done)
                doneCharacter(3);
            break;
        }
        case 4: {
            int rotations[] = {-30, -90, -150, -210, -270, -330};
            int target = -1;
            int value = 0;
            for (int i = 0; i < 6; ++i) {
                if (rotations[i] == getCharacterElement(4)->rotation)
                    continue;

                int mv = 0;
                for (int j = 1; j <= 8; ++j) {
                    if (getCharacterElement(j)->isSafe)
                        continue;
                    mv += isCharacterVisibleForJohn(j, rotations[i]) == visibleImportant;
                }

                if (target == -1 || mv >= value) {
                    value = mv;
                    target = i;
                }
            }

            getCharacterElement(4)->rotation = rotations[target];
            if (done)
                doneCharacter(4);
            break;
        }
        case 5: {
            pick_character();
            if (done)
                doneCharacter(5);
            break;
        }
        case 6: {
            int lights = 0;
            MapElement *elements = get_elements(1);
            for (int i = 0; i < 6; ++i)
                lights += elements[i].visible;
            int len = lights * (8 - lights);
            struct Strategy strategy[len];
            fill_def_strategy(len, strategy);

            int i = 0;
            for (int k = 0; k < 6; ++k) {
                if (!elements[k].visible)
                    continue;

                for (int j = 1; j <= (8 - lights); ++j) {
                    int rich = 0, l;
                    for (l = 0; l < get_whitechapel_len(); ++l) {
                        if (get_whitechapel(l)->isLight) {
                            bool isFilled = false;
                            for (int m = 0; m < 6; ++m) {
                                if (getFinalElementSelectedIndex(&elements[m]) == l) {
                                    isFilled = true;
                                    break;
                                }
                            }
                            if (!isFilled)
                                rich++;
                            if (rich == j)
                                break;
                        }
                    }
                    strategy[i++].lights[k] = l;
                }
            }
            len = i;

            int target = -1;
            int value = 0;
            for (i = 0; i < len; ++i) {
                int mv = 0;
                for (int j = 1; j <= 8; ++j) {
                    if (getCharacterElement(j)->isSafe)
                        continue;
                    mv += isCharacterVisibleInStrategy(j, &strategy[i]) == visibleImportant;
                }

                if (target == -1 || mv > value) {
                    value = mv;
                    target = i;
                }
            }

            for (int j = 0; j < 6; ++j) {
                if (elements[j].visible && getFinalElementSelectedIndex(&elements[j]) != strategy[target].lights[j]) {
                    elements[j].move[0] = strategy[target].lights[j];
                    elements[j].move[1] = -1;
                    elements[j].time = currentTime();
                }
            }

            if (done)
                doneCharacter(6);
            break;
        }
        case 7: {
            int target = getFinalCharacterSelectedIndex(7);
            int out[100];
            int moves = 3;
            if (moves > 0) {
                for (int i = 0; i < 8; ++i) {
                    if (moves <= 0)
                        break;
                    if (i == 6 || !getCharacterElement(i + 1)->isSafe || getMapUISettings()->jackId == i + 1)
                        continue;

                    int len = bfs(getFinalCharacterSelectedIndex(i + 1), target, out, false, false);
                    if (len > 0) {
                        int size = 0;
                        for (int j = len - 1; j >= 0; j--) {
                            getCharacterElement(i + 1)->move[size] = out[j];
                            size++;
                            if (size == moves)
                                break;
                        }
                        size -= fixMoveWithNoCharacter(i + 1);
                        getCharacterElement(i + 1)->move[size] = -1;
                        getCharacterElement(i + 1)->time = currentTime();
                        getCharacterElement(i + 1)->move_len = size;
                        moves -= size;
                    }
                }
            }
            if (moves > 0) {
                for (int i = 0; i < 8; ++i) {
                    if (moves <= 0)
                        break;
                    if (i == 6)
                        continue;

                    int len = bfs(getFinalCharacterSelectedIndex(i + 1), target, out, false, false);
                    if (len > 0) {
                        int size = 0;
                        for (int j = len - 1; j >= 0; j--) {
                            getCharacterElement(i + 1)->move[size] = out[j];
                            size++;
                            if (size == moves)
                                break;
                        }
                        size -= fixMoveWithNoCharacter(i + 1);
                        getCharacterElement(i + 1)->move[size] = -1;
                        getCharacterElement(i + 1)->time = currentTime();
                        getCharacterElement(i + 1)->move_len = size;
                        moves -= size;
                    }
                }
            }
            if (done)
                doneCharacter(7);
            break;
        }
        default:
            break;
    }
}
