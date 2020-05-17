#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

#include "list.h"

typedef enum ValueType {
    VALUE_TYPE_NULL,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_STRING,
    VALUE_TYPE_BOOLEAN,
    VALUE_TYPE_NATIVE_FUNCTION
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        double number;

        char *string;

        bool boolean;

        struct Value *(*native_function)(List *list);
    } value;
} Value;


Value *value_new(ValueType type);

Value *value_new_null(void);

Value *value_new_number(double number);

Value *value_new_string(char *string);

Value *value_new_boolean(bool boolean);

Value *value_new_native_function(Value *(*native_function)(List *list));

char *value_to_string(Value *value);

Value *value_copy(Value *value);

void value_free(Value *value);

#endif
