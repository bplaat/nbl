#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "list.h"
#include "token.h"
#include "utils.h"

List *lexer(char *text) {
    List *tokens_list = list_new();

    char string_buffer[64];

    while (*text != 0) {
        if (isdigit(*text) || (*text == '.' && isdigit(*(text + 1)))) {
            char *string_buffer_current = string_buffer;
            while (
                isdigit(*text) || *text == '.' || *text == 'e' || *text == 'E' ||
                (*text == '-' && (*(string_buffer_current - 1) == 'e' || *(string_buffer_current - 1) == 'E'))
            ) {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = 0;

            Token *token = token_new(TOKEN_TYPE_NUMBER);
            token->value.number = atof(string_buffer);
            list_add(tokens_list, token);
        }

        else if (isalpha(*text) || *text == '_') {
            char *string_buffer_current = string_buffer;
            while (isalnum(*text) || *text == '_') {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = 0;

            Token *token = token_new(TOKEN_TYPE_VARIABLE);
            token->value.string = string_copy(string_buffer);
            list_add(tokens_list, token);
        }

        else if (*text == '=') {
            list_add(tokens_list, token_new(TOKEN_TYPE_ASSIGN));
            text++;
        }

        else if (*text == '(') {
            list_add(tokens_list, token_new(TOKEN_TYPE_LPAREN));
            text++;
        }

        else if (*text == ')') {
            list_add(tokens_list, token_new(TOKEN_TYPE_RPAREN));
            text++;
        }

        else if (*text == '+') {
            list_add(tokens_list, token_new(TOKEN_TYPE_ADD));
            text++;
        }

        else if (*text == '-') {
            list_add(tokens_list, token_new(TOKEN_TYPE_SUB));
            text++;
        }

        else if (*text == '*') {
            if (*(text + 1) == '*') {
                list_add(tokens_list, token_new(TOKEN_TYPE_EXP));
                text += 2;
            }
            else {
                list_add(tokens_list, token_new(TOKEN_TYPE_MUL));
                text++;
            }
        }

        else if (*text == '/') {
            list_add(tokens_list, token_new(TOKEN_TYPE_DIV));
            text++;
        }

        else if (*text == '%') {
            list_add(tokens_list, token_new(TOKEN_TYPE_MOD));
            text++;
        }

        else if (*text == ';') {
            list_add(tokens_list, token_new(TOKEN_TYPE_STOP));
            text++;
        }

        else if (isspace(*text)) {
            text++;
        }

        else {
            printf("[ERROR] Unexpected character: '%c'\n", *text);
            exit(EXIT_FAILURE);
        }
    }

    return tokens_list;
}
