#include <stdio.h>
#include <stdlib.h>

#include "value.h"
#include "utils.h"
#include "list.h"

Value *value_new(ValueType type) {
    Value *value = malloc(sizeof(Value));
    value->type = type;
    return value;
}

Value *value_new_null(void) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_NULL;
    return value;
}

Value *value_new_number(double number) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_NUMBER;
    value->value.number = number;
    return value;
}

Value *value_new_boolean(bool boolean) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_BOOLEAN;
    value->value.boolean = boolean;
    return value;
}

Value *value_new_string(char *string) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_STRING;
    value->value.string = string_copy(string);
    return value;
}

Value *value_new_native_function(Value *(*native_function)(List *args)) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_NATIVE_FUNCTION;
    value->value.native_function = native_function;
    return value;
}


char *value_to_string(Value *value) {
    if (value->type == VALUE_TYPE_NULL) {
        return string_copy("null");
    }

    if (value->type == VALUE_TYPE_NUMBER) {
        return string_format("%g", value->value.number);
    }

    if (value->type == VALUE_TYPE_BOOLEAN) {
        return string_copy(value->value.boolean ? "true" : "false");
    }

    if (value->type == VALUE_TYPE_STRING) {
        return string_copy(value->value.string);
    }

    if (value->type == VALUE_TYPE_NATIVE_FUNCTION) {
        return string_copy("function");
    }

    fprintf(stderr, "[ERROR] value_to_string(): Unexpected value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *value_copy(Value *value) {
    if (value->type == VALUE_TYPE_NULL) {
        return value_new_null();
    }

    if (value->type == VALUE_TYPE_NUMBER) {
        return value_new_number(value->value.number);
    }

    if (value->type == VALUE_TYPE_BOOLEAN) {
        return value_new_boolean(value->value.boolean);
    }

    if (value->type == VALUE_TYPE_STRING) {
        return value_new_string(value->value.string);
    }

    if (value->type == VALUE_TYPE_NATIVE_FUNCTION) {
        return value_new_native_function(value->value.native_function);
    }

    fprintf(stderr, "[ERROR] value_copy(): Unexpected value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

void value_free(Value *value) {
    if (value->type == VALUE_TYPE_STRING) {
        free(value->value.string);
    }

    free(value);
}
