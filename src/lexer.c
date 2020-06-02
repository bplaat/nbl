#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "token.h"
#include "utils.h"

List *lexer(char *text) {
    List *tokens = list_new();

    char string_buffer[64];

    while (*text != '\0') {
        if (isdigit(*text) || (*text == '.' && isdigit(*(text + 1)))) {
            char *string_buffer_current = string_buffer;
            while (
                isdigit(*text) || *text == '.' || *text == 'e' || *text == 'E' ||
                (*text == '-' && (*(text - 1) == 'e' || *(text - 1) == 'E'))
            ) {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = '\0';

            Token *token = token_new(TOKEN_TYPE_NUMBER);
            token->value.number = atof(string_buffer);
            list_add(tokens, token);
            continue;
        }

        if (*text == '\'') {
            text++;
            char *string_buffer_current = string_buffer;
            while (*text != '\'') {
                if (*text == '\\') {
                    text++;
                    if (*text == '\\') {
                        *string_buffer_current++ = '\\';
                    }
                    if (*text == '\'') {
                        *string_buffer_current++ = '\'';
                    }
                    if (*text == '"') {
                        *string_buffer_current++ = '"';
                    }
                    text++;
                } else {
                    *string_buffer_current++ = *text++;
                }
            }
            *string_buffer_current = 0;
            text++;

            Token *token = token_new(TOKEN_TYPE_STRING);
            token->value.string = string_copy(string_buffer);
            list_add(tokens, token);
            continue;
        }

        if (*text == '"') {
            text++;
            char *string_buffer_current = string_buffer;
            while (*text != '"') {
                if (*text == '\\') {
                    text++;
                    if (*text == '\\') {
                        *string_buffer_current++ = '\\';
                    }
                    if (*text == '\'') {
                        *string_buffer_current++ = '\'';
                    }
                    if (*text == '"') {
                        *string_buffer_current++ = '"';
                    }
                    text++;
                } else {
                    *string_buffer_current++ = *text++;
                }
            }
            *string_buffer_current = 0;
            text++;

            Token *token = token_new(TOKEN_TYPE_STRING);
            token->value.string = string_copy(string_buffer);
            list_add(tokens, token);
            continue;
        }

        if (isalpha(*text) || *text == '_') {
            char *string_buffer_current = string_buffer;
            while (isalnum(*text) || *text == '_') {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = '\0';

            if (!strcmp(string_buffer, "null")) {
                list_add(tokens, token_new(TOKEN_TYPE_NULL));
                continue;
            }

            if (!strcmp(string_buffer, "true")) {
                Token *token = token_new(TOKEN_TYPE_BOOLEAN);
                token->value.boolean = true;
                list_add(tokens, token);
                continue;
            }

            if (!strcmp(string_buffer, "false")) {
                Token *token = token_new(TOKEN_TYPE_BOOLEAN);
                token->value.boolean = false;
                list_add(tokens, token);
                continue;
            }

            if (!strcmp(string_buffer, "if")) {
                list_add(tokens, token_new(TOKEN_TYPE_IF));
                continue;
            }

            if (!strcmp(string_buffer, "elif")) {
                list_add(tokens, token_new(TOKEN_TYPE_ELSE_IF));
                continue;
            }

            if (!strcmp(string_buffer, "elseif")) {
                list_add(tokens, token_new(TOKEN_TYPE_ELSE_IF));
                continue;
            }

            if (!strcmp(string_buffer, "else")) {
                list_add(tokens, token_new(TOKEN_TYPE_ELSE));
                continue;
            }

            Token *token = token_new(TOKEN_TYPE_VARIABLE);
            token->value.string = string_copy(string_buffer);
            list_add(tokens, token);
            continue;
        }

        if (*text == 'e' && *(text + 1) == 'l' && *(text + 2) == 's' && *(text + 3) == 'e' && *(text + 4) == ' ' && *(text + 5) == 'i' && *(text + 6) == 'f') {
            list_add(tokens, token_new(TOKEN_TYPE_ELSE_IF));
            text += 7;
            continue;
        }

        if (*text == '=') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_EQUALS));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_ASSIGN));
            text++;
            continue;
        }

        if (*text == '{') {
            list_add(tokens, token_new(TOKEN_TYPE_LBLOCK));
            text++;
            continue;
        }

        if (*text == '}') {
            list_add(tokens, token_new(TOKEN_TYPE_RBLOCK));
            text++;
            continue;
        }

        if (*text == '(') {
            list_add(tokens, token_new(TOKEN_TYPE_LPAREN));
            text++;
            continue;
        }

        if (*text == ')') {
            list_add(tokens, token_new(TOKEN_TYPE_RPAREN));
            text++;
            continue;
        }

        if (*text == ',') {
            list_add(tokens, token_new(TOKEN_TYPE_COMMA));
            text++;
            continue;
        }

        if (*text == ';') {
            list_add(tokens, token_new(TOKEN_TYPE_STOP));
            text++;
            continue;
        }

        if (*text == '+') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_ADD_ASSIGN));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_ADD));
            text++;
            continue;
        }

        if (*text == '-') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_SUB_ASSIGN));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_SUB));
            text++;
            continue;
        }

        if (*text == '*') {
            if (*(text + 1) == '*') {
                if (*(text + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_TYPE_EXP_ASSIGN));
                    text += 3;
                    continue;
                }

                list_add(tokens, token_new(TOKEN_TYPE_EXP));
                text += 2;
                continue;
            }

            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_MUL_ASSIGN));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_MUL));
            text++;
            continue;
        }

        if (*text == '/') {
            if (*(text + 1) == '/') {
                text += 2;
                while (*text != '\r' && *text != '\n' && *text != '\0') {
                    text++;
                }
                continue;
            }

            if (*(text + 1) == '*') {
                text += 2;
                while (*text != '*' && *(text + 1) != '/') {
                    text++;
                }
                text += 2;
                continue;
            }

            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_DIV_ASSIGN));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_DIV));
            text++;
            continue;
        }

        if (*text == '%') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_MOD_ASSIGN));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_MOD));
            text++;
            continue;
        }

        if (*text == '>') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_GREATER_EQUALS));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_GREATER));
            text++;
            continue;
        }

        if (*text == '<') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_LOWER_EQUALS));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_LOWER));
            text++;
            continue;
        }

        if (*text == '!') {
            if (*(text + 1) == '=') {
                list_add(tokens, token_new(TOKEN_TYPE_NOT_EQUALS));
                text += 2;
                continue;
            }

            list_add(tokens, token_new(TOKEN_TYPE_NOT));
            text++;
            continue;
        }

        if (*text == '&' && *(text + 1) == '&') {
            list_add(tokens, token_new(TOKEN_TYPE_AND));
            text += 2;
            continue;
        }

        if (*text == '|' && *(text + 1) == '|') {
            list_add(tokens, token_new(TOKEN_TYPE_OR));
            text += 2;
            continue;
        }

        if (*text == '#') {
            text++;
            while (*text != '\r' && *text != '\n' && *text != '\0') {
                text++;
            }
            continue;
        }

        if (isspace(*text)) {
            text++;
            continue;
        }

        fprintf(stderr, "[ERROR] lexer(): Unexpected character: %c\n", *text);
        exit(EXIT_FAILURE);
    }

    list_add(tokens, token_new(TOKEN_TYPE_END));

    return tokens;
}
