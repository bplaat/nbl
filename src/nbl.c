#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "constants.h"
#include "library.h"
#include "list.h"
#include "token.h"
#include "lexer.h"
#include "node.h"
#include "parser.h"
#include "map.h"
#include "value.h"
#include "interpreter.h"

Map *global_vars_map;

void run(char *text, bool print_result) {
    #ifdef DEBUG
        printf("[DEBUG] Text: %s\n", text);
    #endif

    // Lexer the text
    List *tokens_list = lexer(text);

    // Print tokens list
    ListItem *list_item;
    #ifdef DEBUG
        printf("[DEBUG] Tokens: ");
        list_item = tokens_list->first;
        while (list_item != NULL) {
            char *token_string = token_to_string(list_item->value);
            printf("%s", token_string);
            free(token_string);

            if (list_item->next != NULL) {
                printf(" ");
            }
            list_item = list_item->next;
        }
        printf("\n");
    #endif

    // Parse the tokens list
    List *nodes_list = parser(tokens_list);

    // Print nodes list
    #ifdef DEBUG
        list_item = nodes_list->first;
        while (list_item != NULL) {
            char *node_string = node_to_string(list_item->value);
            printf("[DEBUG] Node: %s\n", node_string);
            free(node_string);
            list_item = list_item->next;
        }
    #endif

    // Interpreter the nodes list
    list_item = nodes_list->first;
    while (list_item != NULL) {
        Value *value = start_interpreter(list_item->value, global_vars_map);
        if (print_result) {
            char *value_string = value_to_string(value);
            printf("%s\n", value_string);
            free(value_string);
        }
        value_free(value);
        list_item = list_item->next;
    }

    // Free tokens list
    list_item = tokens_list->first;
    while (list_item != NULL) {
        token_free(list_item->value);
        list_item = list_item->next;
    }
    list_free(tokens_list);

    // Free nodes list
    list_item = nodes_list->first;
    while (list_item != NULL) {
        node_free(list_item->value);
        list_item = list_item->next;
    }
    list_free(nodes_list);
}

int main(int argc, char **argv) {
    // Get the NBL standard library
    global_vars_map = get_library();

    // When file is given run file
    if (argc >= 2) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *file_buffer = malloc(file_size + 1);
        fread(file_buffer, 1, file_size, file);
        file_buffer[file_size] = '\0';
        fclose(file);

        run(file_buffer, false);
    }

    // Else run REPL loop
    else {
        printf("NBL - New Bastiaan Lanuage Interpreter\n");

        char line_buffer[BUFFER_SIZE];

        for (;;) {
            printf("> ");
            fgets(line_buffer, BUFFER_SIZE, stdin);
            line_buffer[strlen(line_buffer) - 1] = '\0';

            if (line_buffer[0] != '\0') {
                if (!strcmp(line_buffer, ".dump")) {
                    MapItem *global_vars_map_item = global_vars_map->first;
                    while (global_vars_map_item != NULL) {
                        printf("%s = %s\n", global_vars_map_item->key, value_to_string(global_vars_map_item->value));

                        global_vars_map_item = global_vars_map_item->next;
                    }
                    continue;
                }

                if (!strcmp(line_buffer, ".exit")) {
                    break;
                }

                run(line_buffer, true);
            }
        }
    }

    // Free global vars map
    MapItem *global_vars_map_item = global_vars_map->first;
    while (global_vars_map_item != NULL) {
        value_free(global_vars_map_item->value);
        global_vars_map_item = global_vars_map_item->next;
    }
    map_free(global_vars_map);

    return EXIT_SUCCESS;
}
