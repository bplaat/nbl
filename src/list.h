#ifndef LIST_H
#define LIST_H

typedef struct ListItem {
    void *value;
    struct ListItem *next;
} ListItem;

typedef struct List {
    ListItem *first;
    ListItem *last;
} List;

List *list_new(void);

void list_add(List *list, void *value);

void list_free(List *list);

#endif
