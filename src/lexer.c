#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"
#include "constants.h"
#include "list.h"
#include "token.h"
#include "utils.h"

List *lexer(char *text) {
    List *tokens_list = list_new();

    char string_buffer[BUFFER_SIZE];

    while (*text != '\0') {
        if (*text == 'n' && *(text + 1) == 'u' && *(text + 2) == 'l' && *(text + 3) == 'l') {
            list_add(tokens_list, token_new(TOKEN_TYPE_NULL));
            text += 4;
        }

        else if (isdigit(*text) || (*text == '.' && isdigit(*(text + 1)))) {
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

        else if (*text == '\'') {
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
            list_add(tokens_list, token);
        }

        else if (*text == '"') {
            text++;
            char *string_buffer_current = string_buffer;
            while (*text != '"') {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = 0;
            text++;

            Token *token = token_new(TOKEN_TYPE_STRING);
            token->value.string = string_copy(string_buffer);
            list_add(tokens_list, token);
        }

        else if (*text == 't' && *(text + 1) == 'r' && *(text + 2) == 'u' && *(text + 3) == 'e') {
            Token *token = token_new(TOKEN_TYPE_BOOLEAN);
            token->value.boolean = true;
            list_add(tokens_list, token);
            text += 4;
        }

        else if (*text == 'f' && *(text + 1) == 'a' && *(text + 2) == 'l' && *(text + 3) == 's' && *(text + 4) == 'e') {
            Token *token = token_new(TOKEN_TYPE_BOOLEAN);
            token->value.boolean = false;
            list_add(tokens_list, token);
            text += 5;
        }

        else if (*text == 'i' && *(text + 1) == 'f') {
            list_add(tokens_list, token_new(TOKEN_TYPE_IF));
            text += 2;
        }

        else if (*text == 'e' && *(text + 1) == 'l' && *(text + 2) == 's' && *(text + 3) == 'e') {
            if (*(text + 4) == ' ' && *(text + 5) == 'i' && *(text + 6) == 'f') {
                list_add(tokens_list, token_new(TOKEN_TYPE_ELSE_IF));
                text += 7;
            }
            else {
                list_add(tokens_list, token_new(TOKEN_TYPE_ELSE));
                text += 4;
            }
        }

        else if (*text == 'w' && *(text + 1) == 'h' && *(text + 2) == 'i' && *(text + 3) == 'l' && *(text + 4) == 'e') {
            list_add(tokens_list, token_new(TOKEN_TYPE_WHILE));
            text += 5;
        }

        else if (*text == 'd' && *(text + 1) == 'o') {
            list_add(tokens_list, token_new(TOKEN_TYPE_DO));
            text += 2;
        }

        else if (*text == 'f' && *(text + 1) == 'o' && *(text + 2) == 'r') {
            list_add(tokens_list, token_new(TOKEN_TYPE_FOR));
            text += 3;
        }

        else if (*text == 'b' && *(text + 1) == 'r' && *(text + 2) == 'e' && *(text + 3) == 'a' && *(text + 4) == 'k') {
            list_add(tokens_list, token_new(TOKEN_TYPE_BREAK));
            text += 5;
        }

        else if (*text == 'c' && *(text + 1) == 'o' && *(text + 2) == 'n' && *(text + 3) == 't' && *(text + 4) == 'i' && *(text + 5) == 'n' && *(text + 6) == 'u' && *(text + 7) == 'e') {
            list_add(tokens_list, token_new(TOKEN_TYPE_CONTINUE));
            text += 8;
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
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_EQUALS));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_ASSIGN));
                text++;
            }
        }

        else if (*text == '{') {
            list_add(tokens_list, token_new(TOKEN_TYPE_LBLOCK));
            text++;
        }

        else if (*text == '}') {
            list_add(tokens_list, token_new(TOKEN_TYPE_RBLOCK));
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
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_ADD_ASSIGN));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_ADD));
                text++;
            }
        }

        else if (*text == '-') {
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_SUB_ASSIGN));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_SUB));
                text++;
            }
        }

        else if (*text == '*') {
            if (*(text + 1) == '*') {
                if (*(text + 2) == '=') {
                    list_add(tokens_list, token_new(TOKEN_TYPE_EXP_ASSIGN));
                    text += 3;
                } else {
                    list_add(tokens_list, token_new(TOKEN_TYPE_EXP));
                    text += 2;
                }
            }

            else if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_MUL_ASSIGN));
                text += 2;
            }
            else {
                list_add(tokens_list, token_new(TOKEN_TYPE_MUL));
                text++;
            }
        }

        else if (*text == '/') {
            if (*(text + 1) == '*') {
                text += 2;
                while (*text != '*' || *(text + 1) != '/') {
                    text++;
                }
                text += 2;
            }

            else if (*(text + 1) == '/') {
                text += 2;

                while (*text != '\r' && *text != '\n' && *text != '\0') {
                    text++;
                }
            }

            else if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_DIV_ASSIGN));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_DIV));
                text++;
            }
        }

        else if (*text == '%') {
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_MOD_ASSIGN));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_MOD));
                text++;
            }
        }

        else if (*text == '>') {
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_GREATER_EQUALS));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_GREATER));
                text++;
            }
        }

        else if (*text == '<') {
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_LOWER_EQUALS));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_LOWER));
                text++;
            }
        }

        else if (*text == '!') {
            if (*(text + 1) == '=') {
                list_add(tokens_list, token_new(TOKEN_TYPE_NOT_EQUALS));
                text += 2;
            } else {
                list_add(tokens_list, token_new(TOKEN_TYPE_NOT));
                text++;
            }
        }

        else if (*text == '&' && *(text + 1) == '&') {
            list_add(tokens_list, token_new(TOKEN_TYPE_AND));
            text += 2;
        }

        else if (*text == '|' && *(text + 1) == '|') {
            list_add(tokens_list, token_new(TOKEN_TYPE_OR));
            text += 2;
        }

        else if (*text == ',') {
            list_add(tokens_list, token_new(TOKEN_TYPE_COMMA));
            text++;
        }

        else if (*text == ';') {
            list_add(tokens_list, token_new(TOKEN_TYPE_STOP));
            text++;
        }

        else if (*text == '#') {
            text++;

            while (*text != '\r' && *text != '\n' && *text != '\0') {
                text++;
            }
        }

        else if (isspace(*text)) {
            text++;
        }

        else {
            printf("[ERROR] Unexpected character: %c\n", *text);
            exit(EXIT_FAILURE);
        }
    }

    return tokens_list;
}
