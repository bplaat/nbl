// New Bastiaan Language JIT Experiment
// Works on: Win32 & Posix with x86_64 & ARM64
// gcc jit.c -lm -o jit && ./jit "(10 - 2) * 5"
#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Custom windows headers because why not :)
#ifdef _WIN32
#define WIN32

#define MEM_COMMIT 0x00001000
#define MEM_RESERVE 0x00002000
#define PAGE_READWRITE 0x04
#define PAGE_EXECUTE_READ 0x20
#define MEM_RELEASE 0x00008000

extern void *VirtualAlloc(void *lpAddress, size_t dwSize, uint32_t flAllocationType, uint32_t flProtect);
extern bool VirtualProtect(void *lpAddress, size_t dwSize, uint32_t flNewProtect, uint32_t *lpflOldProtect);
extern bool VirtualFree(void *lpAddress, size_t dwSize, uint32_t dwFreeType);

#else

#include <sys/mman.h>

#endif

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
typedef struct Page {
    void *data;
    size_t size;
} Page;

Page *page_new(size_t size);

bool page_make_executable(Page *page);

void page_free(Page *page);

// Page
Page *page_new(size_t size) {
    Page *page = malloc(sizeof(Page));
    page->size = size;
#ifdef WIN32
    page->data = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (page->data == NULL) {
        return NULL;
    }
#else
    page->data = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page->data == (void *)-1) {
        return NULL;
    }
#endif
    return page;
}

bool page_make_executable(Page *page) {
#ifdef WIN32
    uint32_t oldProtect;
    return VirtualProtect(page->data, page->size, PAGE_EXECUTE_READ, &oldProtect);
#else
    if (mprotect(page->data, page->size, PROT_READ | PROT_EXEC) == -1) {
        return false;
    }
    return true;
#endif
}

void page_free(Page *page) {
#ifdef WIN32
    VirtualFree(page->data, 0, MEM_RELEASE);
#else
    munmap(page->data, page->size);
#endif
    free(page);
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
    uint8_t *code;
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

// x86_64
#ifdef __x86_64__
#define X86_64
#endif
#define rax 0
#define rcx 1
#define rdx 2
#define rbx 3
#define rsp 4
#define rbp 5
#define rdi 6
#define rsi 7
#define xmm0 16 + 0
#define xmm1 16 + 1

// arm64
#ifdef __aarch64__
#define ARM64
#endif
#define x0 0
#define x1 1
#define x2 2
#define x3 3
#define x4 4
#define x5 5
#define x6 6
#define x7 7
#define d0 32 + 0
#define d1 32 + 1

#define x86_64_inst1(parser, a) (parser)->code[(parser)->codePos++] = a;
#define x86_64_inst2(parser, a, b) \
    x86_64_inst1(parser, a);       \
    (parser)->code[(parser)->codePos++] = b;
#define x86_64_inst3(parser, a, b, c) \
    x86_64_inst2(parser, a, b);       \
    (parser)->code[(parser)->codePos++] = c;
#define x86_64_inst4(parser, a, b, c, d) \
    x86_64_inst3(parser, a, b, c);       \
    (parser)->code[(parser)->codePos++] = d;
#define x86_64_inst5(parser, a, b, c, d, e) \
    x86_64_inst4(parser, a, b, c, d);       \
    (parser)->code[(parser)->codePos++] = e;

#define arm64_inst(parser, inst)                                \
    *((uint32_t *)&((parser)->code[(parser)->codePos])) = inst; \
    (parser)->codePos += sizeof(uint32_t);

void mov_reg_imm(Parser *parser, int32_t reg, uint64_t imm) {
#ifdef X86_64
    x86_64_inst2(parser, 0x48, 0xb8 | (reg & 7));  // mov reg64, imm
    *((uint64_t *)&parser->code[parser->codePos]) = imm;
    parser->codePos += sizeof(uint64_t);
#endif
#ifdef ARM64
    arm64_inst(parser, 0xD2800000 | ((imm & 0xffff) << 5) | (reg & 31));  // mov reg, imm
    if (imm > 0xffff) {
        arm64_inst(parser, 0xF2A00000 | (((imm >> 16) & 0xffff) << 5) | (reg & 31));  // movk reg, imm, lsl 16
    }
    if (imm > 0xffffffff) {
        arm64_inst(parser, 0xF2C00000 | (((imm >> 32) & 0xffff) << 5) | (reg & 31));  // movk reg, imm, lsl 32
    }
    if (imm > 0xffffffffffff) {
        arm64_inst(parser, 0xF2E00000 | (((imm >> 48) & 0xffff) << 5) | (reg & 31));  // movk reg, imm, lsl 48
    }
#endif
}

void push_reg(Parser *parser, int32_t reg) {
#ifdef X86_64
    if (reg < 16) {
        x86_64_inst1(parser, 0x50 | (reg & 7));  // push reg64
    } else {
        x86_64_inst4(parser, 0x48, 0x83, 0xec, 0x10);                            // sub rsp, 16
        x86_64_inst5(parser, 0xf3, 0x0f, 0x7f, 0x84 | ((reg - 16) << 3), 0x24);  // movdqu XMMWORD PTR [rsp+0x0], xmm0
        x86_64_inst4(parser, 0x00, 0x00, 0x00, 0x00);
    }
#endif
#ifdef ARM64
    if (reg < 32) {
        arm64_inst(parser, 0xF81F0FE0 | (reg & 31));  // str xreg, [sp, -16]!
    } else {
        arm64_inst(parser, 0xFC1F0FE0 | (reg & 31));  // str dreg, [sp, -16]!
    }
#endif
}

void pop_reg(Parser *parser, int32_t reg) {
#ifdef X86_64
    if (reg < 16) {
        x86_64_inst1(parser, 0x58 | (reg & 7));  // pop reg64
    } else {
        x86_64_inst5(parser, 0xf3, 0x0f, 0x6f, 0x84 | ((reg - 16) << 3), 0x24);  // movdqu xmm0, XMMWORD PTR [rsp+0x0]
        x86_64_inst4(parser, 0x00, 0x00, 0x00, 0x00);
        x86_64_inst4(parser, 0x48, 0x83, 0xc4, 0x10);  // add rsp, 16
    }
#endif
#ifdef ARM64
    if (reg < 32) {
        arm64_inst(parser, 0xF84107E0 | (reg & 31));  // ldr xreg, [sp], 16
    } else {
        arm64_inst(parser, 0xFC4107E0 | (reg & 31));  // ldr dreg, [sp], 16
    }
#endif
}

void load_float(Parser *parser, ValueType lhsType, ValueType rhsType) {
#ifdef X86_64
    if (rhsType == VALUE_INT) {
        pop_reg(parser, rdx);
        x86_64_inst5(parser, 0xf2, 0x48, 0x0f, 0x2a, 0xca);  // cvtsi2sd xmm1, rdx
    } else {
        pop_reg(parser, xmm1);
    }
    if (lhsType == VALUE_INT) {
        pop_reg(parser, rax);
        x86_64_inst5(parser, 0xf2, 0x48, 0x0f, 0x2a, 0xc0);  // cvtsi2sd xmm0, rax
    } else {
        pop_reg(parser, xmm0);
    }
#endif
#ifdef ARM64
    if (rhsType == VALUE_INT) {
        pop_reg(parser, x1);
        arm64_inst(parser, 0x9E620021);  // scvtf d1, x1
    } else {
        pop_reg(parser, d1);
    }
    if (lhsType == VALUE_INT) {
        pop_reg(parser, x0);
        arm64_inst(parser, 0x9E620000);  // scvtf d0, x0
    } else {
        pop_reg(parser, d0);
    }
#endif
}

// Parser
ValueType parser(List *tokens, void *codePage, void *dataPage) {
    Parser parser = {.tokens = tokens, .position = 0, .code = codePage, .codePos = 0, .data = dataPage, .dataPos = 0};

#ifdef X86_64
    x86_64_inst4(&parser, 0x48, 0x83, 0xec, 0x38);  // sub rsp, 56
#endif
#ifdef ARM64
    arm64_inst(&parser, 0xA9BF7BFD);  // stp fp, lr, [sp, -16]!
#endif

    ValueType returnType = parser_add(&parser);

#ifdef X86_64
    if (returnType == VALUE_NULL || returnType == VALUE_BOOL || returnType == VALUE_INT || returnType == VALUE_STRING) {
        pop_reg(&parser, rax);
    }
    if (returnType == VALUE_FLOAT) {
        pop_reg(&parser, xmm0);
    }
#endif
#ifdef ARM64
    if (returnType == VALUE_NULL || returnType == VALUE_BOOL || returnType == VALUE_INT || returnType == VALUE_STRING) {
        pop_reg(&parser, x0);
    }
    if (returnType == VALUE_FLOAT) {
        pop_reg(&parser, d0);
    }
#endif
#ifdef X86_64
    x86_64_inst4(&parser, 0x48, 0x83, 0xc4, 0x38);  // add rsp, 56
    parser.code[parser.codePos++] = 0xc3;           // ret
#endif
#ifdef ARM64
    arm64_inst(&parser, 0xA8C17BFD);  // ldp fp, lr, [sp], 16
    arm64_inst(&parser, 0xD65F03C0);  // ret
#endif

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
#ifdef X86_64
                pop_reg(parser, rdx);
                pop_reg(parser, rax);
                x86_64_inst3(parser, 0x48, 0x01, 0xd0);  // add rax, rdx
                push_reg(parser, rax);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                arm64_inst(parser, 0x8B010000);  // add x0, x0, x1
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                x86_64_inst4(parser, 0xf2, 0x0f, 0x58, 0xc1);  // addsd xmm0, xmm1
                push_reg(parser, xmm0);
#endif
#ifdef ARM64
                arm64_inst(parser, 0x1E613800);  // fsub d0, d0, d1
                push_reg(parser, d0);
#endif
                lhsType = VALUE_FLOAT;
                continue;
            }
            if (lhsType == VALUE_STRING && rhsType == VALUE_STRING) {
#ifdef X86_64
#ifdef WIN32
                pop_reg(parser, rdx);
                pop_reg(parser, rcx);
#else
                pop_reg(parser, rsi);
                pop_reg(parser, rdi);
#endif
                mov_reg_imm(parser, rax, (uint64_t)value_string_concat);
                x86_64_inst2(parser, 0xff, 0xd0);  // call rax
                push_reg(parser, rax);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                mov_reg_imm(parser, x2, (uint64_t)value_string_concat);
                arm64_inst(parser, 0xD63F0040);  // blr x2
                push_reg(parser, x0);
#endif
                continue;
            }
        }

        if (current()->type == TOKEN_SUB) {
            parser_eat(parser, TOKEN_SUB);
            ValueType rhsType = parser_mul(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
#ifdef X86_64
                pop_reg(parser, rdx);
                pop_reg(parser, rax);
                x86_64_inst3(parser, 0x48, 0x29, 0xd0);  // sub rax, rdx
                push_reg(parser, rax);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                arm64_inst(parser, 0xCB010000);  // sub x0, x0, x1
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                x86_64_inst4(parser, 0xf2, 0x0f, 0x5c, 0xc1);  // subsd xmm0, xmm1
                push_reg(parser, d0);
#endif
#ifdef ARM64
                arm64_inst(parser, 0x1E613800);  // fsub d0, d0, d1
                push_reg(parser, d0);
#endif
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
#ifdef X86_64
                pop_reg(parser, rdx);
                pop_reg(parser, rax);
                x86_64_inst4(parser, 0x48, 0x0f, 0xaf, 0xc2);  // imul rax, rdx
                push_reg(parser, rax);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                arm64_inst(parser, 0x9B017C00);  // mul x0, x0, x1
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                x86_64_inst4(parser, 0xf2, 0x0f, 0x59, 0xc1);  // mulsd xmm0, xmm1
                push_reg(parser, xmm0);
#endif
#ifdef ARM64
                arm64_inst(parser, 0x1E610800);  // fmul d0, d0, d1
                push_reg(parser, d0);
#endif
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_EXP) {
            parser_eat(parser, TOKEN_EXP);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                mov_reg_imm(parser, rax, (uint64_t)pow);
                x86_64_inst2(parser, 0xff, 0xd0);                    // call rax
                x86_64_inst5(parser, 0xf2, 0x48, 0x0f, 0x2c, 0xc0);  // cvttsd2si rax, xmm0
                push_reg(parser, rax);
#endif
#ifdef ARM64
                mov_reg_imm(parser, x2, (uint64_t)pow);
                arm64_inst(parser, 0xD63F0040);  // blr x2
                arm64_inst(parser, 0x5EE1B800);  // fcvtzs d0, d0
                arm64_inst(parser, 0x9E660000);  // fmov x0, d0
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                mov_reg_imm(parser, rax, (uint64_t)pow);
                x86_64_inst2(parser, 0xff, 0xd0);  // call rax
                push_reg(parser, xmm0);
#endif
#ifdef ARM64
                mov_reg_imm(parser, x2, (uint64_t)pow);
                arm64_inst(parser, 0xD63F0040);  // blr x2
                push_reg(parser, d0);
#endif
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_DIV) {
            parser_eat(parser, TOKEN_DIV);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
#ifdef X86_64
                pop_reg(parser, rcx);
                pop_reg(parser, rax);
                x86_64_inst2(parser, 0x48, 0x99);        // cqo
                x86_64_inst3(parser, 0x48, 0xf7, 0xf9);  // idiv rcx
                push_reg(parser, rax);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                arm64_inst(parser, 0x9AC10C00);  // sdiv x0, x0, x1
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                x86_64_inst4(parser, 0xf2, 0x0f, 0x5e, 0xc1);  // divsd xmm0, xmm1
                push_reg(parser, xmm0);
#endif
#ifdef ARM64
                arm64_inst(parser, 0x1E611800);  // fdiv d0, d0, d1
                push_reg(parser, d0);
#endif
                lhsType = VALUE_FLOAT;
                continue;
            }
        }

        if (current()->type == TOKEN_MOD) {
            parser_eat(parser, TOKEN_MOD);
            ValueType rhsType = parser_unary(parser);
            if (lhsType == VALUE_INT && rhsType == VALUE_INT) {
#ifdef X86_64
                pop_reg(parser, rcx);
                pop_reg(parser, rax);
                x86_64_inst2(parser, 0x48, 0x99);        // cqo
                x86_64_inst3(parser, 0x48, 0xf7, 0xf9);  // idiv rcx
                push_reg(parser, rdx);
#endif
#ifdef ARM64
                pop_reg(parser, x1);
                pop_reg(parser, x0);
                arm64_inst(parser, 0x9AC10802);  // udiv x2, x0, x1
                arm64_inst(parser, 0x9B018040);  // msub x0, x2, x1, x0
                push_reg(parser, x0);
#endif
                continue;
            }
            if ((lhsType == VALUE_FLOAT && rhsType == VALUE_FLOAT) || (lhsType == VALUE_FLOAT && rhsType == VALUE_INT) ||
                (lhsType == VALUE_INT && rhsType == VALUE_FLOAT)) {
                load_float(parser, lhsType, rhsType);
#ifdef X86_64
                mov_reg_imm(parser, rax, (uint64_t)fmod);
                x86_64_inst2(parser, 0xff, 0xd0);  // call rax
                push_reg(parser, xmm0);
#endif
#ifdef ARM64
                mov_reg_imm(parser, x2, (uint64_t)fmod);
                arm64_inst(parser, 0xD63F0040);  // blr x2
                push_reg(parser, d0);
#endif
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
#ifdef X86_64
        if (type == VALUE_INT) {
            pop_reg(parser, rax);
            x86_64_inst3(parser, 0x48, 0xf7, 0xd8);  // neg rax
            push_reg(parser, rax);
            return VALUE_INT;
        }
        if (type == VALUE_FLOAT) {
            x86_64_inst3(parser, 0x0f, 0x57, 0xc0);  // xorps xmm0, xmm0
            pop_reg(parser, xmm1);
            x86_64_inst4(parser, 0xf2, 0x0f, 0x5c, 0xc1);  // subsd xmm0, xmm1
            push_reg(parser, xmm0);
            return VALUE_FLOAT;
        }
#endif
#ifdef ARM64
        if (type == VALUE_INT) {
            pop_reg(parser, x0);
            arm64_inst(parser, 0xCB0003E0);  // sub x0, xzr, x0
            push_reg(parser, x0);
            return VALUE_INT;
        }
        if (type == VALUE_FLOAT) {
            pop_reg(parser, d0);
            arm64_inst(parser, 0x1E614000);  // fneg d0, d0
            push_reg(parser, d0);
            return VALUE_FLOAT;
        }
#endif
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
#ifdef X86_64
        mov_reg_imm(parser, rax, 0);
        push_reg(parser, rax);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, 0);
        push_reg(parser, x0);
#endif
        parser_eat(parser, TOKEN_NULL);
        return VALUE_NULL;
    }
    if (current()->type == TOKEN_FALSE) {
#ifdef X86_64
        mov_reg_imm(parser, rax, 0);
        push_reg(parser, rax);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, 0);
        push_reg(parser, x0);
#endif
        parser_eat(parser, TOKEN_FALSE);
        return VALUE_BOOL;
    }
    if (current()->type == TOKEN_TRUE) {
#ifdef X86_64
        mov_reg_imm(parser, rax, 1);
        push_reg(parser, rax);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, 1);
        push_reg(parser, x0);
#endif
        parser_eat(parser, TOKEN_TRUE);
        return VALUE_BOOL;
    }
    if (current()->type == TOKEN_INT) {
#ifdef X86_64
        mov_reg_imm(parser, rax, current()->integer);
        push_reg(parser, rax);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, current()->integer);
        push_reg(parser, x0);
#endif
        parser_eat(parser, TOKEN_INT);
        return VALUE_INT;
    }
    if (current()->type == TOKEN_FLOAT) {
        uint64_t floatAddr = (uint64_t)&parser->data[parser->dataPos];
        memcpy(&parser->data[parser->dataPos], &current()->floating, sizeof(double));
        parser->dataPos += sizeof(double);

#ifdef X86_64
        mov_reg_imm(parser, rax, floatAddr);
        x86_64_inst4(parser, 0xf2, 0x0f, 0x10, 0x00);  // movsd xmm0, [rax]
        push_reg(parser, xmm0);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, floatAddr);
        arm64_inst(parser, 0xFD400000);  // ldr d0, [x0]
        push_reg(parser, d0);
#endif
        parser_eat(parser, TOKEN_FLOAT);
        return VALUE_FLOAT;
    }
    if (current()->type == TOKEN_STRING) {
        uint64_t stringAddr = (uint64_t)&parser->data[parser->dataPos];
        size_t stringSize = strlen(current()->string) + 1;
        memcpy(&parser->data[parser->dataPos], current()->string, stringSize);
        parser->dataPos += align(stringSize, 8);

#ifdef X86_64
        mov_reg_imm(parser, rax, stringAddr);
        push_reg(parser, rax);
#endif
#ifdef ARM64
        mov_reg_imm(parser, x0, stringAddr);
        push_reg(parser, x0);
#endif
        parser_eat(parser, TOKEN_STRING);
        return VALUE_STRING;
    }

    fprintf(stderr, "Unexpected token: '%s'\n", token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
}

// Main
typedef int64_t (*JitIntFunc)(void);
typedef double (*JitFloatFunc)(void);
typedef char *(*JitStringFunc)(void);

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("New Bastiaan Language JIT\n");
#ifdef WIN32
        printf("Platform: win32, ");
#else
        printf("Platform: posix, ");
#endif
#ifdef X86_64
        printf("arch: x86_64\n");
#endif
#ifdef ARM64
        printf("arch: arm64\n");
#endif
        return EXIT_SUCCESS;
    }

    char *text = argv[1];
    printf("Command: %s\n", text);

    List *tokens = lexer(text);
    size_t PAGE_SIZE = 4096;
    Page *codePage = page_new(PAGE_SIZE);
    Page *dataPage = page_new(PAGE_SIZE);

    ValueType returnType = parser(tokens, codePage->data, dataPage->data);
    page_make_executable(codePage);

    if (returnType == VALUE_NULL || returnType == VALUE_BOOL || returnType == VALUE_INT) {
        JitIntFunc func = codePage->data;
        printf("Result: (int) %" PRIi64 "\n", func());
    }
    if (returnType == VALUE_FLOAT) {
        JitFloatFunc func = codePage->data;
        printf("Result: (float) %g\n", func());
    }
    if (returnType == VALUE_STRING) {
        JitStringFunc func = codePage->data;
        printf("Result: (string) %s\n", func());
    }

    list_free(tokens, (ListFreeFunc *)token_free);
    page_free(codePage);
    page_free(dataPage);
    return EXIT_SUCCESS;
}
