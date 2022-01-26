//
// Created by AmirHossein Aghajari on 12/28/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "linked_list.h"

#define DEBUG 1

static int free_data = 1;

void linked_list_freeDataEnabled(int enabled) {
    free_data = enabled;
}

int validate_linked_list(struct LinkedListNode *node) {
    if (node == NULL) {
#if DEBUG
        fprintf(stderr, "LinkedListNode is NULL!\n");
#endif
        return 0;
    }
    return 1;
}

int linked_list_finder_data(struct LinkedListNode *node, int index, const void *target) {
    return -(*((void **) target) == node->data);
}

int linked_list_finder_index(struct LinkedListNode *node, int index, const void *target) {
    return -(*((int *) target) == index);
}

int linked_list_finder_end(struct LinkedListNode *node, int index, const void *target) {
    return node->next == NULL;
}

int linked_list_finder_node(struct LinkedListNode *node, int index, const void *target) {
    return -(node == *((struct LinkedListNode **) target));
}

int linked_list_indexOf(struct LinkedListNode *node, struct LinkedListNode *list) {
    if (!validate_linked_list(list) || !validate_linked_list(node))
        return -2;

    int i = 0;
    while (node != list) {
        list = list->next;
        if (list == NULL)
            return -1;
        i++;
    }

    return i;
}

int linked_list_indexOfData(const void *data, struct LinkedListNode *list) {
    if (!validate_linked_list(list))
        return -2;

    int i = 0;
    while (list->data != data) {
        list = list->next;
        if (list == NULL)
            return -1;
        i++;
    }

    return i;
}

int linked_list_lastIndexOfData(const void *data, struct LinkedListNode *list) {
    if (!validate_linked_list(list))
        return -2;

    int index = -1;
    int i = 0;
    while (list != NULL) {
        if (data == list->data)
            index = i;
        list = list->next;
        i++;
    }

    return index;
}

int linked_list_size(struct LinkedListNode *list) {
    if (list == NULL)
        return 0;

    int i = 1;
    while (list->next != NULL) {
        list = list->next;
        i++;
    }
    return i;
}

struct LinkedListNode *linked_list_create() {
    struct LinkedListNode *node = malloc(sizeof(struct LinkedListNode));
    if (node == NULL)
        return NULL;

    node->data = NULL;
    node->next = NULL;
    return node;
}

struct LinkedListNode *__linked_list_new__(void *data, ...) {
    struct LinkedListNode *node = linked_list_create();
    if (node == NULL)
        return NULL;
    node->data = data;

    va_list args;
    va_start(args, data);
    struct LinkedListNode *tmp, *connect = node;
    while ((tmp = va_arg(args, struct LinkedListNode*)) != NULL) {
        connect->next = tmp;
        connect = linked_list_peekLast(connect);
    }
    va_end(args);
    return node;
}

struct LinkedListNode *linked_list_find(struct LinkedListNode *list, const void *target,
                                        int (*_Nonnull _finder)(struct LinkedListNode *, int, const void *)) {
    if (!validate_linked_list(list))
        return NULL;
    struct LinkedListNode *real = list;

    int i = 0;
    int comp;
    while (!(comp = _finder(list, i++, target))) {
        list = list->next;
        if (list == NULL)
            return NULL;
    }

    if (comp == -1)
        return list;

    if (comp > 0) {
        while (comp-- >= 1 && list->next != NULL)
            list = list->next;
        return list;
    } else {
        return linked_list_findByIndex(real, ((i += comp) > 0 ? i : 0));
    }
}

struct LinkedListNode *__linked_list_findParent__(struct LinkedListNode *list, int *result, const void *target,
                                                  int (*_Nonnull _finder)(struct LinkedListNode *, int, const void *)) {
    if (!validate_linked_list(list))
        return NULL;
    struct LinkedListNode *real = list;

    int i = 1;
    int comp = 0;
    while (list->next != NULL && !(comp = _finder(list->next, i++, target)))
        list = list->next;

    if (comp == 0) {
        if ((comp = _finder(real, 0, target))) {
            list = real;
            if (result != NULL)
                *result = comp;
        } else {
            return NULL;
        }
    } else if (result != NULL)
        *result = i - 1;

    if (comp == -1)
        return list;

    if (comp > 0) {
        if (result != NULL)
            *result = i + comp - 1;
        while (comp-- >= 1 && list->next != NULL)
            list = list->next;
        return list;
    } else {
        struct LinkedListNode *a = linked_list_findParentByIndex(real, ((i += comp) > 1 ? i : 1));
        if (result != NULL)
            *result = i;
        return a;
    }
}

struct LinkedListNode *linked_list_findParent(struct LinkedListNode *list, const void *target,
                                              int (*_Nonnull _finder)(struct LinkedListNode *, int, const void *)) {
    return __linked_list_findParent__(list, NULL, target, _finder);
}

struct LinkedListNode *linked_list_findByIndex(struct LinkedListNode *list, int index) {
    return linked_list_find(list, &index, linked_list_finder_index);
}

struct LinkedListNode *linked_list_findByData(struct LinkedListNode *list, const void *data) {
    return linked_list_find(list, &data, linked_list_finder_data);
}

struct LinkedListNode *linked_list_findParentByIndex(struct LinkedListNode *list, int index) {
    return linked_list_findParent(list, &index, linked_list_finder_index);
}

struct LinkedListNode *linked_list_findParentByData(struct LinkedListNode *list, const void *data) {
    return linked_list_findParent(list, &data, linked_list_finder_data);
}

struct LinkedListNode *linked_list_findParentByNode(struct LinkedListNode *list, struct LinkedListNode *node) {
    return linked_list_findParent(list, &node, linked_list_finder_node);
}

struct LinkedListNode *linked_list_peekLast(struct LinkedListNode *list) {
    return linked_list_find(list, NULL, linked_list_finder_end);
}

struct LinkedListNode *linked_list_peekLastParent(struct LinkedListNode *list) {
    return linked_list_findParent(list, NULL, linked_list_finder_end);
}

void __shift_right(struct LinkedListNode *node, struct LinkedListNode *list, int size) {
    for (int i = 0; i < size; ++i) {
        void *tmp = list->data;
        list->data = node->data;
        node->data = tmp;
        list = list->next;
        node = node->next;
    }
}

void __shift_left(struct LinkedListNode *list, int size) {
    if (list->next == NULL) {
        if (size > 0)
            list->data = NULL;
        return;
    }

    struct LinkedListNode *node = list->next;
    for (int i = 0; i < size - 1; ++i) {
        list->data = node->data;
        list = list->next;
        node = node->next;
    }
}

int linked_list_insert(struct LinkedListNode *node, struct LinkedListNode *list, const void *target,
                       int (*_Nonnull _finder)(struct LinkedListNode *, int, const void *)) {
    if (!validate_linked_list(node))
        return 0;

    int result = 1;
    struct LinkedListNode *start = __linked_list_findParent__(list, &result, target, _finder);
    if (start == NULL)
        return 0;

    int size = linked_list_size(node);
    struct LinkedListNode *tmp = start->next;
    start->next = node;
    linked_list_peekLast(node)->next = tmp;
    if (result <= 0)
        __shift_right(node, list, size);

    return 1;
}

int __linked_list_insert_at_first__(struct LinkedListNode *node, struct LinkedListNode *list) {
    if (!validate_linked_list(node) || !validate_linked_list(list))
        return 0;

    int size = linked_list_size(node);
    struct LinkedListNode *tmp = list->next;
    list->next = node;
    linked_list_peekLast(node)->next = tmp;
    __shift_right(node, list, size);
    return 1;
}

int linked_list_insertAt(struct LinkedListNode *node, struct LinkedListNode *list, int index) {
    if (index == 0)
        return __linked_list_insert_at_first__(node, list);

    if (index < 0)
        index = linked_list_size(list) + index + 1;

    if (index < 0) {
#if DEBUG
        fprintf(stderr, "Index out of range!\n");
#endif
        return 0;
    }

    int count = linked_list_insert(node, list, &index, linked_list_finder_index);
    if (count == 0) {
        struct LinkedListNode *last = list;
        int size = 0;
        while (!linked_list_finder_end(last, size++, NULL))
            last = last->next;

        for (int i = 0; i < index - size; ++count, ++i)
            last = last->next = linked_list_create();

        last->next = node;
        return ++count;
    }
    return count;
}

int linked_list_append(struct LinkedListNode *node, struct LinkedListNode *list) {
    return linked_list_insert(node, list, NULL, linked_list_finder_end);
}

int __linked_list_connect_before__(struct LinkedListNode **list, ...) {
    int count = 0;
    va_list args;
    va_start(args, list);
    struct LinkedListNode *tmp, *connect = *list;
    while ((tmp = va_arg(args, struct LinkedListNode*)) != NULL) {
        struct LinkedListNode *end = linked_list_peekLast(tmp);
        if (end == NULL)
            continue;
        end->next = connect;
        connect = tmp;
        count++;
    }
    va_end(args);
    *list = connect;
    return count;
}

int __linked_list_connect_after__(struct LinkedListNode **list, ...) {
    int count = 0;
    va_list args;
    va_start(args, list);
    struct LinkedListNode *tmp, *connect = linked_list_peekLast(*list);
    while ((tmp = va_arg(args, struct LinkedListNode*)) != NULL) {
        struct LinkedListNode *end = linked_list_peekLast(tmp);
        if (end == NULL)
            continue;
        connect->next = tmp;
        connect = end;
        count++;
    }
    va_end(args);
    return count;
}

int linked_list_swap(struct LinkedListNode *list, int index1, int index2) {
    struct LinkedListNode *a1 = linked_list_findByIndex(list, index1);
    if (a1 == NULL) return 0;
    struct LinkedListNode *a2 = linked_list_findByIndex(list, index2);
    if (a2 == NULL) return 0;

    void *tmp = a1->data;
    a1->data = a2->data;
    a2->data = tmp;
    return 1;
}

int linked_list_set(struct LinkedListNode *list, int index, void *data) {
    struct LinkedListNode *a1 = linked_list_findByIndex(list, index);
    if (a1 == NULL) return 0;
    if (free_data && a1->data != NULL)
        free(a1->data);
    a1->data = data;
    return 1;
}

void __delete_first_element(struct LinkedListNode *list) {
    struct LinkedListNode *last = list, *parent;
    int size = 0;
    while (!linked_list_finder_end(last, size++, NULL))
        last = (parent = last)->next;

    if (size == 1) {
        if (free_data && last->data != NULL)
            free(last->data);
        list->data = NULL;
    } else {
        __shift_left(list, size);
        parent->next = NULL;
        if (free_data && last->data != NULL)
            free(last->data);
        free(last);
    }
}

int linked_list_remove(struct LinkedListNode *list, const void *target,
                       int (*_Nonnull _finder)(struct LinkedListNode *, int, const void *)) {

    int result = 1;
    struct LinkedListNode *item = __linked_list_findParent__(list, &result, target, _finder);
    if (item == NULL)
        return 0;

    if (result <= 0) {
        __delete_first_element(list);
    } else {
        if (item->next == NULL)
            return 0;

        struct LinkedListNode *next = item->next->next;
        if (free_data && item->data != NULL)
            free(item->next->data);
        free(item->next);
        item->next = next;
    }

    return 1;
}

int linked_list_removeAt(struct LinkedListNode *list, int index) {
    if (index == 0) {
        __delete_first_element(list);
        return 1;
    }
    return linked_list_remove(list, &index, linked_list_finder_index);
}

int linked_list_pollLast(struct LinkedListNode *list) {
    struct LinkedListNode *item = linked_list_peekLastParent(list);
    if (item == NULL)
        return 0;

    if (free_data && item->data != NULL)
        free(item->next->data);
    free(item->next);
    item->next = NULL;
    return 1;
}

int linked_list_pop(struct LinkedListNode *list) {
    return linked_list_removeAt(list, 0);
}

int linked_list_push(struct LinkedListNode *node, struct LinkedListNode *list) {
    return linked_list_insertAt(node, list, 0);
}

int linked_list_removeData(struct LinkedListNode *list, const void *data) {
    return linked_list_remove(list, &data, linked_list_finder_data);
}

int linked_list_removeNode(struct LinkedListNode *list, struct LinkedListNode *node) {
    return linked_list_remove(list, &node, linked_list_finder_node);
}

int linked_list_removeRange(struct LinkedListNode *list, int start, int end) {
    int count = 0;
    for (int i = start + 1; i < end; ++i)
        count += linked_list_removeAt(list, start + 1);
    count += linked_list_removeAt(list, start); // start may be 0
    return count;
}

struct LinkedListNode *linked_list_copy(struct LinkedListNode *list) {
    if (list == NULL)
        return NULL;

    struct LinkedListNode *copy = malloc(sizeof(struct LinkedListNode));
    struct LinkedListNode *cur = copy;
    while (list != NULL) {
        cur->data = list->data;
        if (list->next != NULL) {
            cur->next = malloc(sizeof(struct LinkedListNode));
            cur = cur->next;
        }
        list = list->next;
    }
    cur->next = NULL;
    return copy;
}

int linked_list_toArray(struct LinkedListNode *list, void **array, int start, int end) {
    list = linked_list_findByIndex(list, start);
    if (list == NULL)
        return 0;

    int count = 0;
    for (int i = start; i < end && list != NULL; ++i, list = list->next)
        array[count++] = list->data;
    return count;
}