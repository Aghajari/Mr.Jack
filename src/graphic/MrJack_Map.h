//
// Created by AmirHossein Aghajari on 1/4/22.
//

#ifndef MRJACK_MAP_H
#define MRJACK_MAP_H

typedef struct {
    SDL_Point points[6];
    SDL_Color color;
    bool enabled, isLight, isTunnel, isHome;
    int connectedTo[6];
} Whitechapel;

typedef struct {
    SDL_Rect currentRect;
    char name[12];
    int rotation;
    int selected;
    bool visible;
} MapElement;

void initializeMap(int x);
void render_map(SDL_Renderer *r);

#endif //MRJACK_MAP_H

