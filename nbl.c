#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "map.h"
#include "token.h"
#include "lexer.h"
#include "node.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char *argv[]) {
    printf("NBL (New Bastiaan Lanuage) Interpreter v0.1\nMade by Bastiaan van der Plaat\n\n");
    if (argc >= 2) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *file_buffer = malloc(file_size);
        fread(file_buffer, 1, file_size, file);
        fclose(file);
        puts("# Tokens:");
        List *tokens = lexer(file_buffer);
        free(file_buffer);
        if (tokens != NULL) {
            ListItem *list_item = tokens->first;
            while (list_item != NULL) {
                printf("- ");
                token_dump(list_item->value);
                list_item = list_item->next;
            }

            puts("\n# Parser:");
            Node *node = parser(tokens);

            list_item = tokens->first;
            while (list_item != NULL) {
                token_free(list_item->value);
                list_item = list_item->next;
            }
            list_free(tokens);

            if (node != NULL) {
                node_dump(node);
                puts("\n# Interpreter:");

                Map *vars = map_new();
                Node *pi = node_new(NODE_TYPE_NUMBER);
                pi->value.number = 3.14159;
                map_set(vars, "pi", pi);
                start_interpreter(node, vars);

                puts("\n# Vars:");
                MapItem *map_item = vars->first;
                while (map_item != NULL) {
                    printf("%s = ", map_item->key);
                    node_dump(map_item->value);
                    putchar('\n');
                    node_free(map_item->value);
                    map_item = map_item->next;
                }
                map_free(vars);

                node_free(node);
            }
        }
    }
    return EXIT_SUCCESS;
}
