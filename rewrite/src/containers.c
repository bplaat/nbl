#include "containers.h"

#include <stdlib.h>
#include <string.h>

// List
NblList *nbl_list_new(void) { return nbl_list_new_with_capacity(0); }

NblList *nbl_list_new_with_capacity(int32_t capacity) {
    NblList *list = calloc(1, sizeof(NblList));
    list->allocated = true;
    list->capacity = capacity;
    nbl_list_init(list);
    return list;
}

void nbl_list_init(NblList *list) {
    list->refs = 1;
    if (list->capacity == 0) list->capacity = 8;
    list->items = malloc(list->capacity * sizeof(void *));
}

NblList *nbl_list_ref(NblList *list) {
    list->refs++;
    return list;
}

void *nbl_list_get(NblList *list, int32_t index) {
    if (index < list->size) return list->items[index];
    return NULL;
}

void nbl_list_set(NblList *list, int32_t index, void *item) {
    if (index > list->capacity) {
        while (index > list->capacity) list->capacity <<= 1;
        list->items = realloc(list->items, list->capacity * sizeof(void *));
    }
    if (index > list->size) {
        for (int32_t i = list->size; i < index - 1; i++) {
            list->items[i] = NULL;
        }
        list->size = index + 1;
    }
    list->items[index] = item;
}

void nbl_list_add(NblList *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity <<= 1;
        list->items = realloc(list->items, list->capacity * sizeof(void *));
    }
    list->items[list->size++] = item;
}

void nbl_list_release(NblList *list, NblListReleaser releaser) {
    list->refs--;
    if (list->refs > 0) return;

    if (releaser) {
        for (int32_t i = 0; i < list->size; i++) {
            if (list->items[i]) releaser(list->items[i]);
        }
    }
    free(list->items);
    if (list->allocated) free(list);
}

// Map
uint32_t nbl_map_hash(char *key) {
    uint32_t hash = 2166136261;
    while (*key) {
        hash ^= *key++;
        hash *= 16777619;
    }
    return hash;
}

NblMap *nbl_map_new(void) { return nbl_map_new_with_capacity(0); }

NblMap *nbl_map_new_with_capacity(int32_t capacity) {
    NblMap *map = calloc(1, sizeof(NblMap));
    map->allocated = true;
    map->capacity = capacity;
    nbl_map_init(map);
    return map;
}

void nbl_map_init(NblMap *map) {
    map->refs = 1;
    if (map->capacity == 0) map->capacity = 8;
    map->keys = calloc(map->capacity, sizeof(char *));
    map->values = malloc(map->capacity * sizeof(void *));
}

NblMap *nbl_map_ref(NblMap *map) {
    map->refs++;
    return map;
}

void *nbl_map_get(NblMap *map, char *key) {
    int32_t index = nbl_map_hash(key) & (map->capacity - 1);
    while (map->keys[index]) {
        if (!strcmp(map->keys[index], key)) {
            return map->values[index];
        }
        index = (index + 1) & (map->capacity - 1);
    }
    return NULL;
}

void nbl_map_set(NblMap *map, char *key, void *value) {
    if (map->filled >= map->capacity * 3 / 4) {
        map->capacity <<= 1;
        char **new_keys = calloc(map->capacity, sizeof(char *));
        void **new_values = malloc(map->capacity * sizeof(void *));
        for (int32_t i = 0; i < map->capacity >> 1; i++) {
            if (map->keys[i]) {
                int32_t index = nbl_map_hash(map->keys[i]) & (map->capacity - 1);
                while (new_keys[index]) {
                    index = (index + 1) & (map->capacity - 1);
                }
                new_keys[index] = map->keys[i];
                new_values[index] = map->values[i];
            }
        }
        free(map->keys);
        free(map->values);
        map->keys = new_keys;
        map->values = new_values;
    }

    int32_t index = nbl_map_hash(key) & (map->capacity - 1);
    while (map->keys[index]) {
        if (!strcmp(map->keys[index], key)) {
            map->values[index] = value;
            return;
        }
        index = (index + 1) & (map->capacity - 1);
    }

    map->keys[index] = strdup(key);
    map->values[index] = value;
    map->filled++;
}

void nbl_map_release(NblMap *map, NblMapReleaser releaser) {
    map->refs--;
    if (map->refs > 0) return;

    for (int32_t i = 0; i < map->capacity; i++) {
        if (map->keys[i]) free(map->keys[i]);
        if (releaser && map->values[i]) releaser(map->values[i]);
    }

    free(map->keys);
    free(map->values);
    if (map->allocated) free(map);
}
