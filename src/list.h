#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct ListItem {
    void *value;
    struct ListItem *next;
} ListItem;

typedef struct List {
    ListItem *first;
    ListItem *last;
    uint64_t length;
} List;

List *list_new(void);

void *list_get(List *list, uint64_t position);

void list_add(List *list, void *value);

List *list_copy(List *list, void *(*copy_function)(void *value));

void list_free(List *list, void (*free_function)(void *value));

#endif
