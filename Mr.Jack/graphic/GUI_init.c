//
// Created by AmirHossein Aghajari on 12/30/21.
//

#include <sys/time.h>
#include "GUI_init.h"
#include "GUI_gravity.h"
#include "../linked_list/linked_list.h"

static struct LinkedListNode *ui_list = NULL;

void initialize() {
    TTF_Init();
    if (ui_list != NULL) free(ui_list);
    ui_list = linked_list_create();
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
    remove_element2(target, ui_list);
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
                if (obj->obj != NULL) {
                    GUI_OBJECT *o = obj->obj;
                    if (o->surface != NULL)
                        SDL_FreeSurface(o->surface);
                    if (o->texture != NULL)
                        SDL_DestroyTexture(o->texture);
                }

                free(obj->element);
                obj->element = NULL;
            }
        }
    }
}

void draw_for_all(SDL_Renderer *r) {
    draw_for_all2(r, ui_list);
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
    event_for_all2(ev, ui_list);
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
    return 0;
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
    return 0;
}

void insert_button(GUI_Button *button) {
    insert_button2(button, NULL);
}

void insert_button2(GUI_Button *button, GUI_Panel *panel) {
    button->obj.texture = button->obj.surface = NULL;
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &button->obj;
    pointer->element = button;
    pointer->event = event;
    pointer->render = render_button;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
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
    if (lbl->obj.refresh) {
        lbl->obj.refresh = false;
        free_surface(&lbl->obj);
    }

    if (lbl->text == NULL)
        return -1;

    if (lbl->obj.surface == NULL) {
        lbl->obj.surface = TTF_RenderText_Blended_Wrapped(lbl->font, lbl->text, *(find_color(&lbl->obj)), lbl->obj.rect.w - 8);
        lbl->obj.texture = SDL_CreateTextureFromSurface(r, lbl->obj.surface);
    }

    SDL_Rect rect;
    fix_rect(&lbl->obj.rect, &rect,
             ((SDL_Surface *) lbl->obj.surface)->w,
             ((SDL_Surface *) lbl->obj.surface)->h,
             lbl->obj.gravity);

    SDL_RenderCopy(r, lbl->obj.texture, NULL, &rect);
    //SDL_FreeSurface(surface);
    //SDL_DestroyTexture(t);
    checkClick(&lbl->obj, lbl);
    return 0;
}

void insert_label(GUI_Label *label) {
    insert_label2(label, NULL);
}

void insert_label2(GUI_Label *label, GUI_Panel *panel) {
    label->obj.texture = label->obj.surface = NULL;
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &label->obj;
    pointer->element = label;
    pointer->event = event;
    pointer->render = render_label;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
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
    return 0;
}

void insert_fade_label(GUI_FadeLabel *label) {
    insert_fade_label2(label, NULL);
}

void insert_fade_label2(GUI_FadeLabel *label, GUI_Panel *panel) {
    label->obj.texture = label->obj.surface = NULL;
    label->showing = true;
    label->time = currentTime();

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &label->obj;
    pointer->element = label;
    pointer->event = event;
    pointer->render = render_fade_label;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

// Image

int render_image(GUI_Image *img, SDL_Renderer *r) {
    if (img->obj.refresh) {
        img->obj.refresh = false;
        free_surface(&img->obj);
    }

    if (img->obj.texture == NULL) {
        IMG_Init(IMG_INIT_JPG);
        SDL_Surface *surface = IMG_Load(img->file);
        SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);
        img->obj.surface = surface;
        img->obj.texture = t;
    }

    SDL_Rect rect;
    fix_rect(&img->obj.rect, &rect, ((SDL_Surface *) img->obj.surface)->w, ((SDL_Surface *) img->obj.surface)->h,
             img->obj.gravity);

    SDL_RenderCopy(r, img->obj.texture, NULL, &rect);
    //SDL_FreeSurface(surface);
    //SDL_DestroyTexture(t);
    checkClick(&img->obj, img);
    return 0;
}

void insert_image(GUI_Image *img) {
    insert_image2(img, NULL);
}

void insert_image2(GUI_Image *img, GUI_Panel *panel) {
    img->obj.texture = img->obj.surface = NULL;
    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->event = event;
    pointer->render = render_image;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

// Image FadeIn
int render_fadein_image(GUI_FadeInImage *img, SDL_Renderer *r) {
    return render_fadein_image_rotation(img, r, 0);
}

int render_fadein_image_rotation(GUI_FadeInImage *img, SDL_Renderer *r, int rotation) {
    if (img->obj.refresh) {
        img->obj.refresh = false;
        free_surface(&img->obj);
    }

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Surface *surface = img->obj.surface != NULL ? img->obj.surface : IMG_Load(
            (img->selected && img->selectedFile != NULL) ? img->selectedFile :
            ((img->obj.event.hover && img->hoverFile != NULL) ? img->hoverFile : img->file));
    if (img->selectedFile == NULL && img->hoverFile == NULL && img->obj.surface == NULL) {
        img->obj.surface = surface;
    }

    long long time = currentTime();
    long passed = time - img->time;

    SDL_Texture *t;
    if (img->obj.texture == NULL) {
        t = SDL_CreateTextureFromSurface(r, surface);
        if (img->obj.surface != NULL)
            img->obj.texture = t;
    } else {
        t = img->obj.texture;
    }

    int a = 255;
    if (passed < img->duration) {
        a = (int) (fmin(255, 255 * ((double) passed / img->duration)));
        SDL_SetTextureAlphaMod(t, a);
    }

    SDL_Rect rect;
    fix_rect(&img->obj.rect, &rect, surface->w / 2, surface->h / 2, img->obj.gravity);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    if (rotation == 0) {
        SDL_RenderCopy(r, t, NULL, &rect);
    } else {
        SDL_RenderCopyEx(r, t, NULL, &rect,
                         rotation, NULL, SDL_FLIP_NONE);
    }

    if (img->obj.surface == NULL) {
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(t);
    }

    if (a != 255 && img->obj.gravity == GRAVITY_FILL) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255 - a);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(r, &img->obj.rect);
    }
    checkClick(&img->obj, img);
    return 0;
}

void insert_fadein_image(GUI_FadeInImage *img) {
    insert_fadein_image2(img, NULL);
}

void insert_fadein_image2(GUI_FadeInImage *img, GUI_Panel *panel) {
    img->obj.texture = img->obj.surface = NULL;
    img->time = currentTime();

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->event = event;
    pointer->render = render_fadein_image;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

// Rain

int render_rain(GUI_RainImage *img, SDL_Renderer *r) {
    if (img == NULL)
        return 0;

    if (img->obj.refresh) {
        img->obj.refresh = false;
        free_surface(&img->obj);
    }

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

    if (img->obj.surface == NULL) {
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        char name[15];
        sprintf(name, "assets/%d.png", img->type);
        img->obj.surface = IMG_Load(name);
    }

    if (img->obj.texture == NULL) {
        img->obj.texture = SDL_CreateTextureFromSurface(r, img->obj.surface);
    }
    SDL_SetTextureAlphaMod(img->obj.texture, alpha);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    //fix_rect(&img->obj.rect, &rect, surface->w / 4, surface->h / 4, img->obj.gravity);

    SDL_RenderCopy(r, img->obj.texture, NULL, &img->obj.rect);
    //SDL_FreeSurface(surface);
    //SDL_DestroyTexture(t);
    //checkClick(&img->obj, img);

    return shouldRemove;
}

void insert_rain(GUI_RainImage *img) {
    insert_rain2(img, NULL);
}

void insert_rain2(GUI_RainImage *img, GUI_Panel *panel) {
    img->obj.texture = img->obj.surface = NULL;
    img->time = currentTime();
    img->showing = true;

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    if (pointer == NULL)
        return;

    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->render = render_rain;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

// FadeImage

// Rain

int render_fade_image(GUI_FadeImage *img, SDL_Renderer *r) {
    if (img == NULL)
        return 0;

    if (img->obj.refresh) {
        img->obj.refresh = false;
        free_surface(&img->obj);
    }

    checkClick(&img->obj, img);
    if (img->file == NULL)
        return 0;

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

    if (shouldRemove)
        return 0;

    if (img->obj.surface == NULL) {
        img->obj.surface = IMG_Load(img->file);
    }

    if (img->obj.texture == NULL) {
        img->obj.texture = SDL_CreateTextureFromSurface(r, img->obj.surface);
    }
    SDL_SetTextureAlphaMod(img->obj.texture, alpha);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    //fix_rect(&img->obj.rect, &rect, surface->w / 4, surface->h / 4, img->obj.gravity);

    SDL_RenderCopy(r, img->obj.texture, NULL, &img->obj.rect);
    //SDL_FreeSurface(surface);
    //SDL_DestroyTexture(t);
    //checkClick(&img->obj, img);

    /*if (shouldRemove)
        remove_element(img);
    return shouldRemove;*/
    return 0;
}

void insert_fade_image(GUI_FadeImage *img) {
    insert_fade_image2(img, NULL);
}

void insert_fade_image2(GUI_FadeImage *img, GUI_Panel *panel) {
    img->obj.texture = img->obj.surface = NULL;
    //img->time = currentTime();
    img->showing = true;

    GUI_OBJECT_POINTER *pointer = malloc(sizeof(GUI_OBJECT_POINTER));
    if (pointer == NULL)
        return;

    pointer->obj = &img->obj;
    pointer->element = img;
    pointer->event = event;
    pointer->render = render_fade_image;
    pointer->isPanel = false;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
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
    rect->obj.texture = rect->obj.surface = NULL;
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
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

// Panel

int event_panel(GUI_Panel *pnl, const SDL_Event *ev) {
    event(&pnl->obj, ev);
    event_for_all2(ev, pnl->childs);
    return 0;
}

int render_panel(GUI_Panel *pnl, SDL_Renderer *r) {
    SDL_Color *color = find_color(&pnl->obj);
    SDL_SetRenderDrawColor(r, color->r, color->g, color->b, color->a);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &pnl->obj.rect);
    draw_for_all2(r, pnl->childs);
    checkClick(&pnl->obj, pnl);
    return 0;
}

GUI_Panel *create_panel() {
    return create_panel2((GUI_OBJECT) {});
}

GUI_Panel *create_panel2(GUI_OBJECT object) {
    GUI_Panel *pnl = malloc(sizeof(GUI_Panel));
    pnl->obj.surface = pnl->obj.texture = NULL;
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
    pnl->obj.surface = pnl->obj.texture = NULL;
    pointer->obj = &pnl->obj;
    pointer->element = pnl;
    pointer->event = event_panel;
    pointer->render = render_panel;
    pointer->isPanel = true;
    linked_list_append(linked_list_new(pointer), panel == NULL ? ui_list : panel->childs);
}

void free_panel(GUI_Panel *pnl) {
    for (struct LinkedListNode *cur = pnl->childs; cur != NULL;) {
        if (cur->data != NULL) {
            GUI_OBJECT_POINTER *obj = cur->data;
            if (obj->obj != NULL && obj->element != NULL) {
                GUI_OBJECT *o = obj->obj;
                if (o->surface != NULL)
                    SDL_FreeSurface(o->surface);
                if (o->texture != NULL)
                    SDL_DestroyTexture(o->texture);
                if (obj->isPanel)
                    free_panel(obj->element);
            }
        }
        free(cur->data);
        struct LinkedListNode *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    free(pnl);
}

void free_surface(GUI_OBJECT *o) {
    if (o->surface != NULL)
        SDL_FreeSurface(o->surface);
    if (o->texture != NULL)
        SDL_DestroyTexture(o->texture);
    o->surface = NULL;
    o->texture = NULL;
    o->refresh = false;
}