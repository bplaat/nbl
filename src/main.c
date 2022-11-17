// New Bastiaan Language AST Interpreter
// Made by Bastiaan van der Plaat
#define NBL_IMPLEMENTATION
#include "nbl.h"

// Standard library
static NblValue *env_print(NblContext *context, NblValue *this, NblList *values) {
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

static NblValue *env_println(NblContext *context, NblValue *this, NblList *values) {
    NblValue *value = env_print(context, this, values);
    printf("\n");
    return value;
}

static NblValue *env_exit(NblContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *exitCode = nbl_list_get(values, 0);
    exit(exitCode->integer);
    return NULL;
}

// Read execute print loop
void repl(NblContext *context) {
    char command[1024];
    for (;;) {
        // Read
        printf("> ");
        fgets(command, 1024, stdin);
        if (!strcmp(command, ".exit\n")) {
            break;
        }
        size_t commandSize = strlen(command);
        if (command[0] == '\r' || command[0] == '\n') {
            continue;
        }
        command[commandSize] = ';';
        command[commandSize + 1] = '\0';

        NblValue *returnValue = nbl_context_eval_text_statement(context, command);
        if (returnValue != NULL) {
            char *string = nbl_value_to_string(returnValue);
            printf("%s\n", string);
            free(string);
            nbl_value_free(returnValue);
        }
    }
}

// Main
int main(int argc, char **argv) {
    // Create env
    NblContext *context = nbl_context_new();

    NblList *empty_args = nbl_list_new();
    nbl_map_set(context->env, "print", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(empty_args, NBL_VALUE_NULL, env_print)));
    nbl_map_set(context->env, "println", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_NULL, env_println)));

    NblList *exit_args = nbl_list_new();
    nbl_list_add(exit_args, nbl_argument_new("exitCode", NBL_VALUE_INT, nbl_node_new_value(NULL, nbl_value_new_int(0))));
    nbl_map_set(context->env, "exit", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(exit_args, NBL_VALUE_NULL, env_exit)));

    // Run repl when no arguments are given
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter\n");
        repl(context);
        nbl_context_free(context);
        return EXIT_SUCCESS;
    }

    // Or else run file
    NblList *arguments = nbl_list_new();
    for (int i = 2; i < argc; i++) {
        nbl_list_add(arguments, nbl_value_new_string(argv[i]));
    }
    nbl_map_set(context->env, "arguments", nbl_variable_new(NBL_VALUE_ARRAY, false, nbl_value_new_array(arguments)));

    NblValue *returnValue = nbl_context_eval_file(context, argv[1]);
    if (returnValue->type == NBL_VALUE_INT) {
        exit(returnValue->integer);
    }
    nbl_value_free(returnValue);
    nbl_context_free(context);
    return EXIT_SUCCESS;
}
