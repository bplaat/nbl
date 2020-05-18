#include <stdio.h>
#include <stdlib.h>

#include "value.h"
#include "constants.h"
#include "utils.h"

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

Value *value_new_string(char *string) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_STRING;
    value->value.string = string_copy(string);
    return value;
}

Value *value_new_boolean(bool boolean) {
    Value *value = malloc(sizeof(Value));
    value->type = VALUE_TYPE_BOOLEAN;
    value->value.boolean = boolean;
    return value;
}

Value *value_new_native_function(Value *(*native_function)(List *list)) {
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
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "%g", value->value.number);
        return string_buffer;
    }

    if (value->type == VALUE_TYPE_STRING) {
        return string_copy(value->value.string);
    }

    if (value->type == VALUE_TYPE_BOOLEAN) {
        if (value->value.boolean) {
            return string_copy("true");
        } else {
            return string_copy("false");
        }
    }

    if (value->type == VALUE_TYPE_NATIVE_FUNCTION) {
        return string_copy("function");
    }

    printf("[ERROR] Unkown value type: %d\n", value->type);
    exit(EXIT_FAILURE);
}

Value *value_copy(Value *value) {
    Value *new_value = malloc(sizeof(Value));
    new_value->type = value->type;
    if (value->type == VALUE_TYPE_STRING) {
        new_value->value.string = string_copy(value->value.string);
    } else {
        new_value->value = value->value;
    }
    return new_value;
}

void value_free(Value *value) {
    if (value->type == VALUE_TYPE_STRING) {
        free(value->value.string);
    }

    free(value);
}
