// New Bastiaan Language Bytecode Interpreter
// Made by Bastiaan van der Plaat
// gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 nbl.c -lm -o nbl && ./nbl

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NBL_DEFINE
#include "nbl.h"

int main(int argc, char **argv) {
    // When no path is given run repl
    if (argc == 1) {
        printf("New Bastiaan Language Bytecode Interpreter\n");
        NBLContext *context = nbl_context_new();
        char line[1024];
        for (;;) {
            printf("> ");
            fgets(line, 1024, stdin);
            if (!strcmp(line, ".exit\n")) {
                break;
            }
            if (line[0] == '\r' || line[0] == '\n') {
                continue;
            }
            size_t lineSize = strlen(line);
            line[lineSize] = ';';
            line[lineSize + 1] = '\0';

            NBLValue result = nbl_context_eval_text(context, line);
            char *resultString = nbl_value_to_string(result);
            printf("%s\n", resultString);
            free(resultString);
        }
        nbl_context_free(context);
        return EXIT_SUCCESS;
    }

    // Otherwise run the file
    NBLContext *context = nbl_context_new();
    NBLValue result = nbl_context_eval_file(context, argv[1]);
    char *resultString = nbl_value_to_string(result);
    printf("%s\n", resultString);
    free(resultString);
    nbl_context_free(context);
    return EXIT_SUCCESS;
}
