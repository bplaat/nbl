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
    TOKEN_TYPE_ADD_ASSIGN,
    TOKEN_TYPE_SUB_ASSIGN,
    TOKEN_TYPE_MUL_ASSIGN,
    TOKEN_TYPE_EXP_ASSIGN,
    TOKEN_TYPE_DIV_ASSIGN,
    TOKEN_TYPE_MOD_ASSIGN,

    TOKEN_TYPE_IF,
    TOKEN_TYPE_ELSE_IF,
    TOKEN_TYPE_ELSE,
    TOKEN_TYPE_WHILE,
    TOKEN_TYPE_DO,
    TOKEN_TYPE_FOR,
    TOKEN_TYPE_BREAK,
    TOKEN_TYPE_CONTINUE,
    // TOKEN_TYPE_FUNCTION,
    // TOKEN_TYPE_RETURN,

    TOKEN_TYPE_LBLOCK,
    TOKEN_TYPE_RBLOCK,
    TOKEN_TYPE_LPAREN,
    TOKEN_TYPE_RPAREN,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_STOP,

    TOKEN_TYPE_ADD,
    TOKEN_TYPE_SUB,
    TOKEN_TYPE_MUL,
    TOKEN_TYPE_EXP,
    TOKEN_TYPE_DIV,
    TOKEN_TYPE_MOD,

    TOKEN_TYPE_EQUALS,
    TOKEN_TYPE_NOT_EQUALS,
    TOKEN_TYPE_GREATER,
    TOKEN_TYPE_GREATER_EQUALS,
    TOKEN_TYPE_LOWER,
    TOKEN_TYPE_LOWER_EQUALS,
    TOKEN_TYPE_NOT,
    TOKEN_TYPE_AND,
    TOKEN_TYPE_OR
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
