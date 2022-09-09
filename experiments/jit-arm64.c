// New Bastiaan Language JIT ARM64 Experiment
// gcc jit-arm64.c -lm -o jit-arm64 && ./jit-arm64 "(10 - 2) * 5"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Utils Header
size_t align(size_t size, size_t alignment);

// Utils
size_t align(size_t size, size_t alignment) { return (size + alignment - 1) / alignment * alignment; }

// List header
typedef struct List {
    int32_t refs;
    void **items;
    size_t capacity;
    size_t size;
} List;

#define list_foreach(list, item, block)                   \
    for (size_t index = 0; index < list->size; index++) { \
        item = list_get(list, index);                     \
        block                                             \
    }

List *list_new(void);

List *list_new_with_capacity(size_t capacity);

List *list_ref(List *list);

void *list_get(List *list, size_t index);

void list_add(List *list, void *item);

typedef void ListFreeFunc(void *item);

void list_free(List *list, ListFreeFunc *freeFunc);

// List
List *list_new(void) { return list_new_with_capacity(8); }

List *list_new_with_capacity(size_t capacity) {
    List *list = malloc(sizeof(List));
    list->refs = 1;
    list->items = malloc(sizeof(void *) * capacity);
    list->capacity = capacity;
    list->size = 0;
    return list;
}

List *list_ref(List *list) {
    list->refs++;
    return list;
}

void *list_get(List *list, size_t index) {
    if (index < list->size) {
        return list->items[index];
    }
    return NULL;
}

void list_add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

void list_free(List *list, ListFreeFunc *freeFunc) {
    list->refs--;
    if (list->refs > 0) return;

    if (freeFunc != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            if (list->items[i] != NULL) {
                freeFunc(list->items[i]);
            }
        }
    }
    free(list->items);
    free(list);
}

// Lexer Header
typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_NULL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_KEYWORD,

    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_EXP,
    TOKEN_DIV,
    TOKEN_MOD
} TokenType;

typedef struct Token {
    int32_t refs;
    TokenType type;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
} Token;

Token *token_new(TokenType type);

Token *token_new_int(TokenType type, int64_t integer);

Token *token_new_float(double floating);

Token *token_new_string(TokenType type, char *string);

Token *token_ref(Token *token);

char *token_type_to_string(TokenType type);

void token_free(Token *token);

typedef struct Keyword {
    char *keyword;
    TokenType type;
} Keyword;

List *lexer(char *text);

// Lexer
Token *token_new(TokenType type) {
    Token *token = malloc(sizeof(Token));
    token->refs = 1;
    token->type = type;
    return token;
}

Token *token_new_int(TokenType type, int64_t integer) {
    Token *token = token_new(type);
    token->integer = integer;
    return token;
}

Token *token_new_float(double floating) {
    Token *token = token_new(TOKEN_FLOAT);
    token->floating = floating;
    return token;
}

Token *token_new_string(TokenType type, char *string) {
    Token *token = token_new(type);
    token->string = string;
    return token;
}

char *token_type_to_string(TokenType type) {
    if (type == TOKEN_EOF) return "EOF";
    if (type == TOKEN_UNKNOWN) return "Unknown character";
    if (type == TOKEN_LPAREN) return "(";
    if (type == TOKEN_RPAREN) return ")";

    if (type == TOKEN_NULL) return "null";
    if (type == TOKEN_TRUE) return "true";
    if (type == TOKEN_FALSE) return "false";
    if (type == TOKEN_INT) return "int";
    if (type == TOKEN_FLOAT) return "float";
    if (type == TOKEN_STRING) return "string";
    if (type == TOKEN_KEYWORD) return "keyword";

    if (type == TOKEN_ADD) return "+";
    if (type == TOKEN_SUB) return "-";
    if (type == TOKEN_MUL) return "*";
    if (type == TOKEN_EXP) return "**";
    if (type == TOKEN_DIV) return "/";
    if (type == TOKEN_MOD) return "%";
    return NULL;
}

void token_free(Token *token) {
    token->refs--;
    if (token->refs > 0) return;

    if (token->type == TOKEN_KEYWORD) free(token->string);
    free(token);
}

List *lexer(char *text) {
    Keyword keywords[] = {{"null", TOKEN_NULL}, {"true", TOKEN_TRUE}, {"false", TOKEN_FALSE}};

    List *tokens = list_new_with_capacity(512);
    char *c = text;
    int32_t line = 1;
    char *lineStart = c;
    while (*c != '\0') {
        int32_t column = c - lineStart + 1;

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
            list_add(tokens, token_new_int(TOKEN_INT, strtol(c, &c, 2)));
            continue;
        }
        if (*c == '0' && (isdigit(*(c + 1)) || *(c + 1) == 'o')) {
            if (*(c + 1) == 'o') c++;
            c++;
            list_add(tokens, token_new_int(TOKEN_INT, strtol(c, &c, 8)));
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            list_add(tokens, token_new_int(TOKEN_INT, strtol(c, &c, 16)));
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
                list_add(tokens, token_new_float(strtod(start, &c)));
            } else {
                list_add(tokens, token_new_int(TOKEN_INT, strtol(start, &c, 10)));
            }
            continue;
        }

        // Strings
        if (*c == '"' || *c == '\'') {
            char endChar = *c;
            c++;
            char *ptr = c;
            while (*c != endChar) c++;
            size_t size = c - ptr;
            c++;

            char *string = malloc(size + 1);
            int32_t strpos = 0;
            for (size_t i = 0; i < size; i++) {
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
            list_add(tokens, token_new_string(TOKEN_STRING, string));
            continue;
        }

        // Keywords
        if (isalpha(*c) || *c == '_' || *c == '$') {
            char *ptr = c;
            while (isalnum(*c) || *c == '_' || *c == '$') c++;
            size_t size = c - ptr;

            bool found = false;
            for (size_t i = 0; i < sizeof(keywords) / sizeof(Keyword); i++) {
                Keyword *keyword = &keywords[i];
                size_t keywordSize = strlen(keyword->keyword);
                if (!memcmp(ptr, keyword->keyword, keywordSize) && size == keywordSize) {
                    list_add(tokens, token_new(keyword->type));
                    found = true;
                    break;
                }
            }
            if (!found) {
                char *string = malloc(size + 1);
                memcpy(string, ptr, size);
                string[size] = '\0';
                list_add(tokens, token_new_string(TOKEN_KEYWORD, string));
            }
            continue;
        }

        // Syntax
        if (*c == '(') {
            list_add(tokens, token_new(TOKEN_LPAREN));
            c++;
            continue;
        }
        if (*c == ')') {
            list_add(tokens, token_new(TOKEN_RPAREN));
            c++;
            continue;
        }

        if (*c == '+') {
            list_add(tokens, token_new(TOKEN_ADD));
            c++;
            continue;
        }
        if (*c == '-') {
            list_add(tokens, token_new(TOKEN_SUB));
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '*') {
                list_add(tokens, token_new(TOKEN_EXP));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MUL));
            c++;
            continue;
        }
        if (*c == '/') {
            list_add(tokens, token_new(TOKEN_DIV));
            c++;
            continue;
        }
        if (*c == '%') {
            list_add(tokens, token_new(TOKEN_MOD));
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

        list_add(tokens, token_new_int(TOKEN_UNKNOWN, *c));
        c++;
    }
    list_add(tokens, token_new(TOKEN_EOF));
    return tokens;
}

// Page Header
void *page_alloc(size_t size);

bool page_make_executable(void *page, size_t size);

typedef int64_t (*JitIntFunc)(void);
typedef double (*JitFloatFunc)(void);
typedef char *(*JitStringFunc)(void);

// Page
void *page_alloc(size_t size) {
    void *page = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page != (void *)-1) {
        return page;
    }
    return NULL;
}

bool page_make_executable(void *page, size_t size) {
    if (mprotect(page, size, PROT_READ | PROT_EXEC) == -1) {
        return false;
    }
    return true;
}

// Runtime Header
typedef enum ValueType { VALUE_NULL, VALUE_BOOL, VALUE_INT, VALUE_FLOAT, VALUE_STRING } ValueType;

char *value_string_concat(char *a, char *b);

// Runtime
char *value_string_concat(char *a, char *b) {
    char *c = malloc(strlen(a) + strlen(b) + 1);
    strcpy(c, a);
    strcat(c, b);
    return c;
}

// Parser Header
typedef struct Parser {
    List *tokens;
    int32_t position;
    uint32_t *code;
    int32_t codePos;
    uint8_t *data;
    int32_t dataPos;
} Parser;

ValueType parser(List *tokens, void *codePage, void *dataPage);

void parser_eat(Parser *parser, TokenType type);

ValueType parser_add(Parser *parser);
ValueType parser_mul(Parser *parser);
ValueType parser_unary(Parser *parser);
ValueType parser_primary(Parser *parser);

// Codegen
#define x0 0
#define x1 1
#define x2 2
#define d0 32 + 0
#define d1 32 + 1

void mov_reg_imm(Parser *parser, int32_t reg, uint64_t imm) {
    parser->code[parser->codePos++] = 0xD2800000 | ((imm & 0xffff) << 5) | (reg & 31);                                    // mov reg, imm
    if (imm > 0xffff) parser->code[parser->codePos++] = 0xF2A00000 | (((imm >> 16) & 0xffff) << 5) | (reg & 31);          // movk reg, imm, lsl 16
    if (imm > 0xffffffff) parser->code[parser->codePos++] = 0xF2C00000 | (((imm >> 32) & 0xffff) << 5) | (reg & 31);      // movk reg, imm, lsl 32
    if (imm > 0xffffffffffff) parser->code[parser->codePos++] = 0xF2E00000 | (((imm >> 48) & 0xffff) << 5) | (reg & 31);  // movk reg, imm, lsl 48
}

void push_reg(Parser *parser, int32_t reg) {
    if (reg < 32) {
        parser->code[parser->codePos++] = 0xF81F0FE0 | (reg & 31);  // str xreg, [sp, -16]!
    } else {
        parser->code[parser->codePos++] = 0xFC1F0FE0 | (reg & 31);  // str dreg, [sp, -16]!
    }
}

void pop_reg(Parser *parser, int32_t reg) {
    if (reg < 32) {
        parser->code[parser->codePos++] = 0xF84107E0 | (reg & 31);  // ldr xreg, [sp], 16
    } else {
        parser->code[parser->codePos++] = 0xFC4107E0 | (reg & 31);  // ldr dreg, [sp], 16
    }
}

void load_float(Parser *parser, ValueType lhsType, ValueType rhsType) {
    if (rhsType == VALUE_INT) {
        pop_reg(parser, x1);
        parser->code[parser->codePos++] = 0x9E620021;  // scvtf d1, x1
    } else {
        pop_reg(parser, d1);
    }
    if (lhsType == VALUE_INT) {
        pop_reg(parser, x0);
        parser->code[parser->codePos++] = 0x9E620000;  // scvtf d0, x0
    } else {
        pop_reg(parser, d0);
    }
}

// Parser
ValueType parser(List *tokens, void *codePage, void *dataPage) {
    Parser parser = {.tokens = tokens, .position = 0, .code = codePage, .codePos = 0, .data = dataPage, .dataPos = 0};
    parser.code[parser.codePos++] = 0xA9BF7BFD;  // stp fp, lr, [sp, -16]!

    ValueType returnType = parser_add(&parser);
    if (returnType == VALUE_NULL || returnType == VALUE_BOOL || returnType == VALUE_INT || returnType == VALUE_STRING) {
        pop_reg(&parser, x0);
    }
    if (returnType == VALUE_FLOAT) {
        pop_reg(&parser, d0);
    }

    parser.code[parser.codePos++] = 0xA8C17BFD;  // ldp fp, lr, [sp], 16
    parser.code[parser.codePos++] = 0xD65F03C0;  // ret
    return returnType;
}

#define current() ((Token *)list_get(parser->tokens, parser->position))
#define next(pos) ((Token *)list_get(parser->tokens, parser->position + 1 + pos))

void parser_eat(Parser *parser, TokenType type) {
    if (current()->type == type) {
        parser->position++;
    } else {
        fprintf(stderr, "Unexpected token: '%s' needed '%s'\n", token_type_to_string(current()->type), token_type_to_string(type));
        exit(EXIT_FAILURE);
    }
}

ValueType parser_add(Parser *parser) {
    ValueType lhsType = parser_mul(parser);
    while (current()->type == TOKEN_ADD || current()->type == TOKEN_SUB) {
        if (current()->type == TOKEN_ADD) {
            parser_eat(parser, TOKEN_ADD);
            ValueType rhsType = parser_mul(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0x8B010000;  // add x0, x0, x1
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                parser->code[parser->codePos++] = 0x1E612800;  // fadd d0, d0, d1
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
            if (lhsType == VALUE_STRING && rhsType == VALUE_STRING) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                mov_reg_imm(parser, x2, (uint64_t)value_string_concat);
                parser->code[parser->codePos++] = 0xD63F0040;  // blr x2
                push_reg(parser, x0);
                continue;
            }
        }

        if (current()->type == TOKEN_SUB) {
            parser_eat(parser, TOKEN_SUB);
            ValueType rhsType = parser_mul(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0xCB010000;  // sub x0, x0, x1
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                parser->code[parser->codePos++] = 0x1E613800;  // fsub d0, d0, d1
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        fprintf(stderr, "Type error\n");
        exit(EXIT_FAILURE);
    }
    return lhsType;
}

ValueType parser_mul(Parser *parser) {
    ValueType lhsType = parser_unary(parser);
    while (current()->type == TOKEN_MUL || current()->type == TOKEN_EXP || current()->type == TOKEN_DIV || current()->type == TOKEN_MOD) {
        if (current()->type == TOKEN_MUL) {
            parser_eat(parser, TOKEN_MUL);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0x9B017C00;  // mul x0, x0, x1
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                parser->code[parser->codePos++] = 0x1E610800;  // fmul d0, d0, d1
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_EXP) {
            parser_eat(parser, TOKEN_EXP);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0xAA0003E2;  // mov x2, x0
                parser->code[parser->codePos++] = 0xD1000421;  // sub x1, x1, 1
                parser->code[parser->codePos++] = 0xB4000061;  // cbz x1, +12
                parser->code[parser->codePos++] = 0x9B027C00;  // mul x0, x0, x2
                parser->code[parser->codePos++] = 0x17FFFFFD;  // b -12
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                mov_reg_imm(parser, x2, (uint64_t)pow);
                parser->code[parser->codePos++] = 0xD63F0040;  // blr x2
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_DIV) {
            parser_eat(parser, TOKEN_DIV);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0x9AC10C00;  // sdiv x0, x0, x1
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                parser->code[parser->codePos++] = 0x1E611800;  // fdiv d0, d0, d1
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_MOD) {
            parser_eat(parser, TOKEN_MOD);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                parser->code[parser->codePos++] = 0x9AC10802;  // udiv x2, x0, x1
                parser->code[parser->codePos++] = 0x9B018040;  // msub x0, x2, x1, x0
                push_reg(parser, x0);
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
                mov_reg_imm(parser, x2, (uint64_t)fmod);
                parser->code[parser->codePos++] = 0xD63F0040;  // blr x2
                push_reg(parser, d0);
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        fprintf(stderr, "Type error\n");
        exit(EXIT_FAILURE);
    }
    return lhsType;
}

ValueType parser_unary(Parser *parser) {
    if (current()->type == TOKEN_ADD) {
        parser_eat(parser, TOKEN_ADD);
        ValueType type = parser_unary(parser);
        if (type != VALUE_INT && type != VALUE_FLOAT) {
            fprintf(stderr, "Type error\n");
            exit(EXIT_FAILURE);
        }
        return VALUE_INT;
    }
    if (current()->type == TOKEN_SUB) {
        parser_eat(parser, TOKEN_SUB);
        ValueType type = parser_unary(parser);
        if (type == VALUE_INT) {
            pop_reg(parser, x0);
            parser->code[parser->codePos++] = 0xCB0003E0;  // sub x0, xzr, x0
            push_reg(parser, x0);
            return VALUE_INT;
        }
        if (type == VALUE_FLOAT) {
            pop_reg(parser, d0);
            parser->code[parser->codePos++] = 0x1E614000;  // fneg d0, d0
            push_reg(parser, d0);
            return VALUE_FLOAT;
        }
        fprintf(stderr, "Type error\n");
        exit(EXIT_FAILURE);
    }
    return parser_primary(parser);
}

ValueType parser_primary(Parser *parser) {
    if (current()->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        ValueType type = parser_add(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return type;
    }
    if (current()->type == TOKEN_NULL) {
        mov_reg_imm(parser, x0, 0);
        push_reg(parser, x0);
        parser_eat(parser, TOKEN_NULL);
        return VALUE_NULL;
    }
    if (current()->type == TOKEN_FALSE) {
        mov_reg_imm(parser, x0, 0);
        push_reg(parser, x0);
        parser_eat(parser, TOKEN_FALSE);
        return VALUE_BOOL;
    }
    if (current()->type == TOKEN_TRUE) {
        mov_reg_imm(parser, x0, 1);
        push_reg(parser, x0);
        parser_eat(parser, TOKEN_TRUE);
        return VALUE_BOOL;
    }
    if (current()->type == TOKEN_INT) {
        mov_reg_imm(parser, x0, current()->integer);
        push_reg(parser, x0);
        parser_eat(parser, TOKEN_INT);
        return VALUE_INT;
    }
    if (current()->type == TOKEN_FLOAT) {
        uint64_t floatAddr = (uint64_t)&parser->data[parser->dataPos];
        memcpy(&parser->data[parser->dataPos], &current()->floating, sizeof(double));
        parser->dataPos += sizeof(double);

        mov_reg_imm(parser, x0, floatAddr);
        parser->code[parser->codePos++] = 0xFD400000;  // ldr d0, [x0]
        push_reg(parser, d0);
        parser_eat(parser, TOKEN_FLOAT);
        return VALUE_FLOAT;
    }
    if (current()->type == TOKEN_STRING) {
        uint64_t stringAddr = (uint64_t)&parser->data[parser->dataPos];
        size_t stringSize = strlen(current()->string) + 1;
        memcpy(&parser->data[parser->dataPos], current()->string, stringSize);
        parser->dataPos += align(stringSize, 8);

        mov_reg_imm(parser, x0, stringAddr);
        push_reg(parser, x0);
        parser_eat(parser, TOKEN_STRING);
        return VALUE_STRING;
    }

    fprintf(stderr, "Unexpected token: '%s'\n", token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
}

// Main
int main(int argc, char **argv) {
    if (argc == 1) {
        printf("New Bastiaan Language JIT\n");
        return EXIT_SUCCESS;
    }

    char *text = argv[1];
    printf("Command: %s\n", text);

    List *tokens = lexer(text);
    size_t PAGE_SIZE = 4096;
    void *codePage = page_alloc(PAGE_SIZE);
    void *dataPage = page_alloc(PAGE_SIZE);

    ValueType returnType = parser(tokens, codePage, dataPage);
    page_make_executable(codePage, PAGE_SIZE);

    if (returnType == VALUE_NULL || returnType == VALUE_BOOL || returnType == VALUE_INT) {
        JitIntFunc func = codePage;
        printf("Result: (int) %lld\n", func());
    }
    if (returnType == VALUE_FLOAT) {
        JitFloatFunc func = codePage;
        printf("Result: (float) %g\n", func());
    }
    if (returnType == VALUE_STRING) {
        JitStringFunc func = codePage;
        printf("Result: (string) %s\n", func());
    }

    list_free(tokens, (ListFreeFunc *)token_free);
    munmap(codePage, PAGE_SIZE);
    munmap(dataPage, PAGE_SIZE);
    return EXIT_SUCCESS;
}
