#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum ValueType {
    VALUE_TYPE_NULL,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_STRING,
    VALUE_TYPE_BOOLEAN
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        double number;
        char *string;
        bool boolean;
    } value;
} Value;


Value *value_new(ValueType type);

Value *value_new_null(void);

Value *value_new_number(double number);

Value *value_new_string(char *string);

Value *value_new_boolean(bool boolean);

char *value_to_string(Value *value);

Value *value_copy(Value *value);

void value_free(Value *value);

#endif
