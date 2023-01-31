#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

// ValueType
char *nbl_value_type_to_string(NblValueType type) {
    if (type == NBL_VALUE_ANY) return "any";
    if (type == NBL_VALUE_NULL) return "null";
    if (type == NBL_VALUE_BOOL) return "bool";
    if (type == NBL_VALUE_INT) return "int";
    if (type == NBL_VALUE_FLOAT) return "float";
    if (type == NBL_VALUE_STRING) return "string";
    if (type == NBL_VALUE_ARRAY) return "array";
    if (type == NBL_VALUE_OBJECT) return "object";
    return NULL;
}

// Value
char *nbl_value_to_string(NblValue value) {
    if (value.type == NBL_VALUE_NULL) return strdup("null");
    if (value.type == NBL_VALUE_BOOL) return strdup(value.integer ? "true" : "false");
    if (value.type == NBL_VALUE_INT) {
        char buffer[256];
        sprintf(buffer, "%lld", value.integer);
        return strdup(buffer);
    }
    if (value.type == NBL_VALUE_FLOAT) {
        char buffer[256];
        sprintf(buffer, "%g", value.floating);
        return strdup(buffer);
    }
    if (value.type == NBL_VALUE_STRING) return strdup(value.string);
    // if (value->type == NBL_VALUE_ARRAY) {
    //     NblList *sb = nbl_list_new();
    //     nbl_list_add(sb, strdup("["));
    //     if (value->array->size > 0) nbl_list_add(sb, strdup(" "));
    //     for (int32_t i = 0; i < value->array->size; i++) {
    //         NblValue item = nbl_value_list_get(value->array, i);
    //         nbl_list_add(sb, nbl_value_to_string(&item));
    //         if (i != value->array->size - 1) nbl_list_add(sb, strdup(", "));
    //     }
    //     if (value->array->size > 0) nbl_list_add(sb, strdup(" "));
    //     nbl_list_add(sb, strdup("]"));
    //     char *string = nbl_list_to_string(sb);
    //     nbl_list_release(sb, free);
    //     return string;
    // }
    // if (value->type == NBL_VALUE_OBJECT) {
    //     NblList *sb = nbl_list_new();
    //     nbl_list_add(sb, strdup("{"));
    //     if (value->object->filled > 0) nbl_list_add(sb, strdup(" "));
    //     bool isFirst = true;
    //     for (int32_t i = 0; i < value->object->capacity; i++) {
    //         if (value->object->keys[i]) {
    //             if (!isFirst) {
    //                 isFirst = false;
    //                 nbl_list_add(sb, strdup(", "));
    //             }
    //             nbl_list_add(sb, strdup(value->object->keys[i]));
    //             nbl_list_add(sb, strdup(": "));
    //             nbl_list_add(sb, nbl_value_to_string(&value->object->values[i]));
    //         }
    //     }
    //     if (value->object->filled > 0) nbl_list_add(sb, strdup(" "));
    //     nbl_list_add(sb, strdup("}"));
    //     char *string = nbl_list_to_string(sb);
    //     nbl_list_release(sb, free);
    //     return string;
    // }
    return NULL;
}

void nbl_value_free(NblValue value) {
    if (value.type == NBL_VALUE_STRING) {
        free(value.string);
    }
    if (value.type == NBL_VALUE_ARRAY) {
        nbl_value_list_release(value.array);
    }
    if (value.type == NBL_VALUE_OBJECT) {
        nbl_value_map_release(value.object);
    }
}

// ValueList
NblValueList *nbl_value_list_new(void) { return nbl_value_list_new_with_capacity(0); }

NblValueList *nbl_value_list_new_with_capacity(int32_t capacity) {
    NblValueList *list = calloc(1, sizeof(NblValueList));
    list->allocated = true;
    list->capacity = capacity;
    nbl_value_list_init(list);
    return list;
}

void nbl_value_list_init(NblValueList *list) {
    list->refs = 1;
    if (list->capacity == 0) list->capacity = 8;
    list->items = malloc(list->capacity * sizeof(NblValue));
}

NblValueList *nbl_value_list_ref(NblValueList *list) {
    list->refs++;
    return list;
}

NblValue nbl_value_list_get(NblValueList *list, int32_t index) {
    if (index < list->size) return list->items[index];
    return (NblValue){.type = NBL_VALUE_NULL};
}

void nbl_value_list_set(NblValueList *list, int32_t index, NblValue item) {
    if (index > list->capacity) {
        while (index > list->capacity) list->capacity <<= 1;
        list->items = realloc(list->items, list->capacity * sizeof(NblValue));
    }
    if (index > list->size) {
        for (int32_t i = list->size; i < index - 1; i++) {
            list->items[i] = (NblValue){.type = NBL_VALUE_NULL};
        }
        list->size = index + 1;
    }
    list->items[index] = item;
}

void nbl_value_list_add(NblValueList *list, NblValue item) {
    if (list->size == list->capacity) {
        list->capacity <<= 1;
        list->items = realloc(list->items, list->capacity * sizeof(NblValue));
    }
    list->items[list->size++] = item;
}

void nbl_value_list_release(NblValueList *list) {
    list->refs--;
    if (list->refs > 0) return;

    for (int32_t i = 0; i < list->size; i++) {
        nbl_value_free(list->items[i]);
    }

    free(list->items);
    if (list->allocated) free(list);
}

// ValueMap
NblValueMap *nbl_value_map_new(void) { return nbl_value_map_new_with_capacity(0); }

NblValueMap *nbl_value_map_new_with_capacity(int32_t capacity) {
    NblValueMap *map = calloc(1, sizeof(NblValueMap));
    map->allocated = true;
    map->capacity = capacity;
    nbl_value_map_init(map);
    return map;
}

void nbl_value_map_init(NblValueMap *map) {
    map->refs = 1;
    if (map->capacity == 0) map->capacity = 8;
    map->keys = calloc(map->capacity, sizeof(char *));
    map->values = calloc(map->capacity, sizeof(NblValue));
}

NblValueMap *nbl_value_map_ref(NblValueMap *map) {
    map->refs++;
    return map;
}

NblValue nbl_value_map_get(NblValueMap *map, char *key) {
    int32_t index = nbl_map_hash(key) & (map->capacity - 1);
    while (map->keys[index]) {
        if (!strcmp(map->keys[index], key)) {
            return map->values[index];
        }
        index = (index + 1) & (map->capacity - 1);
    }
    return (NblValue){.type = NBL_VALUE_NULL};
}

void nbl_value_map_set(NblValueMap *map, char *key, NblValue value) {
    if (map->filled >= map->capacity * 3 / 4) {
        map->capacity <<= 1;
        char **new_keys = calloc(map->capacity, sizeof(char *));
        NblValue *new_values = calloc(map->capacity, sizeof(NblValue));
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

void nbl_value_map_release(NblValueMap *map) {
    map->refs--;
    if (map->refs > 0) return;

    for (int32_t i = 0; i < map->capacity; i++) {
        if (map->keys[i]) free(map->keys[i]);
        nbl_value_free(map->values[i]);
    }

    free(map->keys);
    free(map->values);
    if (map->allocated) free(map);
}
