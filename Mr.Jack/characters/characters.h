//
// Created by AmirHossein Aghajari on 1/3/22.
//

#include <stdbool.h>

#ifndef CHARACTERS_H
#define CHARACTERS_H

#define CHARACTERS_COUNT 8

typedef struct {
    int id;
    char *_Nonnull name, *_Nonnull character, *_Nonnull circle;
    char *_Nonnull circleSafe, *_Nonnull innocent, *_Nonnull selected;
    char *_Nonnull disabled, *_Nonnull desc;

    void (*_Nonnull click)(int, int, int, int);
    void (*_Nonnull init)(bool);
    void (*_Nullable renderer)(void *_Nullable);
    void (*_Nullable event)(void *_Nullable);
    void (*_Nonnull move)(int);
} MRJACK_Character;

MRJACK_Character *_Nonnull findCharacter(int id);

void initialize_characters();

MRJACK_Character *_Nonnull pick_character();

MRJACK_Character *_Nonnull pick_card();

void doneCharacter(int character);

void saveCharactersData(int *_Nonnull output);

void loadCharactersData(int *_Nonnull output);

#endif //CHARACTERS_H
