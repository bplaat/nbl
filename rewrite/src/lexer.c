#include "lexer.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

NblSource *nbl_source_new(char *path, char *text) {
    NblSource *source = malloc(sizeof(NblSource));
    source->refs = 1;
    source->path = strdup(path);
    source->text = strdup(text);

    // Reverse loop over path to find basename
    char *c = source->path + strlen(source->path);
    while (*c != '/' && c != source->path) c--;
    source->basename = c + 1;

    source->dirname = strndup(source->path, source->basename - 1 - source->path);
    return source;
}

NblSource *nbl_source_ref(NblSource *source) {
    source->refs++;
    return source;
}

void nbl_source_get_line_column(NblSource *source, int32_t offset, int32_t *_line, int32_t *_column,
                                char **_lineStart) {
    int32_t line = 1;
    int32_t column = 1;
    char *lineStart = source->text;
    char *c = source->text;
    while (*c) {
        if (c - source->text == offset) {
            break;
        }
        if (*c == '\r' || *c == '\n') {
            column = 1;
            line++;
            if (*c == '\r') c++;
            c++;
            lineStart = c;
        } else {
            column++;
            c++;
        }
    }
    *_line = line;
    *_column = column;
    *_lineStart = lineStart;
}

void nbl_source_print_error(NblSource *source, int32_t offset, char *format, ...) {
    int32_t line;
    int32_t column;
    char *lineStart;
    nbl_source_get_line_column(source, offset, &line, &column, &lineStart);

    fprintf(stderr, "%s:%d:%d ERROR: ", source->path, line, column);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    char *c = lineStart;
    while (*c != '\n' && *c != '\r' && *c != '\0') c++;
    int32_t lineLength = c - lineStart;

    fprintf(stderr, "\n%4d | ", line);
    fwrite(lineStart, 1, lineLength, stderr);
    fprintf(stderr, "\n     | ");
    for (int32_t i = 0; i < column - 1; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");
}

void nbl_source_release(NblSource *source) {
    source->refs--;
    if (source->refs > 0) return;

    free(source->path);
    free(source->dirname);
    free(source->text);
    free(source);
}

char *nbl_token_type_to_string(NblTokenType type) {
    if (type == NBL_TOKEN_EOF) return "EOF";
    if (type == NBL_TOKEN_KEYWORD) return "keyword";
    if (type == NBL_TOKEN_INT) return "int";
    if (type == NBL_TOKEN_FLOAT) return "float";
    if (type == NBL_TOKEN_STRING) return "string";

    if (type == NBL_TOKEN_TYPE_ANY) return "any";
    if (type == NBL_TOKEN_NULL) return "null";
    if (type == NBL_TOKEN_TYPE_BOOL) return "bool";
    if (type == NBL_TOKEN_TYPE_INT) return "int";
    if (type == NBL_TOKEN_TYPE_FLOAT) return "float";
    if (type == NBL_TOKEN_TYPE_STRING) return "string";
    if (type == NBL_TOKEN_TYPE_ARRAY) return "array";
    if (type == NBL_TOKEN_TYPE_OBJECT) return "object";
    if (type == NBL_TOKEN_TRUE) return "true";
    if (type == NBL_TOKEN_FALSE) return "false";
    if (type == NBL_TOKEN_LET) return "let";
    if (type == NBL_TOKEN_CONST) return "const";
    if (type == NBL_TOKEN_IF) return "if";
    if (type == NBL_TOKEN_ELSE) return "else";
    if (type == NBL_TOKEN_LOOP) return "loop";
    if (type == NBL_TOKEN_WHILE) return "while";
    if (type == NBL_TOKEN_DO) return "do";
    if (type == NBL_TOKEN_FOR) return "for";
    if (type == NBL_TOKEN_CONTINUE) return "contine";
    if (type == NBL_TOKEN_BREAK) return "break";
    if (type == NBL_TOKEN_RETURN) return "return";

    if (type == NBL_TOKEN_LPAREN) return "(";
    if (type == NBL_TOKEN_RPAREN) return ")";
    if (type == NBL_TOKEN_LCURLY) return "{";
    if (type == NBL_TOKEN_RCURLY) return "}";
    if (type == NBL_TOKEN_LBRACKET) return "[";
    if (type == NBL_TOKEN_RBRACKET) return "]";
    if (type == NBL_TOKEN_QUESTION) return "?";
    if (type == NBL_TOKEN_COLON) return ":";
    if (type == NBL_TOKEN_SEMICOLON) return ";";
    if (type == NBL_TOKEN_COMMA) return ",";
    if (type == NBL_TOKEN_ASSIGN) return "=";
    if (type == NBL_TOKEN_ADD) return "+";
    if (type == NBL_TOKEN_ASSIGN_ADD) return "+=";
    if (type == NBL_TOKEN_INC) return "++";
    if (type == NBL_TOKEN_SUB) return "-";
    if (type == NBL_TOKEN_ASSIGN_SUB) return "-=";
    if (type == NBL_TOKEN_DEC) return "--";
    if (type == NBL_TOKEN_MUL) return "*";
    if (type == NBL_TOKEN_ASSIGN_MUL) return "*=";
    if (type == NBL_TOKEN_EXP) return "**";
    if (type == NBL_TOKEN_ASSIGN_EXP) return "**=";
    if (type == NBL_TOKEN_DIV) return "/";
    if (type == NBL_TOKEN_ASSIGN_DIV) return "/=";
    if (type == NBL_TOKEN_MOD) return "%";
    if (type == NBL_TOKEN_ASSIGN_MOD) return "%=";
    if (type == NBL_TOKEN_AND) return "&";
    if (type == NBL_TOKEN_ASSIGN_AND) return "&=";
    if (type == NBL_TOKEN_XOR) return "^";
    if (type == NBL_TOKEN_ASSIGN_XOR) return "^=";
    if (type == NBL_TOKEN_OR) return "|";
    if (type == NBL_TOKEN_ASSIGN_OR) return "|=";
    if (type == NBL_TOKEN_NOT) return "~";
    if (type == NBL_TOKEN_SHL) return "<<";
    if (type == NBL_TOKEN_ASSIGN_SHL) return "<<=";
    if (type == NBL_TOKEN_SHR) return ">>";
    if (type == NBL_TOKEN_ASSIGN_SHR) return ">>=";
    if (type == NBL_TOKEN_EQ) return "==";
    if (type == NBL_TOKEN_NEQ) return "!=";
    if (type == NBL_TOKEN_LT) return "<";
    if (type == NBL_TOKEN_LTEQ) return "<=";
    if (type == NBL_TOKEN_GT) return ">";
    if (type == NBL_TOKEN_GTEQ) return ">=";
    if (type == NBL_TOKEN_LOGICAL_OR) return "||";
    if (type == NBL_TOKEN_LOGICAL_AND) return "&&";
    if (type == NBL_TOKEN_LOGICAL_NOT) return "!";
    return NULL;
}

void nbl_token_free(NblToken *token) {
    if (token->type == NBL_TOKEN_KEYWORD || token->type == NBL_TOKEN_STRING) {
        free(token->string);
    }
}

bool nbl_lexer(char *path, char *text, NblSource **source, NblToken **_tokens, int32_t *tokensSize) {
    NblKeyword keywords[] = {{"any", NBL_TOKEN_TYPE_ANY},
                             {"null", NBL_TOKEN_NULL},
                             {"bool", NBL_TOKEN_TYPE_BOOL},
                             {"int", NBL_TOKEN_TYPE_INT},
                             {"float", NBL_TOKEN_TYPE_FLOAT},
                             {"string", NBL_TOKEN_TYPE_STRING},
                             {"array", NBL_TOKEN_TYPE_ARRAY},
                             {"object", NBL_TOKEN_TYPE_OBJECT},
                             {"true", NBL_TOKEN_TRUE},
                             {"false", NBL_TOKEN_FALSE},
                             {"let", NBL_TOKEN_LET},
                             {"const", NBL_TOKEN_CONST},
                             {"if", NBL_TOKEN_IF},
                             {"else", NBL_TOKEN_ELSE},
                             {"loop", NBL_TOKEN_LOOP},
                             {"while", NBL_TOKEN_WHILE},
                             {"do", NBL_TOKEN_DO},
                             {"for", NBL_TOKEN_FOR},
                             {"continue", NBL_TOKEN_CONTINUE},
                             {"break", NBL_TOKEN_BREAK},
                             {"return", NBL_TOKEN_RETURN}};

    NblKeyword operators[] = {
        {"(", NBL_TOKEN_LPAREN},       {")", NBL_TOKEN_RPAREN},       {"{", NBL_TOKEN_LCURLY},
        {"}", NBL_TOKEN_RCURLY},       {"[", NBL_TOKEN_LBRACKET},     {"]", NBL_TOKEN_RBRACKET},
        {"?", NBL_TOKEN_QUESTION},     {":", NBL_TOKEN_COLON},        {";", NBL_TOKEN_SEMICOLON},
        {",", NBL_TOKEN_COMMA},        {"=", NBL_TOKEN_ASSIGN},       {"+", NBL_TOKEN_ADD},
        {"-", NBL_TOKEN_SUB},          {"*", NBL_TOKEN_MUL},          {"/", NBL_TOKEN_DIV},
        {"%", NBL_TOKEN_MOD},          {"&", NBL_TOKEN_AND},          {"^", NBL_TOKEN_XOR},
        {"|", NBL_TOKEN_OR},           {"~", NBL_TOKEN_NOT},          {"<", NBL_TOKEN_LT},
        {">", NBL_TOKEN_GT},           {"!", NBL_TOKEN_LOGICAL_NOT},  {"++", NBL_TOKEN_INC},
        {"--", NBL_TOKEN_DEC},         {"**", NBL_TOKEN_EXP},         {"+=", NBL_TOKEN_ASSIGN_ADD},
        {"-=", NBL_TOKEN_ASSIGN_SUB},  {"*=", NBL_TOKEN_ASSIGN_MUL},  {"/=", NBL_TOKEN_ASSIGN_DIV},
        {"%=", NBL_TOKEN_ASSIGN_MOD},  {"&=", NBL_TOKEN_ASSIGN_AND},  {"^=", NBL_TOKEN_ASSIGN_XOR},
        {"|=", NBL_TOKEN_ASSIGN_OR},   {"<<", NBL_TOKEN_SHL},         {">>", NBL_TOKEN_SHR},
        {"==", NBL_TOKEN_EQ},          {"!=", NBL_TOKEN_NEQ},         {"<=", NBL_TOKEN_LTEQ},
        {">=", NBL_TOKEN_GTEQ},        {"&&", NBL_TOKEN_LOGICAL_AND}, {"||", NBL_TOKEN_LOGICAL_OR},
        {"**=", NBL_TOKEN_ASSIGN_EXP}, {"<<=", NBL_TOKEN_ASSIGN_SHL}, {">>=", NBL_TOKEN_ASSIGN_SHR},
    };

    *source = nbl_source_new(path, text);

    int32_t capacity = 1024;
    int32_t size = 0;
    NblToken *tokens = malloc(capacity * sizeof(NblToken));
    char *c = text;
    for (;;) {
        tokens[size].offset = c - text;
        if (size == capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(NblToken));
        }

        // EOF
        if (*c == '\0') {
            tokens[size++].type = NBL_TOKEN_EOF;
            break;
        }

        // Comments
        if (*c == '#' || (*c == '/' && *(c + 1) == '/')) {
            while (*c != '\n' && *c != '\r' && *c != '\0') c++;
            continue;
        }
        if (*c == '/' && *(c + 1) == '*') {
            c += 2;
            while (*c != '*' || *(c + 1) != '/') {
                if (*c == '\0') {
                    nbl_source_print_error(*source, c - text, "Unclosed comment block");
                    return false;
                }
                c++;
            }
            c += 2;
            continue;
        }

        // Numbers
        if (*c == '0' && *(c + 1) == 'b') {
            c += 2;
            tokens[size].type = NBL_TOKEN_INT;
            tokens[size++].integer = strtol(c, &c, 2);
            continue;
        }
        if (*c == '0' && (isdigit(*(c + 1)) || *(c + 1) == 'o')) {
            if (*(c + 1) == 'o') c++;
            c++;
            tokens[size].type = NBL_TOKEN_INT;
            tokens[size++].integer = strtol(c, &c, 8);
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            tokens[size].type = NBL_TOKEN_INT;
            tokens[size++].integer = strtol(c, &c, 16);
            continue;
        }
        if (isdigit(*c)) {
            char *ptr = c;
            bool isFloat = false;
            while (isdigit(*c) || *c == '.') {
                if (*c == '.') isFloat = true;
                c++;
            }
            if (isFloat) {
                tokens[size].type = NBL_TOKEN_FLOAT;
                tokens[size++].floating = strtod(ptr, &c);
            } else {
                tokens[size].type = NBL_TOKEN_INT;
                tokens[size++].integer = strtol(ptr, &c, 10);
            }
            continue;
        }

        // Strings
        if (*c == '"' || *c == '\'') {
            char endChar = *c;
            c++;
            char *ptr = c;
            while (*c != endChar) {
                if (*c == '\0') {
                    nbl_source_print_error(*source, c - text, "Unclosed string");
                    return false;
                }
                c++;
            }
            int32_t strsize = c - ptr;
            c++;

            char *string = malloc(strsize + 1);
            int32_t strpos = 0;
            for (int32_t i = 0; i < strsize; i++) {
                if (ptr[i] == '\\') {
                    i++;
                    if (ptr[i] == 'b')
                        string[strpos++] = '\b';
                    else if (ptr[i] == 'f')
                        string[strpos++] = '\f';
                    else if (ptr[i] == 'n')
                        string[strpos++] = '\n';
                    else if (ptr[i] == 'r')
                        string[strpos++] = '\r';
                    else if (ptr[i] == 't')
                        string[strpos++] = '\t';
                    else if (ptr[i] == 'v')
                        string[strpos++] = '\v';
                    else if (ptr[i] == '\'')
                        string[strpos++] = '\'';
                    else if (ptr[i] == '"')
                        string[strpos++] = '"';
                    else if (ptr[i] == '\\')
                        string[strpos++] = '\\';
                    else
                        string[strpos++] = ptr[i];
                } else {
                    string[strpos++] = ptr[i];
                }
            }
            string[strpos] = '\0';
            tokens[size].type = NBL_TOKEN_STRING;
            tokens[size++].string = string;
            continue;
        }

        // Keywords
        if (isalpha(*c) || *c == '_' || *c == '$') {
            char *ptr = c;
            while (isalnum(*c) || *c == '_' || *c == '$') c++;
            size_t strsize = c - ptr;

            bool found = false;
            for (size_t i = 0, j = sizeof(keywords) / sizeof(NblKeyword) - 1; i < sizeof(keywords) / sizeof(NblKeyword);
                 i++, j--) {
                NblKeyword *keyword = &keywords[j];
                size_t keywordSize = strlen(keyword->keyword);
                if (!memcmp(ptr, keyword->keyword, keywordSize) && strsize == keywordSize) {
                    tokens[size++].type = keyword->type;
                    found = true;
                    break;
                }
            }
            if (!found) {
                tokens[size].type = NBL_TOKEN_KEYWORD;
                tokens[size++].string = strndup(ptr, strsize);
            }
            continue;
        }

        // Operators
        bool found = false;
        for (size_t i = 0, j = sizeof(operators) / sizeof(NblKeyword) - 1; i < sizeof(operators) / sizeof(NblKeyword);
             i++, j--) {
            NblKeyword *operator= & operators[j];
            size_t operatorSize = strlen(operator->keyword);
            if (!memcmp(c, operator->keyword, operatorSize)) {
                tokens[size++].type = operator->type;
                c += operatorSize;
                found = true;
                break;
            }
        }
        if (found) continue;

        // Whitespace
        if (*c == ' ' || *c == '\t') {
            c++;
            continue;
        }
        if (*c == '\n' || *c == '\r') {
            if (*c == '\r') c++;
            c++;
            continue;
        }

        nbl_source_print_error(*source, c - text, "Unknown character: %c", *c);
        return false;
    }
    *_tokens = tokens;
    *tokensSize = size;
    return true;
}
