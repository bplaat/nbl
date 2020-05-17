#ifndef VALUE_H
#define VALUE_H

typedef enum ValueType {
    VALUE_TYPE_NULL,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_STRING
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        double number;
        char *string;
    } value;
} Value;


Value *value_new(ValueType type);

Value *value_new_null(void);

Value *value_new_number(double number);

Value *value_new_string(char *string);

char *value_to_string(Value *value);

Value *value_copy(Value *value);

void value_free(Value *value);

#endif
