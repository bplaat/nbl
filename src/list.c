#include <stdlib.h>

#include "list.h"

List *list_new(void) {
    List *list = malloc(sizeof(List));
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
    return list;
}

void *list_get(List *list, uint64_t position) {
    uint64_t counter = 0;
    for (ListItem *list_item = list->first; list_item != NULL; list_item = list_item->next) {
        if (counter == position) {
            return list_item->value;
        }
        counter++;
    }
    return NULL;
}

void list_add(List *list, void *value) {
    ListItem *list_item = malloc(sizeof(ListItem));
    list_item->value = value;
    list_item->next = NULL;
    if (list->first == NULL) {
        list->first = list_item;
    } else {
        list->last->next = list_item;
    }
    list->last = list_item;
    list->length++;
}

List *list_copy(List *list, void *(*copy_function)(void *value)) {
    List *new_list = list_new();
    for (ListItem *list_item = list->first; list_item != NULL; list_item = list_item->next) {
        list_add(new_list, list_item->value != NULL ? copy_function(list_item->value) : NULL);
    }
    return new_list;
}

void list_free(List *list, void (*free_function)(void *value)) {
    ListItem *list_item = list->first;
    while (list_item != NULL) {
        if (free_function != NULL && list_item->value != NULL) {
            free_function(list_item->value);
        }
        ListItem *next_list_item = list_item->next;
        free(list_item);
        list_item = next_list_item;
    }
    free(list);
}
