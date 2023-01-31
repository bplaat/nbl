#pragma once

#include <stdint.h>

#include "containers.h"

// ValueType
typedef enum NblValueType {
    NBL_VALUE_NULL,
    NBL_VALUE_BOOL,
    NBL_VALUE_INT,
    NBL_VALUE_FLOAT,
    NBL_VALUE_STRING,
    NBL_VALUE_ARRAY,
    NBL_VALUE_OBJECT,
    NBL_VALUE_ANY  // Special type
} NblValueType;

char *nbl_value_type_to_string(NblValueType type);

// Value
typedef struct NblValueList NblValueList;  // Forward define
typedef struct NblValueMap NblValueMap;    // Forward define

typedef struct NblValue {
    NblValueType type;
    union {
        int32_t string_size;
    };
    union {
        int64_t integer;
        double floating;
        char *string;
        NblValueList *array;
        NblValueMap *object;
    };
} NblValue;

char *nbl_value_to_string(NblValue value);

void nbl_value_free(NblValue value);

// ValueList
typedef struct NblValueList {
    bool allocated;
    int32_t refs;
    NblValue *items;
    int32_t capacity;
    int32_t size;
} NblValueList;

NblValueList *nbl_value_list_new(void);

NblValueList *nbl_value_list_new_with_capacity(int32_t capacity);

void nbl_value_list_init(NblValueList *list);

NblValueList *nbl_value_list_ref(NblValueList *list);

NblValue nbl_value_list_get(NblValueList *list, int32_t index);

void nbl_value_list_set(NblValueList *list, int32_t index, NblValue item);

void nbl_value_list_add(NblValueList *list, NblValue item);

void nbl_value_list_release(NblValueList *list);

// ValueMap
typedef struct NblValueMap {
    bool allocated;
    int32_t refs;
    char **keys;
    NblValue *values;
    int32_t capacity;
    int32_t filled;
} NblValueMap;

NblValueMap *nbl_value_map_new(void);

NblValueMap *nbl_value_map_new_with_capacity(int32_t capacity);

void nbl_value_map_init(NblValueMap *map);

NblValueMap *nbl_value_map_ref(NblValueMap *map);

NblValue nbl_value_map_get(NblValueMap *map, char *key);

void nbl_value_map_set(NblValueMap *map, char *key, NblValue value);

void nbl_value_map_release(NblValueMap *map);
