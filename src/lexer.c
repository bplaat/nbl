#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "token.h"
#include "lexer.h"

List *lexer(char *text) {
    List *tokens = list_new();
    char *buffer = malloc(512);

    while (*text != 0) {
        if (isdigit(*text) || (*text == '.' && isdigit(*(text + 1)))) {
            char *buffer_current = buffer;
            while (
                isdigit(*text) || *text == '.' || *text == 'e' || *text == 'E' ||
                (*text == '-' && (*(buffer_current - 1) == 'e' || *(buffer_current - 1) == 'E'))
            ) {
                *buffer_current++ = *text++;
            }
            *buffer_current = 0;

            Token *token = token_new(TOKEN_TYPE_NUMBER);
            token->value.number = atof(buffer);
            list_add(tokens, token);
        }

        else if (isalpha(*text) || *text == '_') {
            char *buffer_current = buffer;
            while (isalnum(*text) || *text == '_') {
                *buffer_current++ = *text++;
            }
            *buffer_current = 0;

            Token *token = token_new(TOKEN_TYPE_VARIABLE);
            char *buffer_copy = malloc(strlen(buffer) + 1);
            strcpy(buffer_copy, buffer);
            token->value.string = buffer_copy;
            list_add(tokens, token);
        }

        else if (*text == '=' || *text == ':') {
            list_add(tokens, token_new(TOKEN_TYPE_ASSIGN));
            text++;
        }

        else if (*text == ';' || *text == ',') {
            list_add(tokens, token_new(TOKEN_TYPE_STOP));
            text++;
        }

        else if (*text == '(') {
            list_add(tokens, token_new(TOKEN_TYPE_PAREN_LEFT));
            text++;
        }

        else if (*text == ')') {
            list_add(tokens, token_new(TOKEN_TYPE_PAREN_RIGHT));
            text++;
        }

        else if (*text == '+') {
            list_add(tokens, token_new(TOKEN_TYPE_ADD));
            text++;
        }

        else if (*text == '-') {
            list_add(tokens, token_new(TOKEN_TYPE_SUB));
            text++;
        }

        else if (*text == '*') {
            list_add(tokens, token_new(TOKEN_TYPE_MUL));
            text++;
        }

        else if (*text == '/') {
            list_add(tokens, token_new(TOKEN_TYPE_DIV));
            text++;
        }

        else if (*text == '%') {
            list_add(tokens, token_new(TOKEN_TYPE_MOD));
            text++;
        }

        else if (isspace(*text)) {
            text++;
        }

        else {
            printf("Unexpected character: '%c'\n", *text);
            ListItem *list_item = tokens->first;
            while (list_item != NULL) {
                token_free(list_item->value);
                list_item = list_item->next;
            }
            free(buffer);
            list_free(tokens);
            return NULL;
        }
    }

    free(buffer);
    return tokens;
}
