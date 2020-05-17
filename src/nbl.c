#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "list.h"
#include "token.h"
#include "lexer.h"
#include "node.h"
#include "parser.h"
#include "map.h"
#include "value.h"
#include "interpreter.h"

Map *global_vars_map;

void run(char *text) {
    #ifdef DEBUG
        printf("[DEBUG] Text: %s\n", text);
    #endif

    // Lexer
    List *tokens_list = lexer(text);
    if (tokens_list != NULL) {
        ListItem *list_item;

        #ifdef DEBUG
            // Print tokens list
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

        // Parser
        List *nodes_list = parser(tokens_list);
        if (nodes_list != NULL) {
            #ifdef DEBUG
                // Print nodes
                list_item = nodes_list->first;
                while (list_item != NULL) {
                    char *node_string = node_to_string(list_item->value);
                    printf("[DEBUG] Node: %s\n", node_string);
                    free(node_string);
                    list_item = list_item->next;
                }
            #endif

            // Interpreter the nodes
            list_item = nodes_list->first;
            while (list_item != NULL) {
                printf("%.15g\n", start_interpreter(list_item->value, global_vars_map));
                list_item = list_item->next;
            }

            // Free nodes
            list_item = nodes_list->first;
            while (list_item != NULL) {
                node_free(list_item->value);
                list_item = list_item->next;
            }
            list_free(nodes_list);
        }

        // Free tokens list
        list_item = tokens_list->first;
        while (list_item != NULL) {
            token_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(tokens_list);
    }
}

int main(int argc, char **argv) {
    // Create global vars map
    global_vars_map = map_new();
    map_set(global_vars_map, "PI", value_new_number(M_PI));
    map_set(global_vars_map, "E", value_new_number(M_E));

    // When file is given run file
    if (argc >= 2) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        uint64_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *file_buffer = malloc(file_size);
        fread(file_buffer, 1, file_size, file);
        fclose(file);

        run(file_buffer);
    }

    // Else run REPL loop
    else {
        printf("NBL - New Bastiaan Lanuage Interpreter\n");

        #define LINE_BUFFER_SIZE 256
        char line_buffer[LINE_BUFFER_SIZE];

        for (;;) {
            printf("> ");
            fgets(line_buffer, LINE_BUFFER_SIZE, stdin);
            line_buffer[strlen(line_buffer) - 1] = '\0';

            if (line_buffer[0] != '\0') {
                if (!strcmp(line_buffer, ".exit")) {
                    break;
                }

                run(line_buffer);
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
