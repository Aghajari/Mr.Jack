//
// Created by AmirHossein Aghajari on 1/7/22.
//

#ifndef INNER_CHARACTERS_H
#define INNER_CHARACTERS_H

void InspecteurLestrade_initialize(bool detective);
void InspecteurLestrade_click(int x1, int y1, int x2, int y2);
void InspecteurLestrade_moved(int target);
void InspecteurLestrade_render(SDL_Renderer *r);
void InspecteurLestrade_event(SDL_Event *ev);

void MissStealthy_initialize(bool detective);
void MissStealthy_click(int x1, int y1, int x2, int y2);
void MissStealthy_moved(int target);

void JermyBert_initialize(bool detective);
void JermyBert_click(int x1, int y1, int x2, int y2);
void JermyBert_moved(int target);

void JohnWatson_initialize(bool detective);
void JohnWatson_click(int x1, int y1, int x2, int y2);
void JohnWatson_moved(int target);
void JohnWatson_render(SDL_Renderer *r);
void JohnWatson_event(SDL_Event *ev);

void SherlockHolmes_initialize(bool detective);
void SherlockHolmes_click(int x1, int y1, int x2, int y2);
void SherlockHolmes_moved(int target);

void JohnSmith_initialize(bool detective);
void JohnSmith_click(int x1, int y1, int x2, int y2);
void JohnSmith_moved(int target);

void SergentGoodley_initialize(bool detective);
void SergentGoodley_click(int x1, int y1, int x2, int y2);
void SergentGoodley_moved(int target);

void SirWilliamGull_initialize(bool detective);
void SirWilliamGull_click(int x1, int y1, int x2, int y2);
void SirWilliamGull_moved(int target);

#endif //INNER_CHARACTERS_H
