#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "utils.h"

Token *token_new(TokenType type) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    return token;
}

char *token_to_string(Token *token) {
    if (token->type == TOKEN_TYPE_NUMBER) {
        char *string_buffer = malloc(64);
        sprintf(string_buffer, "%.15g", token->value.number);
        return string_buffer;
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

    printf("[ERROR] Unkown token type");
    exit(EXIT_FAILURE);
}

void token_free(Token *token) {
    if (token->type == TOKEN_TYPE_VARIABLE) {
        free(token->value.string);
    }
    free(token);
}
