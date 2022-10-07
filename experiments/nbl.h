// New Bastiaan Language Bytecode Interpreter
// Made by Bastiaan van der Plaat

/*
This interpreter is a work in progress, there is a lot that needs to be done before it is as complete as the AST interpreter:
- fix global vars
- local vars
- local vars lexical scoping for closures
- for
- functions | call
- const, let vars (in parser)
- basic std lib (print, exit, Math, Date)

- arrays
- objects
- for in
- classes inheritence instanceof
- exceptions
- rest std lib
- includes
- run test suite again
- fix mem leaks
*/

#ifndef NBL_H
#define NBL_H

// ################################# Header #################################

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Pollyfills
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char *strdup(const char *str);

char *strndup(const char *str, size_t size);

// Utils
char *nbl_file_read(char *path);

// Lexer
typedef enum NBLTokenType {
    NBL_TOKEN_EOF,
    NBL_TOKEN_UNKNOWN,
    NBL_TOKEN_LPAREN,
    NBL_TOKEN_RPAREN,
    NBL_TOKEN_LCURLY,
    NBL_TOKEN_RCURLY,
    NBL_TOKEN_QUESTION,
    NBL_TOKEN_COLON,
    NBL_TOKEN_COMMA,
    NBL_TOKEN_SEMICOLON,

    NBL_TOKEN_IF,
    NBL_TOKEN_ELSE,
    NBL_TOKEN_LOOP,
    NBL_TOKEN_WHILE,
    NBL_TOKEN_DO,
    NBL_TOKEN_FOR,

    NBL_TOKEN_KEYWORD,
    NBL_TOKEN_NULL,
    NBL_TOKEN_TRUE,
    NBL_TOKEN_FALSE,
    NBL_TOKEN_INT,
    NBL_TOKEN_FLOAT,
    NBL_TOKEN_STRING,

    NBL_TOKEN_ASSIGN,
    NBL_TOKEN_ADD,
    NBL_TOKEN_SUB,
    NBL_TOKEN_MUL,
    NBL_TOKEN_EXP,
    NBL_TOKEN_DIV,
    NBL_TOKEN_MOD,
    NBL_TOKEN_AND,
    NBL_TOKEN_XOR,
    NBL_TOKEN_OR,
    NBL_TOKEN_NOT,
    NBL_TOKEN_SHL,
    NBL_TOKEN_SHR,
    NBL_TOKEN_EQ,
    NBL_TOKEN_NEQ,
    NBL_TOKEN_LT,
    NBL_TOKEN_LTEQ,
    NBL_TOKEN_GT,
    NBL_TOKEN_GTEQ,
    NBL_TOKEN_LOGICAL_AND,
    NBL_TOKEN_LOGICAL_OR,
    NBL_TOKEN_LOGICAL_NOT,
} NBLTokenType;

typedef struct NBLToken {
    NBLTokenType type;
    int32_t line;
    int32_t column;
    union {
        char character;
        int64_t integer;
        double floating;
        char *string;
    };
} NBLToken;

char *nbl_token_type_to_string(NBLTokenType type);

typedef struct NBLKeyword {
    char *keyword;
    NBLTokenType type;
} NBLKeyword;

NBLToken *nbl_lexer(char *text, size_t *tokensSize);

// Value
typedef enum NBLValueType { NBL_VALUE_NULL, NBL_VALUE_BOOL, NBL_VALUE_INT, NBL_VALUE_FLOAT, NBL_VALUE_STRING } NBLValueType;

typedef struct NBLValue {
    NBLValueType type;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
} NBLValue;

char *nbl_value_to_string(NBLValue value);

// Opcode
typedef enum NBLOpcodeType {
    NBL_OPCODE_DUP,
    NBL_OPCODE_POP,

    NBL_OPCODE_CONST,
    NBL_OPCODE_LOAD,
    NBL_OPCODE_STORE,
    NBL_OPCODE_RET,

    NBL_OPCODE_JMP,
    NBL_OPCODE_JZ,
    NBL_OPCODE_JNZ,

    NBL_OPCODE_NEG,
    NBL_OPCODE_NOT,
    NBL_OPCODE_LOGICAL_NOT,

    NBL_OPCODE_ADD,
    NBL_OPCODE_SUB,
    NBL_OPCODE_MUL,
    NBL_OPCODE_EXP,
    NBL_OPCODE_DIV,
    NBL_OPCODE_MOD,
    NBL_OPCODE_AND,
    NBL_OPCODE_XOR,
    NBL_OPCODE_OR,
    NBL_OPCODE_SHL,
    NBL_OPCODE_SHR,
    NBL_OPCODE_EQ,
    NBL_OPCODE_NEQ,
    NBL_OPCODE_LT,
    NBL_OPCODE_LTEQ,
    NBL_OPCODE_GT,
    NBL_OPCODE_GTEQ,
    NBL_OPCODE_LOGICAL_AND,
    NBL_OPCODE_LOGICAL_OR
} NBLOpcodeType;

// Var
typedef struct NBLVar {
    char *name;
    NBLValue value;
} NBLVar;

// Env
typedef struct NBLEnv {
    size_t varsCapacity;
    size_t varsSize;
    NBLVar *vars;
} NBLEnv;

// Module
typedef struct NBLModule {
    size_t opcodesCapacity;
    size_t opcodesSize;
    uint8_t *opcodes;
    size_t constsCapacity;
    size_t constsSize;
    NBLValue *consts;
    size_t stackSize;
} NBLModule;

NBLValue nbl_module_run(NBLModule *module, NBLEnv *env);

void nbl_module_dump(NBLModule *module);

// Parser
typedef struct NBLParser {
    NBLToken *tokens;
    size_t tokensSize;
    int32_t position;

    NBLEnv *env;
    NBLModule *module;
} NBLParser;

NBLModule *nbl_parser(NBLToken *tokens, size_t tokensSize, NBLEnv *env);

void nbl_parser_eat(NBLParser *parser, NBLTokenType type);

void nbl_parser_program(NBLParser *parser);
void nbl_parser_block(NBLParser *parser);
void nbl_parser_statement(NBLParser *parser);
void nbl_parser_assigns(NBLParser *parser);
void nbl_parser_assign(NBLParser *parser);
void nbl_parser_tenary(NBLParser *parser);
void nbl_parser_logical(NBLParser *parser);
void nbl_parser_equality(NBLParser *parser);
void nbl_parser_relational(NBLParser *parser);
void nbl_parser_bitwise(NBLParser *parser);
void nbl_parser_shift(NBLParser *parser);
void nbl_parser_add(NBLParser *parser);
void nbl_parser_mul(NBLParser *parser);
void nbl_parser_unary(NBLParser *parser);
void nbl_parser_primary(NBLParser *parser);

// Context
typedef struct NBLContext {
    NBLEnv env;
} NBLContext;

NBLContext *nbl_context_new(void);

NBLValue nbl_context_eval_file(NBLContext *context, char *path);

NBLValue nbl_context_eval_text(NBLContext *context, char *text);

void nbl_context_free(NBLContext *context);

// ################################# Implementation #################################
#ifdef NBL_DEFINE

#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Pollyfills
char *strdup(const char *str) { return strndup(str, strlen(str)); }

char *strndup(const char *str, size_t size) {
    char *copy = malloc(size + 1);
    if (copy == NULL) return NULL;
    memcpy(copy, str, size);
    copy[size] = '\0';
    return copy;
}

// Utils
char *nbl_file_read(char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(fileSize + 1);
    fileSize = fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

// Lexer
char *nbl_token_type_to_string(NBLTokenType type) {
    if (type == NBL_TOKEN_EOF) return "EOF";
    if (type == NBL_TOKEN_UNKNOWN) return "Unknown character";
    if (type == NBL_TOKEN_LPAREN) return "(";
    if (type == NBL_TOKEN_RPAREN) return ")";
    if (type == NBL_TOKEN_LCURLY) return "{";
    if (type == NBL_TOKEN_RCURLY) return "}";
    if (type == NBL_TOKEN_QUESTION) return "?";
    if (type == NBL_TOKEN_COLON) return ":";
    if (type == NBL_TOKEN_COMMA) return ",";
    if (type == NBL_TOKEN_SEMICOLON) return ";";

    if (type == NBL_TOKEN_IF) return "if";
    if (type == NBL_TOKEN_ELSE) return "else";
    if (type == NBL_TOKEN_LOOP) return "loop";
    if (type == NBL_TOKEN_WHILE) return "while";
    if (type == NBL_TOKEN_DO) return "do";
    if (type == NBL_TOKEN_FOR) return "for";

    if (type == NBL_TOKEN_KEYWORD) return "keyword";
    if (type == NBL_TOKEN_NULL) return "null";
    if (type == NBL_TOKEN_TRUE) return "true";
    if (type == NBL_TOKEN_FALSE) return "false";
    if (type == NBL_TOKEN_INT) return "int";
    if (type == NBL_TOKEN_FLOAT) return "float";
    if (type == NBL_TOKEN_STRING) return "string";

    if (type == NBL_TOKEN_ASSIGN) return "=";
    if (type == NBL_TOKEN_ADD) return "+";
    if (type == NBL_TOKEN_SUB) return "-";
    if (type == NBL_TOKEN_MUL) return "*";
    if (type == NBL_TOKEN_EXP) return "**";
    if (type == NBL_TOKEN_DIV) return "/";
    if (type == NBL_TOKEN_MOD) return "%";
    if (type == NBL_TOKEN_AND) return "&";
    if (type == NBL_TOKEN_XOR) return "^";
    if (type == NBL_TOKEN_OR) return "|";
    if (type == NBL_TOKEN_NOT) return "~";
    if (type == NBL_TOKEN_SHL) return "<<";
    if (type == NBL_TOKEN_SHR) return ">>";
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

NBLToken *nbl_lexer(char *text, size_t *tokensSize) {
    size_t capacity = 1024;
    NBLToken *tokens = malloc(capacity * sizeof(NBLToken));
    size_t size = 0;

    NBLKeyword keywords[] = {{"if", NBL_TOKEN_IF},   {"else", NBL_TOKEN_ELSE}, {"loop", NBL_TOKEN_LOOP}, {"while", NBL_TOKEN_WHILE}, {"do", NBL_TOKEN_DO},
                             {"for", NBL_TOKEN_FOR}, {"null", NBL_TOKEN_NULL}, {"true", NBL_TOKEN_TRUE}, {"false", NBL_TOKEN_FALSE}};
    char *c = text;
    int32_t line = 1;
    char *lineStart = c;
    for (;;) {
        tokens[size].line = line;
        tokens[size].column = c - lineStart + 1;

        if (size == capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(NBLToken));
        }

        // EOF
        if (*c == '\0') {
            tokens[size++].type = NBL_TOKEN_EOF;
            break;
        }

        // Comments
        if (*c == '#') {
            while (*c != '\n' && *c != '\r') c++;
            continue;
        }
        if (*c == '/' && *(c + 1) == '/') {
            while (*c != '\n' && *c != '\r') c++;
            continue;
        }
        if (*c == '/' && *(c + 1) == '*') {
            c += 2;
            while (*c != '*' || *(c + 1) != '/') {
                if (*c == '\n' || *c == '\r') {
                    if (*c == '\r') c++;
                    c++;
                    line++;
                    lineStart = c;
                    continue;
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
            char *start = c;
            bool isFloat = false;
            while (isdigit(*c) || *c == '.') {
                if (*c == '.') isFloat = true;
                c++;
            }
            if (isFloat) {
                tokens[size].type = NBL_TOKEN_FLOAT;
                tokens[size++].floating = strtod(start, &c);
            } else {
                tokens[size].type = NBL_TOKEN_INT;
                tokens[size++].integer = strtol(start, &c, 10);
            }
            continue;
        }

        // Strings
        if (*c == '"' || *c == '\'') {
            char endChar = *c;
            c++;
            char *ptr = c;
            while (*c != endChar) c++;
            size_t strsize = c - ptr;
            c++;

            char *string = malloc(strsize + 1);
            int32_t strpos = 0;
            for (size_t i = 0; i < strsize; i++) {
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
            for (size_t i = 0; i < sizeof(keywords) / sizeof(NBLKeyword); i++) {
                NBLKeyword *keyword = &keywords[i];
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

        // Syntax
        if (*c == '(') {
            tokens[size++].type = NBL_TOKEN_LPAREN;
            c++;
            continue;
        }
        if (*c == ')') {
            tokens[size++].type = NBL_TOKEN_RPAREN;
            c++;
            continue;
        }
        if (*c == '{') {
            tokens[size++].type = NBL_TOKEN_LCURLY;
            c++;
            continue;
        }
        if (*c == '}') {
            tokens[size++].type = NBL_TOKEN_RCURLY;
            c++;
            continue;
        }
        if (*c == '?') {
            tokens[size++].type = NBL_TOKEN_QUESTION;
            c++;
            continue;
        }
        if (*c == ':') {
            tokens[size++].type = NBL_TOKEN_COLON;
            c++;
            continue;
        }
        if (*c == ',') {
            tokens[size++].type = NBL_TOKEN_COMMA;
            c++;
            continue;
        }
        if (*c == ';') {
            tokens[size++].type = NBL_TOKEN_SEMICOLON;
            c++;
            continue;
        }

        if (*c == '=') {
            if (*(c + 1) == '=') {
                tokens[size++].type = NBL_TOKEN_EQ;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_ASSIGN;
            c++;
            continue;
        }
        if (*c == '+') {
            tokens[size++].type = NBL_TOKEN_ADD;
            c++;
            continue;
        }
        if (*c == '-') {
            tokens[size++].type = NBL_TOKEN_SUB;
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '*') {
                tokens[size++].type = NBL_TOKEN_EXP;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_MUL;
            c++;
            continue;
        }
        if (*c == '/') {
            tokens[size++].type = NBL_TOKEN_DIV;
            c++;
            continue;
        }
        if (*c == '%') {
            tokens[size++].type = NBL_TOKEN_MOD;
            c++;
            continue;
        }
        if (*c == '&') {
            if (*(c + 1) == '&') {
                tokens[size++].type = NBL_TOKEN_LOGICAL_AND;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_AND;
            c++;
            continue;
        }
        if (*c == '^') {
            tokens[size++].type = NBL_TOKEN_XOR;
            c++;
            continue;
        }
        if (*c == '|') {
            if (*(c + 1) == '|') {
                tokens[size++].type = NBL_TOKEN_LOGICAL_OR;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_OR;
            c++;
            continue;
        }
        if (*c == '~') {
            tokens[size++].type = NBL_TOKEN_NOT;
            c++;
            continue;
        }
        if (*c == '<') {
            if (*(c + 1) == '<') {
                tokens[size++].type = NBL_TOKEN_SHL;
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                tokens[size++].type = NBL_TOKEN_LTEQ;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_LT;
            c++;
            continue;
        }
        if (*c == '>') {
            if (*(c + 1) == '>') {
                tokens[size++].type = NBL_TOKEN_SHR;
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                tokens[size++].type = NBL_TOKEN_GTEQ;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_GT;
            c++;
            continue;
        }
        if (*c == '!') {
            if (*(c + 1) == '=') {
                tokens[size++].type = NBL_TOKEN_NEQ;
                c += 2;
                continue;
            }
            tokens[size++].type = NBL_TOKEN_LOGICAL_NOT;
            c++;
            continue;
        }

        // Whitespace
        if (*c == ' ' || *c == '\t') {
            c++;
            continue;
        }
        if (*c == '\n' || *c == '\r') {
            if (*c == '\r') c++;
            c++;
            line++;
            lineStart = c;
            continue;
        }

        tokens[size].type = NBL_TOKEN_UNKNOWN;
        tokens[size++].character = *c;
        c++;
    }
    *tokensSize = size;
    return tokens;
}

// Value
char *nbl_value_to_string(NBLValue value) {
    if (value.type == NBL_VALUE_NULL) return strdup("null");
    if (value.type == NBL_VALUE_BOOL) return strdup(value.integer ? "true" : "false");
    if (value.type == NBL_VALUE_INT) {
        char buffer[255];
        sprintf(buffer, "%" PRIi64, value.integer);
        return strdup(buffer);
    }
    if (value.type == NBL_VALUE_FLOAT) {
        char buffer[255];
        sprintf(buffer, "%g", value.floating);
        return strdup(buffer);
    }
    if (value.type == NBL_VALUE_STRING) return strdup(value.string);
    return NULL;
}

// Module
NBLValue nbl_module_run(NBLModule *module, NBLEnv *env) {
    size_t pc = 0;
    size_t sp = 0;
    NBLValue stack[sizeof(NBLValue) * module->stackSize];

    for (;;) {
        uint8_t opcode = module->opcodes[pc++];

        if (opcode == NBL_OPCODE_DUP) {
            stack[sp] = stack[sp - 1];
            sp++;
            continue;
        }
        if (opcode == NBL_OPCODE_POP) {
            sp--;
            continue;
        }

        if (opcode == NBL_OPCODE_CONST) {
            stack[sp++] = module->consts[module->opcodes[pc++]];
            continue;
        }
        if (opcode == NBL_OPCODE_LOAD) {
            stack[sp++] = env->vars[module->opcodes[pc++]].value;
            continue;
        }
        if (opcode == NBL_OPCODE_STORE) {
            env->vars[module->opcodes[pc++]].value = stack[--sp];
            continue;
        }
        if (opcode == NBL_OPCODE_RET) {
            return stack[--sp];
        }

        if (opcode == NBL_OPCODE_JMP) {
            int8_t offset = (int8_t)module->opcodes[pc++];
            pc += offset;
            continue;
        }
        if (opcode == NBL_OPCODE_JZ) {
            int8_t offset = (int8_t)module->opcodes[pc++];
            if (stack[--sp].integer == 0) pc += offset;
            continue;
        }
        if (opcode == NBL_OPCODE_JNZ) {
            int8_t offset = (int8_t)module->opcodes[pc++];
            if (stack[--sp].integer != 0) pc += offset;
            continue;
        }

        if (opcode >= NBL_OPCODE_NEG && opcode <= NBL_OPCODE_LOGICAL_NOT) {
            NBLValue unary = stack[--sp];

            if (opcode == NBL_OPCODE_NEG) {
                if (unary.type == NBL_VALUE_INT) {
                    stack[sp++].integer = -unary.integer;
                    continue;
                }
                if (unary.type == NBL_VALUE_FLOAT) {
                    stack[sp++].floating = -unary.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_NOT && unary.type == NBL_VALUE_INT) {
                stack[sp++].integer = ~unary.integer;
                continue;
            }
            if (opcode == NBL_OPCODE_LOGICAL_NOT && unary.type == NBL_VALUE_BOOL) {
                stack[sp++].integer = !unary.integer;
                continue;
            }

            fprintf(stderr, "Type error\n");
            exit(EXIT_FAILURE);
        }

        if (opcode >= NBL_OPCODE_ADD && opcode <= NBL_OPCODE_LOGICAL_OR) {
            NBLValue rhs = stack[--sp];
            NBLValue lhs = stack[--sp];

            if (opcode == NBL_OPCODE_ADD) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer + rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating + rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating + rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.integer + rhs.floating;
                    continue;
                }

                if (lhs.type == NBL_VALUE_STRING && rhs.type == NBL_VALUE_STRING) {
                    char *string = malloc(strlen(lhs.string) + strlen(rhs.string) + 1);
                    strcpy(string, lhs.string);
                    strcat(string, rhs.string);
                    // TODO
                    stack[sp].type = NBL_VALUE_STRING;
                    stack[sp++].string = string;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_SUB) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer - rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating - rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating - rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.integer - rhs.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_MUL) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer * rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating * rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.floating * rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = lhs.integer * rhs.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_EXP) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = pow(lhs.integer, rhs.integer);
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = pow(lhs.floating, rhs.floating);
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = pow(lhs.floating, rhs.integer);
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = pow(lhs.integer, rhs.floating);
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_DIV) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = rhs.integer != 0 ? lhs.integer / rhs.integer : 0;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = rhs.floating != 0 ? lhs.floating / rhs.floating : 0;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = rhs.integer != 0 ? lhs.floating / rhs.integer : 0;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = rhs.floating != 0 ? lhs.integer / rhs.floating : 0;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_MOD) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer % rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = fmod(lhs.floating, rhs.floating);
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = fmod(lhs.floating, rhs.integer);
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_FLOAT;
                    stack[sp++].floating = fmod(lhs.integer, rhs.floating);
                    continue;
                }
            }

            if (opcode == NBL_OPCODE_AND) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer & rhs.integer;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_XOR) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer ^ rhs.integer;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_OR) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer | rhs.integer;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_SHL) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer << rhs.integer;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_SHR) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_INT;
                    stack[sp++].integer = lhs.integer >> rhs.integer;
                    continue;
                }
            }

            if (opcode == NBL_OPCODE_EQ) {
                if (lhs.type == NBL_VALUE_NULL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = rhs.integer == NBL_VALUE_NULL;
                    continue;
                }
                if (rhs.type == NBL_VALUE_NULL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer == NBL_VALUE_NULL;
                    continue;
                }

                if ((lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) || (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer == rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.floating == rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.floating == rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer == rhs.floating;
                    continue;
                }

                if (lhs.type == NBL_VALUE_STRING && rhs.type == NBL_VALUE_STRING) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = !strcmp(lhs.string, rhs.string);
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_EQ) {
                if (lhs.type == NBL_VALUE_NULL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = rhs.integer != NBL_VALUE_NULL;
                    continue;
                }
                if (rhs.type == NBL_VALUE_NULL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer != NBL_VALUE_NULL;
                    continue;
                }

                if ((lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) || (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer != rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.floating != rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.floating != rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer != rhs.floating;
                    continue;
                }

                if (lhs.type == NBL_VALUE_STRING && rhs.type == NBL_VALUE_STRING) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = strcmp(lhs.string, rhs.string);
                    continue;
                }
            }

            if (opcode == NBL_OPCODE_LT) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer < rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating < rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating < rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.integer < rhs.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_LTEQ) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer <= rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating <= rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating <= rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.integer <= rhs.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_GT) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer > rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating > rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating > rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.integer > rhs.floating;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_GTEQ) {
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer >= rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating >= rhs.floating;
                    continue;
                }
                if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.floating >= rhs.integer;
                    continue;
                }
                if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].floating = lhs.integer >= rhs.floating;
                    continue;
                }
            }

            if (opcode == NBL_OPCODE_LOGICAL_AND) {
                if (lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer && rhs.integer;
                    continue;
                }
            }
            if (opcode == NBL_OPCODE_LOGICAL_OR) {
                if (lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) {
                    stack[sp].type = NBL_VALUE_BOOL;
                    stack[sp++].integer = lhs.integer || rhs.integer;
                    continue;
                }
            }

            fprintf(stderr, "Type error\n");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Unkown opcode\n");
        exit(EXIT_FAILURE);
    }

    return (NBLValue){.type = NBL_VALUE_NULL};
}

void nbl_module_dump(NBLModule *module) {
    printf("Constants (%zu):\n", module->constsSize);
    for (size_t i = 0; i < module->constsSize; i++) {
        printf("    %zu = %s\n", i, nbl_value_to_string(module->consts[i]));
    }

    printf("Opcodes:\n");
    size_t i = 0;
    for (;;) {
        uint8_t opcode = module->opcodes[i++];
        printf("    ");
        if (opcode == NBL_OPCODE_DUP) printf("dup\n");
        if (opcode == NBL_OPCODE_POP) printf("pop\n");

        if (opcode == NBL_OPCODE_CONST) printf("const %d\n", module->opcodes[i++]);
        if (opcode == NBL_OPCODE_LOAD) printf("load %d\n", module->opcodes[i++]);
        if (opcode == NBL_OPCODE_STORE) printf("store %d\n", module->opcodes[i++]);
        if (opcode == NBL_OPCODE_RET) {
            printf("ret\n");
            break;
        }

        if (opcode == NBL_OPCODE_JMP) printf("jmp %d\n", (int8_t)module->opcodes[i++]);
        if (opcode == NBL_OPCODE_JZ) printf("jz %d\n", (int8_t)module->opcodes[i++]);
        if (opcode == NBL_OPCODE_JNZ) printf("jnz %d\n", (int8_t)module->opcodes[i++]);

        if (opcode == NBL_OPCODE_NEG) printf("neg\n");
        if (opcode == NBL_OPCODE_NOT) printf("not\n");
        if (opcode == NBL_OPCODE_LOGICAL_NOT) printf("logical_not\n");

        if (opcode == NBL_OPCODE_ADD) printf("add\n");
        if (opcode == NBL_OPCODE_SUB) printf("sub\n");
        if (opcode == NBL_OPCODE_MUL) printf("mul\n");
        if (opcode == NBL_OPCODE_EXP) printf("exp\n");
        if (opcode == NBL_OPCODE_DIV) printf("div\n");
        if (opcode == NBL_OPCODE_MOD) printf("mod\n");
        if (opcode == NBL_OPCODE_AND) printf("and\n");
        if (opcode == NBL_OPCODE_XOR) printf("xor\n");
        if (opcode == NBL_OPCODE_OR) printf("or\n");
        if (opcode == NBL_OPCODE_SHL) printf("shl\n");
        if (opcode == NBL_OPCODE_SHR) printf("shr\n");
        if (opcode == NBL_OPCODE_EQ) printf("eq\n");
        if (opcode == NBL_OPCODE_NEQ) printf("neq\n");
        if (opcode == NBL_OPCODE_LT) printf("lt\n");
        if (opcode == NBL_OPCODE_LTEQ) printf("lteq\n");
        if (opcode == NBL_OPCODE_GT) printf("gt\n");
        if (opcode == NBL_OPCODE_GTEQ) printf("gteq\n");
        if (opcode == NBL_OPCODE_LOGICAL_AND) printf("logical_and\n");
        if (opcode == NBL_OPCODE_LOGICAL_OR) printf("logical_or\n");
    }
}

// Parser
NBLModule *nbl_parser(NBLToken *tokens, size_t tokensSize, NBLEnv *env) {
    NBLModule *module = malloc(sizeof(NBLModule));
    module->stackSize = 64;

    module->constsCapacity = 128;
    module->constsSize = 0;
    module->consts = malloc(sizeof(NBLValue) * module->constsCapacity);

    module->opcodesCapacity = 1024;
    module->opcodesSize = 0;
    module->opcodes = malloc(module->opcodesCapacity);

    NBLParser parser = {.tokens = tokens,
                        .tokensSize = tokensSize,
                        .position = 0,

                        .env = env,
                        .module = module};

    nbl_parser_program(&parser);

    module->opcodes[module->opcodesSize++] = NBL_OPCODE_RET;

    return module;
}

#define nbl_parser_current() ((NBLToken *)&parser->tokens[parser->position])
#define nbl_parser_next(pos) ((NBLToken *)&parser->tokens[parser->position + 1 + pos])

void nbl_parser_eat(NBLParser *parser, NBLTokenType type) {
    if (nbl_parser_current()->type == type) {
        parser->position++;
    } else {
        fprintf(stderr, "Unexpected token: '%s' needed '%s'\n", nbl_token_type_to_string(nbl_parser_current()->type), nbl_token_type_to_string(type));
        exit(EXIT_FAILURE);
    }
}

void nbl_parser_program(NBLParser *parser) {
    while (nbl_parser_current()->type != NBL_TOKEN_EOF) {
        nbl_parser_statement(parser);
    }
}

void nbl_parser_block(NBLParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_LCURLY) {
        nbl_parser_eat(parser, NBL_TOKEN_LCURLY);
        while (nbl_parser_current()->type != NBL_TOKEN_RCURLY) {
            nbl_parser_statement(parser);
        }
        nbl_parser_eat(parser, NBL_TOKEN_RCURLY);
    } else {
        nbl_parser_statement(parser);
    }
}

void nbl_parser_statement(NBLParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_SEMICOLON) {
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_LCURLY) {
        nbl_parser_block(parser);
        return;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_IF) {
        nbl_parser_eat(parser, NBL_TOKEN_IF);
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        nbl_parser_assign(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JZ;
        parser->module->opcodesSize++;
        size_t thenBlock = parser->module->opcodesSize;
        nbl_parser_block(parser);
        parser->module->opcodes[thenBlock - 1] = parser->module->opcodesSize - thenBlock + (nbl_parser_current()->type == NBL_TOKEN_ELSE ? 2 : 0);

        if (nbl_parser_current()->type == NBL_TOKEN_ELSE) {
            nbl_parser_eat(parser, NBL_TOKEN_ELSE);

            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JMP;
            parser->module->opcodesSize++;
            size_t elseBlock = parser->module->opcodesSize;
            nbl_parser_block(parser);
            parser->module->opcodes[elseBlock - 1] = parser->module->opcodesSize - elseBlock;
        }
        return;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_LOOP) {
        nbl_parser_eat(parser, NBL_TOKEN_LOOP);
        int32_t loopBlock = parser->module->opcodesSize;
        nbl_parser_block(parser);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JMP;
        parser->module->opcodesSize++;
        *(int8_t *)&parser->module->opcodes[parser->module->opcodesSize - 1] = loopBlock - (int32_t)parser->module->opcodesSize;
        return;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_WHILE) {
        nbl_parser_eat(parser, NBL_TOKEN_WHILE);
        int32_t loopBlock = parser->module->opcodesSize;
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        nbl_parser_assign(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JZ;
        parser->module->opcodesSize++;
        size_t conditionBlock = parser->module->opcodesSize;
        nbl_parser_block(parser);

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JMP;
        parser->module->opcodesSize++;
        *(int8_t *)&parser->module->opcodes[parser->module->opcodesSize - 1] = loopBlock - (int32_t)parser->module->opcodesSize;

        parser->module->opcodes[conditionBlock - 1] = parser->module->opcodesSize - conditionBlock;
        return;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_DO) {
        nbl_parser_eat(parser, NBL_TOKEN_DO);
        int32_t loopBlock = parser->module->opcodesSize;
        nbl_parser_block(parser);

        nbl_parser_eat(parser, NBL_TOKEN_WHILE);
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        nbl_parser_assign(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JNZ;
        parser->module->opcodesSize++;
        *(int8_t *)&parser->module->opcodes[parser->module->opcodesSize - 1] = loopBlock - (int32_t)parser->module->opcodesSize;
        return;
    }

    nbl_parser_assigns(parser);
    nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
}

void nbl_parser_assigns(NBLParser *parser) {
    for (;;) {
        nbl_parser_assign(parser);
        if (nbl_parser_current()->type == NBL_TOKEN_COMMA) {
            nbl_parser_eat(parser, NBL_TOKEN_COMMA);
        } else {
            break;
        }
    }
}

void nbl_parser_assign(NBLParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_KEYWORD && nbl_parser_next(0)->type == NBL_TOKEN_ASSIGN) {
        char *name = nbl_parser_current()->string;
        nbl_parser_eat(parser, NBL_TOKEN_KEYWORD);
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN);

        nbl_parser_assign(parser);

        int32_t varIndex = -1;
        for (size_t i = 0; i < parser->env->varsSize; i++) {
            NBLVar *var = &parser->env->vars[i];
            if (!strcmp(var->name, name)) {
                varIndex = i;
                break;
            }
        }
        if (varIndex == -1) {
            varIndex = parser->env->varsSize++;
            parser->env->vars[varIndex].name = name;
        }

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_STORE;
        parser->module->opcodes[parser->module->opcodesSize++] = varIndex;
        return;
    }

    nbl_parser_tenary(parser);
}

void nbl_parser_tenary(NBLParser *parser) {
    nbl_parser_logical(parser);

    if (nbl_parser_current()->type == NBL_TOKEN_QUESTION) {
        nbl_parser_eat(parser, NBL_TOKEN_QUESTION);

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JZ;
        parser->module->opcodesSize++;
        size_t thenBlock = parser->module->opcodesSize;
        nbl_parser_tenary(parser);
        parser->module->opcodes[thenBlock - 1] = parser->module->opcodesSize - thenBlock + 2;

        nbl_parser_eat(parser, NBL_TOKEN_COLON);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_JMP;
        parser->module->opcodesSize++;
        size_t elseBlock = parser->module->opcodesSize;
        nbl_parser_tenary(parser);
        parser->module->opcodes[elseBlock - 1] = parser->module->opcodesSize - elseBlock;
    }
}

void nbl_parser_logical(NBLParser *parser) {
    nbl_parser_equality(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_AND || nbl_parser_current()->type == NBL_TOKEN_LOGICAL_OR) {
        if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_AND) {
            nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_AND);
            nbl_parser_equality(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LOGICAL_AND;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_OR) {
            nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_OR);
            nbl_parser_equality(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LOGICAL_OR;
            continue;
        }
    }
}

void nbl_parser_equality(NBLParser *parser) {
    nbl_parser_relational(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_EQ || nbl_parser_current()->type == NBL_TOKEN_NEQ) {
        if (nbl_parser_current()->type == NBL_TOKEN_EQ) {
            nbl_parser_eat(parser, NBL_TOKEN_EQ);
            nbl_parser_relational(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_EQ;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_NEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_NEQ);
            nbl_parser_relational(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_NEQ;
            continue;
        }
    }
}

void nbl_parser_relational(NBLParser *parser) {
    nbl_parser_bitwise(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_LT || nbl_parser_current()->type == NBL_TOKEN_LTEQ || nbl_parser_current()->type == NBL_TOKEN_GT ||
           nbl_parser_current()->type == NBL_TOKEN_GTEQ) {
        if (nbl_parser_current()->type == NBL_TOKEN_LT) {
            nbl_parser_eat(parser, NBL_TOKEN_LT);
            nbl_parser_bitwise(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LT;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_LTEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_LTEQ);
            nbl_parser_bitwise(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LTEQ;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_GT) {
            nbl_parser_eat(parser, NBL_TOKEN_GT);
            nbl_parser_bitwise(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_GT;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_GTEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_GTEQ);
            nbl_parser_bitwise(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_GTEQ;
            continue;
        }
    }
}

void nbl_parser_bitwise(NBLParser *parser) {
    nbl_parser_shift(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_AND || nbl_parser_current()->type == NBL_TOKEN_XOR || nbl_parser_current()->type == NBL_TOKEN_OR) {
        if (nbl_parser_current()->type == NBL_TOKEN_AND) {
            nbl_parser_eat(parser, NBL_TOKEN_AND);
            nbl_parser_shift(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_AND;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_XOR) {
            nbl_parser_eat(parser, NBL_TOKEN_XOR);
            nbl_parser_shift(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_XOR;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_OR) {
            nbl_parser_eat(parser, NBL_TOKEN_OR);
            nbl_parser_shift(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_OR;
            continue;
        }
    }
}

void nbl_parser_shift(NBLParser *parser) {
    nbl_parser_add(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_SHL || nbl_parser_current()->type == NBL_TOKEN_SHR) {
        if (nbl_parser_current()->type == NBL_TOKEN_SHL) {
            nbl_parser_eat(parser, NBL_TOKEN_SHL);
            nbl_parser_add(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_SHL;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_SHR) {
            nbl_parser_eat(parser, NBL_TOKEN_SHR);
            nbl_parser_add(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_SHR;
            continue;
        }
    }
}

void nbl_parser_add(NBLParser *parser) {
    nbl_parser_mul(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_ADD || nbl_parser_current()->type == NBL_TOKEN_SUB) {
        if (nbl_parser_current()->type == NBL_TOKEN_ADD) {
            nbl_parser_eat(parser, NBL_TOKEN_ADD);
            nbl_parser_mul(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_ADD;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_SUB) {
            nbl_parser_eat(parser, NBL_TOKEN_SUB);
            nbl_parser_mul(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_SUB;
            continue;
        }
    }
}

void nbl_parser_mul(NBLParser *parser) {
    nbl_parser_unary(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_MUL || nbl_parser_current()->type == NBL_TOKEN_EXP || nbl_parser_current()->type == NBL_TOKEN_DIV ||
           nbl_parser_current()->type == NBL_TOKEN_MOD) {
        if (nbl_parser_current()->type == NBL_TOKEN_MUL) {
            nbl_parser_eat(parser, NBL_TOKEN_MUL);
            nbl_parser_unary(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_MUL;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_EXP) {
            nbl_parser_eat(parser, NBL_TOKEN_EXP);
            nbl_parser_unary(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_EXP;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_DIV) {
            nbl_parser_eat(parser, NBL_TOKEN_DIV);
            nbl_parser_unary(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_DIV;
            continue;
        }
        if (nbl_parser_current()->type == NBL_TOKEN_MOD) {
            nbl_parser_eat(parser, NBL_TOKEN_MOD);
            nbl_parser_unary(parser);
            parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_MOD;
            continue;
        }
    }
}

void nbl_parser_unary(NBLParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_ADD) {
        nbl_parser_eat(parser, NBL_TOKEN_ADD);
        nbl_parser_unary(parser);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_SUB) {
        nbl_parser_eat(parser, NBL_TOKEN_SUB);
        nbl_parser_unary(parser);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_NEG;
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_NOT) {
        nbl_parser_eat(parser, NBL_TOKEN_NOT);
        nbl_parser_unary(parser);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_NOT;
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_NOT) {
        nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_NOT);
        nbl_parser_unary(parser);
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LOGICAL_NOT;
        return;
    }
    nbl_parser_primary(parser);
}

void nbl_parser_primary(NBLParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_LPAREN) {
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        nbl_parser_add(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_NULL) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize++].type = NBL_VALUE_NULL;
        nbl_parser_eat(parser, NBL_TOKEN_NULL);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_FALSE) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize].type = NBL_VALUE_BOOL;
        parser->module->consts[parser->module->constsSize++].integer = 0;
        nbl_parser_eat(parser, NBL_TOKEN_FALSE);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TRUE) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize].type = NBL_VALUE_BOOL;
        parser->module->consts[parser->module->constsSize++].integer = 1;
        nbl_parser_eat(parser, NBL_TOKEN_TRUE);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_INT) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize].type = NBL_VALUE_INT;
        parser->module->consts[parser->module->constsSize++].integer = nbl_parser_current()->integer;
        nbl_parser_eat(parser, NBL_TOKEN_INT);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_FLOAT) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize].type = NBL_VALUE_FLOAT;
        parser->module->consts[parser->module->constsSize++].floating = nbl_parser_current()->floating;
        nbl_parser_eat(parser, NBL_TOKEN_FLOAT);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_STRING) {
        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_CONST;
        parser->module->opcodes[parser->module->opcodesSize++] = parser->module->constsSize;
        parser->module->consts[parser->module->constsSize].type = NBL_VALUE_STRING;
        parser->module->consts[parser->module->constsSize++].string = nbl_parser_current()->string;
        nbl_parser_eat(parser, NBL_TOKEN_STRING);
        return;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_KEYWORD) {
        char *name = nbl_parser_current()->string;
        nbl_parser_eat(parser, NBL_TOKEN_KEYWORD);

        int32_t varIndex = -1;
        for (size_t i = 0; i < parser->env->varsSize; i++) {
            NBLVar *var = &parser->env->vars[i];
            if (!strcmp(var->name, name)) {
                varIndex = i;
                break;
            }
        }
        if (varIndex == -1) {
            fprintf(stderr, "Can't find variable: '%s'\n", name);
            exit(EXIT_FAILURE);
        }

        parser->module->opcodes[parser->module->opcodesSize++] = NBL_OPCODE_LOAD;
        parser->module->opcodes[parser->module->opcodesSize++] = varIndex;
        return;
    }

    fprintf(stderr, "Unexpected token: '%s'\n", nbl_token_type_to_string(nbl_parser_current()->type));
    exit(EXIT_FAILURE);
}

// Context
NBLContext *nbl_context_new(void) {
    NBLContext *context = malloc(sizeof(NBLContext));

    context->env.varsCapacity = 64;
    context->env.varsSize = 0;
    context->env.vars = malloc(sizeof(NBLVar) * context->env.varsCapacity);

    return context;
}

NBLValue nbl_context_eval_file(NBLContext *context, char *path) { return nbl_context_eval_text(context, nbl_file_read(path)); }

NBLValue nbl_context_eval_text(NBLContext *context, char *text) {
    (void)context;
    size_t tokensSize;
    NBLToken *tokens = nbl_lexer(text, &tokensSize);
    NBLModule *module = nbl_parser(tokens, tokensSize, &context->env);

    nbl_module_dump(module);

    return nbl_module_run(module, &context->env);
}

void nbl_context_free(NBLContext *context) {
    free(context->env.vars);
    free(context);
}

#endif

#endif
