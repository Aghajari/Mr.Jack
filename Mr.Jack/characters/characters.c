//
// Created by AmirHossein Aghajari on 1/3/22.
//

#include <stdlib.h>
#include <sys/time.h>
#include "../graphic/GUI_init.h"
#include "characters.h"
#include "inner_characters.h"
#include "../linked_list/linked_list.h"

MRJACK_Character c1 = {
        .id = 1,
        .name = "Inspecteur Lestrade",
        .desc = "This very good sleuth from Scotland Yard was sent to put an end to Jack's crimes.\n\n"
                "Special ability (MANDATORY): Move one police cordon. This will free one of the exits but block another!! This ability can be used before or after the movement, as the player sees fit.",
        .character = "assets/c1.png",
        .selected = "assets/cs1.png",
        .disabled = "assets/cu1.png",
        .circle = "assets/m1.png",
        .circleSafe = "assets/m12.png",
        .innocent = "assets/i1.png",
        .renderer = (void (*)(void *)) InspecteurLestrade_render,
        .event = (void (*)(void *)) InspecteurLestrade_event,
        .click = InspecteurLestrade_click,
        .init = InspecteurLestrade_initialize,
        .move = InspecteurLestrade_moved
};

MRJACK_Character c2 = {
        .id = 2,
        .name = "Miss Stealthy",
        .desc = "Women are frequently abused in this district and Miss Stealthy is one of the very first fighting to improve the lot of her kind.\n\n"
                "Special ability (OPTIONAL): During her movement, Miss Stealthy can cross any hex (building, 4 gaslight, garden) but she must stop her movement on a street hex.",
        .character = "assets/c2.png",
        .selected = "assets/cs2.png",
        .disabled = "assets/cu2.png",
        .circle = "assets/m2.png",
        .circleSafe = "assets/m22.png",
        .innocent = "assets/i2.png",
        .renderer = NULL,
        .event = NULL,
        .click = MissStealthy_click,
        .init = MissStealthy_initialize,
        .move = MissStealthy_moved
};

MRJACK_Character c3 = {
        .id = 3,
        .name = "Jermy Bert",
        .desc = "Reporter for the STAR, he invented the name \"Jack\".\n\n"
                "Special ability (MANDATORY): Journalists like to search where it stinks... Jeremy Bert opens a manhole and closes another (move one covered manhole tile on any other open manhole hex.)\n"
                "This ability can be used before or after the movement, as the player sees fit.",
        .character = "assets/c3.png",
        .selected = "assets/cs3.png",
        .disabled = "assets/cu3.png",
        .circle = "assets/m3.png",
        .circleSafe = "assets/m32.png",
        .innocent = "assets/i3.png",
        .renderer = NULL,
        .event = NULL,
        .click = JermyBert_click,
        .init = JermyBert_initialize,
        .move = JermyBert_moved
};

MRJACK_Character c4 = {
        .id = 4,
        .name = "John H. Watson",
        .desc = "Doctor Watson is the faithful sidekick and chronicler of Sherlock Holmes.\n\n"
                "Special ability (MANDATORY): Watson bears a lantern, pictured on his character token.\n"
                "This lantern illuminates all the characters standing straight ahead of him!! (It is important to note that Watson himself is not illumated by the lantern!!)\n"
                "Any player moving Watson, one way or another, chooses the final facing of the character, and therefore the line illuminated by the lantern.",
        .character = "assets/c4.png",
        .selected = "assets/cs4.png",
        .disabled = "assets/cu4.png",
        .circle = "assets/m4.png",
        .circleSafe = "assets/m42.png",
        .innocent = "assets/i4.png",
        .renderer = (void (*)(void *)) JohnWatson_render,
        .event = (void (*)(void *)) JohnWatson_event,
        .click = JohnWatson_click,
        .init = JohnWatson_initialize,
        .move = JohnWatson_moved
};

MRJACK_Character c5 = {
        .id = 5,
        .name = "Sherlock Holmes",
        .desc = "The best private detective in the world is on the scene of course, in order to solve the most famous mystery\n\n"
                "Special ability (MANDATORY): AFTER MOVEMENT, Sherlock Holmes secretly draws the first card from the alibi pile and places it face down in front of him.",
        .character = "assets/c5.png",
        .selected = "assets/cs5.png",
        .disabled = "assets/cu5.png",
        .circle = "assets/m5.png",
        .circleSafe = "assets/m52.png",
        .innocent = "assets/i5.png",
        .renderer = NULL,
        .event = NULL,
        .click = SherlockHolmes_click,
        .init = SherlockHolmes_initialize,
        .move = SherlockHolmes_moved
};

MRJACK_Character c6 = {
        .id = 6,
        .name = "John Smith",
        .desc = "This city lamplighter is in charge of the gaslights that keep on shutting off.\n\n"
                "Special ability (MANDATORY): Move one of the lit gaslight tiles onto one of the shut-off gaslight hex. This ability can be used before or after the movement, as the player sees fit.",
        .character = "assets/c6.png",
        .selected = "assets/cs6.png",
        .disabled = "assets/cu6.png",
        .circle = "assets/m6.png",
        .circleSafe = "assets/m62.png",
        .innocent = "assets/i6.png",
        .renderer = NULL,
        .event = NULL,
        .click = JohnSmith_click,
        .init = JohnSmith_initialize,
        .move = JohnSmith_moved
};

MRJACK_Character c7 = {
        .id = 7,
        .name = "Sergent Goodley",
        .desc = "He has a loud whistle that can direct other investigators towards him.\n\n"
                "Special ability (MANDATORY): Sergeant Goodley calls for help with his whistle!! You then get 3 movement points to use as you see fit on one or several characters in order to bring them closer to Sergeant Goodley!! This ability can be used before or after the movement, as the player sees fit.",
        .character = "assets/c7.png",
        .selected = "assets/cs7.png",
        .disabled = "assets/cu7.png",
        .circle = "assets/m7.png",
        .circleSafe = "assets/m72.png",
        .innocent = "assets/i7.png",
        .renderer = NULL,
        .event = NULL,
        .click = SergentGoodley_click,
        .init = SergentGoodley_initialize,
        .move = SergentGoodley_moved
};

MRJACK_Character c8 = {
        .id = 8,
        .name = "Sir William Gull",
        .desc = "The physician of the queen was sent by Her Majesty to lend a hand to the police.\n\n"
                "Special ability (OPTIONAL): Instead of moving normally William Gull, you can exchange this character's location with the location of any other character.",
        .character = "assets/c8.png",
        .selected = "assets/cs8.png",
        .disabled = "assets/cu8.png",
        .circle = "assets/m8.png",
        .circleSafe = "assets/m82.png",
        .innocent = "assets/i8.png",
        .renderer = NULL,
        .event = NULL,
        .click = SirWilliamGull_click,
        .init = SirWilliamGull_initialize,
        .move = SirWilliamGull_moved
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
    srand(currentTime());
    shuffle(characters_innocent);
    shuffle(characters_card);
}

long long int ignoreTimeCharacter = 0;
MRJACK_Character* _Nonnull pick_character() {
    long long int time = currentTime();
    if (ignoreTimeCharacter > time)
        return linked_list_findByIndex(characters_innocent, picked1)->data;
    ignoreTimeCharacter = time + 200;

    if (picked1 >= 8)
        picked1 = 7;
    return linked_list_findByIndex(characters_innocent, picked1++)->data;
}

MRJACK_Character * _Nonnull pick_card() {
    if (picked2 == -1)
        initialize_characters();

    if (picked2 == 8) {
        picked2 = 0;
        shuffle(characters_card);
    }
    return linked_list_findByIndex(characters_card, picked2++)->data;
}

MRJACK_Character* _Nonnull findCharacter(int id) {
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

void saveCharactersData(int * _Nonnull output){
    output[0] = picked1;
    output[1] = picked2;

    struct LinkedListNode *cur = characters_innocent;
    for (int i = 0; i < 8; ++i) {
        output[i + 2] = ((MRJACK_Character *)cur->data)->id;
        cur = cur->next;
    }
    cur = characters_card;
    for (int i = 0; i < 8; ++i) {
        output[i + 10] = ((MRJACK_Character *)cur->data)->id;
        cur = cur->next;
    }
}

void loadCharactersData(int * _Nonnull output){
    picked1 = output[0];
    picked2 = output[1];

    struct LinkedListNode *cur = characters_innocent;
    for (int i = 0; i < 8; ++i) {
        cur->data = findCharacter(output[i + 2]);
        cur = cur->next;
    }
    cur = characters_card;
    for (int i = 0; i < 8; ++i) {
        cur->data = findCharacter(output[i + 10]);
        cur = cur->next;
    }
}