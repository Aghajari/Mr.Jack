//
// Created by AmirHossein Aghajari on 12/30/21.
//

#ifndef GUI_INIT_H
#define GUI_INIT_H
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include <stdbool.h>

#define is_in_area(x, y, x1, x2, y1, y2) ((x) >= (x1) && (x) <= (x2) && (y) >= (y1) && (y) <= (y2))

typedef struct {
    void *obj, *element;
    int (*event)(), (*render)();
    bool isPanel;
} GUI_OBJECT_POINTER;

typedef struct {
    bool pressing, hover, pressed;
} GUI_Event;

typedef struct {
    SDL_Rect rect;
    SDL_Color color, pressingColor, hoverColor;
    int gravity;
    GUI_Event event;
    void (*clickListener)(const void *element);
    void *texture, *surface;
    int target;
    bool refresh;
} GUI_OBJECT;

typedef struct GUI_Button {
    GUI_OBJECT obj;
} GUI_Button;

typedef struct {
    GUI_OBJECT obj;
    char *text;
    TTF_Font *font;
} GUI_Label;

typedef struct {
    GUI_OBJECT obj;
    char *file;
} GUI_Image;

typedef struct {
    GUI_OBJECT obj;
    struct LinkedListNode* childs;
} GUI_Panel;

typedef struct {
    GUI_OBJECT obj;
    char *text;
    TTF_Font *font;
    long long time;
    int duration, showDuration, delay;
    bool showing;
    void (*listener)();
} GUI_FadeLabel;

typedef struct {
    GUI_OBJECT obj;
    long long time;
    int duration;
    char *file, *hoverFile, *selectedFile;
    bool selected;
} GUI_FadeInImage;

typedef struct {
    GUI_OBJECT obj;
    int type;
    long long time;
    int duration, showDuration;
    bool showing;
} GUI_RainImage;

typedef struct {
    GUI_OBJECT obj;
    char *file;
    long long time;
    int duration, showDuration;
    bool showing;
} GUI_FadeImage;

typedef struct {
    GUI_OBJECT obj;
    long long time;
    int duration, showDuration, delay;
    bool showing;
} GUI_FadeRect;

void initialize();
void draw_for_all(SDL_Renderer *r);
void draw_for_all2(SDL_Renderer *r, struct LinkedListNode *l);
int event(GUI_OBJECT *obj, const SDL_Event *ev);
void event_for_all(SDL_Event *ev);
void event_for_all2(const SDL_Event *ev, struct LinkedListNode *l);
void draw_rain(SDL_Renderer *r, struct LinkedListNode *l);

void remove_element(const void *target);

void insert_button(GUI_Button *button);
void insert_label(GUI_Label *label);
void insert_fade_label(GUI_FadeLabel *label);
void insert_image(GUI_Image *img);
void insert_fade_image(GUI_FadeImage *img);
void insert_fadein_image(GUI_FadeInImage *img);
void insert_rain(GUI_RainImage *img);
void insert_panel(GUI_Panel *panel);
void insert_fade_rect(GUI_FadeRect *rect);

void insert_button2(GUI_Button *button, GUI_Panel *panel);
void insert_label2(GUI_Label *label, GUI_Panel *panel);
void insert_fade_label2(GUI_FadeLabel *label, GUI_Panel *panel);
void insert_image2(GUI_Image *img, GUI_Panel *panel);
void insert_fade_image2(GUI_FadeImage *img, GUI_Panel *panel);
void insert_fadein_image2(GUI_FadeInImage *img, GUI_Panel *panel);
void insert_rain2(GUI_RainImage *img, GUI_Panel *panel);
void insert_panel2(GUI_Panel *pnl, GUI_Panel *panel);
void insert_fade_rect2(GUI_FadeRect *pnl, GUI_Panel *panel);

GUI_Panel *create_panel();
GUI_Panel *create_panel2(GUI_OBJECT object);
void free_panel(GUI_Panel *pnl);
void free_surface(GUI_OBJECT *o);

long long currentTime();

int render_fadein_image(GUI_FadeInImage *img, SDL_Renderer *r);
int render_fadein_image_rotation(GUI_FadeInImage *img, SDL_Renderer *r, int rotation);
int render_label(GUI_Label *lbl, SDL_Renderer *r);
int render_image(GUI_Image *img, SDL_Renderer *r);
#endif
