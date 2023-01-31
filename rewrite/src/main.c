// New Bastiaan Language AST Interpreter v1.1

#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

/*
nblcontext
let const = += ...
if else while do while for
casts ++ --

for in
functions
include
*/

int main(int argc, char **argv) {
    bool verbose = true;

    if (argc == 1) {
        printf("New Bastiaan Language Interpreter 1.1\n");
        for (;;) {
            printf("> ");
            char text[512];
            fgets(text, sizeof(text), stdin);
            if (!strcmp(text, ".exit\n")) {
                break;
            }
            if (text[0] == '\r' || text[0] == '\n') {
                continue;
            }

            NblSource *source;
            NblToken *tokens;
            int32_t tokensSize;
            if (!nbl_lexer("text", text, &source, &tokens, &tokensSize)) {
                continue;
            }

            if (verbose) {
                printf("Tokens: ");
                for (int32_t i = 0; i < tokensSize; i++) {
                    NblToken *token = &tokens[i];
                    printf("%s", nbl_token_type_to_string(token->type));
                    if (i != tokensSize - 1) printf(" ");
                }
                printf("\n");
            }

            NblNode *node = nbl_parser(source, tokens, tokensSize);
            if (!node) continue;
            if (verbose) {
                printf("Node:\n");
                nbl_node_print(node, stdout);
            }

            for (int32_t i = 0; i < tokensSize; i++) {
                NblToken *token = &tokens[i];
                nbl_token_free(token);
            }
            free(tokens);
            nbl_source_release(source);

            // NblValue result = nbl_interpreter(node);
            // if (verbose) {
            //     char *resultString = nbl_value_to_string(&result);
            //     printf("Result: %s\n", resultString);
            //     free(resultString);
            // }
            // nbl_value_free(&result);
            nbl_node_release(node);
        }
        return EXIT_SUCCESS;
    }

    char *text = nbl_file_read(argv[1]);
    NblSource *source;
    NblToken *tokens;
    int32_t tokensSize;
    if (!nbl_lexer(argv[1], text, &source, &tokens, &tokensSize)) {
        return EXIT_FAILURE;
    }
    free(text);

    NblNode *node = nbl_parser(source, tokens, tokensSize);
    if (!node) return EXIT_FAILURE;

    for (int32_t i = 0; i < tokensSize; i++) {
        NblToken *token = &tokens[i];
        nbl_token_free(token);
    }
    free(tokens);
    nbl_source_release(source);

    NblValue result = nbl_interpreter(node);
    char *resultString = nbl_value_to_string(result);
    printf("Result: %s\n", resultString);
    free(resultString);
    nbl_value_free(result);
    nbl_node_release(node);

    return EXIT_SUCCESS;
}
