#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "library.h"
#include "value.h"
#include "list.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef M_E
#define M_E (2.7182818284590452354)
#endif

// Language
Value *type_function(List *args) {
    Value *value = list_get(args, 0);

    if (value->type == VALUE_TYPE_NULL) {
        return value_new_string("null");
    }

    if (value->type == VALUE_TYPE_NUMBER) {
        return value_new_string("number");
    }

    if (value->type == VALUE_TYPE_BOOLEAN) {
        return value_new_string("boolean");
    }

    if (value->type == VALUE_TYPE_STRING) {
        return value_new_string("string");
    }

    if (value->type == VALUE_TYPE_NATIVE_FUNCTION) {
        return value_new_string("function");
    }

    fprintf(stderr, "[ERROR] type_function(): Unexpected value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *exit_function(List *args) {
    Value *value = list_get(args, 0);

    if (value != NULL && value->type == VALUE_TYPE_NUMBER) {
        exit(value->value.number);
    }

    exit(0);
}

Value *print_function(List *args) {
    for (ListItem *list_item = args->first; list_item != NULL; list_item = list_item->next) {
        Value *value = list_item->value;
        if (value->type == VALUE_TYPE_STRING) {
            printf("%s", value->value.string);
        } else {
            char *value_string = value_to_string(value);
            printf("%s", value_string);
            free(value_string);
        }
    }
    return value_new_null();
}


Value *println_function(List *args) {
    Value *value = print_function(args);
    printf("\n");
    return value;
}

// Math
Value *floor_function(List *args) {
    Value *x = list_get(args, 0);
    if (x->type == VALUE_TYPE_NUMBER) {
        return value_new_number(floor(x->value.number));
    }
    return value_new_null();
}

Value *round_function(List *args) {
    Value *x = list_get(args, 0);
    if (x->type == VALUE_TYPE_NUMBER) {
        return value_new_number(round(x->value.number));
    }
    return value_new_null();
}

Value *ceil_function(List *args) {
    Value *x = list_get(args, 0);
    if (x->type == VALUE_TYPE_NUMBER) {
        return value_new_number(ceil(x->value.number));
    }
    return value_new_null();
}

void library_load(State *state) {
    // Language
    state_set(state, "type", value_new_native_function(type_function));
    state_set(state, "exit", value_new_native_function(exit_function));
    state_set(state, "print", value_new_native_function(print_function));
    state_set(state, "println", value_new_native_function(println_function));

    // Math
    state_set(state, "pi", value_new_number(M_PI));
    state_set(state, "e", value_new_number(M_E));

    state_set(state, "floor", value_new_native_function(floor_function));
    state_set(state, "round", value_new_native_function(round_function));
    state_set(state, "ceil", value_new_native_function(ceil_function));
}
