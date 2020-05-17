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

    printf("[ERROR] Unkown value type\n");
    exit(EXIT_FAILURE);
}

Value *value_copy(Value *value) {
    Value *new_value = malloc(sizeof(Value));
    new_value->type = value->type;
    new_value->value = value->value;
    return new_value;
}

void value_free(Value *value) {
    if (value->type == VALUE_TYPE_STRING) {
        free(value->value.string);
    }
    free(value);
}
