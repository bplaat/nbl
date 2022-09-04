#include <time.h>

#include "nbl.h"

// Fix missing math constants
#ifndef M_E
#define M_E 2.718281828459045
#endif
#ifndef M_LN2
#define M_LN2 0.6931471805599453
#endif
#ifndef M_LN10
#define M_LN10 2.302585092994046
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.4342944819032518
#endif
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865476
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730951
#endif

// Utils
int64_t random_seed;

double random_random(void) {
    double x = sin(random_seed++ * 10000);
    return x - floor(x);
}

// Math
Value *env_math_abs(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    if (x->type == VALUE_INT) {
        return value_new_int(x->integer < 0 ? -x->integer : x->integer);
    }
    if (x->type == VALUE_FLOAT) {
        return value_new_float(x->floating < 0 ? -x->floating : x->floating);
    }
    return value_new_null();
}
Value *env_math_sin(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(sin(x->floating));
}
Value *env_math_cos(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(cos(x->floating));
}
Value *env_math_tan(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(tan(x->floating));
}
Value *env_math_asin(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(asin(x->floating));
}
Value *env_math_acos(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(acos(x->floating));
}
Value *env_math_atan(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(atan(x->floating));
}
Value *env_math_atan2(Value *this, List *values) {
    (void)this;
    Value *y = list_get(values, 0);
    Value *x = list_get(values, 1);
    return value_new_float(atan2(y->floating, x->floating));
}
Value *env_math_pow(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    Value *y = list_get(values, 1);
    return value_new_float(pow(x->floating, y->floating));
}
Value *env_math_sqrt(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(sqrt(x->floating));
}
Value *env_math_floor(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(floor(x->floating));
}
Value *env_math_ceil(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(ceil(x->floating));
}
Value *env_math_round(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(round(x->floating));
}
Value *env_math_min(Value *this, List *values) {
    (void)this;
    Value *first = list_get(values, 0);
    int64_t minInteger;
    double minFloating;
    if (first->type == VALUE_INT) {
        minInteger = first->integer;
        minFloating = first->integer;
    }
    if (first->type == VALUE_FLOAT) {
        minInteger = first->floating;
        minFloating = first->floating;
    }

    bool onlyInteger = true;
    list_foreach(values, Value * value, {
        if (value->type != VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == VALUE_INT) {
                minInteger = MIN(minInteger, value->integer);
            }
            if (value->type == VALUE_FLOAT) {
                minInteger = MIN(minInteger, value->floating);
            }
        }
        if (value->type == VALUE_INT) {
            minFloating = MIN(minFloating, value->integer);
        }
        if (value->type == VALUE_FLOAT) {
            minFloating = MIN(minFloating, value->floating);
        }
    });
    return onlyInteger ? value_new_int(minInteger) : value_new_float(minFloating);
}
Value *env_math_max(Value *this, List *values) {
    (void)this;
    Value *first = list_get(values, 0);
    int64_t maxInteger;
    double maxFloating;
    if (first->type == VALUE_INT) {
        maxInteger = first->integer;
        maxFloating = first->integer;
    }
    if (first->type == VALUE_FLOAT) {
        maxInteger = first->floating;
        maxFloating = first->floating;
    }

    bool onlyInteger = true;
    list_foreach(values, Value * value, {
        if (value->type != VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == VALUE_INT) {
                maxInteger = MAX(maxInteger, value->integer);
            }
            if (value->type == VALUE_FLOAT) {
                maxInteger = MAX(maxInteger, value->floating);
            }
        }
        if (value->type == VALUE_INT) {
            maxFloating = MAX(maxFloating, value->integer);
        }
        if (value->type == VALUE_FLOAT) {
            maxFloating = MAX(maxFloating, value->floating);
        }
    });
    return onlyInteger ? value_new_int(maxInteger) : value_new_float(maxFloating);
}
Value *env_math_exp(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(exp(x->floating));
}
Value *env_math_log(Value *this, List *values) {
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(log(x->floating));
}
Value *env_math_random(Value *this, List *values) {
    (void)this;
    (void)values;
    return value_new_float(random_random());
}

// String
Value *env_string_length(Value *this, List *values) {
    (void)values;
    return value_new_int(strlen(this->string));
}

// Array
Value *env_array_length(Value *this, List *values) {
    (void)values;
    return value_new_int(this->array->size);
}

Value *env_array_push(Value *this, List *values) {
    list_foreach(values, Value *value, {
        list_add(this->array, value_ref(value));
    });
    return value_new_int(this->array->size);
}

// Object
Value *env_object_length(Value *this, List *values) {
    (void)values;
    return value_new_int(this->object->size);
}

Value *env_object_keys(Value *this, List *values) {
    (void)values;
    List *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_new_string(this->object->keys[i]));
    }
    return value_new_array(items);
}

Value *env_object_values(Value *this, List *values) {
    (void)values;
    List *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_retrieve(this->object->values[i]));
    }
    return value_new_array(items);
}

// Root
Value *env_type(Value *this, List *values) {
    (void)this;
    Value *value = list_get(values, 0);
    return value_new_string(value_type_to_string(value->type));
}

Value *env_print(Value *this, List *values) {
    (void)this;
    for (size_t i = 0; i < values->size; i++) {
        char *string = value_to_string(list_get(values, i));
        printf("%s", string);
        free(string);
        if (i != values->size - 1) printf(" ");
    }
    return value_new_null();
}

Value *env_println(Value *this, List *values) {
    Value *value = env_print(this, values);
    printf("\n");
    return value;
}

Value *env_time(Value *this, List *values) {
    (void)this;
    (void)values;
    return value_new_int(time(NULL));
}

Value *env_exit(Value *this, List *values) {
    (void)this;
    Value *exitCode = list_get(values, 0);
    if (exitCode->type == VALUE_INT) {
        exit(exitCode->integer);
    }
    return value_new_null();
}

Map *std_env(void) {
    Map *env = map_new();

    List *empty_args = list_new();

    // Math
    Map *math = map_new();
    map_set(env, "Math", variable_new(VALUE_OBJECT, false, value_new_object(math)));
    map_set(math, "E", value_new_float(M_E));
    map_set(math, "LN2", value_new_float(M_LN2));
    map_set(math, "LN10", value_new_float(M_LN10));
    map_set(math, "LOG2E", value_new_float(M_LOG2E));
    map_set(math, "LOG10E", value_new_float(M_LOG10E));
    map_set(math, "PI", value_new_float(M_PI));
    map_set(math, "SQRT1_2", value_new_float(M_SQRT1_2));
    map_set(math, "SQRT2", value_new_float(M_SQRT2));

    List *math_float_args = list_new();
    list_add(math_float_args, argument_new("x", VALUE_FLOAT, NULL));
    List *math_float_float_args = list_new();
    list_add(math_float_float_args, argument_new("x", VALUE_FLOAT, NULL));
    list_add(math_float_float_args, argument_new("y", VALUE_FLOAT, NULL));
    List *math_float_float_reverse_args = list_new();
    list_add(math_float_float_reverse_args, argument_new("y", VALUE_FLOAT, NULL));
    list_add(math_float_float_reverse_args, argument_new("x", VALUE_FLOAT, NULL));
    map_set(math, "abs", value_new_native_function(math_float_args, VALUE_ANY, env_math_abs));
    map_set(math, "sin", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_sin));
    map_set(math, "cos", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_cos));
    map_set(math, "tan", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_tan));
    map_set(math, "asin", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_asin));
    map_set(math, "acos", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_acos));
    map_set(math, "atan", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_atan));
    map_set(math, "atan2", value_new_native_function(math_float_float_reverse_args, VALUE_FLOAT, env_math_atan2));
    map_set(math, "pow", value_new_native_function(math_float_float_args, VALUE_FLOAT, env_math_pow));
    map_set(math, "sqrt", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_sqrt));
    map_set(math, "floor", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_floor));
    map_set(math, "ceil", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_ceil));
    map_set(math, "round", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_round));
    map_set(math, "min", value_new_native_function(list_ref(empty_args), VALUE_ANY, env_math_min));
    map_set(math, "max", value_new_native_function(list_ref(empty_args), VALUE_ANY, env_math_max));
    map_set(math, "exp", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_exp));
    map_set(math, "log", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_log));
    random_seed = time(NULL);
    map_set(math, "random", value_new_native_function(list_ref(empty_args), VALUE_FLOAT, env_math_random));

    // String
    Map *string = map_new();
    map_set(env, "String", variable_new(VALUE_CLASS, false, value_new_class(string)));
    map_set(string, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_string_length));

    // Array
    Map *array = map_new();
    map_set(env, "Array", variable_new(VALUE_CLASS, false, value_new_class(array)));
    map_set(array, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_array_length));
    map_set(array, "push", value_new_native_function(list_ref(empty_args), VALUE_INT, env_array_push));

    // Object
    Map *object = map_new();
    map_set(env, "Object", variable_new(VALUE_CLASS, false, value_new_class(object)));
    map_set(object, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_object_length));
    map_set(object, "keys", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_keys));
    map_set(object, "values", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_values));

    // Root
    List *type_args = list_new();
    list_add(type_args, argument_new("value", VALUE_ANY, NULL));
    map_set(env, "type", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(type_args, VALUE_ANY, env_type)));

    map_set(env, "print", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(empty_args, VALUE_NULL, env_print)));
    map_set(env, "println", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(list_ref(empty_args), VALUE_NULL, env_println)));

    map_set(env, "time", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(list_ref(empty_args), VALUE_INT, env_time)));

    List *exit_args = list_new();
    list_add(exit_args, argument_new("exitCode", VALUE_INT, node_new_value(NULL, value_new_int(0))));
    map_set(env, "exit", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(exit_args, VALUE_NULL, env_exit)));

    return env;
}

char *file_read(char *path) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(fileSize + 1);
    fileSize = fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

void repl(void) {
    Map *env = std_env();
    char *command = malloc(1024);
    for (;;) {
        // Read
        printf("> ");
        fgets(command, 1024, stdin);
        if (!strcmp(command, ".exit\n")) {
            break;
        }
        size_t realCommandSize = strlen(command);
        command[realCommandSize] = ';';
        command[realCommandSize + 1] = '\0';

        // Parse
        List *tokens = lexer(command);
        Parser parser = {.text = command, .tokens = tokens, .position = 0};
        Node *node = parser_statement(&parser);
        if (node == NULL) {
            continue;
        }

        // Run
        Value *returnValue = interpreter(command, env, node);
        if (returnValue != NULL) {
            char *string = value_to_string(returnValue);
            printf("%s\n", string);
            free(string);
            value_free(returnValue);
        }

        // Cleanup
        node_free(node);
        list_free(tokens, (ListFreeFunc *)token_free);
    }
    free(command);
    map_free(env, (MapFreeFunc *)variable_free);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter\n");
        repl();
        return EXIT_SUCCESS;
    }

    // Read and parse
    char *text = file_read(argv[1]);
    List *tokens = lexer(text);
    // list_foreach(tokens, Token *token, {
    //     printf("%s ", token_type_to_string(token->type));
    // });
    // printf("\n");
    Node *node = parser(text, tokens);

    // Run
    Map *env = std_env();
    Value *returnValue = interpreter(text, env, node);
    if (returnValue->type == VALUE_INT) {
        exit(returnValue->integer);
    }
    value_free(returnValue);

    // Cleanup
    map_free(env, (MapFreeFunc *)variable_free);
    node_free(node);
    list_free(tokens, (ListFreeFunc *)token_free);
    free(text);
    return EXIT_SUCCESS;
}
