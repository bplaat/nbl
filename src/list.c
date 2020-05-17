#include <stdlib.h>

#include "list.h"

List *list_new(void) {
    List *list = malloc(sizeof(List));
    list->first = NULL;
    list->last = NULL;
    return list;
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
}

void list_free(List *list) {
    ListItem *list_item = list->first;
    while (list_item != NULL) {
        ListItem *next_list_item = list_item->next;
        free(list_item);
        list_item = next_list_item;
    }
    free(list);
}
