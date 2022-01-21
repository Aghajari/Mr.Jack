//
// Created by AmirHossein Aghajari on 1/3/22.
//

#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "characters.h"
#include "inner_characters.h"
#include "../linked_list/linked_list.h"

MRJACK_Character c1 = {
        .id = 1,
        .name = "Inspecteur Lestrade",
        .desc = "This very good sleuth from Scotland Yard was sent to put an end to Jack's crimes.\n\n"
                "Special ability (MANDATORY): Move one police cordon. This will free one of the exits but block another!! This ability can be used before or after the movement, as the player sees fit.",
        .character = "c1.png",
        .selected = "cs1.png",
        .circle = "m1.png",
        .innocent = "i1.png",
        .click = InspecteurLestrade_click,
        .init = InspecteurLestrade_initialize
};

MRJACK_Character c2 = {
        .id = 2,
        .name = "Miss Stealthy",
        .desc = "Women are frequently abused in this district and Miss Stealthy is one of the very first fighting to improve the lot of her kind.\n\n"
                "Special ability (OPTIONAL): During her movement, Miss Stealthy can cross any hex (building, 4 gaslight, garden) but she must stop her movement on a street hex.",
        .character = "c2.png",
        .selected = "cs2.png",
        .circle = "m2.png",
        .innocent = "i2.png",
        .click = MissStealthy_click,
        .init = MissStealthy_initialize

};

MRJACK_Character c3 = {
        .id = 3,
        .name = "Jermy Bert",
        .desc = "Reporter for the STAR, he invented the name \"Jack\".\n\n"
                "Special ability (MANDATORY): Journalists like to search where it stinks... Jeremy Bert opens a manhole and closes another (move one covered manhole tile on any other open manhole hex.)\n"
                "This ability can be used before or after the movement, as the player sees fit.",
        .character = "c3.png",
        .selected = "cs3.png",
        .circle = "m3.png",
        .innocent = "i3.png",
        .click = JermyBert_click,
        .init = JermyBert_initialize
};

MRJACK_Character c4 = {
        .id = 4,
        .name = "John H. Watson",
        .desc = "Doctor Watson is the faithful sidekick and chronicler of Sherlock Holmes.\n\n"
                "Special ability (MANDATORY): Watson bears a lantern, pictured on his character token.\n"
                "This lantern illuminates all the characters standing straight ahead of him!! (It is important to note that Watson himself is not illumated by the lantern!!)\n"
                "Any player moving Watson, one way or another, chooses the final facing of the character, and therefore the line illuminated by the lantern.",
        .character = "c4.png",
        .selected = "cs4.png",
        .circle = "m4.png",
        .innocent = "i4.png",
        .click = JohnWatson_click,
        .init = JohnWatson_initialize
};

MRJACK_Character c5 = {
        .id = 5,
        .name = "Sherlock Holmes",
        .desc = "The best private detective in the world is on the scene of course, in order to solve the most famous mystery\n\n"
                "Special ability (MANDATORY): AFTER MOVEMENT, Sherlock Holmes secretly draws the first card from the alibi pile and places it face down in front of him.",
        .character = "c5.png",
        .selected = "cs5.png",
        .circle = "m5.png",
        .innocent = "i5.png",
        .click = SherlockHolmes_click,
        .init = SherlockHolmes_initialize
};

MRJACK_Character c6 = {
        .id = 6,
        .name = "John Smith",
        .desc = "This city lamplighter is in charge of the gaslights that keep on shutting off.\n\n"
                "Special ability (MANDATORY): Move one of the lit gaslight tiles onto one of the shut-off gaslight hex. This ability can be used before or after the movement, as the player sees fit.",
        .character = "c6.png",
        .selected = "cs6.png",
        .circle = "m6.png",
        .innocent = "i6.png",
        .click = JohnSmith_click,
        .init = JohnSmith_initialize
};

MRJACK_Character c7 = {
        .id = 7,
        .name = "Sergent Goodley",
        .desc = "He has a loud whistle that can direct other investigators towards him.\n\n"
                "Special ability (MANDATORY): Sergeant Goodley calls for help with his whistle!! You then get 3 movement points to use as you see fit on one or several characters in order to bring them closer to Sergeant Goodley!! This ability can be used before or after the movement, as the player sees fit.",
        .character = "c7.png",
        .selected = "cs7.png",
        .circle = "m7.png",
        .innocent = "i7.png",
        .click = SergentGoodley_click,
        .init = SergentGoodley_initialize
};

MRJACK_Character c8 = {
        .id = 8,
        .name = "Sir William Gull",
        .desc = "The physician of the queen was sent by Her Majesty to lend a hand to the police.\n\n"
                "Special ability (OPTIONAL): Instead of moving normally William Gull, you can exchange this character's location with the location of any other character.",
        .character = "c8.png",
        .selected = "cs8.png",
        .circle = "m8.png",
        .innocent = "i8.png",
        .click = SirWilliamGull_click,
        .init = SirWilliamGull_initialize
};

void shuffle(struct LinkedListNode *list) {
    int n = 8; // 8 characters

    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);

        if (i != j)
            linked_list_swap(list, i, j);
    }
}

struct LinkedListNode* characters_innocent;
struct LinkedListNode* characters_card;
int picked1, picked2 = -1;

void initialize_characters() {
    characters_innocent = linked_list_new(&c8);
    characters_card = linked_list_new(&c8);
    for (int i = 7; i >= 1; --i) {
        linked_list_push(linked_list_new(findCharacter(i)), characters_innocent);
        linked_list_push(linked_list_new(findCharacter(i)), characters_card);
    }

    picked1 = picked2 = 0;
    srand(time(0));
    shuffle(characters_innocent);
    shuffle(characters_card);
}

MRJACK_Character* pick_character() {
    return linked_list_findByIndex(characters_innocent, picked1++)->data;
}

MRJACK_Character *pick_card() {
    if (picked2 == -1)
        initialize_characters();

    MRJACK_Character *character = linked_list_findByIndex(characters_card, picked2++)->data;
    if (picked2 == 8) {
        picked2 = 0;
        shuffle(characters_card);
    }
    return character;
}

MRJACK_Character* findCharacter(int id) {
    switch (id) {
        case 1:
            return &c1;
        case 2:
            return &c2;
        case 3:
            return &c3;
        case 4:
            return &c4;
        case 5:
            return &c5;
        case 6:
            return &c6;
        case 7:
            return &c7;
        default:
            return &c8;
    }
}
