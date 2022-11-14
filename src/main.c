// New Bastiaan Language AST Interpreter
// Made by Bastiaan van der Plaat
#define NBL_IMPLEMENTATION
#include "nbl.h"

// Standard library
static NblValue *env_print(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    for (size_t i = 0; i < values->size; i++) {
        char *string = nbl_value_to_string(nbl_list_get(values, i));
        printf("%s", string);
        free(string);
        if (i != values->size - 1) printf(" ");
    }
    return nbl_value_new_null();
}

static NblValue *env_println(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *value = env_print(context, this, values);
    printf("\n");
    return value;
}

static NblValue *env_exit(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *exitCode = nbl_list_get(values, 0);
    if (exitCode->type == NBL_VALUE_INT) {
        exit(exitCode->integer);
    }
    return nbl_value_new_null();
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
        NblList *tokens = nbl_lexer("text", command);
        NblParser parser = {.tokens = tokens, .position = 0};
        NblNode *node = nbl_parser_statement(&parser);
        if (node == NULL) {
            continue;
        }

        // Run
        NblValue *returnValue = nbl_interpreter(env, node);
        if (returnValue != NULL) {
            char *string = nbl_value_to_string(returnValue);
            printf("%s\n", string);
            free(string);
            nbl_value_free(returnValue);
        }

        // Cleanup
        nbl_node_free(node);
        nbl_list_free(tokens, (NblListFreeFunc *)nbl_token_free);
    }
    free(command);
    nbl_map_free(env, (NblMapFreeFunc *)nbl_variable_free);
}

// Main
int main(int argc, char **argv) {
    // Create env
    NblMap *env = nbl_std_env();

    NblList *empty_args = nbl_list_new();
    nbl_map_set(env, "print", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(empty_args, NBL_VALUE_NULL, env_print)));
    nbl_map_set(env, "println", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_NULL, env_println)));

    NblList *exit_args = nbl_list_new();
    nbl_list_add(exit_args, nbl_argument_new("exitCode", NBL_VALUE_INT, nbl_node_new_value(NULL, nbl_value_new_int(0))));
    nbl_map_set(env, "exit", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(exit_args, NBL_VALUE_NULL, env_exit)));

    // Run repl when no arguments are given
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter\n");
        repl(env);
        return EXIT_SUCCESS;
    }

    // Or else read file and execute
    char *path = argv[1];
    char *text = nbl_file_read(path);
    if (text == NULL) {
        fprintf(stderr, "Can't read file: %s\n", path);
        return EXIT_FAILURE;
    }
    NblList *tokens = nbl_lexer(path, text);
    // list_foreach(tokens, Token *token, {
    //     printf("%s ", token_type_to_string(token->type));
    // });
    // printf("\n");
    NblNode *node = nbl_parser(tokens, false);

    // Run
    NblList *arguments = nbl_list_new();
    for (int i = 2; i < argc; i++) {
        nbl_list_add(arguments, nbl_value_new_string(argv[i]));
    }
    nbl_map_set(env, "arguments", nbl_variable_new(NBL_VALUE_ARRAY, false, nbl_value_new_array(arguments)));

    NblValue *returnValue = nbl_interpreter(env, node);
    if (returnValue->type == NBL_VALUE_INT) {
        exit(returnValue->integer);
    }
    nbl_value_free(returnValue);

    // Cleanup
    nbl_map_free(env, (NblMapFreeFunc *)nbl_variable_free);
    nbl_node_free(node);
    nbl_list_free(tokens, (NblListFreeFunc *)nbl_token_free);
    free(text);
    return EXIT_SUCCESS;
}
