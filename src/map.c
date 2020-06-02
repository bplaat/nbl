#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "utils.h"

Map *map_new(void) {
    Map *map = malloc(sizeof(Map));
    map->first = NULL;
    map->last = NULL;
    return map;
}

void *map_get(Map *map, char *key) {
    for (MapItem *map_item = map->first; map_item != NULL; map_item = map_item->next) {
        if (!strcmp(map_item->key, key)) {
            return map_item->value;
        }
    }

    return NULL;
}

void map_set(Map *map, char *key, void *value) {
    for (MapItem *map_item = map->first; map_item != NULL;  map_item = map_item->next) {
        if (!strcmp(map_item->key, key)) {
            map_item->value = value;
            return;
        }
    }

    MapItem *map_item = malloc(sizeof(MapItem));
    map_item->key = string_copy(key);
    map_item->value = value;
    map_item->next = NULL;
    if (map->first == NULL) {
        map->first = map_item;
    } else {
        map->last->next = map_item;
    }
    map->last = map_item;
}

Map *map_copy(Map *map, void *(*copy_function)(void *value)) {
    Map *new_map = map_new();
    for (MapItem *map_item = map->first; map_item != NULL; map_item = map_item->next) {
        map_set(new_map, map_item->key, map_item->value != NULL ? copy_function(map_item->value) : NULL);
    }
    return new_map;
}

void map_free(Map *map, void (*free_function)(void *value)) {
    MapItem *map_item = map->first;
    while (map_item != NULL) {
        free(map_item->key);
        if (free_function != NULL && map_item->value != NULL) {
            free_function(map_item->value);
        }
        MapItem *next_map_item = map_item->next;
        free(map_item);
        map_item = next_map_item;
    }
    free(map);
}
