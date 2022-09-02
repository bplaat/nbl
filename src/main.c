#include "nbl.h"

Value *env_type(List *values) {
    Value *value = list_get(values, 0);
    return value_new_string(value_type_to_string(value->type));
}

Value *env_print(List *values) {
    for (size_t i = 0; i < values->size; i++) {
        char *string = value_to_string(list_get(values, i));
        printf("%s", string);
        free(string);
        if (i != values->size - 1) printf(" ");
    }
    return value_new_null();
}

Value *env_println(List *values) {
    Value *value = env_print(values);
    printf("\n");
    return value;
}

Value *env_exit(List *values) {
    Value *exitCode = list_get(values, 0);
    if (exitCode->type == VALUE_INT) {
        exit(exitCode->integer);
    }
    return value_new_null();
}

Value *env_array_length(List *values) {
    Value *arrayValue = list_get(values, 0);
    return value_new_int(arrayValue->array->size);
}

Value *env_array_push(List *values) {
    Value *arrayValue = list_get(values, 0);
    for (size_t i = 1; i < values->size; i++) {
        list_add(arrayValue->array, list_get(values, i));
    }
    return value_new_int(arrayValue->array->size);
}

Value *env_string_length(List *values) {
    Value *stringValue = list_get(values, 0);
    return value_new_int(strlen(stringValue->string));
}

Map *std_env(void) {
    Map *env = map_new();

    List *type_args = list_new();
    list_add(type_args, argument_new("value", VALUE_ANY, NULL));
    map_set(env, "type", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(type_args, VALUE_ANY, env_type)));

    map_set(env, "print", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(list_new(), VALUE_NULL, env_print)));
    map_set(env, "println", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(list_new(), VALUE_NULL, env_println)));

    List *exit_args = list_new();
    list_add(exit_args, argument_new("exitCode", VALUE_INT, node_new_value(NULL, value_new_int(0))));
    map_set(env, "exit", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(exit_args, VALUE_NULL, env_exit)));

    List *array_length_args = list_new();
    list_add(array_length_args, argument_new("array", VALUE_ARRAY, NULL));
    map_set(env, "array_length", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(array_length_args, VALUE_INT, env_array_length)));

    List *array_push_args = list_new();
    list_add(array_push_args, argument_new("array", VALUE_ARRAY, NULL));
    map_set(env, "array_push", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(array_push_args, VALUE_INT, env_array_push)));

    List *string_length_args = list_new();
    list_add(string_length_args, argument_new("string", VALUE_STRING, NULL));
    map_set(env, "string_length", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(string_length_args, VALUE_INT, env_string_length)));

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

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter\n");
        return EXIT_SUCCESS;
    }

    // Reading and parsing code
    char *text = file_read(argv[1]);
    List *tokens = lexer(text);
    // for (size_t i = 0; i < tokens->size; i++) {
    //     Token *token = list_get(tokens, i);
    //     printf("%s ", token_type_to_string(token->type));
    // }

    Node *node = parser(text, tokens);

    // Start running code
    Interpreter interpreter;
    interpreter.text = text;
    interpreter.env = std_env();
    Scope scope = {.function = &(FunctionScope){.returnValue = NULL},
                   .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                   .block = &(BlockScope){.env = map_new_from_parent(interpreter.env)}};
    interpreter_node(&interpreter, &scope, node);

    map_free(scope.block->env, (MapFreeFunc *)variable_free);
    map_free(interpreter.env, (MapFreeFunc *)variable_free);

    if (scope.function->returnValue != NULL) {
        value_free(scope.function->returnValue);
        if (scope.function->returnValue->type == VALUE_INT) {
            exit(scope.function->returnValue->integer);
        }
    }

    // Cleanup
    node_free(node);
    list_free(tokens, (ListFreeFunc *)token_free);
    free(text);
    return EXIT_SUCCESS;
}
