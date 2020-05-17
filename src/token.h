#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef enum TokenType {
    TOKEN_TYPE_NULL,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_BOOLEAN,
    TOKEN_TYPE_VARIABLE,

    TOKEN_TYPE_ASSIGN,

    TOKEN_TYPE_LPAREN,
    TOKEN_TYPE_RPAREN,

    TOKEN_TYPE_ADD,
    TOKEN_TYPE_SUB,

    TOKEN_TYPE_MUL,
    TOKEN_TYPE_EXP,
    TOKEN_TYPE_DIV,
    TOKEN_TYPE_MOD,

    TOKEN_TYPE_STOP
} TokenType;

typedef struct Token {
    TokenType type;
    union {
        double number;
        char *string;
        bool boolean;
    } value;
} Token;

Token *token_new(TokenType type);

char *token_to_string(Token *token);

void token_free(Token *token);

#endif
