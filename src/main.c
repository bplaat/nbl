// New Bastiaan Language AST Interpreter
// Made by Bastiaan van der Plaat
#define NBL_IMPLEMENTATION
#include "nbl.h"

// Standard library
static NblValue *env_print(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    for (size_t i = 0; i < values->size; i++) {
        char *string = value_to_string(list_get(values, i));
        printf("%s", string);
        free(string);
        if (i != values->size - 1) printf(" ");
    }
    return value_new_null();
}

static NblValue *env_println(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *value = env_print(context, this, values);
    printf("\n");
    return value;
}

static NblValue *env_exit(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *exitCode = list_get(values, 0);
    if (exitCode->type == NBL_VALUE_INT) {
        exit(exitCode->integer);
    }
    return value_new_null();
}

// Read execute print loop
void repl(NblMap *env) {
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
        NblList *tokens = lexer("text", command);
        NblParser parser = {.tokens = tokens, .position = 0};
        NblNode *node = parser_statement(&parser);
        if (node == NULL) {
            continue;
        }

        // Run
        NblValue *returnValue = interpreter(env, node);
        if (returnValue != NULL) {
            char *string = value_to_string(returnValue);
            printf("%s\n", string);
            free(string);
            value_free(returnValue);
        }

        // Cleanup
        node_free(node);
        list_free(tokens, (NblListFreeFunc *)token_free);
    }
    free(command);
    map_free(env, (NblMapFreeFunc *)variable_free);
}

// Main
int main(int argc, char **argv) {
    // Create env
    NblMap *env = std_env();

    NblList *empty_args = list_new();
    map_set(env, "print", variable_new(NBL_VALUE_NATIVE_FUNCTION, false, value_new_native_function(empty_args, NBL_VALUE_NULL, env_print)));
    map_set(env, "println", variable_new(NBL_VALUE_NATIVE_FUNCTION, false, value_new_native_function(list_ref(empty_args), NBL_VALUE_NULL, env_println)));

    NblList *exit_args = list_new();
    list_add(exit_args, argument_new("exitCode", NBL_VALUE_INT, node_new_value(NULL, value_new_int(0))));
    map_set(env, "exit", variable_new(NBL_VALUE_NATIVE_FUNCTION, false, value_new_native_function(exit_args, NBL_VALUE_NULL, env_exit)));

    // Run repl when no arguments are given
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter\n");
        repl(env);
        return EXIT_SUCCESS;
    }

    // Or else read file and execute
    char *path = argv[1];
    char *text = file_read(path);
    if (text == NULL) {
        fprintf(stderr, "Can't read file: %s\n", path);
        return EXIT_FAILURE;
    }
    NblList *tokens = lexer(path, text);
    // list_foreach(tokens, Token *token, {
    //     printf("%s ", token_type_to_string(token->type));
    // });
    // printf("\n");
    NblNode *node = parser(tokens, false);

    // Run
    NblList *arguments = list_new();
    for (int i = 2; i < argc; i++) {
        list_add(arguments, value_new_string(argv[i]));
    }
    map_set(env, "arguments", variable_new(NBL_VALUE_ARRAY, false, value_new_array(arguments)));

    NblValue *returnValue = interpreter(env, node);
    if (returnValue->type == NBL_VALUE_INT) {
        exit(returnValue->integer);
    }
    value_free(returnValue);

    // Cleanup
    map_free(env, (NblMapFreeFunc *)variable_free);
    node_free(node);
    list_free(tokens, (NblListFreeFunc *)token_free);
    free(text);
    return EXIT_SUCCESS;
}
