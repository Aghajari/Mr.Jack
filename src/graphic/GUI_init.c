//
// Created by AmirHossein Aghajari on 12/30/21.
//

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include <stdbool.h>
#include <sys/time.h>
#include "GUI_init.h"
#include "GUI_gravity.h"
#include "../linked_list/linked_list.h"

static struct LinkedListNode *list = NULL;

void initialize() {
    TTF_Init();
    if (list != NULL)
        free(list);
    list = linked_list_create();
}

int element_finder(struct LinkedListNode *node, int index, const void *target) {
    return -(node->data != NULL && ((GUI_OBJECT_POINTER *) node->data)->element == target);
}

void remove_element2(const void *target, struct LinkedListNode *l) {
    linked_list_remove(l, target, element_finder);

    if (l == NULL || l->next == NULL)
        return;

    for (struct LinkedListNode *cur = l->next; cur != NULL; cur = cur->next) {
        GUI_OBJECT_POINTER *obj = cur->data;
        if (obj->isPanel)
            remove_element2(target, ((GUI_Panel *) obj->element)->childs);
    }
}

void remove_element(const void *target) {
    remove_element2(target, list);
}

void draw_for_all2(SDL_Renderer *r, struct LinkedListNode *l) {
    if (l == NULL || l->next == NULL)
        return;

    for (struct LinkedListNode *cur = l->next; cur != NULL; cur = cur->next) {
        GUI_OBJECT_POINTER *obj = cur->data;

        if (obj->render != NULL)
            obj->render(obj->element, r);
    }
}

void draw_rain(SDL_Renderer *r, struct LinkedListNode *l) {
    if (l == NULL || l->next == NULL)
        return;

    for (struct LinkedListNode *cur = l->next; cur != NULL; cur = cur->next) {
        GUI_OBJECT_POINTER *obj = cur->data;

        if (obj != NULL && obj->render != NULL && obj->element != NULL) {
            if (obj->render(obj->element, r)) {
                free(obj->element);
                obj->element = NULL;
            }
        }
    }
}

void draw_for_all(SDL_Renderer *r) {
    draw_for_all2(r, list);
}

void event_for_all2(const SDL_Event *ev, struct LinkedListNode *l) {
    if (l == NULL || l->next == NULL)
        return;

    for (struct LinkedListNode *cur = l->next; cur != NULL; cur = cur->next) {
        GUI_OBJECT_POINTER *obj = cur->data;

        if (obj->event != NULL)
            obj->event(obj->obj, ev);
    }
}

void event_for_all(SDL_Event *ev) {
    event_for_all2(ev, list);
}

// HELPER FUNCTIONS

bool is_in_rect(const SDL_Event *ev, SDL_Rect rect) {
    return ev->button.x >= rect.x &&
           ev->button.x <= (rect.x + rect.w) &&
           ev->button.y >= rect.y &&
           ev->button.y <= (rect.y + rect.h);
}

int event(GUI_OBJECT *obj, const SDL_Event *ev) {
    // react on mouse click within button rectangle by setting 'pressed'
    if (ev->type == SDL_MOUSEBUTTONDOWN) {
        obj->event.pressing = ev->button.button == SDL_BUTTON_LEFT && is_in_rect(ev, obj->rect);
    } else if (ev->type == SDL_MOUSEBUTTONUP && obj->event.pressing) {
        obj->event.pressed = is_in_rect(ev, obj->rect);
        obj->event.pressing = false;
    }
    obj->event.hover = is_in_rect(ev, obj->rect);
}

bool is_color_ready(SDL_Color *color) {
    return color->r != 0 || color->g != 0 || color->b != 0 || color->a != 0;
}

SDL_Color *find_color(GUI_OBJECT *object) {
    if (object->event.pressing && is_color_ready(&object->pressingColor))
        return &object->pressingColor;
    else if (object->event.hover && is_color_ready(&object->hoverColor))
        return &object->hoverColor;
    else
        return &object->color;
}

void checkClick(GUI_OBJECT *obj, const void *element) {
    if (obj->event.pressed) {
        obj->event.pressed = false;
        if (obj->clickListener != NULL)
            obj->clickListener(element);
    }
}

// BUTTON

int render_button(GUI_Button *btn, SDL_Renderer *r) {
    SDL_Color *color = find_color(&btn->obj);
    SDL_SetRenderDrawColor(r, color->r, color->g, color->b, color->a);
    SDL_RenderFillRect(r, &btn->obj.rect);
    checkClick(&btn->obj, btn);
}

void insert_button(GUI_Button *button) {
    insert_button2(button, NULL);
}

void insert_button2(GUI_Button *button, GUI_Panel *panel) {
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &button->obj;
    pointer->element = button;
    pointer->event = event;
    pointer->render = render_button;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}


// LABEL

void fix_rect(SDL_Rect *src, SDL_Rect *dest, int w, int h, int gravity) {
    dest->x = src->x;
    dest->y = src->y;
    dest->w = w;
    dest->h = h;

    if (gravity == GRAVITY_FILL || gravity == GRAVITY_FILL2) {
        dest->w = src->w;
        dest->h = src->h;
        return;
    }

    if (src->w > 0) {
        if (src->w >= dest->w) {
            switch (gravity & GRAVITY_MASK_H) {
                case GRAVITY_CENTER_H:
                    dest->x += (src->w - dest->w) / 2;
                    break;
                case GRAVITY_RIGHT:
                    dest->x = src->w - dest->w;
                    break;
                case GRAVITY_LEFT:
                default:
                    break;
            }
        } else {
            dest->w = src->w;
        }
    }

    if (src->h > 0) {
        if (src->h >= dest->h) {
            switch (gravity & GRAVITY_MASK_V) {
                case GRAVITY_CENTER_V:
                    dest->y += (src->h - dest->h) / 2;
                    break;
                case GRAVITY_BOTTOM:
                    dest->y = src->h - dest->h;
                    break;
                case GRAVITY_TOP:
                default:
                    break;
            }
        } else {
            dest->h = src->h;
        }
    }
}

int render_label(GUI_Label *lbl, SDL_Renderer *r) {
    SDL_Surface *surface = TTF_RenderText_Solid(lbl->font, lbl->text, *(find_color(&lbl->obj)));
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);

    SDL_Rect rect;
    fix_rect(&lbl->obj.rect, &rect, surface->w, surface->h, lbl->obj.gravity);

    SDL_RenderCopy(r, t, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);
    checkClick(&lbl->obj, lbl);
}

void insert_label(GUI_Label *label) {
    insert_label2(label, NULL);
}

void insert_label2(GUI_Label *label, GUI_Panel *panel) {
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &label->obj;
    pointer->element = label;
    pointer->event = event;
    pointer->render = render_label;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// FadeLabel

long long currentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long) tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int render_fade_label(GUI_FadeLabel *lbl, SDL_Renderer *r) {
    SDL_Color *targetColor = find_color(&lbl->obj);
    SDL_Color color = {.r = targetColor->r, .g = targetColor->g, .b = targetColor->b};

    long long time = currentTime();
    bool shouldRemove = false;

    if (lbl->showing) {
        long passed = time - (lbl->time + lbl->delay);
        if (passed <= 0) {
            return 0;
        } else {
            if (lbl->listener != NULL) {
                lbl->listener();
                lbl->listener = NULL;
            }
            color.a = (int) fmax(1, (targetColor->a * fmin(1, (double) passed / lbl->duration)));
            if (passed >= lbl->duration)
                lbl->showing = false;
        }
    } else if (lbl->showDuration >= 0) {
        long passed = time - (lbl->time + lbl->duration + lbl->showDuration + lbl->delay);
        if (passed > 0) {
            color.a = (int) fmax(1, targetColor->a - (int) (targetColor->a * fmin(1, (double) passed / lbl->duration)));
            shouldRemove = passed >= lbl->duration;
        } else {
            color.a = targetColor->a;
        }
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_Surface *surface = TTF_RenderText_Solid(lbl->font, lbl->text, color);
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);

    SDL_Rect rect;
    fix_rect(&lbl->obj.rect, &rect, surface->w, surface->h, lbl->obj.gravity);

    SDL_RenderCopy(r, t, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);
    checkClick(&lbl->obj, lbl);

    if (shouldRemove)
        remove_element(lbl);
}

void insert_fade_label(GUI_FadeLabel *label) {
    insert_fade_label2(label, NULL);
}

void insert_fade_label2(GUI_FadeLabel *label, GUI_Panel *panel) {
    label->showing = true;
    label->time = currentTime();

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &label->obj;
    pointer->element = label;
    pointer->event = event;
    pointer->render = render_fade_label;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// Image

int render_image(GUI_Image *img, SDL_Renderer *r) {
    IMG_Init(IMG_INIT_JPG);
    SDL_Surface *surface = IMG_Load(img->file);
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);

    SDL_Rect rect;
    fix_rect(&img->obj.rect, &rect, surface->w, surface->h, img->obj.gravity);

    SDL_RenderCopy(r, t, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);
    checkClick(&img->obj, img);
}

void insert_image(GUI_Image *img) {
    insert_image2(img, NULL);
}

void insert_image2(GUI_Image *img, GUI_Panel *panel) {
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->event = event;
    pointer->render = render_image;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// Image FadeIn

int render_fadein_image(GUI_FadeInImage *img, SDL_Renderer *r) {
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Surface *surface = IMG_Load(
            (img->selected && img->selectedFile != NULL) ? img->selectedFile :
            ((img->obj.event.hover && img->hoverFile != NULL) ? img->hoverFile : img->file));

    long long time = currentTime();
    long passed = time - img->time;

    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);
    int a = 255;
    if (passed < img->duration) {
        a = (int) (fmin(255, 255 * ((double) passed / img->duration)));
        SDL_SetTextureAlphaMod(t, a);
    }

    SDL_Rect rect;
    fix_rect(&img->obj.rect, &rect, surface->w / 2, surface->h / 2, img->obj.gravity);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(r, t, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);

    if (a != 255 && img->obj.gravity == GRAVITY_FILL) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255 - a);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(r, &img->obj.rect);
    }
    checkClick(&img->obj, img);
}

void insert_fadein_image(GUI_FadeInImage *img) {
    insert_fadein_image2(img, NULL);
}

void insert_fadein_image2(GUI_FadeInImage *img, GUI_Panel *panel) {
    img->time = currentTime();

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->event = event;
    pointer->render = render_fadein_image;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// Rain

int render_rain(GUI_RainImage *img, SDL_Renderer *r) {
    if (img == NULL)
        return 0;

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    char name[10];
    sprintf(name, "%d.png", img->type);
    SDL_Surface *surface = IMG_Load(name);

    long long time = currentTime();
    bool shouldRemove = false;
    int alpha = 255;

    if (img->showing) {
        long passed = time - img->time;
        if (passed <= 0) {
            return 0;
        } else {
            alpha = (int) fmax(1, (255 * fmin(1, (double) passed / img->duration)));
            if (passed >= img->duration)
                img->showing = false;
        }
    } else if (img->showDuration >= 0) {
        long passed = time - (img->time + img->duration + img->showDuration);
        if (passed > 0) {
            alpha = (int) fmax(1, 255 - (int) (255 * fmin(1, (double) passed / img->duration)));
            shouldRemove = passed >= img->duration;
        }
    }
    SDL_SetSurfaceAlphaMod(surface, alpha);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);
    //fix_rect(&img->obj.rect, &rect, surface->w / 4, surface->h / 4, img->obj.gravity);

    SDL_RenderCopy(r, t, NULL, &img->obj.rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(t);
    //checkClick(&img->obj, img);

    return shouldRemove;
}

void insert_rain(GUI_RainImage *img) {
    insert_rain2(img, NULL);
}

void insert_rain2(GUI_RainImage *img, GUI_Panel *panel) {
    img->time = currentTime();
    img->showing = true;

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    if (pointer == NULL)
        return;

    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->render = render_rain;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// FadeRect


int render_fade_rect(GUI_FadeRect *img, SDL_Renderer *r) {
    if (img == NULL)
        return 0;

    long long time = currentTime();
    bool shouldRemove = false;
    int alpha = 255;

    if (img->showing) {
        long passed = time - (img->time + img->delay);
        if (passed <= 0) {
            return 0;
        } else {
            alpha = (int) fmax(1, (255 * fmin(1, (double) passed / img->duration)));
            if (passed >= img->duration)
                img->showing = false;
        }
    } else if (img->showDuration >= 0) {
        long passed = time - (img->time + img->duration + img->showDuration + img->delay);
        if (passed > 0) {
            alpha = (int) fmax(1, 255 - (int) (255 * fmin(1, (double) passed / img->duration)));
            shouldRemove = passed >= img->duration;
        }
    }

    SDL_Color *color = &img->obj.color;
    SDL_SetRenderDrawColor(r, color->r, color->g, color->b, alpha);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &img->obj.rect);

    return shouldRemove;
}

void insert_fade_rect(GUI_FadeRect *rect) {
    insert_fade_rect2(rect, NULL);
}

void insert_fade_rect2(GUI_FadeRect *rect, GUI_Panel *panel) {
    rect->time = currentTime();
    rect->showing = true;

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    if (pointer == NULL)
        return;

    pointer->obj = &rect->obj;
    pointer->element = rect;
    pointer->event = event;
    pointer->render = render_fade_rect;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}

// Panel

int event_panel(GUI_Panel *pnl, const SDL_Event *ev) {
    event(&pnl->obj, ev);
    event_for_all2(ev, pnl->childs);
}

int render_panel(GUI_Panel *pnl, SDL_Renderer *r) {
    SDL_Color *color = find_color(&pnl->obj);
    SDL_SetRenderDrawColor(r, color->r, color->g, color->b, color->a);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &pnl->obj.rect);
    draw_for_all2(r, pnl->childs);
    checkClick(&pnl->obj, pnl);
}

GUI_Panel *create_panel() {
    return create_panel2((GUI_OBJECT) {});
}

GUI_Panel *create_panel2(GUI_OBJECT object) {
    GUI_Panel *pnl = malloc(sizeof(GUI_Panel));
    pnl->obj.color = object.color;
    pnl->obj.pressingColor = object.pressingColor;
    pnl->obj.hoverColor = object.hoverColor;
    pnl->obj.rect = object.rect;
    pnl->obj.gravity = object.gravity;
    pnl->obj.clickListener = object.clickListener;
    pnl->obj.event.pressing = false;
    pnl->obj.event.pressed = false;
    pnl->obj.event.hover = false;
    pnl->childs = linked_list_create();
    return pnl;
}

void insert_panel(GUI_Panel *pnl) {
    insert_panel2(pnl, NULL);
}

void insert_panel2(GUI_Panel *pnl, GUI_Panel *panel) {
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &pnl->obj;
    pointer->element = pnl;
    pointer->event = event_panel;
    pointer->render = render_panel;
    pointer->isPanel = true;
    linked_list_append(linked_list_new(pointer), panel == NULL ? list : panel->childs);
}