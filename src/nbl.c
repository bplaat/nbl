#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "map.h"
#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "node.h"
#include "parser.h"
#include "value.h"
#include "state.h"
#include "library.h"
#include "interpreter.h"

List *run(State *state, char *text) {
    // Lexer line buffer
    List *tokens = lexer(text);

    #ifdef DEBUG
    // Print tokens
    printf("[DEBUG] Tokens: ");
    for (ListItem *list_item = tokens->first; list_item != NULL; list_item = list_item->next) {
        char *token_string = token_to_string(list_item->value);
        printf("%s", token_string);
        free(token_string);

        if (list_item->next != NULL) {
            printf(" ");
        }
    }
    printf("\n");
    #endif

    // Parse tokens
    List *nodes = parser(tokens);

    // Free tokens
    list_free(tokens, token_free);

    #ifdef DEBUG
    // Print nodes
    for (ListItem *list_item = nodes->first; list_item != NULL; list_item = list_item->next) {
        char *node_string = node_to_string(list_item->value);
        printf("[DEBUG] Node: %s\n", node_string);
        free(node_string);
    }
    #endif

    // Interpreter and store answers
    List *answers = list_new();
    for (ListItem *list_item = nodes->first; list_item != NULL; list_item = list_item->next) {
        Value *value = interpreter(state, list_item->value);
        if (value != NULL) {
            list_add(answers, value);
        }
    }

    // Free nodes
    list_free(nodes, node_free);

    // Return answers
    return answers;
}

int main(int argc, char **argv) {
    // Load default library
    State *state = state_new();
    library_load(state);

    // Run file
    if (argc >= 2) {
        char *file_buffer = file_read(argv[1]);
        List *answers = run(state, file_buffer);
        list_free(answers, value_free);
        free(file_buffer);
    }

    // REPL
    else {
        printf("New Bastiaan Language\n");

        char line_buffer[512];

        for (;;) {
            // Read string for stdin
            printf("> ");
            if (fgets(line_buffer, sizeof(line_buffer), stdin) == NULL) {
                return EXIT_FAILURE;
            }
            line_buffer[strlen(line_buffer) - 1] = '\0';

            if (line_buffer[0] != '\0') {
                // Check for dump
                if (!strcmp(line_buffer, ".dump")) {
                    printf("State env dump:\n");
                    for (MapItem *map_item = state->env->first; map_item != NULL; map_item = map_item->next) {
                        char *value_string = value_to_string(map_item->value);
                        printf("%s = %s\n", map_item->key, value_string);
                        free(value_string);
                    }
                    continue;
                }

                // Check for exit
                if (!strcmp(line_buffer, ".exit")) {
                    break;
                }

                // Run line and print answers and free
                List *answers = run(state, line_buffer);
                for (ListItem *list_item = answers->first; list_item != NULL; list_item = list_item->next) {
                    char *value_string = value_to_string(list_item->value);
                    printf("%s\n", value_string);
                    free(value_string);
                }
                list_free(answers, value_free);
            }
        }
    }

    // Free state
    state_free(state);

    return EXIT_SUCCESS;
}
