#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "library.h"
#include "constants.h"
#include "map.h"
#include "value.h"

// Types
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
    Map *library = map_new();

    // Types
    map_set(library, "type", value_new_native_function(do_type));
    map_set(library, "len", value_new_native_function(do_len));
    map_set(library, "number", value_new_native_function(do_number));
    map_set(library, "string", value_new_native_function(do_string));

    // I/O
    map_set(library, "print", value_new_native_function(do_print));
    map_set(library, "println", value_new_native_function(do_println));
    map_set(library, "input", value_new_native_function(do_input));

    // Math
    map_set(library, "pi", value_new_number(M_PI));
    map_set(library, "e", value_new_number(M_E));

    map_set(library, "floor", value_new_native_function(do_floor));
    map_set(library, "round", value_new_native_function(do_round));
    map_set(library, "ceil", value_new_native_function(do_ceil));

    map_set(library, "sin", value_new_native_function(do_sin));
    map_set(library, "asin", value_new_native_function(do_asin));
    map_set(library, "cos", value_new_native_function(do_cos));
    map_set(library, "acos", value_new_native_function(do_acos));
    map_set(library, "tan", value_new_native_function(do_tan));
    map_set(library, "atan", value_new_native_function(do_atan));

    return library;
}
