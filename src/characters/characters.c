//
// Created by AmirHossein Aghajari on 1/3/22.
//

#include "characters.h"

MRJACK_Character c1 = {
        .id = 1,
        .name = "Inspecteur Lestrade",
        .character = "c1.png",
        .circle = "m1.png",
        .innocent = "i1.png",
};

MRJACK_Character c2 = {
        .id = 2,
        .name = "Miss Stealthy",
        .character = "c2.png",
        .circle = "m2.png",
        .innocent = "i2.png",
};

MRJACK_Character c3 = {
        .id = 3,
        .name = "Jermy Bert",
        .character = "c3.png",
        .circle = "m3.png",
        .innocent = "i3.png",
};

MRJACK_Character c4 = {
        .id = 4,
        .name = "John H. Watson",
        .character = "c4.png",
        .circle = "m4.png",
        .innocent = "i4.png",
};

MRJACK_Character c5 = {
        .id = 5,
        .name = "Sherlock Holmes",
        .character = "c5.png",
        .circle = "m5.png",
        .innocent = "i5.png",
};

MRJACK_Character c6 = {
        .id = 6,
        .name = "John Smith",
        .character = "c6.png",
        .circle = "m6.png",
        .innocent = "i6.png",
};

MRJACK_Character c7 = {
        .id = 7,
        .name = "Sergent Goodley",
        .character = "c7.png",
        .circle = "m7.png",
        .innocent = "i7.png",
};

MRJACK_Character c8 = {
        .id = 8,
        .name = "Sir William Gull",
        .character = "c8.png",
        .circle = "m8.png",
        .innocent = "i8.png",
};

MRJACK_Character findCharacter(int id) {
    switch (id) {
        case 1:
            return c1;
        case 2:
            return c2;
        case 3:
            return c3;
        case 4:
            return c4;
        case 5:
            return c5;
        case 6:
            return c6;
        case 7:
            return c7;
        default:
            return c8;
    }
}