#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "library.h"
#include "constants.h"
#include "map.h"
#include "utils.h"
#include "value.h"

Map *library;

// Core
Value *do_type(List *arguments) {
    Value *value = arguments->first->value;

    if (value->type == VALUE_TYPE_NULL) {
        return value_new_string("null");
    }

    if (value->type == VALUE_TYPE_NUMBER) {
        return value_new_string("number");
    }

    if (value->type == VALUE_TYPE_STRING) {
        return value_new_string("string");
    }

    if (value->type == VALUE_TYPE_BOOLEAN) {
        return value_new_string("boolean");
    }

    if (value->type == VALUE_TYPE_NATIVE_FUNCTION) {
        return value_new_string("function");
    }

    printf("[ERROR] Unkown value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *do_len(List *arguments) {
    Value *value = arguments->first->value;

    if (value->type == VALUE_TYPE_STRING) {
        return value_new_number(strlen(value->value.string));
    }

    printf("[ERROR] Unsupported value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *do_number(List *arguments) {
    Value *value = arguments->first->value;

    if (value->type == VALUE_TYPE_NUMBER) {
        return value_new_number(value->value.number);
    }

    if (value->type == VALUE_TYPE_STRING) {
        return value_new_number(atof(value->value.string));
    }

    printf("[ERROR] Unsupported value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *do_string(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_string(value_to_string(value));
}

Value *do_exit(List *arguments) {
    free_library();
    if (arguments->first != NULL && ((Value *)arguments->first->value)->type == VALUE_TYPE_NUMBER) {
        Value *value = arguments->first->value;
        exit(floor(value->value.number));
    }
    exit(0);
}

// I/O
Value *do_print(List *arguments) {
    ListItem *list_item = arguments->first;
    while (list_item != NULL) {
        char *value_string = value_to_string(list_item->value);
        printf("%s", value_string);
        free(value_string);
        list_item = list_item->next;
    }

    return value_new_null();
}

Value *do_println(List *arguments) {
    Value *value = do_print(arguments);
    printf("\n");
    return value;
}

Value *do_input(List *arguments) {
    value_free(do_print(arguments));

    char line_buffer[BUFFER_SIZE];
    fgets(line_buffer, BUFFER_SIZE, stdin);
    line_buffer[strlen(line_buffer) - 1] = '\0';

    return value_new_string(line_buffer);
}

// Math
Value *do_floor(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(floor(value->value.number));
}

Value *do_round(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(round(value->value.number));
}

Value *do_ceil(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(ceil(value->value.number));
}

Value *do_sin(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(sin(value->value.number));
}

Value *do_asin(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(asin(value->value.number));
}

Value *do_cos(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(cos(value->value.number));
}

Value *do_acos(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(acos(value->value.number));
}

Value *do_tan(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(tan(value->value.number));
}

Value *do_atan(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(atan(value->value.number));
}

Map *get_library(void) {
    library = map_new();

    // Core
    map_set(library, string_copy("type"), value_new_native_function(do_type));
    map_set(library, string_copy("len"),value_new_native_function(do_len));
    map_set(library, string_copy("number"),value_new_native_function(do_number));
    map_set(library, string_copy("string"),value_new_native_function(do_string));
    map_set(library, string_copy("exit"),value_new_native_function(do_exit));

    // I/O
    map_set(library, string_copy("print"),value_new_native_function(do_print));
    map_set(library, string_copy("println"),value_new_native_function(do_println));
    map_set(library, string_copy("input"),value_new_native_function(do_input));

    // Math
    map_set(library, string_copy("pi"),value_new_number(M_PI));
    map_set(library, string_copy("e"),value_new_number(M_E));

    map_set(library, string_copy("floor"),value_new_native_function(do_floor));
    map_set(library, string_copy("round"),value_new_native_function(do_round));
    map_set(library, string_copy("ceil"),value_new_native_function(do_ceil));

    map_set(library, string_copy("sin"),value_new_native_function(do_sin));
    map_set(library, string_copy("asin"),value_new_native_function(do_asin));
    map_set(library, string_copy("cos"),value_new_native_function(do_cos));
    map_set(library, string_copy("acos"),value_new_native_function(do_acos));
    map_set(library, string_copy("tan"), value_new_native_function(do_tan));
    map_set(library, string_copy("atan"), value_new_native_function(do_atan));

    return library;
}

void free_library() {
    MapItem *map_item = library->first;
    while (map_item != NULL) {
        free(map_item->key);
        value_free(map_item->value);
        map_item = map_item->next;
    }
    map_free(library);
}