#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

        else if (isalpha(*text) || *text == '_') {
            char *string_buffer_current = string_buffer;
            while (isalnum(*text) || *text == '_') {
                *string_buffer_current++ = *text++;
            }
            *string_buffer_current = 0;

            if (!strcmp(string_buffer, "null")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_NULL));
            }
            else if (!strcmp(string_buffer, "true")) {
                Token *token = token_new(TOKEN_TYPE_BOOLEAN);
                token->value.boolean = true;
                list_add(tokens_list, token);
            }
            else if (!strcmp(string_buffer, "false")) {
                Token *token = token_new(TOKEN_TYPE_BOOLEAN);
                token->value.boolean = false;
                list_add(tokens_list, token);
            }
            else if (!strcmp(string_buffer, "if")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_IF));
            }
            else if (!strcmp(string_buffer, "else if")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_ELSE_IF));
            }
            else if (!strcmp(string_buffer, "else")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_ELSE));
            }
            else if (!strcmp(string_buffer, "while")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_WHILE));
            }
            else if (!strcmp(string_buffer, "do")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_DO));
            }
            else if (!strcmp(string_buffer, "for")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_FOR));
            }
            else if (!strcmp(string_buffer, "break")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_BREAK));
            }
            else if (!strcmp(string_buffer, "continue")) {
                list_add(tokens_list, token_new(TOKEN_TYPE_CONTINUE));
            }
            // else if (!strcmp(string_buffer, "function")) {
            //     list_add(tokens_list, token_new(TOKEN_TYPE_FUNCTION));
            // }
            // else if (!strcmp(string_buffer, "return")) {
            //     list_add(tokens_list, token_new(TOKEN_TYPE_RETURN));
            // }

            else {
                Token *token = token_new(TOKEN_TYPE_VARIABLE);
                token->value.string = string_copy(string_buffer);
                list_add(tokens_list, token);
            }
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
