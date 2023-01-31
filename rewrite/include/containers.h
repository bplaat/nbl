#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// List
typedef struct NblList {
    bool allocated;
    int32_t refs;
    void **items;
    int32_t capacity;
    int32_t size;
} NblList;

NblList *nbl_list_new(void);

NblList *nbl_list_new_with_capacity(int32_t capacity);

void nbl_list_init(NblList *list);

NblList *nbl_list_ref(NblList *list);

void *nbl_list_get(NblList *list, int32_t index);

void nbl_list_set(NblList *list, int32_t index, void *item);

void nbl_list_add(NblList *list, void *item);

typedef void (*NblListReleaser)(void *);

void nbl_list_release(NblList *list, NblListReleaser releaser);

// Map
typedef struct NblMap {
    bool allocated;
    int32_t refs;
    char **keys;
    void **values;
    int32_t capacity;
    int32_t filled;
} NblMap;

uint32_t nbl_map_hash(char *key);

NblMap *nbl_map_new(void);

NblMap *nbl_map_new_with_capacity(int32_t capacity);

void nbl_map_init(NblMap *map);

NblMap *nbl_map_ref(NblMap *map);

void *nbl_map_get(NblMap *map, char *key);

void nbl_map_set(NblMap *map, char *key, void *value);

typedef void (*NblMapReleaser)(void *);

void nbl_map_release(NblMap *map, NblMapReleaser releaser);
