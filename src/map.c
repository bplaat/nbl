#include <stdlib.h>
#include <string.h>

#include "map.h"

Map *map_new(void) {
    Map *map = malloc(sizeof(Map));
    map->first = NULL;
    map->last = NULL;
    return map;
}

void *map_get(Map *map, char *key) {
    MapItem *map_item = map->first;
    while (map_item != NULL) {
        if (!strcmp(map_item->key, key)) {
            return map_item->value;
        }
        map_item = map_item->next;
    }
    return NULL;
}

void map_set(Map *map, char *key, void *value) {
    MapItem *map_item = map->first;
    while (map_item != NULL) {
        if (!strcmp(map_item->key, key)) {
            map_item->value = value;
            return;
        }
        map_item = map_item->next;
    }

    map_item = malloc(sizeof(MapItem));
    map_item->key = key;
    map_item->value = value;
    map_item->next = NULL;
    if (map->first == NULL) {
        map->first = map_item;
    } else {
        map->last->next = map_item;
    }
    map->last = map_item;
}

void map_free(Map *map) {
    MapItem *map_item = map->first;
    while (map_item != NULL) {
        MapItem *next_map_item = map_item->next;
        free(map_item);
        map_item = next_map_item;
    }
    free(map);
}
