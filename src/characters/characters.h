//
// Created by AmirHossein Aghajari on 1/3/22.
//

#ifndef CHARACTERS_H
#define CHARACTERS_H

#define CHARACTERS_COUNT 8

typedef struct {
    int id;
    char *name, *character, *circle, *innocent;
} MRJACK_Character;

MRJACK_Character findCharacter(int id);

#endif //CHARACTERS_H
