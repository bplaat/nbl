#include <stdio.h>
#include <stdlib.h>
#include "token.h"

Token *token_new(TokenType type) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    return token;
}

void token_dump(Token *token) {
    if (token->type == TOKEN_TYPE_NUMBER) printf("%.15g\n", token->value.number);
    if (token->type == TOKEN_TYPE_VARIABLE) puts(token->value.string);

    if (token->type == TOKEN_TYPE_PAREN_LEFT) puts("(");
    if (token->type == TOKEN_TYPE_PAREN_RIGHT) puts(")");
    if (token->type == TOKEN_TYPE_STOP) puts("STOP");
    if (token->type == TOKEN_TYPE_SET) puts("=");
    if (token->type == TOKEN_TYPE_ADD) puts("+");
    if (token->type == TOKEN_TYPE_SUB) puts("-");
    if (token->type == TOKEN_TYPE_MUL) puts("*");
    if (token->type == TOKEN_TYPE_DIV) puts("/");
    if (token->type == TOKEN_TYPE_MOD) puts("%");
}

void token_free(Token *token) {
    if (token->type == TOKEN_TYPE_VARIABLE) {
        free(token->value.string);
    }
    free(token);
}
