// New Bastiaan Language Interpreter
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utils header
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

size_t align(size_t size, size_t alignment);

char *file_read(char *path);

char *format(char *fmt, ...);

void error(char *text, size_t line, size_t position, char *fmt, ...);

// Utils
size_t align(size_t size, size_t alignment) { return (size + alignment - 1) / alignment * alignment; }

char *file_read(char *path) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(fileSize + 1);
    fileSize = fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

char *format(char *fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return strdup(buffer);
}

void error(char *text, size_t line, size_t position, char *fmt, ...) {
    fprintf(stderr, "text:%lu:%lu ERROR: ", line + 1, position + 1);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    // Seek to the right line in text
    char *c = text;
    for (size_t i = 0; i < line; i++) {
        while (*c != '\n' && *c != '\r') c++;
        if (*c == '\r') c++;
        c++;
    }
    char *lineStart = c;
    while (*c != '\n' && *c != '\r') c++;
    size_t lineLength = c - lineStart;

    fprintf(stderr, "\n%4lu | ", line + 1);
    fwrite(lineStart, 1, lineLength, stderr);
    fprintf(stderr, "\n     | ");
    for (size_t i = 0; i < position; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");
    exit(EXIT_FAILURE);
}

// List header
typedef struct List {
    void **items;
    size_t capacity;
    size_t size;
} List;

List *list_new(size_t capacity);

void *list_get(List *list, size_t index);

void list_set(List *list, size_t index, void *item);

void list_add(List *list, void *item);

typedef void ListFreeFunc(void *item);

void list_free(List *list, ListFreeFunc *freeFunc);

// List
List *list_new(size_t capacity) {
    List *list = malloc(sizeof(List));
    list->items = malloc(sizeof(void *) * capacity);
    list->capacity = capacity;
    list->size = 0;
    return list;
}

void *list_get(List *list, size_t index) {
    if (index < list->size) {
        return list->items[index];
    }
    return NULL;
}

void list_set(List *list, size_t index, void *item) {
    if (index >= list->capacity) {
        while (index >= list->capacity) list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    // if (index > list->size) {
    //     for (size_t i = list->size; i < index; i++) {
    //         list->items[i] = NULL;
    //     }
    //     list->size = index;
    // }
    list->items[index] = item;
}

void list_add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

void list_free(List *list, ListFreeFunc *freeFunction) {
    for (size_t i = 0; i < list->size; i++) {
        freeFunction(list_get(list, i));
    }
    free(list->items);
    free(list);
}

// Map header
typedef struct Map Map;

struct Map {
    Map *parent;
    char **keys;
    void **items;
    size_t capacity;
    size_t size;
};

Map *map_new(size_t capacity);

Map *map_new_child(size_t capacity, Map *parent);

void *map_get(Map *map, char *key);

void *map_get_without_parent(Map *map, char *key);

void map_set(Map *map, char *key, void *item);

void map_set_without_parent(Map *map, char *key, void *item);

typedef void MapFreeFunc(void *item);

void map_free(Map *map, MapFreeFunc *freeFunction);

void map_free_without_parent(Map *map, MapFreeFunc *freeFunction);

// Map
Map *map_new(size_t capacity) {
    Map *map = malloc(sizeof(Map));
    map->keys = malloc(sizeof(char *) * capacity);
    map->items = malloc(sizeof(void *) * capacity);
    map->capacity = capacity;
    map->size = 0;
    return map;
}

Map *map_new_child(size_t capacity, Map *parent) {
    Map *map = map_new(capacity);
    map->parent = parent;
    return map;
}

void *map_get(Map *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->items[i];
        }
    }
    if (map->parent != NULL) {
        return map_get(map->parent, key);
    }
    return NULL;
}

void *map_get_without_parent(Map *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->items[i];
        }
    }
    return NULL;
}

bool map_set_try(Map *map, char *key, void *item, bool checkParent) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            map->items[i] = item;
            return true;
        }
    }
    if (map->parent != NULL && checkParent) {
        return map_set_try(map->parent, key, item, checkParent);
    }
    return false;
}

void map_set(Map *map, char *key, void *item) {
    if (!map_set_try(map, key, item, true)) {
        if (map->size == map->capacity) {
            map->capacity *= 2;
            map->keys = realloc(map->keys, sizeof(char *) * map->capacity);
            map->items = realloc(map->items, sizeof(void *) * map->capacity);
        }
        map->keys[map->size] = key;
        map->items[map->size] = item;
        map->size++;
    }
}

void map_set_without_parent(Map *map, char *key, void *item) {
    if (!map_set_try(map, key, item, false)) {
        if (map->size == map->capacity) {
            map->capacity *= 2;
            map->keys = realloc(map->keys, sizeof(char *) * map->capacity);
            map->items = realloc(map->items, sizeof(void *) * map->capacity);
        }
        map->keys[map->size] = key;
        map->items[map->size] = item;
        map->size++;
    }
}

void map_free(Map *map, MapFreeFunc *freeFunction) {
    if (map->parent != NULL) {
        map_free(map->parent, freeFunction);
    }
    map_free_without_parent(map, freeFunction);
}

void map_free_without_parent(Map *map, MapFreeFunc *freeFunction) {
    for (size_t i = 0; i < map->size; i++) {
        free(map->keys[i]);
        freeFunction(map->items[i]);
    }
    free(map->keys);
    free(map->items);
    free(map);
}

// Lexer header
typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURLY,
    TOKEN_RCURLY,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_POINT,
    TOKEN_FAT_ARROW,

    TOKEN_KEYWORD,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,

    TOKEN_ASSIGN,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_EXP,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_AND,
    TOKEN_XOR,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_SHL,
    TOKEN_SHR,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_LTEQ,
    TOKEN_GT,
    TOKEN_GTEQ,
    TOKEN_LOGICAL_AND,
    TOKEN_LOGICAL_OR,
    TOKEN_LOGICAL_NOT,

    TOKEN_TYPE_ANY,
    TOKEN_NULL,
    TOKEN_TYPE_BOOLEAN,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_ARRAY,
    TOKEN_TYPE_OBJECT,
    TOKEN_TYPE_FUNCTION,
    TOKEN_FUNCTION,

    TOKEN_CONST,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_CONTINUE,
    TOKEN_BREAK,
    TOKEN_RETURN
} TokenType;

typedef struct Token {
    TokenType type;
    size_t line;
    size_t position;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
} Token;

Token *token_new(TokenType type, size_t line, size_t position);

Token *token_new_int(size_t line, size_t position, int64_t integer);

Token *token_new_float(size_t line, size_t position, double floating);

Token *token_new_string(TokenType type, size_t line, size_t position, char *string);

bool token_type_is_type(TokenType type);

char *token_type_to_string(TokenType type);

void token_free(Token *token);

int64_t string_to_int(char *string);

double string_to_float(char *string);

typedef struct Keyword {
    char *keyword;
    TokenType type;
} Keyword;

List *lexer(char *text);

// Lexer
Token *token_new(TokenType type, size_t line, size_t position) {
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->line = line;
    token->position = position;
    return token;
}

Token *token_new_int(size_t line, size_t position, int64_t integer) {
    Token *token = token_new(TOKEN_INT, line, position);
    token->integer = integer;
    return token;
}

Token *token_new_float(size_t line, size_t position, double floating) {
    Token *token = token_new(TOKEN_FLOAT, line, position);
    token->floating = floating;
    return token;
}

Token *token_new_string(TokenType type, size_t line, size_t position, char *string) {
    Token *token = token_new(type, line, position);
    token->string = string;
    return token;
}

bool token_type_is_type(TokenType type) {
    return type == TOKEN_TYPE_ANY || type == TOKEN_NULL || type == TOKEN_TYPE_BOOLEAN || type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT ||
           type == TOKEN_TYPE_STRING || type == TOKEN_TYPE_ARRAY || type == TOKEN_TYPE_OBJECT || type == TOKEN_TYPE_FUNCTION;
}

char *token_type_to_string(TokenType type) {
    if (type == TOKEN_KEYWORD) return "keyword";
    if (type == TOKEN_INT) return "int";
    if (type == TOKEN_FLOAT) return "float";
    if (type == TOKEN_STRING) return "string";

    if (type == TOKEN_EOF) return "EOF";
    if (type == TOKEN_LPAREN) return "(";
    if (type == TOKEN_RPAREN) return ")";
    if (type == TOKEN_LCURLY) return "{";
    if (type == TOKEN_RCURLY) return "}";
    if (type == TOKEN_LBRACKET) return "[";
    if (type == TOKEN_RBRACKET) return "]";
    if (type == TOKEN_SEMICOLON) return ";";
    if (type == TOKEN_COLON) return ":";
    if (type == TOKEN_COMMA) return ",";
    if (type == TOKEN_POINT) return ".";
    if (type == TOKEN_FAT_ARROW) return "=>";

    if (type == TOKEN_ASSIGN) return "=";
    if (type == TOKEN_ADD) return "+";
    if (type == TOKEN_SUB) return "-";
    if (type == TOKEN_MUL) return "*";
    if (type == TOKEN_EXP) return "**";
    if (type == TOKEN_DIV) return "/";
    if (type == TOKEN_MOD) return "%";
    if (type == TOKEN_AND) return "&";
    if (type == TOKEN_XOR) return "^";
    if (type == TOKEN_OR) return "|";
    if (type == TOKEN_NOT) return "~";
    if (type == TOKEN_SHL) return "<<";
    if (type == TOKEN_SHR) return ">>";
    if (type == TOKEN_EQ) return "==";
    if (type == TOKEN_NEQ) return "!=";
    if (type == TOKEN_LT) return "<";
    if (type == TOKEN_LTEQ) return "<=";
    if (type == TOKEN_GT) return ">";
    if (type == TOKEN_GTEQ) return ">=";
    if (type == TOKEN_LOGICAL_OR) return "||";
    if (type == TOKEN_LOGICAL_AND) return "&&";
    if (type == TOKEN_LOGICAL_NOT) return "!";

    if (type == TOKEN_TYPE_ANY) return "any";
    if (type == TOKEN_NULL) return "null";
    if (type == TOKEN_TYPE_BOOLEAN) return "boolean";
    if (type == TOKEN_TRUE) return "true";
    if (type == TOKEN_FALSE) return "false";
    if (type == TOKEN_TYPE_INT) return "int";
    if (type == TOKEN_TYPE_FLOAT) return "float";
    if (type == TOKEN_TYPE_STRING) return "string";
    if (type == TOKEN_TYPE_ARRAY) return "array";
    if (type == TOKEN_TYPE_OBJECT) return "object";
    if (type == TOKEN_TYPE_FUNCTION) return "function";
    if (type == TOKEN_FUNCTION) return "fn";

    if (type == TOKEN_CONST) return "const";
    if (type == TOKEN_LET) return "let";
    if (type == TOKEN_IF) return "if";
    if (type == TOKEN_ELSE) return "else";
    if (type == TOKEN_WHILE) return "while";
    if (type == TOKEN_DO) return "do";
    if (type == TOKEN_FOR) return "for";
    if (type == TOKEN_IN) return "in";
    if (type == TOKEN_CONTINUE) return "continue";
    if (type == TOKEN_BREAK) return "break";
    if (type == TOKEN_RETURN) return "return";
    return NULL;
}

void token_free(Token *token) {
    if (token->type == TOKEN_KEYWORD || token->type == TOKEN_STRING) free(token->string);
    free(token);
}

int64_t string_to_int(char *string) {
    char *c = string;
    if (*c == '0' && *(c + 1) == 'b') {
        c += 2;
        return strtol(c, NULL, 2);
    }
    if (*c == '0' && isdigit(*(c + 1))) {
        c++;
        return strtol(c, NULL, 8);
    }
    if (*c == '0' && *(c + 1) == 'x') {
        c += 2;
        return strtol(c, NULL, 16);
    }
    return strtol(c, NULL, 10);
}

double string_to_float(char *string) { return strtod(string, NULL); }

List *lexer(char *text) {
    Keyword keywords[] = {{"any", TOKEN_TYPE_ANY},
                          {"null", TOKEN_NULL},
                          {"boolean", TOKEN_TYPE_BOOLEAN},
                          {"true", TOKEN_TRUE},
                          {"false", TOKEN_FALSE},
                          {"int", TOKEN_TYPE_INT},
                          {"float", TOKEN_TYPE_FLOAT},
                          {"string", TOKEN_TYPE_STRING},
                          {"array", TOKEN_TYPE_ARRAY},
                          {"object", TOKEN_TYPE_OBJECT},
                          {"function", TOKEN_TYPE_FUNCTION},
                          {"fn", TOKEN_FUNCTION},

                          {"const", TOKEN_CONST},
                          {"let", TOKEN_LET},
                          {"if", TOKEN_IF},
                          {"else", TOKEN_ELSE},
                          {"while", TOKEN_WHILE},
                          {"do", TOKEN_DO},
                          {"for", TOKEN_FOR},
                          {"in", TOKEN_IN},
                          {"continue", TOKEN_CONTINUE},
                          {"break", TOKEN_BREAK},
                          {"return", TOKEN_RETURN}};

    List *tokens = list_new(1024);
    char *c = text;
    int32_t line = 0;
    char *lineStart = c;
    while (*c != '\0') {
        size_t position = c - lineStart;

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
            while (*c != '*' && *(c + 1) != '/') {
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

        // Integers
        if (*c == '0' && *(c + 1) == 'b') {
            c += 2;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 2)));
            continue;
        }
        if (*c == '0' && isdigit(*(c + 1))) {
            c++;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 8)));
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 16)));
            continue;
        }
        if (isdigit(*c) || (*c == '.' && isdigit(*(c + 1)))) {
            double floating = strtod(c, &c);
            if ((double)((int64_t)floating) == floating) {
                list_add(tokens, token_new_int(line, position, (int64_t)floating));
            } else {
                list_add(tokens, token_new_float(line, position, floating));
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
            list_add(tokens, token_new_string(TOKEN_STRING, line, position, string));
            continue;
        }

        // Keywords
        if (isalpha(*c) || *c == '_' || *c == '$') {
            char *ptr = c;
            while (isalnum(*c) || *c == '_' || *c == '$') c++;
            size_t size = c - ptr;
            char *string = malloc(size + 1);
            memcpy(string, ptr, size);
            string[size] = '\0';

            bool found = false;
            for (size_t i = 0; i < sizeof(keywords) / sizeof(Keyword); i++) {
                Keyword *keyword = &keywords[i];
                if (!strcmp(string, keyword->keyword)) {
                    list_add(tokens, token_new(keyword->type, line, position));
                    found = true;
                    break;
                }
            }
            if (!found) {
                list_add(tokens, token_new_string(TOKEN_KEYWORD, line, position, string));
            }
            continue;
        }

        // Syntax
        if (*c == '(') {
            list_add(tokens, token_new(TOKEN_LPAREN, line, position));
            c++;
            continue;
        }
        if (*c == ')') {
            list_add(tokens, token_new(TOKEN_RPAREN, line, position));
            c++;
            continue;
        }
        if (*c == '{') {
            list_add(tokens, token_new(TOKEN_LCURLY, line, position));
            c++;
            continue;
        }
        if (*c == '}') {
            list_add(tokens, token_new(TOKEN_RCURLY, line, position));
            c++;
            continue;
        }
        if (*c == '[') {
            list_add(tokens, token_new(TOKEN_LBRACKET, line, position));
            c++;
            continue;
        }
        if (*c == ']') {
            list_add(tokens, token_new(TOKEN_RBRACKET, line, position));
            c++;
            continue;
        }
        if (*c == ';') {
            list_add(tokens, token_new(TOKEN_SEMICOLON, line, position));
            c++;
            continue;
        }
        if (*c == ':') {
            list_add(tokens, token_new(TOKEN_COLON, line, position));
            c++;
            continue;
        }
        if (*c == ',') {
            list_add(tokens, token_new(TOKEN_COMMA, line, position));
            c++;
            continue;
        }
        if (*c == '.') {
            list_add(tokens, token_new(TOKEN_POINT, line, position));
            c++;
            continue;
        }

        // Operators
        if (*c == '=') {
            if (*(c + 1) == '>') {
                list_add(tokens, token_new(TOKEN_FAT_ARROW, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_EQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_ASSIGN, line, position));
            c++;
            continue;
        }
        if (*c == '+') {
            list_add(tokens, token_new(TOKEN_ADD, line, position));
            c++;
            continue;
        }
        if (*c == '-') {
            list_add(tokens, token_new(TOKEN_SUB, line, position));
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '*') {
                list_add(tokens, token_new(TOKEN_EXP, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MUL, line, position));
            c++;
            continue;
        }
        if (*c == '/') {
            list_add(tokens, token_new(TOKEN_DIV, line, position));
            c++;
            continue;
        }
        if (*c == '%') {
            list_add(tokens, token_new(TOKEN_MOD, line, position));
            c++;
            continue;
        }
        if (*c == '^') {
            list_add(tokens, token_new(TOKEN_XOR, line, position));
            c++;
            continue;
        }
        if (*c == '~') {
            list_add(tokens, token_new(TOKEN_NOT, line, position));
            c++;
            continue;
        }
        if (*c == '<') {
            if (*(c + 1) == '<') {
                list_add(tokens, token_new(TOKEN_SHL, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_LTEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LT, line, position));
            c++;
            continue;
        }
        if (*c == '>') {
            if (*(c + 1) == '>') {
                list_add(tokens, token_new(TOKEN_SHR, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_GTEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_GT, line, position));
            c++;
            continue;
        }
        if (*c == '!') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_NEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LOGICAL_NOT, line, position));
            c++;
            continue;
        }
        if (*c == '|') {
            if (*(c + 1) == '|') {
                list_add(tokens, token_new(TOKEN_LOGICAL_OR, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_OR, line, position));
            c++;
            continue;
        }
        if (*c == '&') {
            if (*(c + 1) == '&') {
                list_add(tokens, token_new(TOKEN_LOGICAL_AND, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_AND, line, position));
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

        error(text, line, position, "Unexpected character: '%c'", *c);
    }
    list_add(tokens, token_new(TOKEN_EOF, line, c - lineStart));
    return tokens;
}

// Value header

// Forward defines
typedef struct Node Node;
void node_free(Node *node);

typedef enum ValueType {
    VALUE_ANY,
    VALUE_NULL,
    VALUE_BOOLEAN,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_ARRAY,
    VALUE_OBJECT,
    VALUE_FUNCTION,
    VALUE_NATIVE_FUNCTION
} ValueType;

typedef struct Argument {
    char *name;
    ValueType type;
    Node *defaultNode;
} Argument;

Argument *argument_new(char *name, ValueType type, Node *defaultNode);

void argument_free(Argument *argument);

typedef struct Value Value;

struct Value {
    ValueType type;
    union {
        bool boolean;
        int64_t integer;
        double floating;
        char *string;
        List *array;
        Map *object;
        struct {
            List *arguments;
            ValueType returnType;
            union {
                Node *functionNode;
                Value *(*nativeFunc)(List *values);
            };
        };
    };
};

Value *value_new(ValueType type);

Value *value_new_null(void);

Value *value_new_boolean(bool boolean);

Value *value_new_int(int64_t integer);

Value *value_new_float(double integer);

Value *value_new_string(char *string);

Value *value_new_array(List *array);

Value *value_new_object(Map *map);

Value *value_new_function(List *args, ValueType returnType, Node *node);

Value *value_new_native_function(List *args, ValueType returnType, Value *(*nativeFunc)(List *values));

char *value_type_to_string(ValueType type);

ValueType token_type_to_value_type(TokenType type);

char *value_to_string(Value *value);

void value_free(Value *value);

// Value

Argument *argument_new(char *name, ValueType type, Node *defaultNode) {
    Argument *argument = malloc(sizeof(Argument));
    argument->name = name;
    argument->type = type;
    argument->defaultNode = defaultNode;
    return argument;
}

void argument_free(Argument *argument) {
    free(argument->name);
    if (argument->defaultNode != NULL) {
        node_free(argument->defaultNode);
    }
    free(argument);
}

Value *value_new(ValueType type) {
    Value *value = malloc(sizeof(Value));
    value->type = type;
    return value;
}

Value *value_new_null(void) { return value_new(VALUE_NULL); }

Value *value_new_boolean(bool boolean) {
    Value *value = value_new(VALUE_BOOLEAN);
    value->boolean = boolean;
    return value;
}

Value *value_new_int(int64_t integer) {
    Value *value = value_new(VALUE_INT);
    value->integer = integer;
    return value;
}

Value *value_new_float(double floating) {
    Value *value = value_new(VALUE_FLOAT);
    value->floating = floating;
    return value;
}

Value *value_new_string(char *string) {
    Value *value = value_new(VALUE_STRING);
    value->string = string;
    return value;
}

Value *value_new_array(List *array) {
    Value *value = value_new(VALUE_ARRAY);
    value->array = array;
    return value;
}

Value *value_new_object(Map *object) {
    Value *value = value_new(VALUE_OBJECT);
    value->object = object;
    return value;
}

Value *value_new_function(List *args, ValueType returnType, Node *functionNode) {
    Value *value = value_new(VALUE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->functionNode = functionNode;
    return value;
}

Value *value_new_native_function(List *args, ValueType returnType, Value *(*nativeFunc)(List *values)) {
    Value *value = value_new(VALUE_NATIVE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->nativeFunc = nativeFunc;
    return value;
}

char *value_type_to_string(ValueType type) {
    if (type == VALUE_NULL) return "null";
    if (type == VALUE_BOOLEAN) return "boolean";
    if (type == VALUE_INT) return "int";
    if (type == VALUE_FLOAT) return "float";
    if (type == VALUE_STRING) return "string";
    if (type == VALUE_ARRAY) return "array";
    if (type == VALUE_OBJECT) return "object";
    if (type == VALUE_FUNCTION || type == VALUE_NATIVE_FUNCTION) return "function";
    return NULL;
}

char *value_to_string(Value *value) {
    if (value->type == VALUE_NULL) return strdup("null");
    if (value->type == VALUE_BOOLEAN) return strdup(value->boolean ? "true" : "false");
    if (value->type == VALUE_INT) return format("%lld", value->integer);
    if (value->type == VALUE_FLOAT) return format("%g", value->floating);
    if (value->type == VALUE_STRING) return strdup(value->string);
    if (value->type == VALUE_ARRAY) return strdup("array");
    if (value->type == VALUE_OBJECT) return strdup("object");
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) return strdup("function");
    return NULL;
}

ValueType token_type_to_value_type(TokenType type) {
    if (type == TOKEN_TYPE_ANY) return VALUE_ANY;
    if (type == TOKEN_NULL) return VALUE_NULL;
    if (type == TOKEN_TYPE_BOOLEAN) return VALUE_BOOLEAN;
    if (type == TOKEN_TYPE_INT) return VALUE_INT;
    if (type == TOKEN_TYPE_FLOAT) return VALUE_FLOAT;
    if (type == TOKEN_TYPE_STRING) return VALUE_STRING;
    if (type == TOKEN_TYPE_ARRAY) return VALUE_ARRAY;
    if (type == TOKEN_TYPE_OBJECT) return VALUE_OBJECT;
    if (type == TOKEN_TYPE_FUNCTION) return VALUE_FUNCTION;
    return 0;
}

void value_free(Value *value) {
    if (value->type == VALUE_STRING) {
        free(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        list_free(value->array, (ListFreeFunc *)value_free);
    }
    if (value->type == VALUE_OBJECT) {
        map_free(value->object, (MapFreeFunc *)value_free);
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        list_free(value->arguments, (ListFreeFunc *)argument_free);
    }
    if (value->type == VALUE_FUNCTION) {
        node_free(value->functionNode);
    }
    free(value);
}

// Parser header
typedef enum NodeType {
    NODE_PROGRAM,
    NODE_NODES,
    NODE_BLOCK,
    NODE_IF,
    NODE_WHILE,
    NODE_DOWHILE,
    NODE_FOR,
    NODE_FORIN,
    NODE_CONTINUE,
    NODE_BREAK,
    NODE_RETURN,

    NODE_VALUE,
    NODE_ARRAY,
    NODE_OBJECT,
    NODE_CALL,

    NODE_VARIABLE,
    NODE_GET,

    NODE_NEG,
    NODE_NOT,
    NODE_LOGICAL_NOT,
    NODE_CAST,

    NODE_CONST_ASSIGN,
    NODE_LET_ASSIGN,
    NODE_ASSIGN,

    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_EXP,
    NODE_DIV,
    NODE_MOD,
    NODE_AND,
    NODE_XOR,
    NODE_OR,
    NODE_SHL,
    NODE_SHR,
    NODE_EQ,
    NODE_NEQ,
    NODE_LT,
    NODE_LTEQ,
    NODE_GT,
    NODE_GTEQ,
    NODE_LOGICAL_AND,
    NODE_LOGICAL_OR
} NodeType;

struct Node {
    NodeType type;
    Token *token;
    union {
        Value *value;
        char *string;
        struct {
            ValueType castType;
            Node *unary;
        };
        struct {
            ValueType declarationType;
            Node *lhs;
            Node *rhs;
        };
        struct {
            union {
                Node *condition;
                Node *iterator;
            };
            Node *thenBlock;
            union {
                Node *elseBlock;
                Node *incrementBlock;
                Node *variable;
            };
        };
        struct {
            Node *function;
            List *keys;
            List *nodes;
        };
    };
};

Node *node_new(NodeType type, Token *token);

Node *node_new_value(Token *token, Value *value);

Node *node_new_string(NodeType type, Token *token, char *string);

Node *node_new_unary(NodeType type, Token *token, Node *unary);

Node *node_new_cast(Token *token, ValueType castType, Node *unary);

Node *node_new_operation(NodeType type, Token *token, Node *lhs, Node *rhs);

Node *node_new_multiple(NodeType type, Token *token);

typedef struct Parser {
    char *text;
    List *tokens;
    int32_t position;
} Parser;

Node *parser(char *text, List *tokens);

void parser_eat(Parser *parser, TokenType type);

Node *parser_program(Parser *parser);
Node *parser_block(Parser *parser);
Node *parser_statement(Parser *parser);
Node *parser_declarations(Parser *parser);
Node *parser_assigns(Parser *parser);
Node *parser_assign(Parser *parser);
Node *parser_logical(Parser *parser);
Node *parser_bitwise(Parser *parser);
Node *parser_equality(Parser *parser);
Node *parser_relational(Parser *parser);
Node *parser_shift(Parser *parser);
Node *parser_add(Parser *parser);
Node *parser_mul(Parser *parser);
Node *parser_unary(Parser *parser);
Node *parser_primary(Parser *parser);
Node *parser_identifier(Parser *parser);
Node *parser_identifier_suffix(Parser *parser, Node *node);
Argument *parser_argument(Parser *parser);

// Parser
Node *node_new(NodeType type, Token *token) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->token = token;
    return node;
}

Node *node_new_value(Token *token, Value *value) {
    Node *node = node_new(NODE_VALUE, token);
    node->value = value;
    return node;
}

Node *node_new_string(NodeType type, Token *token, char *string) {
    Node *node = node_new(type, token);
    node->string = string;
    return node;
}

Node *node_new_unary(NodeType type, Token *token, Node *unary) {
    Node *node = node_new(type, token);
    node->unary = unary;
    return node;
}

Node *node_new_cast(Token *token, ValueType castType, Node *unary) {
    Node *node = node_new(NODE_CAST, token);
    node->castType = castType;
    node->unary = unary;
    return node;
}

Node *node_new_operation(NodeType type, Token *token, Node *lhs, Node *rhs) {
    Node *node = node_new(type, token);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *node_new_multiple(NodeType type, Token *token) {
    Node *node = node_new(type, token);
    node->nodes = list_new(8);
    return node;
}

void node_free(Node *node) {
    if (node->type == NODE_VALUE) {
        value_free(node->value);
    }
    if (node->type == NODE_VARIABLE) {
        free(node->string);
    }
    if (node->type == NODE_RETURN || (node->type >= NODE_NEG && node->type <= NODE_CAST)) {
        node_free(node->unary);
    }
    if (node->type >= NODE_CONST_ASSIGN && node->type <= NODE_LOGICAL_OR) {
        node_free(node->lhs);
        node_free(node->rhs);
    }
    if (node->type >= NODE_IF && node->type <= NODE_FORIN) {
        node_free(node->condition);
        node_free(node->thenBlock);
        if (node->elseBlock != NULL) node_free(node->elseBlock);
    }
    if ((node->type >= NODE_PROGRAM && node->type <= NODE_BLOCK) || (node->type >= NODE_ARRAY && node->type <= NODE_CALL)) {
        if (node->type == NODE_CALL) node_free(node->function);
        if (node->type == NODE_OBJECT) list_free(node->keys, free);
        list_free(node->nodes, (ListFreeFunc *)node_free);
    }
    free(node);
}

Node *parser(char *text, List *tokens) {
    Parser parser;
    parser.text = text;
    parser.tokens = tokens;
    parser.position = 0;
    return parser_program(&parser);
}

#define current() ((Token *)list_get(parser->tokens, parser->position))
#define next(pos) ((Token *)list_get(parser->tokens, parser->position + 1 + pos))

void parser_eat(Parser *parser, TokenType type) {
    if (current()->type == type) {
        parser->position++;
    } else {
        error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed '%s'", token_type_to_string(current()->type),
              token_type_to_string(type));
    }
}

Node *parser_program(Parser *parser) {
    Node *programNode = node_new_multiple(NODE_PROGRAM, current());
    while (current()->type != TOKEN_EOF) {
        Node *node = parser_statement(parser);
        if (node != NULL) list_add(programNode->nodes, node);
    }
    return programNode;
}

Node *parser_block(Parser *parser) {
    Node *blockNode = node_new_multiple(NODE_BLOCK, current());
    if (current()->type == TOKEN_LCURLY) {
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            Node *node = parser_statement(parser);
            if (node != NULL) list_add(blockNode->nodes, node);
        }
        parser_eat(parser, TOKEN_RCURLY);
    } else {
        Node *node = parser_statement(parser);
        if (node != NULL) list_add(blockNode->nodes, node);
    }
    return blockNode;
}

Node *parser_statement(Parser *parser) {
    if (current()->type == TOKEN_SEMICOLON) {
        parser_eat(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    if (current()->type == TOKEN_LCURLY) {
        return parser_block(parser);
    }

    if (current()->type == TOKEN_IF) {
        Node *node = node_new(NODE_IF, current());
        parser_eat(parser, TOKEN_IF);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        node->thenBlock = parser_block(parser);
        if (current()->type == TOKEN_ELSE) {
            parser_eat(parser, TOKEN_ELSE);
            node->elseBlock = parser_block(parser);
        } else {
            node->elseBlock = NULL;
        }
        return node;
    }

    if (current()->type == TOKEN_WHILE) {
        Node *node = node_new(NODE_WHILE, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_WHILE);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        node->thenBlock = parser_block(parser);
        return node;
    }

    if (current()->type == TOKEN_DO) {
        Node *node = node_new(NODE_DOWHILE, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_DO);
        node->thenBlock = parser_block(parser);
        parser_eat(parser, TOKEN_WHILE);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FOR) {
        parser_eat(parser, TOKEN_FOR);
        parser_eat(parser, TOKEN_LPAREN);
        Node *declarations;
        if (current()->type != TOKEN_SEMICOLON) {
            declarations = parser_declarations(parser);
        }

        if (current()->type == TOKEN_IN) {
            Node *node = node_new(NODE_FORIN, current());
            if (declarations->type == NODE_NODES) {
                error(parser->text, declarations->token->line, declarations->token->position, "You can only declare one variable in a for in loop");
            }
            node->variable = declarations;
            parser_eat(parser, TOKEN_IN);
            node->iterator = parser_logical(parser);
            parser_eat(parser, TOKEN_RPAREN);
            node->thenBlock = parser_block(parser);
            return node;
        }

        Node *blockNode = node_new_multiple(NODE_BLOCK, current());
        list_add(blockNode->nodes, declarations);

        Node *node = node_new(NODE_FOR, current());
        parser_eat(parser, TOKEN_SEMICOLON);
        if (current()->type != TOKEN_SEMICOLON) {
            node->condition = parser_assigns(parser);
        } else {
            node->condition = NULL;
        }
        parser_eat(parser, TOKEN_SEMICOLON);

        if (current()->type != TOKEN_RPAREN) {
            node->incrementBlock = parser_assigns(parser);
        } else {
            node->incrementBlock = NULL;
        }
        parser_eat(parser, TOKEN_RPAREN);

        node->thenBlock = parser_block(parser);
        list_add(blockNode->nodes, node);

        if (blockNode->nodes->size == 1) {
            return list_get(blockNode->nodes, 0);
        }
        return blockNode;
    }

    if (current()->type == TOKEN_CONTINUE) {
        Token *token = current();
        parser_eat(parser, TOKEN_CONTINUE);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_CONTINUE, token);
    }
    if (current()->type == TOKEN_BREAK) {
        Token *token = current();
        parser_eat(parser, TOKEN_BREAK);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_BREAK, token);
    }
    if (current()->type == TOKEN_RETURN) {
        Token *token = current();
        parser_eat(parser, TOKEN_RETURN);
        Node *node = node_new_unary(NODE_RETURN, token, parser_logical(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        Token *nameToken = current();
        char *name = strdup(current()->string);
        parser_eat(parser, TOKEN_KEYWORD);

        List *arguments = list_new(4);
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(arguments, parser_argument(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);

        ValueType returnType = VALUE_ANY;
        if (current()->type == TOKEN_COLON) {
            parser_eat(parser, TOKEN_COLON);
            if (token_type_is_type(current()->type)) {
                returnType = token_type_to_value_type(current()->type);
                parser_eat(parser, current()->type);
            } else {
                error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
            }
        }

        Node *variable = node_new_string(NODE_VARIABLE, nameToken, name);
        if (current()->type == TOKEN_FAT_ARROW) {
            Token *token = current();
            parser_eat(parser, TOKEN_FAT_ARROW);
            Node *returnNode = node_new_unary(NODE_RETURN, token, parser_logical(parser));
            Node *node = node_new_operation(NODE_CONST_ASSIGN, functionToken, variable,
                                            node_new_value(functionToken, value_new_function(arguments, returnType, returnNode)));
            node->declarationType = VALUE_FUNCTION;
            return node;
        }

        Value *functionValue = value_new_function(arguments, returnType, parser_block(parser));
        Node *node = node_new_operation(NODE_CONST_ASSIGN, functionToken, variable, node_new_value(functionToken, functionValue));
        node->declarationType = VALUE_FUNCTION;
        return node;
    }

    Node *node = parser_declarations(parser);
    parser_eat(parser, TOKEN_SEMICOLON);
    return node;
}

Node *parser_declarations(Parser *parser) {
    if (current()->type == TOKEN_CONST || current()->type == TOKEN_LET) {
        Node *blockNode = node_new_multiple(NODE_NODES, current());
        NodeType assignType;
        if (current()->type == TOKEN_CONST) {
            assignType = NODE_CONST_ASSIGN;
            parser_eat(parser, TOKEN_CONST);
        }
        if (current()->type == TOKEN_LET) {
            assignType = NODE_LET_ASSIGN;
            parser_eat(parser, TOKEN_LET);
        }

        for (;;) {
            Node *variable = node_new_string(NODE_VARIABLE, current(), strdup(current()->string));
            parser_eat(parser, TOKEN_KEYWORD);

            ValueType declarationType = VALUE_ANY;
            if (current()->type == TOKEN_COLON) {
                parser_eat(parser, TOKEN_COLON);
                if (token_type_is_type(current()->type)) {
                    declarationType = token_type_to_value_type(current()->type);
                    parser_eat(parser, current()->type);
                } else {
                    error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token",
                          token_type_to_string(current()->type));
                }
            }

            Node *node;
            if (current()->type == TOKEN_ASSIGN) {
                Token *token = current();
                parser_eat(parser, TOKEN_ASSIGN);
                node = node_new_operation(assignType, token, variable, parser_assign(parser));
            } else {
                node = node_new_operation(assignType, variable->token, variable, node_new_value(variable->token, value_new_null()));
            }
            node->declarationType = declarationType;
            list_add(blockNode->nodes, node);

            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        if (blockNode->nodes->size == 0) {
            return NULL;
        } else if (blockNode->nodes->size == 1) {
            return list_get(blockNode->nodes, 0);
        }
        return blockNode;
    }
    return parser_assigns(parser);
}

Node *parser_assigns(Parser *parser) {
    Node *blockNode = node_new_multiple(NODE_NODES, current());
    for (;;) {
        Node *node = parser_assign(parser);
        list_add(blockNode->nodes, node);
        if (current()->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
        } else {
            break;
        }
    }
    if (blockNode->nodes->size == 0) {
        return NULL;
    } else if (blockNode->nodes->size == 1) {
        return list_get(blockNode->nodes, 0);
    }
    return blockNode;
}

Node *parser_assign(Parser *parser) {
    Node *lhs = parser_logical(parser);  // TODO
    if (current()->type == TOKEN_ASSIGN) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN);
        return node_new_operation(NODE_ASSIGN, token, lhs, parser_assign(parser));
    }
    return lhs;
}

Node *parser_logical(Parser *parser) {
    Node *node = parser_bitwise(parser);
    while (current()->type == TOKEN_LOGICAL_AND || current()->type == TOKEN_LOGICAL_OR) {
        if (current()->type == TOKEN_LOGICAL_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_AND);
            node = node_new_operation(NODE_LOGICAL_AND, token, node, parser_bitwise(parser));
        }
        if (current()->type == TOKEN_LOGICAL_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_OR);
            node = node_new_operation(NODE_LOGICAL_OR, token, node, parser_bitwise(parser));
        }
    }
    return node;
}

Node *parser_bitwise(Parser *parser) {
    Node *node = parser_equality(parser);
    while (current()->type == TOKEN_AND || current()->type == TOKEN_XOR || current()->type == TOKEN_OR) {
        if (current()->type == TOKEN_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_AND);
            node = node_new_operation(NODE_AND, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_XOR) {
            Token *token = current();
            parser_eat(parser, TOKEN_XOR);
            node = node_new_operation(NODE_XOR, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_OR);
            node = node_new_operation(NODE_OR, token, node, parser_equality(parser));
        }
    }
    return node;
}

Node *parser_equality(Parser *parser) {
    Node *node = parser_relational(parser);
    while (current()->type == TOKEN_EQ || current()->type == TOKEN_NEQ) {
        if (current()->type == TOKEN_EQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_EQ);
            node = node_new_operation(NODE_EQ, token, node, parser_relational(parser));
        }
        if (current()->type == TOKEN_NEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_NEQ);
            node = node_new_operation(NODE_NEQ, token, node, parser_relational(parser));
        }
    }
    return node;
}

Node *parser_relational(Parser *parser) {
    Node *node = parser_shift(parser);
    while (current()->type == TOKEN_LT || current()->type == TOKEN_LTEQ || current()->type == TOKEN_GT || current()->type == TOKEN_GTEQ) {
        if (current()->type == TOKEN_LT) {
            Token *token = current();
            parser_eat(parser, TOKEN_LT);
            node = node_new_operation(NODE_LT, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_LTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_LTEQ);
            node = node_new_operation(NODE_LTEQ, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_GT) {
            Token *token = current();
            parser_eat(parser, TOKEN_GT);
            node = node_new_operation(NODE_GT, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_GTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_GTEQ);
            node = node_new_operation(NODE_GTEQ, token, node, parser_shift(parser));
        }
    }
    return node;
}

Node *parser_shift(Parser *parser) {
    Node *node = parser_add(parser);
    while (current()->type == TOKEN_SHL || current()->type == TOKEN_SHR) {
        if (current()->type == TOKEN_SHL) {
            Token *token = current();
            parser_eat(parser, TOKEN_SHL);
            node = node_new_operation(NODE_SHL, token, node, parser_add(parser));
        }

        if (current()->type == TOKEN_SHR) {
            Token *token = current();
            parser_eat(parser, TOKEN_SHR);
            node = node_new_operation(NODE_SHR, token, node, parser_add(parser));
        }
    }
    return node;
}

Node *parser_add(Parser *parser) {
    Node *node = parser_mul(parser);
    while (current()->type == TOKEN_ADD || current()->type == TOKEN_SUB) {
        if (current()->type == TOKEN_ADD) {
            Token *token = current();
            parser_eat(parser, TOKEN_ADD);
            node = node_new_operation(NODE_ADD, token, node, parser_mul(parser));
        }

        if (current()->type == TOKEN_SUB) {
            Token *token = current();
            parser_eat(parser, TOKEN_SUB);
            node = node_new_operation(NODE_SUB, token, node, parser_mul(parser));
        }
    }
    return node;
}

Node *parser_mul(Parser *parser) {
    Node *node = parser_unary(parser);
    while (current()->type == TOKEN_MUL || current()->type == TOKEN_EXP || current()->type == TOKEN_DIV || current()->type == TOKEN_MOD) {
        if (current()->type == TOKEN_MUL) {
            Token *token = current();
            parser_eat(parser, TOKEN_MUL);
            node = node_new_operation(NODE_MUL, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_EXP) {
            Token *token = current();
            parser_eat(parser, TOKEN_EXP);
            node = node_new_operation(NODE_EXP, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_DIV) {
            Token *token = current();
            parser_eat(parser, TOKEN_DIV);
            node = node_new_operation(NODE_DIV, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_MOD) {
            Token *token = current();
            parser_eat(parser, TOKEN_MOD);
            node = node_new_operation(NODE_MOD, token, node, parser_unary(parser));
        }
    }
    return node;
}

Node *parser_unary(Parser *parser) {
    if (current()->type == TOKEN_ADD) {
        parser_eat(parser, TOKEN_ADD);
        return parser_unary(parser);
    }
    if (current()->type == TOKEN_SUB) {
        Token *token = current();
        parser_eat(parser, TOKEN_SUB);
        return node_new_unary(NODE_NEG, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_NOT) {
        Token *token = current();
        parser_eat(parser, TOKEN_NOT);
        return node_new_unary(NODE_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LOGICAL_NOT) {
        Token *token = current();
        parser_eat(parser, TOKEN_LOGICAL_NOT);
        return node_new_unary(NODE_LOGICAL_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LPAREN && token_type_is_type(next(0)->type) && next(1)->type == TOKEN_RPAREN) {
        Token *token = current();
        parser_eat(parser, TOKEN_LPAREN);
        if (token_type_is_type(current()->type)) {
            ValueType castType = token_type_to_value_type(current()->type);
            parser_eat(parser, current()->type);
            parser_eat(parser, TOKEN_RPAREN);
            return node_new_cast(token, castType, parser_unary(parser));
        }
        error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
    }
    return parser_primary(parser);
}

Node *parser_primary(Parser *parser) {
    if (current()->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        Node *node = parser_logical(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return node;
    }
    if (current()->type == TOKEN_NULL) {
        Node *node = node_new_value(current(), value_new(VALUE_NULL));
        parser_eat(parser, TOKEN_NULL);
        return node;
    }
    if (current()->type == TOKEN_TRUE) {
        Node *node = node_new_value(current(), value_new_boolean(true));
        parser_eat(parser, TOKEN_TRUE);
        return node;
    }
    if (current()->type == TOKEN_FALSE) {
        Node *node = node_new_value(current(), value_new_boolean(false));
        parser_eat(parser, TOKEN_FALSE);
        return node;
    }
    if (current()->type == TOKEN_INT) {
        Node *node = node_new_value(current(), value_new_int(current()->integer));
        parser_eat(parser, TOKEN_INT);
        return node;
    }
    if (current()->type == TOKEN_FLOAT) {
        Node *node = node_new_value(current(), value_new_float(current()->floating));
        parser_eat(parser, TOKEN_FLOAT);
        return node;
    }
    if (current()->type == TOKEN_STRING) {
        Node *node = node_new_value(current(), value_new_string(strdup(current()->string)));
        parser_eat(parser, TOKEN_STRING);
        return node;
    }
    if (current()->type == TOKEN_LBRACKET) {
        Node *node = node_new_multiple(NODE_ARRAY, current());
        parser_eat(parser, TOKEN_LBRACKET);
        while (current()->type != TOKEN_RBRACKET) {
            list_add(node->nodes, parser_assign(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RBRACKET);
        return node;
    }
    if (current()->type == TOKEN_LCURLY) {
        Node *node = node_new_multiple(NODE_OBJECT, current());
        node->keys = list_new(node->nodes->capacity);
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            list_add(node->keys, strdup(current()->string));
            parser_eat(parser, TOKEN_KEYWORD);
            parser_eat(parser, TOKEN_ASSIGN);
            list_add(node->nodes, parser_logical(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RCURLY);
        return node;
    }
    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        List *arguments = list_new(4);
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(arguments, parser_argument(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);

        ValueType returnType = VALUE_ANY;
        if (current()->type == TOKEN_COLON) {
            parser_eat(parser, TOKEN_COLON);
            if (token_type_is_type(current()->type)) {
                returnType = token_type_to_value_type(current()->type);
                parser_eat(parser, current()->type);
            } else {
                error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
            }
        }

        if (current()->type == TOKEN_FAT_ARROW) {
            Token *token = current();
            parser_eat(parser, TOKEN_FAT_ARROW);
            Node *returnNode = node_new_unary(NODE_RETURN, token, parser_logical(parser));
            return node_new_value(functionToken, value_new_function(arguments, returnType, returnNode));
        }
        return node_new_value(functionToken, value_new_function(arguments, returnType, parser_block(parser)));
    }

    return parser_identifier(parser);
}

Node *parser_identifier(Parser *parser) {
    if (current()->type == TOKEN_KEYWORD) {
        Node *node = node_new_string(NODE_VARIABLE, current(), strdup(current()->string));
        parser_eat(parser, TOKEN_KEYWORD);
        return parser_identifier_suffix(parser, node);
    }

    error(parser->text, current()->line, current()->position, "Unexpected token: '%s'", token_type_to_string(current()->type));
    return NULL;
}

Node *parser_identifier_suffix(Parser *parser, Node *node) {
    while (current()->type == TOKEN_LBRACKET || current()->type == TOKEN_POINT) {
        Token *token = current();
        if (current()->type == TOKEN_LBRACKET) {
            parser_eat(parser, TOKEN_LBRACKET);
            Node *indexOrKey = parser_assign(parser);
            parser_eat(parser, TOKEN_RBRACKET);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
        if (current()->type == TOKEN_POINT) {
            parser_eat(parser, TOKEN_POINT);
            Node *indexOrKey = node_new_value(current(), value_new_string(strdup(current()->string)));
            parser_eat(parser, TOKEN_KEYWORD);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
    }
    if (current()->type == TOKEN_LPAREN) {
        Node *callNode = node_new_multiple(NODE_CALL, current());
        callNode->function = node;
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(callNode->nodes, parser_assign(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);
        return callNode;
    }
    return node;
}

Argument *parser_argument(Parser *parser) {
    char *name = strdup(current()->string);
    parser_eat(parser, TOKEN_KEYWORD);
    ValueType type = VALUE_ANY;
    if (current()->type == TOKEN_COLON) {
        parser_eat(parser, TOKEN_COLON);
        if (!token_type_is_type(current()->type)) {
            error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
        }
        type = token_type_to_value_type(current()->type);
        parser_eat(parser, current()->type);
    }
    Node *defaultNode = NULL;
    if (current()->type == TOKEN_ASSIGN) {
        parser_eat(parser, TOKEN_ASSIGN);
        defaultNode = parser_logical(parser);
    }
    return argument_new(name, type, defaultNode);
}

// Standard library header
Value *env_type(List *values);
Value *env_print(List *values);
Value *env_println(List *values);
Value *env_exit(List *values);
Value *env_array_length(List *values);
Value *env_array_push(List *values);
Value *env_string_length(List *values);

// Stanard library
Value *env_type(List *values) {
    Value *value = list_get(values, 0);
    return value_new_string(strdup(value_type_to_string(value->type)));
}

Value *env_print(List *values) {
    for (size_t i = 0; i < values->size; i++) {
        printf("%s", value_to_string(list_get(values, i)));
        if (i != values->size - 1) printf(" ");
    }
    return value_new_null();
}

Value *env_println(List *values) {
    Value *value = env_print(values);
    printf("\n");
    return value;
}

Value *env_exit(List *values) {
    Value *exitCode = list_get(values, 0);
    if (exitCode->type == VALUE_INT) {
        exit(exitCode->integer);
    }
    return value_new_null();
}

Value *env_array_length(List *values) {
    Value *arrayValue = list_get(values, 0);
    return value_new_int(arrayValue->array->size);
}

Value *env_array_push(List *values) {
    Value *arrayValue = list_get(values, 0);
    for (size_t i = 1; i < values->size; i++) {
        list_add(arrayValue->array, list_get(values, i));
    }
    return value_new_int(arrayValue->array->size);
}

Value *env_string_length(List *values) {
    Value *stringValue = list_get(values, 0);
    return value_new_int(strlen(stringValue->string));
}

// Interpreter header
typedef struct Variable {
    bool mutable;
    ValueType type;
    Value *value;
} Variable;

Variable *variable_new(bool mutable, ValueType type, Value *value) {
    Variable *variable = malloc(sizeof(Variable));
    variable->mutable = mutable;
    variable->type = type;
    variable->value = value;
    return variable;
}

void variable_free(Variable *variable) {
    value_free(variable->value);
    free(variable);
}

typedef struct Interpreter {
    char *text;
    Map *env;
} Interpreter;

typedef struct FunctionScope {
    Value *returnValue;
} FunctionScope;

typedef struct LoopScope {
    bool inLoop;
    bool isContinuing;
    bool isBreaking;
} LoopScope;

typedef struct BlockScope {
    Map *env;
} BlockScope;

typedef struct Scope {
    FunctionScope *function;
    LoopScope *loop;
    BlockScope *block;
} Scope;

void interpreter(char *text, Node *node);

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node);

// Interpreter
void interpreter(char *text, Node *node) {
    // Init standard library
    Map *env = map_new(16);

    List *type_args = list_new(4);
    list_add(type_args, argument_new("value", VALUE_ANY, NULL));
    map_set(env, "type", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(type_args, VALUE_ANY, env_type)));

    map_set(env, "print", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(list_new(4), VALUE_NULL, env_print)));
    map_set(env, "println", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(list_new(4), VALUE_NULL, env_println)));

    List *exit_args = list_new(4);
    list_add(exit_args, argument_new("exitCode", VALUE_INT, node_new_value(NULL, value_new_int(0))));
    map_set(env, "exit", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(exit_args, VALUE_NULL, env_exit)));

    List *array_length_args = list_new(4);
    list_add(array_length_args, argument_new("array", VALUE_ARRAY, NULL));
    map_set(env, "array_length", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(array_length_args, VALUE_INT, env_array_length)));

    List *array_push_args = list_new(4);
    list_add(array_push_args, argument_new("array", VALUE_ARRAY, NULL));
    map_set(env, "array_push", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(array_push_args, VALUE_INT, env_array_push)));

    List *string_length_args = list_new(4);
    list_add(string_length_args, argument_new("string", VALUE_STRING, NULL));
    map_set(env, "string_length", variable_new(false, VALUE_NATIVE_FUNCTION, value_new_native_function(string_length_args, VALUE_INT, env_string_length)));

    // Start running code!
    Interpreter interpreter;
    interpreter.text = text;
    interpreter.env = env;

    Scope scope = {.function = &(FunctionScope){.returnValue = NULL},
                   .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                   .block = &(BlockScope){.env = map_new_child(8, env)}};
    interpreter_node(&interpreter, &scope, node);
    if (scope.function->returnValue != NULL && scope.function->returnValue->type == VALUE_INT) {
        exit(scope.function->returnValue->integer);
    }
}

#define interpreter_statement(interpreter, scope, node)      \
    interpreter_node(interpreter, scope, node);              \
    if ((scope)->function->returnValue != NULL) return NULL; \
    if ((scope)->loop->inLoop) {                             \
        if ((scope)->loop->isContinuing) return NULL;        \
        if ((scope)->loop->isBreaking) return NULL;          \
    }

#define interpreter_statement_in_loop(interpreter, scope, node) \
    interpreter_node(interpreter, scope, node);                 \
    if ((scope)->function->returnValue != NULL) return NULL;

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node) {
    if (node->type == NODE_PROGRAM) {
        for (size_t i = 0; i < node->nodes->size; i++) {
            Node *child = list_get(node->nodes, i);
            interpreter_statement(interpreter, scope, child);
        }
        return NULL;
    }
    if (node->type == NODE_NODES) {
        for (size_t i = 0; i < node->nodes->size; i++) {
            Node *child = list_get(node->nodes, i);
            interpreter_statement(interpreter, scope, child);
        }
        return NULL;
    }
    if (node->type == NODE_BLOCK) {
        Scope newScope = {.function = scope->function, .loop = scope->loop, .block = &(BlockScope){.env = map_new_child(8, scope->block->env)}};
        for (size_t i = 0; i < node->nodes->size; i++) {
            Node *child = list_get(node->nodes, i);
            interpreter_statement(interpreter, &newScope, child);
        }
        return NULL;
    }
    if (node->type == NODE_IF) {
        Value *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOLEAN) {
            error(interpreter->text, node->token->line, node->token->position, "Type error");
        }

        if (condition->boolean) {
            interpreter_statement(interpreter, scope, node->thenBlock);
        } else if (node->elseBlock != NULL) {
            interpreter_statement(interpreter, scope, node->elseBlock);
        }
        return NULL;
    }
    if (node->type == NODE_WHILE) {
        Scope newScope = {.function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOLEAN) {
                error(interpreter->text, node->token->line, node->token->position, "Type error");
            }
            if (!condition->boolean) {
                break;
            }
            interpreter_statement_in_loop(interpreter, &newScope, node->thenBlock);
            if (newScope.loop->isContinuing) {
                newScope.loop->isContinuing = false;
            }
            if (newScope.loop->isBreaking) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_DOWHILE) {
        Scope newScope = {.function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            interpreter_statement_in_loop(interpreter, &newScope, node->thenBlock);
            if (newScope.loop->isContinuing) {
                newScope.loop->isContinuing = false;
            }
            if (newScope.loop->isBreaking) {
                break;
            }
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOLEAN) {
                error(interpreter->text, node->token->line, node->token->position, "Type error");
            }
            if (!condition->boolean) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_FOR) {
        Scope newScope = {.function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOLEAN) {
                error(interpreter->text, node->token->line, node->token->position, "Type error");
            }
            if (!condition->boolean) {
                break;
            }
            interpreter_statement_in_loop(interpreter, &newScope, node->thenBlock);
            if (newScope.loop->isContinuing) {
                newScope.loop->isContinuing = false;
            }
            if (newScope.loop->isBreaking) {
                break;
            }
            interpreter_node(interpreter, &newScope, node->incrementBlock);
        }
        return NULL;
    }
    if (node->type == NODE_FORIN) {
        Value *iterator = interpreter_node(interpreter, scope, node->iterator);
        if (iterator->type != VALUE_STRING && iterator->type != VALUE_ARRAY && iterator->type != VALUE_OBJECT) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a string, array or object it is: %s",
                  value_type_to_string(iterator->type));
        }

        Scope newScope = {.function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        size_t size;
        if (iterator->type == VALUE_STRING) size = strlen(iterator->string);
        if (iterator->type == VALUE_ARRAY) size = iterator->array->size;
        if (iterator->type == VALUE_OBJECT) size = iterator->object->size;
        for (size_t i = 0; i < size; i++) {
            Scope newIteratorScope = {.function = newScope.function, .loop = newScope.loop, .block = &(BlockScope){.env = map_new_child(8, newScope.block->env)}};
            Value *iteratorValue;
            if (iterator->type == VALUE_STRING) {
                char character[] = {iterator->string[i], '\0'};
                iteratorValue = value_new_string(strdup(character));
            }
            if (iterator->type == VALUE_ARRAY) {
                iteratorValue = list_get(iterator->array, i);
            }
            if (iterator->type == VALUE_OBJECT) {
                iteratorValue = value_new_string(strdup(iterator->object->keys[i]));
            }
            map_set(newIteratorScope.block->env, node->variable->lhs->string, variable_new(node->variable->type == NODE_LET_ASSIGN, node->variable->declarationType, iteratorValue));
            interpreter_statement_in_loop(interpreter, &newIteratorScope, node->thenBlock);
            if (newScope.loop->isContinuing) {
                newScope.loop->isContinuing = false;
            }
            if (newScope.loop->isBreaking) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_CONTINUE) {
        if (!scope->loop->inLoop) {
            error(interpreter->text, node->token->line, node->token->position, "Not in a loop");
        }
        scope->loop->isContinuing = true;
        return NULL;
    }
    if (node->type == NODE_BREAK) {
        if (!scope->loop->inLoop) {
            error(interpreter->text, node->token->line, node->token->position, "Not in a loop");
        }
        scope->loop->isBreaking = true;
        return NULL;
    }
    if (node->type == NODE_RETURN) {
        scope->function->returnValue = interpreter_node(interpreter, scope, node->unary);
        return NULL;
    }

    if (node->type == NODE_VALUE) {
        return node->value;
    }
    if (node->type == NODE_ARRAY) {
        Value *arrayValue = value_new_array(list_new(align(node->nodes->size, 8)));
        for (size_t i = 0; i < node->nodes->size; i++) {
            list_add(arrayValue->array, interpreter_node(interpreter, scope, list_get(node->nodes, i)));
        }
        return arrayValue;
    }
    if (node->type == NODE_OBJECT) {
        Value *objectValue = value_new_object(map_new(align(node->nodes->size, 8)));
        for (size_t i = 0; i < node->nodes->size; i++) {
            map_set(objectValue->object, list_get(node->keys, i), interpreter_node(interpreter, scope, list_get(node->nodes, i)));
        }
        return objectValue;
    }
    if (node->type == NODE_CALL) {
        Value *functionValue = interpreter_node(interpreter, scope, node->function);
        if (functionValue->type != VALUE_FUNCTION && functionValue->type != VALUE_NATIVE_FUNCTION) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a function");
        }

        List *values = list_new(align(node->nodes->size, 8));
        for (size_t i = 0; i < MAX(functionValue->arguments->size, node->nodes->size); i++) {
            Argument *argument = list_get(functionValue->arguments, i);
            if (list_get(node->nodes, i) == NULL && argument != NULL) {
                if (argument->defaultNode != NULL) {
                    Value *defaultValue = interpreter_node(interpreter, scope, argument->defaultNode);
                    if (argument->type != VALUE_ANY && defaultValue->type != argument->type) {
                        error(interpreter->text, argument->defaultNode->token->line, argument->defaultNode->token->position,
                              "Unexpected function default argument type: '%s' needed '%s'", value_type_to_string(defaultValue->type),
                              value_type_to_string(argument->type));
                    }
                    list_add(values, defaultValue);
                    continue;
                }

                error(interpreter->text, node->token->line, node->token->position, "Not all function arguments are given");
            }

            Value *nodeValue = interpreter_node(interpreter, scope, list_get(node->nodes, i));
            if (argument != NULL && argument->type != VALUE_ANY && nodeValue->type != argument->type) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function argument type: '%s' needed '%s'",
                      value_type_to_string(nodeValue->type), value_type_to_string(argument->type));
            }
            list_add(values, nodeValue);
        }

        if (functionValue->type == VALUE_FUNCTION) {
            Scope newScope = {.function = &(FunctionScope){.returnValue = NULL},
                              .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                              .block = &(BlockScope){.env = map_new_child(8, scope->block->env)}};
            for (size_t i = 0; i < functionValue->arguments->size; i++) {
                Argument *argument = list_get(functionValue->arguments, i);
                map_set_without_parent(newScope.block->env, argument->name, variable_new(true, argument->type, list_get(values, i)));
            }
            map_set_without_parent(newScope.block->env, "arguments", variable_new(false, VALUE_ARRAY, value_new_array(values)));
            interpreter_node(interpreter, &newScope, functionValue->functionNode);

            if (functionValue->returnType != VALUE_ANY && newScope.function->returnValue->type != functionValue->returnType) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function return type: '%s' needed '%s'",
                      value_type_to_string(newScope.function->returnValue->type), value_type_to_string(functionValue->returnType));
            }
            return newScope.function->returnValue != NULL ? newScope.function->returnValue : value_new_null();
        }
        if (functionValue->type == VALUE_NATIVE_FUNCTION) {
            Value *returnValue = functionValue->nativeFunc(values);
            if (functionValue->returnType != VALUE_ANY && returnValue->type != functionValue->returnType) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function return type: '%s' needed '%s'",
                      value_type_to_string(returnValue->type), value_type_to_string(functionValue->returnType));
            }
            return returnValue;
        }
    }

    if (node->type == NODE_VARIABLE) {
        Variable *variable = map_get(scope->block->env, node->string);
        if (variable != NULL) {
            return variable->value;
        }
        error(interpreter->text, node->token->line, node->token->position, "Can't find variable: '%s'", node->string);
    }
    if (node->type == NODE_GET) {
        Value *containerValue = interpreter_node(interpreter, scope, node->lhs);
        if (containerValue->type != VALUE_STRING && containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a string, array or object it is: %s",
                  value_type_to_string(containerValue->type));
        }

        Value *indexOrKey = interpreter_node(interpreter, scope, node->rhs);
        if (containerValue->type == VALUE_STRING) {
            if (indexOrKey->type != VALUE_INT) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "String index is not an int");
            }
            if (indexOrKey->integer >= 0 && indexOrKey->integer <= (int64_t)strlen(containerValue->string)) {
                char character[] = {containerValue->string[indexOrKey->integer], '\0'};
                return value_new_string(strdup(character));
            }
            return value_new_null();
        }
        if (containerValue->type == VALUE_ARRAY) {
            if (indexOrKey->type != VALUE_INT) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Array index is not an int");
            }
            Value *value = list_get(containerValue->array, indexOrKey->integer);
            return value != NULL ? value : value_new_null();
        }
        if (containerValue->type == VALUE_OBJECT) {
            if (indexOrKey->type != VALUE_STRING) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Object key is not a string");
            }
            Value *value = map_get(containerValue->object, indexOrKey->string);
            if (value != NULL) return value;
            error(interpreter->text, node->token->line, node->token->position, "Can't find key in object");
        }
    }

    if (node->type >= NODE_NEG && node->type <= NODE_CAST) {
        Value *unary = interpreter_node(interpreter, scope, node->unary);
        if (node->type == NODE_NEG) {
            if (unary->type == VALUE_INT) {
                return value_new_int(-unary->integer);
            }
            if (unary->type == VALUE_FLOAT) {
                return value_new_float(-unary->floating);
            }
        }
        if (node->type == NODE_NOT) {
            if (unary->type == VALUE_INT) {
                return value_new_int(~unary->integer);
            }
        }
        if (node->type == NODE_LOGICAL_NOT) {
            if (unary->type == VALUE_BOOLEAN) {
                return value_new_boolean(!unary->boolean);
            }
        }
        if (node->type == NODE_CAST) {
            if (node->castType == VALUE_BOOLEAN) {
                if (unary->type == VALUE_NULL) return value_new_boolean(false);
                if (unary->type == VALUE_BOOLEAN) return value_new_boolean(unary->boolean);
                if (unary->type == VALUE_INT) return value_new_boolean(unary->integer != 0);
                if (unary->type == VALUE_FLOAT) return value_new_boolean(unary->floating != 0.0);
                if (unary->type == VALUE_STRING) return value_new_boolean(!(!strcmp(unary->string, "") || !strcmp(unary->string, "0")));
            }

            if (node->castType == VALUE_INT) {
                if (unary->type == VALUE_NULL) return value_new_int(0);
                if (unary->type == VALUE_BOOLEAN) return value_new_int(unary->boolean ? 1 : 0);
                if (unary->type == VALUE_INT) return value_new_int(unary->integer);
                if (unary->type == VALUE_FLOAT) return value_new_int(unary->floating);
                if (unary->type == VALUE_STRING) return value_new_int(string_to_int(unary->string));
            }

            if (node->castType == VALUE_FLOAT) {
                if (unary->type == VALUE_NULL) return value_new_float(0.0);
                if (unary->type == VALUE_BOOLEAN) return value_new_float(unary->boolean ? 1.0 : 0.0);
                if (unary->type == VALUE_INT) return value_new_float(unary->integer);
                if (unary->type == VALUE_FLOAT) return value_new_float(unary->floating);
                if (unary->type == VALUE_STRING) return value_new_float(string_to_float(unary->string));
            }

            if (node->castType == VALUE_STRING) {
                return value_new_string(value_to_string(unary));
            }
        }

        error(interpreter->text, node->token->line, node->token->position, "Type error");
    }

    if (node->type == NODE_CONST_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get_without_parent(scope->block->env, node->lhs->string) == NULL) {
            map_set_without_parent(scope->block->env, node->lhs->string, variable_new(false, node->declarationType, rhs));
            return rhs;
        }
        error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't redeclare const variable: '%s'", node->lhs->string);
    }
    if (node->type == NODE_LET_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get_without_parent(scope->block->env, node->lhs->string) == NULL) {
            map_set_without_parent(scope->block->env, node->lhs->string, variable_new(true, node->declarationType, rhs));
            return rhs;
        }
        error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't redeclare variable: '%s'", node->lhs->string);
    }
    if (node->type == NODE_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (node->lhs->type == NODE_GET) {
            Value *containerValue = interpreter_node(interpreter, scope, node->lhs->lhs);
            if (containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT) {
                error(interpreter->text, node->token->line, node->token->position, "Variable is not an array or object it is: %s",
                      value_type_to_string(containerValue->type));
            }

            Value *indexOrKey = interpreter_node(interpreter, scope, node->lhs->rhs);
            if (containerValue->type == VALUE_ARRAY) {
                if (indexOrKey->type != VALUE_INT) {
                    error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Array index is not an int");
                }
                list_set(containerValue->array, indexOrKey->integer, rhs);
            }
            if (containerValue->type == VALUE_OBJECT) {
                if (indexOrKey->type != VALUE_STRING) {
                    error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Object key is not a string");
                }
                map_set(containerValue->object, indexOrKey->string, rhs);
            }
        } else {
            Variable *variable = map_get(scope->block->env, node->lhs->string);
            if (variable == NULL) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Variable: '%s' is not declared", node->lhs->string);
            }
            if (!variable->mutable) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't mutate const variable: '%s'", node->lhs->string);
            }
            if (variable->type != VALUE_ANY && variable->type != rhs->type) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Unexpected variable type: '%s' needed '%s'",
                      value_type_to_string(rhs->type), value_type_to_string(variable->type));
            }
            variable->value = rhs;
        }
        return rhs;
    }

    if (node->type >= NODE_ADD && node->type <= NODE_LOGICAL_OR) {
        Value *lhs = interpreter_node(interpreter, scope, node->lhs);
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);

        if (node->type == NODE_ADD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(lhs->integer + rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->floating + rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(lhs->floating + rhs->integer);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->integer + rhs->floating);

            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                char *string = malloc(strlen(lhs->string) + strlen(rhs->string) + 1);
                stpcpy(string, lhs->string);
                strcat(string, rhs->string);
                return value_new_string(string);
            }
        }

        if (node->type == NODE_SUB) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(lhs->integer - rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->floating - rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->integer - rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(lhs->floating - rhs->integer);
        }
        if (node->type == NODE_MUL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(lhs->integer * rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->floating * rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->integer * rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(lhs->floating * rhs->integer);
        }
        if (node->type == NODE_EXP) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(pow(lhs->integer, rhs->integer));
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(pow(lhs->floating, rhs->floating));
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(pow(lhs->integer, rhs->floating));
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(pow(lhs->floating, rhs->integer));
        }
        if (node->type == NODE_DIV) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(lhs->integer / rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->floating / rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(lhs->integer / rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(lhs->floating / rhs->integer);
        }
        if (node->type == NODE_MOD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_int(lhs->integer % rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_float(fmod(lhs->floating, rhs->floating));
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_float(fmod(lhs->integer, rhs->floating));
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_float(fmod(lhs->floating, rhs->integer));
        }

        if (node->type == NODE_AND) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                return value_new_int(lhs->integer & rhs->integer);
            }
        }
        if (node->type == NODE_XOR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                return value_new_int(lhs->integer ^ rhs->integer);
            }
        }
        if (node->type == NODE_OR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                return value_new_int(lhs->integer | rhs->integer);
            }
        }
        if (node->type == NODE_SHL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                return value_new_int(lhs->integer << rhs->integer);
            }
        }
        if (node->type == NODE_SHR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                return value_new_int(lhs->integer >> rhs->integer);
            }
        }

        if (node->type == NODE_EQ) {
            if (lhs->type == VALUE_NULL) return value_new_boolean(rhs->type == VALUE_NULL);
            if (rhs->type == VALUE_NULL) return value_new_boolean(lhs->type == VALUE_NULL);
            if (lhs->type == VALUE_BOOLEAN && rhs->type == VALUE_BOOLEAN) return value_new_boolean(lhs->boolean == rhs->boolean);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer == rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating == rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer == rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating == rhs->integer);
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) return value_new_boolean(!strcmp(lhs->string, rhs->string));
        }
        if (node->type == NODE_NEQ) {
            if (lhs->type == VALUE_NULL) return value_new_boolean(rhs->type != VALUE_NULL);
            if (rhs->type == VALUE_NULL) return value_new_boolean(lhs->type != VALUE_NULL);
            if (lhs->type == VALUE_BOOLEAN && rhs->type == VALUE_BOOLEAN) return value_new_boolean(lhs->boolean != rhs->boolean);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer != rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating != rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer != rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating != rhs->integer);
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) return value_new_boolean(strcmp(lhs->string, rhs->string));
        }

        if (node->type == NODE_LT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer < rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating < rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer < rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating < rhs->integer);
        }
        if (node->type == NODE_LTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer <= rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating <= rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer <= rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating <= rhs->integer);
        }
        if (node->type == NODE_GT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer > rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating > rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer > rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating > rhs->integer);
        }
        if (node->type == NODE_GTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) return value_new_boolean(lhs->integer >= rhs->integer);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->floating >= rhs->floating);
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) return value_new_boolean(lhs->integer >= rhs->floating);
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) return value_new_boolean(lhs->floating >= rhs->integer);
        }

        if (node->type == NODE_LOGICAL_AND) {
            if (lhs->type == VALUE_BOOLEAN && rhs->type == VALUE_BOOLEAN) {
                return value_new_boolean(lhs->boolean && rhs->boolean);
            }
        }
        if (node->type == NODE_LOGICAL_OR) {
            if (lhs->type == VALUE_BOOLEAN && rhs->type == VALUE_BOOLEAN) {
                return value_new_boolean(lhs->boolean || rhs->boolean);
            }
        }

        error(interpreter->text, node->token->line, node->token->position, "Type error");
    }

    fprintf(stderr, "Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

// Main
int main(int argc, char **argv) {
    if (argc == 1) {
        printf("New Bastiaan Language Interpreter v0.1\n");
        return EXIT_SUCCESS;
    }

    char *text = file_read(argv[1]);
    List *tokens = lexer(text);
    // printf("Tokens:\n");
    // for (size_t i = 0; i < tokens->size; i++) {
    //     Token *token = list_get(tokens, i);
    //     printf("%s ", token_type_to_string(token->type));
    // }
    // printf("\n");

    Node *node = parser(text, tokens);
    interpreter(text, node);

    node_free(node);
    list_free(tokens, (ListFreeFunc *)token_free);
    free(text);
}
