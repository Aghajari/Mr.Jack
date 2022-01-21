//
// Created by AmirHossein Aghajari on 1/3/22.
//

#include <stdbool.h>

#ifndef CHARACTERS_H
#define CHARACTERS_H

#define CHARACTERS_COUNT 8

typedef struct {
    int id;
    char *name, *character, *circle, *innocent, *selected, *desc;
    void (* _Nonnull click)(int, int, int, int);
    void (* _Nonnull init)(bool);
} MRJACK_Character;

MRJACK_Character *findCharacter(int id);
void initialize_characters();
MRJACK_Character *pick_character();
MRJACK_Character *pick_card();


#endif //CHARACTERS_H
