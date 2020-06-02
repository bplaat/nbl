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
    if (token->type == TOKEN_TYPE_NULL) {
        return string_copy("null");
    }

    if (token->type == TOKEN_TYPE_NUMBER) {
        return string_format("%g", token->value.number);
    }

    if (token->type == TOKEN_TYPE_BOOLEAN) {
        return string_copy(token->value.boolean ? "true" : "false");
    }

    if (token->type == TOKEN_TYPE_STRING) {
        return string_format("'%s'", token->value.string);
    }

    if (token->type == TOKEN_TYPE_VARIABLE) {
        return string_copy(token->value.string);
    }

    if (token->type == TOKEN_TYPE_ASSIGN) {
        return string_copy("=");
    }

    if (token->type == TOKEN_TYPE_ADD_ASSIGN) {
        return string_copy("+=");
    }

    if (token->type == TOKEN_TYPE_SUB_ASSIGN) {
        return string_copy("-=");
    }

    if (token->type == TOKEN_TYPE_MUL_ASSIGN) {
        return string_copy("*=");
    }

    if (token->type == TOKEN_TYPE_EXP_ASSIGN) {
        return string_copy("**=");
    }

    if (token->type == TOKEN_TYPE_DIV_ASSIGN) {
        return string_copy("/=");
    }

    if (token->type == TOKEN_TYPE_MOD_ASSIGN) {
        return string_copy("%=");
    }

    if (token->type == TOKEN_TYPE_IF) {
        return string_copy("if");
    }

    if (token->type == TOKEN_TYPE_ELSE_IF) {
        return string_copy("else if");
    }

    if (token->type == TOKEN_TYPE_ELSE) {
        return string_copy("else");
    }

    if (token->type == TOKEN_TYPE_LBLOCK) {
        return string_copy("{");
    }

    if (token->type == TOKEN_TYPE_RBLOCK) {
        return string_copy("}");
    }

    if (token->type == TOKEN_TYPE_LPAREN) {
        return string_copy("(");
    }

    if (token->type == TOKEN_TYPE_RPAREN) {
        return string_copy(")");
    }

    if (token->type == TOKEN_TYPE_COMMA) {
        return string_copy(",");
    }

    if (token->type == TOKEN_TYPE_STOP) {
        return string_copy(";");
    }

    if (token->type == TOKEN_TYPE_END) {
        return string_copy("END");
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

    if (token->type == TOKEN_TYPE_EQUALS) {
        return string_copy("==");
    }

    if (token->type == TOKEN_TYPE_NOT_EQUALS) {
        return string_copy("!=");
    }

    if (token->type == TOKEN_TYPE_GREATER) {
        return string_copy(">");
    }

    if (token->type == TOKEN_TYPE_GREATER_EQUALS) {
        return string_copy(">=");
    }

    if (token->type == TOKEN_TYPE_LOWER) {
        return string_copy("<");
    }

    if (token->type == TOKEN_TYPE_LOWER_EQUALS) {
        return string_copy("<=");
    }

    if (token->type == TOKEN_TYPE_NOT) {
        return string_copy("!");
    }

    if (token->type == TOKEN_TYPE_AND) {
        return string_copy("&&");
    }

    if (token->type == TOKEN_TYPE_OR) {
        return string_copy("||");
    }

    fprintf(stderr, "[ERROR] token_to_string(): Unkown token type: %d\n", token->type);
    exit(EXIT_FAILURE);
}

void token_free(Token *token) {
    if (token->type == TOKEN_TYPE_STRING || token->type == TOKEN_TYPE_VARIABLE) {
        free(token->value.string);
    }

    free(token);
}
