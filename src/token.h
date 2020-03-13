#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType {
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_VARIABLE,

    TOKEN_TYPE_ASSIGN,
    TOKEN_TYPE_STOP,

    TOKEN_TYPE_PAREN_LEFT,
    TOKEN_TYPE_PAREN_RIGHT,
    TOKEN_TYPE_ADD,
    TOKEN_TYPE_SUB,
    TOKEN_TYPE_MUL,
    TOKEN_TYPE_DIV,
    TOKEN_TYPE_MOD
} TokenType;

typedef struct Token {
    TokenType type;
    union {
        double number;
        char *string;
    } value;
} Token;

Token *token_new(TokenType type);

void token_dump(Token *token);

void token_free(Token *token);

#endif
