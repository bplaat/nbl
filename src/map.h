#ifndef MAP_H
#define MAP_H

typedef struct MapItem {
    char *key;
    void *value;
    struct MapItem *next;
} MapItem;

typedef struct Map {
    MapItem *first;
    MapItem *last;
} Map;

Map *map_new(void);

void *map_get(Map *map, char *key);

void map_set(Map *map, char *key, void *value);

void map_free(Map *map);

#endif
