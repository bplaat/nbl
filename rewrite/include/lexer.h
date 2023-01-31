#pragma once

#include <stdbool.h>
#include <stdint.h>

// Source
typedef struct NblSource {
    int32_t refs;
    char *path;
    char *dirname;
    char *basename;
    char *text;
} NblSource;

NblSource *nbl_source_new(char *path, char *text);

NblSource *nbl_source_ref(NblSource *source);

void nbl_source_get_line_column(NblSource *source, int32_t offset, int32_t *line, int32_t *column, char **lineStart);

void nbl_source_print_error(NblSource *source, int32_t offset, char *format, ...);

void nbl_source_release(NblSource *source);

// Token
typedef enum NblTokenType {
    NBL_TOKEN_EOF,
    NBL_TOKEN_KEYWORD,
    NBL_TOKEN_INT,
    NBL_TOKEN_FLOAT,
    NBL_TOKEN_STRING,

    NBL_TOKEN_TYPE_BEGIN,
    NBL_TOKEN_TYPE_ANY,
    NBL_TOKEN_NULL,
    NBL_TOKEN_TYPE_BOOL,
    NBL_TOKEN_TYPE_INT,
    NBL_TOKEN_TYPE_FLOAT,
    NBL_TOKEN_TYPE_STRING,
    NBL_TOKEN_TYPE_ARRAY,
    NBL_TOKEN_TYPE_OBJECT,
    NBL_TOKEN_TYPE_END,
    NBL_TOKEN_TRUE,
    NBL_TOKEN_FALSE,
    NBL_TOKEN_LET,
    NBL_TOKEN_CONST,
    NBL_TOKEN_IF,
    NBL_TOKEN_ELSE,
    NBL_TOKEN_LOOP,
    NBL_TOKEN_WHILE,
    NBL_TOKEN_DO,
    NBL_TOKEN_FOR,
    NBL_TOKEN_CONTINUE,
    NBL_TOKEN_BREAK,
    NBL_TOKEN_RETURN,

    NBL_TOKEN_LPAREN,
    NBL_TOKEN_RPAREN,
    NBL_TOKEN_LCURLY,
    NBL_TOKEN_RCURLY,
    NBL_TOKEN_LBRACKET,
    NBL_TOKEN_RBRACKET,
    NBL_TOKEN_QUESTION,
    NBL_TOKEN_COLON,
    NBL_TOKEN_SEMICOLON,
    NBL_TOKEN_COMMA,
    NBL_TOKEN_ASSIGN,
    NBL_TOKEN_ADD,
    NBL_TOKEN_ASSIGN_ADD,
    NBL_TOKEN_INC,
    NBL_TOKEN_SUB,
    NBL_TOKEN_ASSIGN_SUB,
    NBL_TOKEN_DEC,
    NBL_TOKEN_MUL,
    NBL_TOKEN_ASSIGN_MUL,
    NBL_TOKEN_EXP,
    NBL_TOKEN_ASSIGN_EXP,
    NBL_TOKEN_DIV,
    NBL_TOKEN_ASSIGN_DIV,
    NBL_TOKEN_MOD,
    NBL_TOKEN_ASSIGN_MOD,
    NBL_TOKEN_AND,
    NBL_TOKEN_ASSIGN_AND,
    NBL_TOKEN_XOR,
    NBL_TOKEN_ASSIGN_XOR,
    NBL_TOKEN_OR,
    NBL_TOKEN_ASSIGN_OR,
    NBL_TOKEN_NOT,
    NBL_TOKEN_SHL,
    NBL_TOKEN_ASSIGN_SHL,
    NBL_TOKEN_SHR,
    NBL_TOKEN_ASSIGN_SHR,
    NBL_TOKEN_EQ,
    NBL_TOKEN_NEQ,
    NBL_TOKEN_LT,
    NBL_TOKEN_LTEQ,
    NBL_TOKEN_GT,
    NBL_TOKEN_GTEQ,
    NBL_TOKEN_LOGICAL_AND,
    NBL_TOKEN_LOGICAL_OR,
    NBL_TOKEN_LOGICAL_NOT
} NblTokenType;

char *nbl_token_type_to_string(NblTokenType type);

typedef struct NblToken {
    NblTokenType type;
    int32_t offset;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
} NblToken;

void nbl_token_free(NblToken *token);

typedef struct NblKeyword {
    char *keyword;
    NblTokenType type;
} NblKeyword;

// Lexer
bool nbl_lexer(char *path, char *text, NblSource **source, NblToken **tokens, int32_t *tokensSize);
