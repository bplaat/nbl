#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "constants.h"
#include "utils.h"

Token *token_new(TokenType type) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    return token;
}

char *token_to_string(Token *token) {
    if (token->type == TOKEN_TYPE_NULL) {
        return string_copy("null");
    }

    if (token->type == TOKEN_TYPE_NUMBER) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "%g", token->value.number);
        return string_buffer;
    }

    if (token->type == TOKEN_TYPE_STRING) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "'%s'", token->value.string);
        return string_buffer;
    }

    if (token->type == TOKEN_TYPE_BOOLEAN) {
        if (token->value.boolean) {
            return string_copy("true");
        } else {
            return string_copy("false");
        }
    }

    if (token->type == TOKEN_TYPE_VARIABLE) {
        return string_copy(token->value.string);
    }

    if (token->type == TOKEN_TYPE_ASSIGN) {
        return string_copy("=");
    }

    if (token->type == TOKEN_TYPE_LPAREN) {
        return string_copy("(");
    }

    if (token->type == TOKEN_TYPE_RPAREN) {
        return string_copy(")");
    }

    if (token->type == TOKEN_TYPE_ADD) {
        return string_copy("+");
    }

    if (token->type == TOKEN_TYPE_SUB) {
        return string_copy("-");
    }

    if (token->type == TOKEN_TYPE_MUL) {
        return string_copy("*");
    }

    if (token->type == TOKEN_TYPE_EXP) {
        return string_copy("**");
    }

    if (token->type == TOKEN_TYPE_DIV) {
        return string_copy("/");
    }

    if (token->type == TOKEN_TYPE_MOD) {
        return string_copy("%");
    }

    if (token->type == TOKEN_TYPE_STOP) {
        return string_copy(";");
    }

    printf("[ERROR] Unkown token type: %d\n", token->type);
    exit(EXIT_FAILURE);
}

void token_free(Token *token) {
    if (token->type == TOKEN_TYPE_VARIABLE) {
        free(token->value.string);
    }
    free(token);
}
