#include <stdlib.h>

#include "value.h"
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

void value_free(Value *value) {
    if (value->type == VALUE_TYPE_STRING) {
        free(value->value.string);
    }
    free(value);
}
