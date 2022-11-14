// New Bastiaan Language AST Interpreter
// Made by Bastiaan van der Plaat
#ifndef NBL_HEADER
#define NBL_HEADER

#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Custom windows headers because TokenType name conflict :(
#ifdef _WIN32

typedef struct _SYSTEMTIME {
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;
} SYSTEMTIME;

typedef struct _FILETIME {
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
} FILETIME;

typedef union _LARGE_INTEGER {
    struct {
        uint32_t LowPart;
        uint32_t HighPart;
    };
    uint64_t QuadPart;
} LARGE_INTEGER;

extern void GetLocalTime(SYSTEMTIME *lpSystemTime);
extern bool SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, FILETIME *lpFileTime);

#else
#include <sys/time.h>
#endif

// Polyfills header
#ifndef M_E
#define M_E 2.718281828459045
#endif
#ifndef M_LN2
#define M_LN2 0.6931471805599453
#endif
#ifndef M_LN10
#define M_LN10 2.302585092994046
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.4342944819032518
#endif
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865476
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730951
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char *strdup(const char *str);

char *strndup(const char *str, size_t size);

// Utils header
extern int64_t random_seed;

double random_random(void);

int64_t time_ms(void);

char *file_read(char *path);

typedef struct Token Token;  // Forward define

void print_error(Token *token, char *fmt, ...);

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

void list_set(List *list, size_t index, void *item);

void list_add(List *list, void *item);

char *list_to_string(List *list);

typedef void ListFreeFunc(void *item);

void list_free(List *list, ListFreeFunc *freeFunc);

// Map header
typedef struct Map {
    int32_t refs;
    char **keys;
    void **values;
    size_t capacity;
    size_t size;
} Map;

#define map_foreach(map, key, value, block)              \
    for (size_t index = 0; index < map->size; index++) { \
        key = map->keys[index];                          \
        value = map->values[index];                      \
        block                                            \
    }

Map *map_new(void);

Map *map_new_with_capacity(size_t capacity);

Map *map_ref(Map *map);

void *map_get(Map *map, char *key);

void map_set(Map *map, char *key, void *item);

typedef void MapFreeFunc(void *item);

void map_free(Map *map, MapFreeFunc *freeFunc);

// Lexer header
typedef struct Source {
    int32_t refs;
    char *path;
    char *basename;
    char *dirname;
    char *text;
} Source;

Source *source_new(char *path, char *text);

Source *source_ref(Source *source);

void source_free(Source *source);

typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURLY,
    TOKEN_RCURLY,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_QUESTION,
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
    TOKEN_ASSIGN_ADD,
    TOKEN_INC,
    TOKEN_SUB,
    TOKEN_ASSIGN_SUB,
    TOKEN_DEC,
    TOKEN_MUL,
    TOKEN_ASSIGN_MUL,
    TOKEN_EXP,
    TOKEN_ASSIGN_EXP,
    TOKEN_DIV,
    TOKEN_ASSIGN_DIV,
    TOKEN_MOD,
    TOKEN_ASSIGN_MOD,
    TOKEN_AND,
    TOKEN_ASSIGN_AND,
    TOKEN_XOR,
    TOKEN_ASSIGN_XOR,
    TOKEN_OR,
    TOKEN_ASSIGN_OR,
    TOKEN_NOT,
    TOKEN_SHL,
    TOKEN_ASSIGN_SHL,
    TOKEN_SHR,
    TOKEN_ASSIGN_SHR,
    TOKEN_INSTANCEOF,
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
    TOKEN_TYPE_BOOL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_ARRAY,
    TOKEN_TYPE_OBJECT,
    TOKEN_CLASS,
    TOKEN_EXTENDS,
    TOKEN_ABSTRACT,
    TOKEN_TYPE_INSTANCE,
    TOKEN_TYPE_FUNCTION,
    TOKEN_FUNCTION,

    TOKEN_CONST,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_LOOP,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_CONTINUE,
    TOKEN_BREAK,
    TOKEN_RETURN,
    TOKEN_THROW,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_FINALLY,
    TOKEN_INCLUDE
} TokenType;

struct Token {
    int32_t refs;
    TokenType type;
    Source *source;
    int32_t line;
    int32_t column;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
};

Token *token_new(TokenType type, Source *source, int32_t line, int32_t column);

Token *token_new_int(TokenType type, Source *source, int32_t line, int32_t column, int64_t integer);

Token *token_new_float(Source *source, int32_t line, int32_t column, double floating);

Token *token_new_string(TokenType type, Source *source, int32_t line, int32_t column, char *string);

Token *token_ref(Token *token);

bool token_type_is_type(TokenType type);

char *token_type_to_string(TokenType type);

void token_free(Token *token);

int64_t string_to_int(char *string);

double string_to_float(char *string);

typedef struct Keyword {
    char *keyword;
    TokenType type;
} Keyword;

List *lexer(char *path, char *text);

// Value
typedef struct Node Node;                              // Forward define
typedef struct InterpreterContext InterpreterContext;  // Forward define

typedef enum ValueType {
    VALUE_ANY,
    VALUE_NULL,
    VALUE_BOOL,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_ARRAY,
    VALUE_OBJECT,
    VALUE_CLASS,
    VALUE_INSTANCE,
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
    int32_t refs;
    ValueType type;
    union {
        bool boolean;
        int64_t integer;
        double floating;
        char *string;
        List *array;
        struct {
            Map *object;
            union {
                Value *parentClass;
                Value *instanceClass;
            };
            bool abstract;
        };
        struct {
            List *arguments;
            ValueType returnType;
            union {
                Node *functionNode;
                Value *(*nativeFunc)(InterpreterContext *context, Value *this, List *values);
            };
        };
    };
};

Value *value_new(ValueType type);

Value *value_new_null(void);

Value *value_new_bool(bool boolean);

Value *value_new_int(int64_t integer);

Value *value_new_float(double integer);

Value *value_new_string(char *string);

Value *value_new_string_format(char *format, ...);

Value *value_new_array(List *array);

Value *value_new_object(Map *object);

Value *value_new_class(Map *object, Value *parentClass, bool abstract);

Value *value_new_instance(Map *object, Value *instanceClass);

Value *value_new_function(List *args, ValueType returnType, Node *node);

Value *value_new_native_function(List *args, ValueType returnType, Value *(*nativeFunc)(InterpreterContext *context, Value *this, List *values));

char *value_type_to_string(ValueType type);

ValueType token_type_to_value_type(TokenType type);

char *value_to_string(Value *value);

Value *value_class_get(Value *instance, char *key);

bool value_class_instanceof(Value *instance, Value *class);

Value *value_ref(Value *value);

Value *value_retrieve(Value *value);

void value_clear(Value *value);

void value_free(Value *value);

// Parser
typedef enum NodeType {
    NODE_PROGRAM,
    NODE_NODES,
    NODE_BLOCK,
    NODE_IF,
    NODE_TRY,
    NODE_TENARY,
    NODE_LOOP,
    NODE_WHILE,
    NODE_DOWHILE,
    NODE_FOR,
    NODE_FORIN,
    NODE_CONTINUE,
    NODE_BREAK,
    NODE_RETURN,
    NODE_THROW,
    NODE_INCLUDE,

    NODE_VALUE,
    NODE_ARRAY,
    NODE_OBJECT,
    NODE_CLASS,
    NODE_CALL,

    NODE_NEG,
    NODE_INC_PRE,
    NODE_DEC_PRE,
    NODE_INC_POST,
    NODE_DEC_POST,
    NODE_NOT,
    NODE_LOGICAL_NOT,
    NODE_CAST,

    NODE_VARIABLE,
    NODE_CONST_ASSIGN,
    NODE_LET_ASSIGN,
    NODE_ASSIGN,
    NODE_GET,
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
    NODE_INSTANCEOF,
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
    int32_t refs;
    NodeType type;
    Token *token;
    union {
        Value *value;
        char *string;
        List *array;
        struct {
            Map *object;
            Node *parentClass;
            bool abstract;
        };
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
                Node *catchVariable;
            };
            union {
                Node *thenBlock;
                Node *tryBlock;
            };
            union {
                Node *elseBlock;
                Node *incrementBlock;
                Node *forinVariable;
                Node *catchBlock;
            };
            Node *finallyBlock;
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

Node *node_ref(Node *node);

void node_free(Node *node);

typedef struct Parser {
    List *tokens;
    int32_t position;
} Parser;

Node *parser(List *tokens, bool included);

void parser_eat(Parser *parser, TokenType type);

ValueType parser_eat_type(Parser *parser);

Node *parser_program(Parser *parser, bool included);
Node *parser_block(Parser *parser);
Node *parser_statement(Parser *parser);
Node *parser_declarations(Parser *parser);
Node *parser_assigns(Parser *parser);
Node *parser_assign(Parser *parser);
Node *parser_tenary(Parser *parser);
Node *parser_logical(Parser *parser);
Node *parser_equality(Parser *parser);
Node *parser_relational(Parser *parser);
Node *parser_instanceof(Parser *parser);
Node *parser_bitwise(Parser *parser);
Node *parser_shift(Parser *parser);
Node *parser_add(Parser *parser);
Node *parser_mul(Parser *parser);
Node *parser_unary(Parser *parser);
Node *parser_primary(Parser *parser);
Node *parser_primary_suffix(Parser *parser, Node *node);
Node *parser_function(Parser *parser, Token *token);
Node *parser_class(Parser *parser, Token *token, bool abstract);
Argument *parser_argument(Parser *parser);

// Standard library
Map *std_env(void);

// Interpreter
typedef struct Variable {
    ValueType type;
    bool mutable;
    Value *value;
} Variable;

Variable *variable_new(ValueType type, bool mutable, Value *value);

void variable_free(Variable *variable);

typedef struct ExceptionScope {
    Value *exceptionValue;
} ExceptionScope;

typedef struct FunctionScope {
    Value *returnValue;
} FunctionScope;

typedef struct LoopScope {
    bool inLoop;
    bool isContinuing;
    bool isBreaking;
} LoopScope;

typedef struct BlockScope BlockScope;

struct BlockScope {
    BlockScope *parentBlock;
    Map *env;
};

typedef struct Scope {
    ExceptionScope *exception;
    FunctionScope *function;
    LoopScope *loop;
    BlockScope *block;
} Scope;

typedef struct Interpreter {
    Map *env;
} Interpreter;

struct InterpreterContext {
    Map *env;
    Scope *scope;
    Node *node;
};

Variable *block_scope_get(BlockScope *block, char *key);

Value *interpreter(Map *env, Node *node);

Value *type_error_exception(ValueType expected, ValueType got);

Value *interpreter_call(InterpreterContext *context, Value *callValue, Value *this, List *arguments);

Value *interpreter_throw(InterpreterContext *context, Value *exception);

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node);

#endif

#if defined(NBL_IMPLEMENTATION) && !defined(NBL_CODE)
#define NBL_CODE

// Polyfills
char *strdup(const char *str) { return strndup(str, strlen(str)); }

char *strndup(const char *str, size_t size) {
    char *copy = malloc(size + 1);
    if (copy == NULL) return NULL;
    memcpy(copy, str, size);
    copy[size] = '\0';
    return copy;
}

// Utils
int64_t random_seed;

double random_random(void) {
    double x = sin(random_seed++ * 10000);
    return x - floor(x);
}

int64_t time_ms(void) {
#ifdef _WIN32
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    FILETIME fileTime;
    SystemTimeToFileTime(&localTime, &fileTime);
    LARGE_INTEGER date, adjust;
    date.HighPart = fileTime.dwHighDateTime;
    date.LowPart = fileTime.dwLowDateTime;
    adjust.QuadPart = 11644473600000 * 10000;
    date.QuadPart -= adjust.QuadPart;
    return date.QuadPart / 10000;
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec;
#endif
}

char *file_read(char *path) {
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

void print_error(Token *token, char *fmt, ...) {
    fprintf(stderr, "%s:%d:%d ERROR: ", token->source->path, token->line, token->column);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    // Seek to the right line in text
    char *c = token->source->text;
    for (int32_t i = 0; i < token->line - 1; i++) {
        while (*c != '\n' && *c != '\r') c++;
        if (*c == '\r') c++;
        c++;
    }
    char *lineStart = c;
    while (*c != '\n' && *c != '\r' && *c != '\0') c++;
    int32_t lineLength = c - lineStart;

    fprintf(stderr, "\n%4d | ", token->line);
    fwrite(lineStart, 1, lineLength, stderr);
    fprintf(stderr, "\n     | ");
    for (int32_t i = 0; i < token->column - 1; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");
}

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

void list_set(List *list, size_t index, void *item) {
    if (index > list->capacity) {
        while (index > list->capacity) list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    if (index > list->size) {
        for (size_t i = list->size; i < index - 1; i++) {
            list->items[i] = NULL;
        }
        list->size = index + 1;
    }
    list->items[index] = item;
}

void list_add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

char *list_to_string(List *list) {
    size_t size = 0;
    for (size_t i = 0; i < list->size; i++) {
        size += strlen(list_get(list, i));
    }
    char *string = malloc(size + 1);
    string[0] = '\0';
    for (size_t i = 0; i < list->size; i++) {
        strcat(string, list_get(list, i));
    }
    return string;
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

// Map
Map *map_new(void) { return map_new_with_capacity(8); }

Map *map_new_with_capacity(size_t capacity) {
    Map *map = malloc(sizeof(Map));
    map->refs = 1;
    map->keys = malloc(sizeof(char *) * capacity);
    map->values = malloc(sizeof(void *) * capacity);
    map->capacity = capacity;
    map->size = 0;
    return map;
}

Map *map_ref(Map *map) {
    map->refs++;
    return map;
}

void *map_get(Map *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->values[i];
        }
    }
    return NULL;
}

void map_set(Map *map, char *key, void *item) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            map->values[i] = item;
            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->keys = realloc(map->keys, sizeof(char *) * map->capacity);
        map->values = realloc(map->values, sizeof(void *) * map->capacity);
    }
    map->keys[map->size] = strdup(key);
    map->values[map->size] = item;
    map->size++;
}

void map_free(Map *map, MapFreeFunc *freeFunc) {
    map->refs--;
    if (map->refs > 0) return;

    for (size_t i = 0; i < map->size; i++) {
        free(map->keys[i]);
        if (freeFunc != NULL) {
            freeFunc(map->values[i]);
        }
    }
    free(map->keys);
    free(map->values);
    free(map);
}

// Lexer
Source *source_new(char *path, char *text) {
    Source *source = malloc(sizeof(Source));
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

Source *source_ref(Source *source) {
    source->refs++;
    return source;
}

void source_free(Source *source) {
    source->refs--;
    if (source->refs > 0) return;

    free(source->path);
    free(source->text);
    free(source->dirname);
    free(source);
}

Token *token_new(TokenType type, Source *source, int32_t line, int32_t column) {
    Token *token = malloc(sizeof(Token));
    token->refs = 1;
    token->source = source_ref(source);
    token->type = type;
    token->line = line;
    token->column = column;
    return token;
}

Token *token_new_int(TokenType type, Source *source, int32_t line, int32_t column, int64_t integer) {
    Token *token = token_new(type, source, line, column);
    token->integer = integer;
    return token;
}

Token *token_new_float(Source *source, int32_t line, int32_t column, double floating) {
    Token *token = token_new(TOKEN_FLOAT, source, line, column);
    token->floating = floating;
    return token;
}

Token *token_new_string(TokenType type, Source *source, int32_t line, int32_t column, char *string) {
    Token *token = token_new(type, source, line, column);
    token->string = string;
    return token;
}

Token *token_ref(Token *token) {
    token->refs++;
    return token;
}

bool token_type_is_type(TokenType type) {
    return type == TOKEN_TYPE_ANY || type == TOKEN_NULL || type == TOKEN_TYPE_BOOL || type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT ||
           type == TOKEN_TYPE_STRING || type == TOKEN_TYPE_ARRAY || type == TOKEN_TYPE_OBJECT || type == TOKEN_TYPE_FUNCTION || type == TOKEN_CLASS ||
           type == TOKEN_TYPE_INSTANCE;
}

char *token_type_to_string(TokenType type) {
    if (type == TOKEN_EOF) return "EOF";
    if (type == TOKEN_UNKNOWN) return "Unknown character";
    if (type == TOKEN_LPAREN) return "(";
    if (type == TOKEN_RPAREN) return ")";
    if (type == TOKEN_LCURLY) return "{";
    if (type == TOKEN_RCURLY) return "}";
    if (type == TOKEN_LBRACKET) return "[";
    if (type == TOKEN_RBRACKET) return "]";
    if (type == TOKEN_QUESTION) return "?";
    if (type == TOKEN_SEMICOLON) return ";";
    if (type == TOKEN_COLON) return ":";
    if (type == TOKEN_COMMA) return ",";
    if (type == TOKEN_POINT) return ".";
    if (type == TOKEN_FAT_ARROW) return "=>";

    if (type == TOKEN_KEYWORD) return "keyword";
    if (type == TOKEN_INT) return "int";
    if (type == TOKEN_FLOAT) return "float";
    if (type == TOKEN_STRING) return "string";

    if (type == TOKEN_ASSIGN) return "=";
    if (type == TOKEN_ADD) return "+";
    if (type == TOKEN_INC) return "++";
    if (type == TOKEN_ASSIGN_ADD) return "+=";
    if (type == TOKEN_SUB) return "-";
    if (type == TOKEN_DEC) return "--";
    if (type == TOKEN_ASSIGN_SUB) return "-=";
    if (type == TOKEN_MUL) return "*";
    if (type == TOKEN_ASSIGN_MUL) return "*=";
    if (type == TOKEN_EXP) return "**";
    if (type == TOKEN_ASSIGN_EXP) return "**=";
    if (type == TOKEN_DIV) return "/";
    if (type == TOKEN_ASSIGN_DIV) return "/=";
    if (type == TOKEN_MOD) return "%";
    if (type == TOKEN_ASSIGN_MOD) return "%=";
    if (type == TOKEN_AND) return "&";
    if (type == TOKEN_ASSIGN_AND) return "&=";
    if (type == TOKEN_XOR) return "^";
    if (type == TOKEN_ASSIGN_XOR) return "^=";
    if (type == TOKEN_OR) return "|";
    if (type == TOKEN_ASSIGN_OR) return "|=";
    if (type == TOKEN_NOT) return "~";
    if (type == TOKEN_SHL) return "<<";
    if (type == TOKEN_ASSIGN_SHL) return "<<=";
    if (type == TOKEN_SHR) return ">>";
    if (type == TOKEN_ASSIGN_SHR) return ">>=";
    if (type == TOKEN_EQ) return "==";
    if (type == TOKEN_NEQ) return "!=";
    if (type == TOKEN_LT) return "<";
    if (type == TOKEN_LTEQ) return "<=";
    if (type == TOKEN_GT) return ">";
    if (type == TOKEN_GTEQ) return ">=";
    if (type == TOKEN_LOGICAL_OR) return "||";
    if (type == TOKEN_LOGICAL_AND) return "&&";
    if (type == TOKEN_LOGICAL_NOT) return "!";
    if (type == TOKEN_INSTANCEOF) return "instanceof";

    if (type == TOKEN_TYPE_ANY) return "any";
    if (type == TOKEN_NULL) return "null";
    if (type == TOKEN_TYPE_BOOL) return "bool";
    if (type == TOKEN_TRUE) return "true";
    if (type == TOKEN_FALSE) return "false";
    if (type == TOKEN_TYPE_INT) return "int";
    if (type == TOKEN_TYPE_FLOAT) return "float";
    if (type == TOKEN_TYPE_STRING) return "string";
    if (type == TOKEN_TYPE_ARRAY) return "array";
    if (type == TOKEN_TYPE_OBJECT) return "object";
    if (type == TOKEN_CLASS) return "class";
    if (type == TOKEN_EXTENDS) return "extends";
    if (type == TOKEN_ABSTRACT) return "abstract";
    if (type == TOKEN_TYPE_INSTANCE) return "instance";
    if (type == TOKEN_TYPE_FUNCTION) return "function";
    if (type == TOKEN_FUNCTION) return "fn";

    if (type == TOKEN_CONST) return "const";
    if (type == TOKEN_LET) return "let";
    if (type == TOKEN_IF) return "if";
    if (type == TOKEN_ELSE) return "else";
    if (type == TOKEN_LOOP) return "loop";
    if (type == TOKEN_WHILE) return "while";
    if (type == TOKEN_DO) return "do";
    if (type == TOKEN_FOR) return "for";
    if (type == TOKEN_IN) return "in";
    if (type == TOKEN_CONTINUE) return "continue";
    if (type == TOKEN_BREAK) return "break";
    if (type == TOKEN_RETURN) return "return";
    if (type == TOKEN_THROW) return "throw";
    if (type == TOKEN_TRY) return "try";
    if (type == TOKEN_CATCH) return "catch";
    if (type == TOKEN_FINALLY) return "finally";
    if (type == TOKEN_INCLUDE) return "include";
    return NULL;
}

void token_free(Token *token) {
    token->refs--;
    if (token->refs > 0) return;

    source_free(token->source);
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

List *lexer(char *path, char *text) {
    Keyword keywords[] = {{"instanceof", TOKEN_INSTANCEOF},
                          {"any", TOKEN_TYPE_ANY},
                          {"null", TOKEN_NULL},
                          {"bool", TOKEN_TYPE_BOOL},
                          {"true", TOKEN_TRUE},
                          {"false", TOKEN_FALSE},
                          {"int", TOKEN_TYPE_INT},
                          {"float", TOKEN_TYPE_FLOAT},
                          {"string", TOKEN_TYPE_STRING},
                          {"array", TOKEN_TYPE_ARRAY},
                          {"object", TOKEN_TYPE_OBJECT},
                          {"class", TOKEN_CLASS},
                          {"extends", TOKEN_EXTENDS},
                          {"abstract", TOKEN_ABSTRACT},
                          {"instance", TOKEN_TYPE_INSTANCE},
                          {"function", TOKEN_TYPE_FUNCTION},
                          {"fn", TOKEN_FUNCTION},

                          {"const", TOKEN_CONST},
                          {"let", TOKEN_LET},
                          {"if", TOKEN_IF},
                          {"else", TOKEN_ELSE},
                          {"loop", TOKEN_LOOP},
                          {"while", TOKEN_WHILE},
                          {"do", TOKEN_DO},
                          {"for", TOKEN_FOR},
                          {"in", TOKEN_IN},
                          {"continue", TOKEN_CONTINUE},
                          {"break", TOKEN_BREAK},
                          {"return", TOKEN_RETURN},
                          {"throw", TOKEN_THROW},
                          {"try", TOKEN_TRY},
                          {"catch", TOKEN_CATCH},
                          {"finally", TOKEN_FINALLY},
                          {"include", TOKEN_INCLUDE}};

    Source *source = source_new(path, text);
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

        // Integers
        if (*c == '0' && *(c + 1) == 'b') {
            c += 2;
            list_add(tokens, token_new_int(TOKEN_INT, source, line, column, strtol(c, &c, 2)));
            continue;
        }
        if (*c == '0' && (isdigit(*(c + 1)) || *(c + 1) == 'o')) {
            if (*(c + 1) == 'o') c++;
            c++;
            list_add(tokens, token_new_int(TOKEN_INT, source, line, column, strtol(c, &c, 8)));
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            list_add(tokens, token_new_int(TOKEN_INT, source, line, column, strtol(c, &c, 16)));
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
                list_add(tokens, token_new_float(source, line, column, strtod(start, &c)));
            } else {
                list_add(tokens, token_new_int(TOKEN_INT, source, line, column, strtol(start, &c, 10)));
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
            list_add(tokens, token_new_string(TOKEN_STRING, source, line, column, string));
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
                    list_add(tokens, token_new(keyword->type, source, line, column));
                    found = true;
                    break;
                }
            }
            if (!found) {
                list_add(tokens, token_new_string(TOKEN_KEYWORD, source, line, column, strndup(ptr, size)));
            }
            continue;
        }

        // Syntax
        if (*c == '(') {
            list_add(tokens, token_new(TOKEN_LPAREN, source, line, column));
            c++;
            continue;
        }
        if (*c == ')') {
            list_add(tokens, token_new(TOKEN_RPAREN, source, line, column));
            c++;
            continue;
        }
        if (*c == '{') {
            list_add(tokens, token_new(TOKEN_LCURLY, source, line, column));
            c++;
            continue;
        }
        if (*c == '}') {
            list_add(tokens, token_new(TOKEN_RCURLY, source, line, column));
            c++;
            continue;
        }
        if (*c == '[') {
            list_add(tokens, token_new(TOKEN_LBRACKET, source, line, column));
            c++;
            continue;
        }
        if (*c == ']') {
            list_add(tokens, token_new(TOKEN_RBRACKET, source, line, column));
            c++;
            continue;
        }
        if (*c == '?') {
            list_add(tokens, token_new(TOKEN_QUESTION, source, line, column));
            c++;
            continue;
        }
        if (*c == ';') {
            list_add(tokens, token_new(TOKEN_SEMICOLON, source, line, column));
            c++;
            continue;
        }
        if (*c == ':') {
            list_add(tokens, token_new(TOKEN_COLON, source, line, column));
            c++;
            continue;
        }
        if (*c == ',') {
            list_add(tokens, token_new(TOKEN_COMMA, source, line, column));
            c++;
            continue;
        }
        if (*c == '.') {
            list_add(tokens, token_new(TOKEN_POINT, source, line, column));
            c++;
            continue;
        }

        // Operators
        if (*c == '=') {
            if (*(c + 1) == '>') {
                list_add(tokens, token_new(TOKEN_FAT_ARROW, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_EQ, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_ASSIGN, source, line, column));
            c++;
            continue;
        }
        if (*c == '+') {
            if (*(c + 1) == '+') {
                list_add(tokens, token_new(TOKEN_INC, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_ADD, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_ADD, source, line, column));
            c++;
            continue;
        }
        if (*c == '-') {
            if (*(c + 1) == '-') {
                list_add(tokens, token_new(TOKEN_DEC, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_SUB, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_SUB, source, line, column));
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_MUL, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '*') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_EXP, source, line, column));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_EXP, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MUL, source, line, column));
            c++;
            continue;
        }
        if (*c == '/') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_DIV, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_DIV, source, line, column));
            c++;
            continue;
        }
        if (*c == '%') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_MOD, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MOD, source, line, column));
            c++;
            continue;
        }
        if (*c == '^') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_XOR, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_XOR, source, line, column));
            c++;
            continue;
        }
        if (*c == '~') {
            list_add(tokens, token_new(TOKEN_NOT, source, line, column));
            c++;
            continue;
        }
        if (*c == '<') {
            if (*(c + 1) == '<') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_SHL, source, line, column));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_SHL, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_LTEQ, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LT, source, line, column));
            c++;
            continue;
        }
        if (*c == '>') {
            if (*(c + 1) == '>') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_SHR, source, line, column));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_SHR, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_GTEQ, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_GT, source, line, column));
            c++;
            continue;
        }
        if (*c == '!') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_NEQ, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LOGICAL_NOT, source, line, column));
            c++;
            continue;
        }
        if (*c == '|') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_OR, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '|') {
                list_add(tokens, token_new(TOKEN_LOGICAL_OR, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_OR, source, line, column));
            c++;
            continue;
        }
        if (*c == '&') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_AND, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '&') {
                list_add(tokens, token_new(TOKEN_LOGICAL_AND, source, line, column));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_AND, source, line, column));
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

        list_add(tokens, token_new_int(TOKEN_UNKNOWN, source, line, column, *c));
        c++;
    }
    list_add(tokens, token_new(TOKEN_EOF, source, line, c - lineStart));
    source_free(source);
    return tokens;
}

// Value
Argument *argument_new(char *name, ValueType type, Node *defaultNode) {
    Argument *argument = malloc(sizeof(Argument));
    argument->name = strdup(name);
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
    value->refs = 1;
    value->type = type;
    return value;
}

Value *value_new_null(void) { return value_new(VALUE_NULL); }

Value *value_new_bool(bool boolean) {
    Value *value = value_new(VALUE_BOOL);
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
    value->string = strdup(string);
    return value;
}

Value *value_new_string_format(char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return value_new_string(buffer);
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

Value *value_new_class(Map *object, Value *parentClass, bool abstract) {
    Value *value = value_new(VALUE_CLASS);
    value->object = object;
    value->parentClass = parentClass;
    value->abstract = abstract;
    return value;
}

Value *value_new_instance(Map *object, Value *instanceClass) {
    Value *value = value_new(VALUE_INSTANCE);
    value->object = object;
    value->instanceClass = instanceClass;
    return value;
}

Value *value_new_function(List *args, ValueType returnType, Node *functionNode) {
    Value *value = value_new(VALUE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->functionNode = functionNode;
    return value;
}

Value *value_new_native_function(List *args, ValueType returnType, Value *(*nativeFunc)(InterpreterContext *context, Value *this, List *values)) {
    Value *value = value_new(VALUE_NATIVE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->nativeFunc = nativeFunc;
    return value;
}

char *value_type_to_string(ValueType type) {
    if (type == VALUE_ANY) return "any";
    if (type == VALUE_NULL) return "null";
    if (type == VALUE_BOOL) return "bool";
    if (type == VALUE_INT) return "int";
    if (type == VALUE_FLOAT) return "float";
    if (type == VALUE_STRING) return "string";
    if (type == VALUE_ARRAY) return "array";
    if (type == VALUE_OBJECT) return "object";
    if (type == VALUE_CLASS) return "class";
    if (type == VALUE_INSTANCE) return "instance";
    if (type == VALUE_FUNCTION || type == VALUE_NATIVE_FUNCTION) return "function";
    return NULL;
}

char *value_to_string(Value *value) {
    if (value->type == VALUE_NULL) {
        return strdup("null");
    }
    if (value->type == VALUE_BOOL) {
        return strdup(value->boolean ? "true" : "false");
    }
    if (value->type == VALUE_INT) {
        char buffer[255];
        sprintf(buffer, "%" PRIi64, value->integer);
        return strdup(buffer);
    }
    if (value->type == VALUE_FLOAT) {
        char buffer[255];
        sprintf(buffer, "%g", value->floating);
        return strdup(buffer);
    }
    if (value->type == VALUE_STRING) {
        return strdup(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        List *sb = list_new();
        list_add(sb, strdup("["));
        if (value->array->size > 0) list_add(sb, strdup(" "));
        for (size_t i = 0; i < value->array->size; i++) {
            Value *item = list_get(value->array, i);
            list_add(sb, item != NULL ? value_to_string(item) : strdup("null"));
            if (i != value->array->size - 1) list_add(sb, strdup(", "));
        }
        if (value->array->size > 0) list_add(sb, strdup(" "));
        list_add(sb, strdup("]"));
        char *string = list_to_string(sb);
        list_free(sb, free);
        return string;
    }
    if (value->type == VALUE_OBJECT || value->type == VALUE_CLASS || value->type == VALUE_INSTANCE) {
        List *sb = list_new();
        list_add(sb, strdup("{"));
        if (value->object->size > 0) list_add(sb, strdup(" "));
        for (size_t i = 0; i < value->object->size; i++) {
            list_add(sb, strdup(value->object->keys[i]));
            list_add(sb, strdup(" = "));
            list_add(sb, value_to_string(value->object->values[i]));
            if (i != value->object->size - 1) list_add(sb, strdup(", "));
        }
        if (value->object->size > 0) list_add(sb, strdup(" "));
        list_add(sb, strdup("}"));
        char *string = list_to_string(sb);
        list_free(sb, free);
        return string;
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        List *sb = list_new();
        list_add(sb, "fn (");
        for (size_t i = 0; i < value->arguments->size; i++) {
            Argument *argument = list_get(value->arguments, i);
            list_add(sb, argument->name);
            if (argument->type != VALUE_ANY) {
                list_add(sb, ": ");
                list_add(sb, value_type_to_string(argument->type));
            }
            if (i != value->arguments->size - 1) list_add(sb, ", ");
        }
        list_add(sb, ")");
        if (value->returnType != VALUE_ANY) {
            list_add(sb, ": ");
            list_add(sb, value_type_to_string(value->returnType));
        }
        char *string = list_to_string(sb);
        list_free(sb, NULL);
        return string;
    }
    return NULL;
}

ValueType token_type_to_value_type(TokenType type) {
    if (type == TOKEN_TYPE_ANY) return VALUE_ANY;
    if (type == TOKEN_NULL) return VALUE_NULL;
    if (type == TOKEN_TYPE_BOOL) return VALUE_BOOL;
    if (type == TOKEN_TYPE_INT) return VALUE_INT;
    if (type == TOKEN_TYPE_FLOAT) return VALUE_FLOAT;
    if (type == TOKEN_TYPE_STRING) return VALUE_STRING;
    if (type == TOKEN_TYPE_ARRAY) return VALUE_ARRAY;
    if (type == TOKEN_TYPE_OBJECT) return VALUE_OBJECT;
    if (type == TOKEN_CLASS) return VALUE_CLASS;
    if (type == TOKEN_TYPE_INSTANCE) return VALUE_INSTANCE;
    if (type == TOKEN_TYPE_FUNCTION) return VALUE_FUNCTION;
    return 0;
}

Value *value_class_get(Value *instance, char *key) {
    Value *value = map_get(instance->object, key);
    if (value != NULL) {
        return value;
    }
    if (instance->parentClass != NULL) {
        return value_class_get(instance->parentClass, key);
    }
    return NULL;
}

bool value_class_instanceof(Value *instance, Value *class) {
    bool result = instance->instanceClass == class;
    if (result) {
        return true;
    }
    if (instance->instanceClass->parentClass != NULL) {
        return value_class_instanceof(instance->instanceClass, class);
    }
    return false;
}

Value *value_ref(Value *value) {
    value->refs++;
    return value;
}

Value *value_retrieve(Value *value) {
    if (value->type == VALUE_NULL) return value_new_null();
    if (value->type == VALUE_BOOL) return value_new_bool(value->boolean);
    if (value->type == VALUE_INT) return value_new_int(value->integer);
    if (value->type == VALUE_FLOAT) return value_new_float(value->floating);
    if (value->type == VALUE_STRING) return value_new_string(value->string);
    return value_ref(value);
}

void value_clear(Value *value) {
    if (value->type == VALUE_STRING) {
        free(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        list_free(value->array, (ListFreeFunc *)value_free);
    }
    if (value->type == VALUE_OBJECT || value->type == VALUE_CLASS || value->type == VALUE_INSTANCE) {
        map_free(value->object, (MapFreeFunc *)value_free);
        if ((value->type == VALUE_CLASS || value->type == VALUE_INSTANCE) && value->parentClass != NULL) {
            value_free(value->parentClass);
        }
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        list_free(value->arguments, (ListFreeFunc *)argument_free);
    }
    if (value->type == VALUE_FUNCTION) {
        node_free(value->functionNode);
    }
}

void value_free(Value *value) {
    value->refs--;
    if (value->refs > 0) return;
    value_clear(value);
    free(value);
}

// Parser
Node *node_new(NodeType type, Token *token) {
    Node *node = malloc(sizeof(Node));
    node->refs = 1;
    node->type = type;
    if (token != NULL) {
        node->token = token_ref(token);
    }
    return node;
}

Node *node_new_value(Token *token, Value *value) {
    Node *node = node_new(NODE_VALUE, token);
    node->value = value;
    return node;
}

Node *node_new_string(NodeType type, Token *token, char *string) {
    Node *node = node_new(type, token);
    node->string = strdup(string);
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
    node->nodes = list_new();
    return node;
}

Node *node_ref(Node *node) {
    node->refs++;
    return node;
}

void node_free(Node *node) {
    node->refs--;
    if (node->refs > 0) return;

    if (node->token != NULL) {
        token_free(node->token);
    }
    if (node->type == NODE_VALUE) {
        value_free(node->value);
    }
    if (node->type == NODE_VARIABLE) {
        free(node->string);
    }
    if (node->type == NODE_ARRAY) {
        list_free(node->array, (ListFreeFunc *)node_free);
    }
    if (node->type == NODE_OBJECT || node->type == NODE_CLASS) {
        map_free(node->object, (MapFreeFunc *)node_free);
        if (node->type == NODE_CLASS && node->parentClass != NULL) {
            node_free(node->parentClass);
        }
    }
    if ((node->type >= NODE_RETURN && node->type <= NODE_INCLUDE) || (node->type >= NODE_NEG && node->type <= NODE_CAST)) {
        node_free(node->unary);
    }
    if (node->type >= NODE_GET && node->type <= NODE_LOGICAL_OR) {
        node_free(node->lhs);
        node_free(node->rhs);
    }
    if (node->type >= NODE_IF && node->type <= NODE_FORIN) {
        if (node->condition != NULL) node_free(node->condition);
        node_free(node->thenBlock);
        if (node->elseBlock != NULL) node_free(node->elseBlock);
        if (node->type == NODE_TRY && node->finallyBlock != NULL) node_free(node->finallyBlock);
    }
    if ((node->type >= NODE_PROGRAM && node->type <= NODE_BLOCK) || node->type == NODE_CALL) {
        if (node->type == NODE_CALL) node_free(node->function);
        list_free(node->nodes, (ListFreeFunc *)node_free);
    }
    free(node);
}

Node *parser(List *tokens, bool included) {
    Parser parser = {.tokens = tokens, .position = 0};
    return parser_program(&parser, included);
}

#define current() ((Token *)list_get(parser->tokens, parser->position))
#define next(pos) ((Token *)list_get(parser->tokens, parser->position + 1 + pos))

void parser_eat(Parser *parser, TokenType type) {
    if (current()->type == type) {
        parser->position++;
    } else {
        print_error(current(), "Unexpected token: '%s' needed '%s'", token_type_to_string(current()->type), token_type_to_string(type));
        exit(EXIT_FAILURE);
    }
}

ValueType parser_eat_type(Parser *parser) {
    if (token_type_is_type(current()->type)) {
        ValueType type = token_type_to_value_type(current()->type);
        parser->position++;
        return type;
    }
    print_error(current(), "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
    return VALUE_ANY;
}

Node *parser_program(Parser *parser, bool included) {
    Node *programNode = node_new_multiple(included ? NODE_NODES : NODE_PROGRAM, current());
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

    if (current()->type == TOKEN_TRY) {
        Node *node = node_new(NODE_TRY, current());
        parser_eat(parser, TOKEN_TRY);
        node->tryBlock = parser_block(parser);

        parser_eat(parser, TOKEN_CATCH);
        parser_eat(parser, TOKEN_LPAREN);
        Node *declarations = parser_declarations(parser);
        if (declarations->type != NODE_CONST_ASSIGN && declarations->type != NODE_LET_ASSIGN) {
            print_error(declarations->token, "You can only declare one variable in a catch block");
            exit(EXIT_FAILURE);
        }
        node->catchVariable = declarations;
        parser_eat(parser, TOKEN_RPAREN);
        node->catchBlock = parser_block(parser);

        if (current()->type == TOKEN_FINALLY) {
            parser_eat(parser, TOKEN_FINALLY);
            node->finallyBlock = parser_block(parser);
        } else {
            node->finallyBlock = NULL;
        }
        return node;
    }

    if (current()->type == TOKEN_LOOP) {
        Node *node = node_new(NODE_LOOP, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_LOOP);
        node->condition = NULL;
        node->thenBlock = parser_block(parser);
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
        Token *token = current();
        parser_eat(parser, TOKEN_FOR);
        parser_eat(parser, TOKEN_LPAREN);
        Node *declarations;
        if (current()->type != TOKEN_SEMICOLON) {
            declarations = parser_declarations(parser);
        }

        if (current()->type == TOKEN_IN) {
            Node *node = node_new(NODE_FORIN, token);
            if (declarations->type != NODE_CONST_ASSIGN && declarations->type != NODE_LET_ASSIGN) {
                print_error(declarations->token, "You can only declare one variable in a for in loop");
                exit(EXIT_FAILURE);
            }
            node->forinVariable = declarations;
            parser_eat(parser, TOKEN_IN);
            node->iterator = parser_tenary(parser);
            parser_eat(parser, TOKEN_RPAREN);
            node->thenBlock = parser_block(parser);
            return node;
        }

        Node *blockNode = node_new_multiple(NODE_BLOCK, token);
        list_add(blockNode->nodes, declarations);

        Node *node = node_new(NODE_FOR, token);
        parser_eat(parser, TOKEN_SEMICOLON);
        if (current()->type != TOKEN_SEMICOLON) {
            node->condition = parser_tenary(parser);
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
        Node *node = node_new_unary(NODE_RETURN, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == TOKEN_THROW) {
        Token *token = current();
        parser_eat(parser, TOKEN_THROW);
        Node *node = node_new_unary(NODE_THROW, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == TOKEN_INCLUDE) {
        Token *token = current();
        parser_eat(parser, TOKEN_INCLUDE);
        Node *node = node_new_unary(NODE_INCLUDE, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        Token *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        Node *node =
            node_new_operation(NODE_CONST_ASSIGN, functionToken, node_new_string(NODE_VARIABLE, nameToken, name), parser_function(parser, functionToken));
        node->declarationType = VALUE_FUNCTION;
        return node;
    }
    if (current()->type == TOKEN_ABSTRACT || current()->type == TOKEN_CLASS) {
        Token *classToken = current();
        bool abstract = false;
        if (current()->type == TOKEN_ABSTRACT) {
            abstract = true;
            parser_eat(parser, TOKEN_ABSTRACT);
        }
        parser_eat(parser, TOKEN_CLASS);
        Token *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        Node *node =
            node_new_operation(NODE_CONST_ASSIGN, classToken, node_new_string(NODE_VARIABLE, nameToken, name), parser_class(parser, classToken, abstract));
        node->declarationType = VALUE_CLASS;
        return node;
    }

    Node *node = parser_declarations(parser);
    parser_eat(parser, TOKEN_SEMICOLON);
    return node;
}

Node *parser_declarations(Parser *parser) {
    if (current()->type == TOKEN_CONST || current()->type == TOKEN_LET) {
        List *nodes = list_new();
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
            char *name = current()->string;
            parser_eat(parser, TOKEN_KEYWORD);
            Node *variable = node_new_string(NODE_VARIABLE, current(), name);

            ValueType declarationType = VALUE_ANY;
            if (current()->type == TOKEN_COLON) {
                parser_eat(parser, TOKEN_COLON);
                declarationType = parser_eat_type(parser);
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
            list_add(nodes, node);

            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        if (nodes->size == 0) {
            list_free(nodes, NULL);
            return NULL;
        }
        if (nodes->size == 1) {
            Node *firstNode = list_get(nodes, 0);
            list_free(nodes, NULL);
            return firstNode;
        }
        Node *nodesNode = node_new(NODE_NODES, current());
        nodesNode->nodes = nodes;
        return nodesNode;
    }
    return parser_assigns(parser);
}

Node *parser_assigns(Parser *parser) {
    List *nodes = list_new();
    for (;;) {
        Node *node = parser_assign(parser);
        list_add(nodes, node);
        if (current()->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
        } else {
            break;
        }
    }
    if (nodes->size == 0) {
        list_free(nodes, NULL);
        return NULL;
    }
    if (nodes->size == 1) {
        Node *firstNode = list_get(nodes, 0);
        list_free(nodes, NULL);
        return firstNode;
    }
    Node *nodesNode = node_new(NODE_NODES, current());
    nodesNode->nodes = nodes;
    return nodesNode;
}

Node *parser_assign(Parser *parser) {
    Node *lhs = parser_tenary(parser);  // TODO
    if (current()->type == TOKEN_ASSIGN) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN);
        return node_new_operation(NODE_ASSIGN, token, lhs, parser_assign(parser));
    }
    if (current()->type == TOKEN_ASSIGN_ADD) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_ADD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_ADD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SUB) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SUB);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SUB, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MUL) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MUL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MUL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_EXP) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_EXP);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_EXP, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MOD) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MOD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MOD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_AND) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_AND);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_AND, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_XOR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_XOR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_XOR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_OR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_OR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_OR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHL) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHR, token, node_ref(lhs), parser_assign(parser)));
    }
    return lhs;
}

Node *parser_tenary(Parser *parser) {
    Node *node = parser_logical(parser);
    if (current()->type == TOKEN_QUESTION) {
        Node *tenaryNode = node_new(NODE_TENARY, current());
        parser_eat(parser, TOKEN_QUESTION);
        tenaryNode->condition = node;
        tenaryNode->thenBlock = parser_tenary(parser);
        parser_eat(parser, TOKEN_COLON);
        tenaryNode->elseBlock = parser_tenary(parser);
        return tenaryNode;
    }
    return node;
}

Node *parser_logical(Parser *parser) {
    Node *node = parser_equality(parser);
    while (current()->type == TOKEN_LOGICAL_AND || current()->type == TOKEN_LOGICAL_OR) {
        if (current()->type == TOKEN_LOGICAL_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_AND);
            node = node_new_operation(NODE_LOGICAL_AND, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_LOGICAL_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_OR);
            node = node_new_operation(NODE_LOGICAL_OR, token, node, parser_equality(parser));
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
    Node *node = parser_instanceof(parser);
    while (current()->type == TOKEN_LT || current()->type == TOKEN_LTEQ || current()->type == TOKEN_GT || current()->type == TOKEN_GTEQ) {
        if (current()->type == TOKEN_LT) {
            Token *token = current();
            parser_eat(parser, TOKEN_LT);
            node = node_new_operation(NODE_LT, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_LTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_LTEQ);
            node = node_new_operation(NODE_LTEQ, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_GT) {
            Token *token = current();
            parser_eat(parser, TOKEN_GT);
            node = node_new_operation(NODE_GT, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_GTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_GTEQ);
            node = node_new_operation(NODE_GTEQ, token, node, parser_instanceof(parser));
        }
    }
    return node;
}

Node *parser_instanceof(Parser *parser) {
    Node *node = parser_bitwise(parser);
    while (current()->type == TOKEN_INSTANCEOF) {
        Token *token = current();
        parser_eat(parser, TOKEN_INSTANCEOF);
        node = node_new_operation(NODE_INSTANCEOF, token, node, parser_bitwise(parser));
    }
    return node;
}

Node *parser_bitwise(Parser *parser) {
    Node *node = parser_shift(parser);
    while (current()->type == TOKEN_AND || current()->type == TOKEN_XOR || current()->type == TOKEN_OR) {
        if (current()->type == TOKEN_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_AND);
            node = node_new_operation(NODE_AND, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_XOR) {
            Token *token = current();
            parser_eat(parser, TOKEN_XOR);
            node = node_new_operation(NODE_XOR, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_OR);
            node = node_new_operation(NODE_OR, token, node, parser_shift(parser));
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
    if (current()->type == TOKEN_INC) {
        Token *token = current();
        parser_eat(parser, TOKEN_INC);
        return node_new_unary(NODE_INC_PRE, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_DEC) {
        Token *token = current();
        parser_eat(parser, TOKEN_DEC);
        return node_new_unary(NODE_DEC_PRE, token, parser_unary(parser));
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
        ValueType castType = parser_eat_type(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return node_new_cast(token, castType, parser_unary(parser));
    }
    return parser_primary(parser);
}

Node *parser_primary(Parser *parser) {
    if (current()->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        Node *node = parser_tenary(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_NULL) {
        Node *node = node_new_value(current(), value_new(VALUE_NULL));
        parser_eat(parser, TOKEN_NULL);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_TRUE) {
        Node *node = node_new_value(current(), value_new_bool(true));
        parser_eat(parser, TOKEN_TRUE);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_FALSE) {
        Node *node = node_new_value(current(), value_new_bool(false));
        parser_eat(parser, TOKEN_FALSE);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_INT) {
        Node *node = node_new_value(current(), value_new_int(current()->integer));
        parser_eat(parser, TOKEN_INT);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_FLOAT) {
        Node *node = node_new_value(current(), value_new_float(current()->floating));
        parser_eat(parser, TOKEN_FLOAT);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_STRING) {
        Node *node = node_new_value(current(), value_new_string(current()->string));
        parser_eat(parser, TOKEN_STRING);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_KEYWORD) {
        Token *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        return parser_primary_suffix(parser, node_new_string(NODE_VARIABLE, nameToken, name));
    }
    if (current()->type == TOKEN_LBRACKET) {
        Node *node = node_new(NODE_ARRAY, current());
        node->array = list_new();
        parser_eat(parser, TOKEN_LBRACKET);
        while (current()->type != TOKEN_RBRACKET) {
            list_add(node->array, parser_assign(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RBRACKET);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_LCURLY) {
        Node *node = node_new(NODE_OBJECT, current());
        node->object = map_new();
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            if (current()->type == TOKEN_FUNCTION) {
                Token *functionToken = current();
                parser_eat(parser, TOKEN_FUNCTION);
                char *keyName = current()->string;
                parser_eat(parser, TOKEN_KEYWORD);
                map_set(node->object, keyName, parser_function(parser, functionToken));
                continue;
            }

            Token *keyToken = current();
            char *keyName = current()->string;
            parser_eat(parser, TOKEN_KEYWORD);
            if (current()->type == TOKEN_ASSIGN) {
                parser_eat(parser, TOKEN_ASSIGN);
                map_set(node->object, keyName, parser_tenary(parser));
            } else {
                map_set(node->object, keyName, node_new_string(NODE_VARIABLE, keyToken, keyName));
            }
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RCURLY);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        return parser_primary_suffix(parser, parser_function(parser, functionToken));
    }
    if (current()->type == TOKEN_ABSTRACT || current()->type == TOKEN_CLASS) {
        Token *classToken = current();
        bool abstract = false;
        if (current()->type == TOKEN_ABSTRACT) {
            abstract = true;
            parser_eat(parser, TOKEN_ABSTRACT);
        }
        parser_eat(parser, TOKEN_CLASS);
        return parser_class(parser, classToken, abstract);
    }

    print_error(current(), "Unexpected token: '%s'", token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
    return NULL;
}

Node *parser_primary_suffix(Parser *parser, Node *node) {
    while (current()->type == TOKEN_LBRACKET || current()->type == TOKEN_POINT || current()->type == TOKEN_LPAREN || current()->type == TOKEN_INC ||
           current()->type == TOKEN_DEC) {
        Token *token = current();
        if (current()->type == TOKEN_LBRACKET) {
            parser_eat(parser, TOKEN_LBRACKET);
            Node *indexOrKey = parser_assign(parser);
            parser_eat(parser, TOKEN_RBRACKET);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
        if (current()->type == TOKEN_POINT) {
            parser_eat(parser, TOKEN_POINT);
            Token *keyToken = current();
            char *key = current()->string;
            parser_eat(parser, TOKEN_KEYWORD);
            node = node_new_operation(NODE_GET, token, node, node_new_value(keyToken, value_new_string(key)));
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
            node = callNode;
        }
        if (current()->type == TOKEN_INC) {
            parser_eat(parser, TOKEN_INC);
            node = node_new_unary(NODE_INC_POST, token, node);
        }
        if (current()->type == TOKEN_DEC) {
            parser_eat(parser, TOKEN_DEC);
            node = node_new_unary(NODE_DEC_POST, token, node);
        }
    }
    return node;
}

Node *parser_function(Parser *parser, Token *token) {
    List *arguments = list_new();
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
        returnType = parser_eat_type(parser);
    }

    if (current()->type == TOKEN_FAT_ARROW) {
        Token *fatArrowToken = current();
        parser_eat(parser, TOKEN_FAT_ARROW);
        return node_new_value(token, value_new_function(arguments, returnType, node_new_unary(NODE_RETURN, fatArrowToken, parser_tenary(parser))));
    }
    return node_new_value(token, value_new_function(arguments, returnType, parser_block(parser)));
}

Node *parser_class(Parser *parser, Token *token, bool abstract) {
    Node *parentClass = NULL;
    if (current()->type == TOKEN_EXTENDS) {
        parser_eat(parser, TOKEN_EXTENDS);
        parentClass = parser_tenary(parser);
    }

    Map *object = map_new();
    parser_eat(parser, TOKEN_LCURLY);
    while (current()->type != TOKEN_RCURLY) {
        if (current()->type == TOKEN_FUNCTION) {
            Token *functionToken = current();
            parser_eat(parser, TOKEN_FUNCTION);
            char *keyName = current()->string;
            parser_eat(parser, TOKEN_KEYWORD);
            map_set(object, keyName, parser_function(parser, functionToken));
            continue;
        }

        char *keyName = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        parser_eat(parser, TOKEN_ASSIGN);
        map_set(object, keyName, parser_tenary(parser));

        if (current()->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
        } else {
            break;
        }
    }
    parser_eat(parser, TOKEN_RCURLY);

    Node *classNode = node_new(NODE_CLASS, token);
    classNode->object = object;
    classNode->parentClass = parentClass;
    classNode->abstract = abstract;
    return classNode;
}

Argument *parser_argument(Parser *parser) {
    char *name = current()->string;
    parser_eat(parser, TOKEN_KEYWORD);
    ValueType type = VALUE_ANY;
    if (current()->type == TOKEN_COLON) {
        parser_eat(parser, TOKEN_COLON);
        type = parser_eat_type(parser);
    }
    Node *defaultNode = NULL;
    if (current()->type == TOKEN_ASSIGN) {
        parser_eat(parser, TOKEN_ASSIGN);
        defaultNode = parser_tenary(parser);
    }
    return argument_new(name, type, defaultNode);
}

// Standard library

// Math
static Value *env_math_abs(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    if (x->type == VALUE_INT) {
        return value_new_int(x->integer < 0 ? -x->integer : x->integer);
    }
    if (x->type == VALUE_FLOAT) {
        return value_new_float(x->floating < 0 ? -x->floating : x->floating);
    }
    return value_new_null();
}
static Value *env_math_sin(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(sin(x->floating));
}
static Value *env_math_cos(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(cos(x->floating));
}
static Value *env_math_tan(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(tan(x->floating));
}
static Value *env_math_asin(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(asin(x->floating));
}
static Value *env_math_acos(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(acos(x->floating));
}
static Value *env_math_atan(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(atan(x->floating));
}
static Value *env_math_atan2(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *y = list_get(values, 0);
    Value *x = list_get(values, 1);
    return value_new_float(atan2(y->floating, x->floating));
}
static Value *env_math_pow(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    Value *y = list_get(values, 1);
    return value_new_float(pow(x->floating, y->floating));
}
static Value *env_math_sqrt(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(sqrt(x->floating));
}
static Value *env_math_floor(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(floor(x->floating));
}
static Value *env_math_ceil(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(ceil(x->floating));
}
static Value *env_math_round(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(round(x->floating));
}
static Value *env_math_min(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *first = list_get(values, 0);
    int64_t minInteger;
    double minFloating;
    if (first->type == VALUE_INT) {
        minInteger = first->integer;
        minFloating = first->integer;
    }
    if (first->type == VALUE_FLOAT) {
        minInteger = first->floating;
        minFloating = first->floating;
    }

    bool onlyInteger = true;
    list_foreach(values, Value * value, {
        if (value->type != VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == VALUE_INT) {
                minInteger = MIN(minInteger, value->integer);
            }
            if (value->type == VALUE_FLOAT) {
                minInteger = MIN(minInteger, value->floating);
            }
        }
        if (value->type == VALUE_INT) {
            minFloating = MIN(minFloating, value->integer);
        }
        if (value->type == VALUE_FLOAT) {
            minFloating = MIN(minFloating, value->floating);
        }
    });
    return onlyInteger ? value_new_int(minInteger) : value_new_float(minFloating);
}
static Value *env_math_max(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *first = list_get(values, 0);
    int64_t maxInteger;
    double maxFloating;
    if (first->type == VALUE_INT) {
        maxInteger = first->integer;
        maxFloating = first->integer;
    }
    if (first->type == VALUE_FLOAT) {
        maxInteger = first->floating;
        maxFloating = first->floating;
    }

    bool onlyInteger = true;
    list_foreach(values, Value * value, {
        if (value->type != VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == VALUE_INT) {
                maxInteger = MAX(maxInteger, value->integer);
            }
            if (value->type == VALUE_FLOAT) {
                maxInteger = MAX(maxInteger, value->floating);
            }
        }
        if (value->type == VALUE_INT) {
            maxFloating = MAX(maxFloating, value->integer);
        }
        if (value->type == VALUE_FLOAT) {
            maxFloating = MAX(maxFloating, value->floating);
        }
    });
    return onlyInteger ? value_new_int(maxInteger) : value_new_float(maxFloating);
}
static Value *env_math_exp(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(exp(x->floating));
}
static Value *env_math_log(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *x = list_get(values, 0);
    return value_new_float(log(x->floating));
}
static Value *env_math_random(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    (void)values;
    return value_new_float(random_random());
}

// Exception
static Value *env_exception_constructor(InterpreterContext *context, Value *this, List *values) {
    Value *error = list_get(values, 0);
    map_set(this->object, "error", value_retrieve(error));
    map_set(this->object, "path", value_new_string(context->node->token->source->path));
    map_set(this->object, "text", value_new_string(context->node->token->source->text));
    map_set(this->object, "line", value_new_int(context->node->token->line));
    map_set(this->object, "column", value_new_int(context->node->token->column));
    return value_new_null();
}

// String
static Value *env_string_constructor(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *first = list_get(values, 0);
    char *string = value_to_string(first);
    Value *value = value_new_string(string);
    free(string);
    return value;
}
static Value *env_string_length(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)values;
    return value_new_int(strlen(this->string));
}

// Array
static Value *env_array_constructor(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *first = list_get(values, 0);
    if (first != NULL && first->type == VALUE_ARRAY) {
        return value_ref(first);
    }
    return value_new_array(list_new());
}
static Value *env_array_length(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)values;
    return value_new_int(this->array->size);
}
static Value *env_array_push(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    list_foreach(values, Value * value, { list_add(this->array, value_retrieve(value)); });
    return value_new_int(this->array->size);
}
static Value *env_array_foreach(InterpreterContext *context, Value *this, List *values) {
    Value *function = list_get(values, 0);
    list_foreach(this->array, Value * value, {
        List *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        interpreter_call(context, function, NULL, arguments);
        list_free(arguments, (ListFreeFunc *)value_free);
    });
    return value_new_null();
}
static Value *env_array_map(InterpreterContext *context, Value *this, List *values) {
    Value *function = list_get(values, 0);
    List *items = list_new();
    list_foreach(this->array, Value * value, {
        List *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        list_add(items, interpreter_call(context, function, NULL, arguments));
        list_free(arguments, (ListFreeFunc *)value_free);
    });
    return value_new_array(items);
}
static Value *env_array_filter(InterpreterContext *context, Value *this, List *values) {
    Value *function = list_get(values, 0);
    List *items = list_new();
    list_foreach(this->array, Value * value, {
        List *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        Value *returnValue = interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != VALUE_BOOL) {
            return interpreter_throw(context,
                                     value_new_string_format("Array filter condition type is not a bool it is: %s", value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            list_add(items, value);
        }
        value_free(returnValue);
        list_free(arguments, (ListFreeFunc *)value_free);
    });
    return value_new_array(items);
}
static Value *env_array_find(InterpreterContext *context, Value *this, List *values) {
    Value *function = list_get(values, 0);
    list_foreach(this->array, Value * value, {
        List *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        Value *returnValue = interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != VALUE_BOOL) {
            return interpreter_throw(context,
                                     value_new_string_format("Array find condition type is not a bool it is: %s", value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            value_free(returnValue);
            list_free(arguments, (ListFreeFunc *)value_free);
            return value_retrieve(value);
        }
        value_free(returnValue);
        list_free(arguments, (ListFreeFunc *)value_free);
    });
    return value_new_null();
}

// Object
static Value *env_object_constructor(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *first = list_get(values, 0);
    if (first != NULL && first->type == VALUE_OBJECT) {
        return value_ref(first);
    }
    return value_new_object(map_new());
}
static Value *env_object_length(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)values;
    return value_new_int(this->object->size);
}
static Value *env_object_keys(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)values;
    List *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_new_string(this->object->keys[i]));
    }
    return value_new_array(items);
}
static Value *env_object_values(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)values;
    List *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_retrieve(this->object->values[i]));
    }
    return value_new_array(items);
}

// Date
static Value *env_date_now(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    (void)values;
    return value_new_int(time_ms());
}

// Root
static Value *env_type(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *value = list_get(values, 0);
    return value_new_string(value_type_to_string(value->type));
}

static Value *env_assert(InterpreterContext *context, Value *this, List *values) {
    (void)context;
    (void)this;
    Value *assertion = list_get(values, 0);
    if (!assertion->boolean) {
        return interpreter_throw(context, value_new_string("Assertion failed"));
    }
    return value_new_null();
}

Map *std_env(void) {
    Map *env = map_new();

    List *empty_args = list_new();

    // Math
    Map *math = map_new();
    map_set(env, "Math", variable_new(VALUE_OBJECT, false, value_new_object(math)));
    map_set(math, "E", value_new_float(M_E));
    map_set(math, "LN2", value_new_float(M_LN2));
    map_set(math, "LN10", value_new_float(M_LN10));
    map_set(math, "LOG2E", value_new_float(M_LOG2E));
    map_set(math, "LOG10E", value_new_float(M_LOG10E));
    map_set(math, "PI", value_new_float(M_PI));
    map_set(math, "SQRT1_2", value_new_float(M_SQRT1_2));
    map_set(math, "SQRT2", value_new_float(M_SQRT2));

    List *math_float_args = list_new();
    list_add(math_float_args, argument_new("x", VALUE_FLOAT, NULL));
    List *math_float_float_args = list_new();
    list_add(math_float_float_args, argument_new("x", VALUE_FLOAT, NULL));
    list_add(math_float_float_args, argument_new("y", VALUE_FLOAT, NULL));
    List *math_float_float_reverse_args = list_new();
    list_add(math_float_float_reverse_args, argument_new("y", VALUE_FLOAT, NULL));
    list_add(math_float_float_reverse_args, argument_new("x", VALUE_FLOAT, NULL));
    map_set(math, "abs", value_new_native_function(math_float_args, VALUE_ANY, env_math_abs));
    map_set(math, "sin", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_sin));
    map_set(math, "cos", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_cos));
    map_set(math, "tan", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_tan));
    map_set(math, "asin", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_asin));
    map_set(math, "acos", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_acos));
    map_set(math, "atan", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_atan));
    map_set(math, "atan2", value_new_native_function(math_float_float_reverse_args, VALUE_FLOAT, env_math_atan2));
    map_set(math, "pow", value_new_native_function(math_float_float_args, VALUE_FLOAT, env_math_pow));
    map_set(math, "sqrt", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_sqrt));
    map_set(math, "floor", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_floor));
    map_set(math, "ceil", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_ceil));
    map_set(math, "round", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_round));
    map_set(math, "min", value_new_native_function(empty_args, VALUE_ANY, env_math_min));
    map_set(math, "max", value_new_native_function(list_ref(empty_args), VALUE_ANY, env_math_max));
    map_set(math, "exp", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_exp));
    map_set(math, "log", value_new_native_function(list_ref(math_float_args), VALUE_FLOAT, env_math_log));
    random_seed = time_ms();
    map_set(math, "random", value_new_native_function(list_ref(empty_args), VALUE_FLOAT, env_math_random));

    // Exception
    Map *exception = map_new();
    List *exception_constructor_args = list_new();
    list_add(exception_constructor_args, argument_new("error", VALUE_STRING, NULL));
    map_set(env, "Exception", variable_new(VALUE_CLASS, false, value_new_class(exception, NULL, false)));
    map_set(exception, "constructor", value_new_native_function(exception_constructor_args, VALUE_ANY, env_exception_constructor));

    // String
    Map *string = map_new();
    map_set(env, "String", variable_new(VALUE_CLASS, false, value_new_class(string, NULL, false)));
    map_set(string, "constructor", value_new_native_function(list_ref(empty_args), VALUE_STRING, env_string_constructor));
    map_set(string, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_string_length));

    // Array
    Map *array = map_new();
    List *array_function_args = list_new();
    list_add(array_function_args, argument_new("function", VALUE_FUNCTION, NULL));
    map_set(env, "Array", variable_new(VALUE_CLASS, false, value_new_class(array, NULL, false)));
    map_set(array, "constructor", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_array_constructor));
    map_set(array, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_array_length));
    map_set(array, "push", value_new_native_function(list_ref(empty_args), VALUE_INT, env_array_push));
    map_set(array, "foreach", value_new_native_function(array_function_args, VALUE_NULL, env_array_foreach));
    map_set(array, "map", value_new_native_function(list_ref(array_function_args), VALUE_ARRAY, env_array_map));
    map_set(array, "filter", value_new_native_function(list_ref(array_function_args), VALUE_ARRAY, env_array_filter));
    map_set(array, "find", value_new_native_function(list_ref(array_function_args), VALUE_ANY, env_array_find));

    // Object
    Map *object = map_new();
    map_set(env, "Object", variable_new(VALUE_CLASS, false, value_new_class(object, NULL, false)));
    map_set(object, "constructor", value_new_native_function(list_ref(empty_args), VALUE_OBJECT, env_object_constructor));
    map_set(object, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_object_length));
    map_set(object, "keys", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_keys));
    map_set(object, "values", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_values));

    // Date
    Map *date = map_new();
    map_set(env, "Date", variable_new(VALUE_CLASS, false, value_new_class(date, NULL, false)));
    map_set(date, "now", value_new_native_function(list_ref(empty_args), VALUE_INT, env_date_now));

    // Root
    List *type_args = list_new();
    list_add(type_args, argument_new("value", VALUE_ANY, NULL));
    map_set(env, "type", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(type_args, VALUE_ANY, env_type)));

    List *assertion_args = list_new();
    list_add(assertion_args, argument_new("assertion", VALUE_ANY, NULL));
    map_set(env, "assert", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(assertion_args, VALUE_NULL, env_assert)));

    return env;
}

// Interpreter
Variable *variable_new(ValueType type, bool mutable, Value *value) {
    Variable *variable = malloc(sizeof(Variable));
    variable->type = type;
    variable->mutable = mutable;
    variable->value = value;
    return variable;
}

void variable_free(Variable *variable) {
    value_free(variable->value);
    free(variable);
}

Variable *block_scope_get(BlockScope *block, char *key) {
    Variable *variable = map_get(block->env, key);
    if (variable == NULL && block->parentBlock != NULL) {
        return block_scope_get(block->parentBlock, key);
    }
    return variable;
}

Value *interpreter(Map *env, Node *node) {
    Interpreter interpreter = {.env = env};
    Scope scope = {.exception = &(ExceptionScope){.exceptionValue = NULL},
                   .function = &(FunctionScope){.returnValue = NULL},
                   .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                   .block = &(BlockScope){.parentBlock = NULL, .env = env}};
    Value *returnValue = interpreter_node(&interpreter, &scope, node);
    if (scope.exception->exceptionValue != NULL) {
        Value *path = value_class_get(scope.exception->exceptionValue, "path");
        Value *text = value_class_get(scope.exception->exceptionValue, "text");
        Value *line = value_class_get(scope.exception->exceptionValue, "line");
        Value *column = value_class_get(scope.exception->exceptionValue, "column");
        Token *token = token_new(TOKEN_THROW, source_new(path->string, text->string), line->integer, column->integer);
        Value *error = value_class_get(scope.exception->exceptionValue, "error");
        print_error(token, "Uncatched exception: %s", error->string);
        token_free(token);
    }
    if (returnValue != NULL) {
        return returnValue;
    }
    if (scope.function->returnValue != NULL) {
        return scope.function->returnValue;
    } else {
        return value_new_null();
    }
}

Value *type_error_exception(ValueType expected, ValueType got) {
    return value_new_string_format("Unexpected type: '%s' expected '%s'", value_type_to_string(got), value_type_to_string(expected));
}

#define interpreter_statement_in_try(interpreter, scope, node, cleanup) \
    {                                                                   \
        Value *nodeValue = interpreter_node(interpreter, scope, node);  \
        if (nodeValue != NULL) value_free(nodeValue);                   \
        if ((scope)->function->returnValue != NULL) {                   \
            cleanup;                                                    \
            return NULL;                                                \
        }                                                               \
    }

#define interpreter_statement_in_loop(interpreter, scope, node, cleanup) \
    {                                                                    \
        interpreter_statement_in_try(interpreter, scope, node, cleanup); \
        if ((scope)->exception->exceptionValue != NULL) {                \
            cleanup;                                                     \
            return NULL;                                                 \
        }                                                                \
    }

#define interpreter_statement(interpreter, scope, node, cleanup)      \
    interpreter_statement_in_loop(interpreter, scope, node, cleanup); \
    if ((scope)->loop->inLoop) {                                      \
        if ((scope)->loop->isContinuing) {                            \
            cleanup;                                                  \
            return NULL;                                              \
        }                                                             \
        if ((scope)->loop->isBreaking) {                              \
            cleanup;                                                  \
            return NULL;                                              \
        }                                                             \
    }

Value *interpreter_call(InterpreterContext *context, Value *callValue, Value *this, List *arguments) {
    if (callValue->type == VALUE_FUNCTION) {
        Scope functionScope = {.exception = context->scope->exception,
                               .function = &(FunctionScope){.returnValue = NULL},
                               .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                               .block = &(BlockScope){.parentBlock = context->scope->block, .env = map_new()}};
        if (this != NULL) {
            if (this->instanceClass->parentClass != NULL) {
                map_set(functionScope.block->env, "super",
                        variable_new(VALUE_INSTANCE, false, value_new_instance(map_ref(this->object), value_ref(this->instanceClass->parentClass))));
            }
            map_set(functionScope.block->env, "this", variable_new(VALUE_INSTANCE, false, value_ref(this)));
        }
        map_set(functionScope.block->env, "arguments", variable_new(VALUE_ARRAY, false, value_new_array(list_ref(arguments))));
        for (size_t i = 0; i < callValue->arguments->size; i++) {
            Argument *argument = list_get(callValue->arguments, i);
            map_set(functionScope.block->env, argument->name, variable_new(argument->type, true, value_ref(list_get(arguments, i))));
        }
        Interpreter interpreter = {.env = context->env};
        interpreter_node(&interpreter, &functionScope, callValue->functionNode);
        map_free(functionScope.block->env, (MapFreeFunc *)variable_free);
        if (callValue->returnType != VALUE_ANY && context->scope->exception->exceptionValue == NULL &&
            functionScope.function->returnValue->type != callValue->returnType) {
            ValueType returnValueType = functionScope.function->returnValue->type;
            value_free(functionScope.function->returnValue);
            return interpreter_throw(context, type_error_exception(callValue->returnType, returnValueType));
        }

        if (functionScope.function->returnValue != NULL) {
            return functionScope.function->returnValue;
        }
        return value_new_null();
    }

    if (callValue->type == VALUE_NATIVE_FUNCTION) {
        Value *returnValue = callValue->nativeFunc(context, this, arguments);
        if (callValue->returnType != VALUE_ANY && context->scope->exception->exceptionValue == NULL && returnValue->type != callValue->returnType) {
            return interpreter_throw(context, type_error_exception(callValue->returnType, returnValue->type));
        }
        return returnValue;
    }

    if (callValue->type == VALUE_CLASS) {
        Value *instance = value_new_instance(map_new(), value_ref(callValue));
        Value *constructorFunction = value_class_get(callValue, "constructor");
        if (constructorFunction != NULL) {
            Value *newReturnValue = interpreter_call(context, constructorFunction, instance, arguments);
            if (newReturnValue->type == VALUE_NULL) {
                value_free(newReturnValue);
            } else {
                value_free(instance);
                return newReturnValue;
            }
        }
        return instance;
    }
    return NULL;
}

Value *interpreter_throw(InterpreterContext *context, Value *exception) {
    if (exception->type == VALUE_STRING) {
        Value *exceptionClass = ((Variable *)map_get(context->env, "Exception"))->value;
        List *arguments = list_new();
        list_add(arguments, exception);
        exception = interpreter_call(context, exceptionClass, NULL, arguments);
        list_free(arguments, (ListFreeFunc *)value_free);
    }
    if (exception->type != VALUE_INSTANCE) {
        return interpreter_throw(context, type_error_exception(VALUE_INSTANCE, exception->type));
    }
    context->scope->exception->exceptionValue = exception;
    return value_new_null();
}

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node) {
    if (node->type == NODE_PROGRAM) {
        list_foreach(node->nodes, Node * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NODE_NODES) {
        list_foreach(node->nodes, Node * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NODE_BLOCK) {
        Scope blockScope = {.exception = scope->exception,
                            .function = scope->function,
                            .loop = scope->loop,
                            .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
        list_foreach(node->nodes, Node * child,
                     { interpreter_statement(interpreter, &blockScope, child, { map_free(blockScope.block->env, (MapFreeFunc *)variable_free); }); });
        map_free(blockScope.block->env, (MapFreeFunc *)variable_free);
        return NULL;
    }
    if (node->type == NODE_IF) {
        Value *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOL) {
            ValueType conditionType = condition->type;
            value_free(condition);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
            return interpreter_throw(&context, type_error_exception(VALUE_BOOL, conditionType));
        }
        if (condition->boolean) {
            interpreter_statement(interpreter, scope, node->thenBlock, { value_free(condition); });
        } else if (node->elseBlock != NULL) {
            interpreter_statement(interpreter, scope, node->elseBlock, { value_free(condition); });
        }
        value_free(condition);
        return NULL;
    }
    if (node->type == NODE_TENARY) {
        Value *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOL) {
            ValueType conditionType = condition->type;
            value_free(condition);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
            return interpreter_throw(&context, type_error_exception(VALUE_BOOL, conditionType));
        }
        if (condition->boolean) {
            value_free(condition);
            return interpreter_node(interpreter, scope, node->thenBlock);
        }
        value_free(condition);
        return interpreter_node(interpreter, scope, node->elseBlock);
    }
    if (node->type == NODE_TRY) {
        Scope tryScope = {.exception = &(ExceptionScope){.exceptionValue = NULL}, .function = scope->function, .loop = scope->loop, .block = scope->block};
        interpreter_statement_in_try(interpreter, &tryScope, node->tryBlock, {});
        if (tryScope.exception->exceptionValue != NULL) {
            Scope catchScope = {.exception = scope->exception,
                                .function = scope->function,
                                .loop = scope->loop,
                                .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
            map_set(catchScope.block->env, node->catchVariable->lhs->string,
                    variable_new(node->catchVariable->declarationType, node->catchVariable->type == NODE_LET_ASSIGN,
                                 value_ref(tryScope.exception->exceptionValue)));
            interpreter_statement(interpreter, &catchScope, node->catchBlock, { map_free(catchScope.block->env, (MapFreeFunc *)variable_free); });
            map_free(catchScope.block->env, (MapFreeFunc *)variable_free);
            value_free(tryScope.exception->exceptionValue);
        }
        if (node->finallyBlock != NULL) {
            interpreter_statement(interpreter, scope, node->finallyBlock, {});
        }
        return NULL;
    }
    if (node->type == NODE_LOOP) {
        Scope loopScope = {.exception = scope->exception,
                           .function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = scope->block};
        for (;;) {
            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_WHILE) {
        Scope loopScope = {.exception = scope->exception,
                           .function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                ValueType conditionType = condition->type;
                value_free(condition);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return interpreter_throw(&context, type_error_exception(VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_DOWHILE) {
        Scope loopScope = {.exception = scope->exception,
                           .function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = scope->block};
        for (;;) {
            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }

            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                ValueType conditionType = condition->type;
                value_free(condition);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return interpreter_throw(&context, type_error_exception(VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);
        }
        return NULL;
    }
    if (node->type == NODE_FOR) {
        Scope loopScope = {.exception = scope->exception,
                           .function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                ValueType conditionType = condition->type;
                value_free(condition);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return interpreter_throw(&context, type_error_exception(VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }

            interpreter_statement(interpreter, &loopScope, node->incrementBlock, {});
        }
        return NULL;
    }
    if (node->type == NODE_FORIN) {
        Value *iterator = interpreter_node(interpreter, scope, node->iterator);
        if (iterator->type != VALUE_STRING && iterator->type != VALUE_ARRAY && iterator->type != VALUE_OBJECT && iterator->type != VALUE_CLASS &&
            iterator->type != VALUE_INSTANCE) {
            ValueType iteratorType = iterator->type;
            value_free(iterator);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->iterator};
            return interpreter_throw(
                &context, value_new_string_format("Variable is not a string, array, object, class or instance it is: %s", value_type_to_string(iteratorType)));
        }

        Scope loopScope = {.exception = scope->exception,
                           .function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
        size_t size;
        if (iterator->type == VALUE_STRING) {
            size = strlen(iterator->string);
        }
        if (iterator->type == VALUE_ARRAY) {
            size = iterator->array->size;
        }
        if (iterator->type == VALUE_OBJECT || iterator->type == VALUE_CLASS || iterator->type == VALUE_INSTANCE) {
            size = iterator->object->size;
        }

        for (size_t i = 0; i < size; i++) {
            Value *iteratorValue;
            if (iterator->type == VALUE_STRING) {
                char character[] = {iterator->string[i], '\0'};
                iteratorValue = value_new_string(character);
            }
            if (iterator->type == VALUE_ARRAY) {
                Value *value = list_get(iterator->array, i);
                iteratorValue = value != NULL ? value_retrieve(value) : value_new_null();
            }
            if (iterator->type == VALUE_OBJECT || iterator->type == VALUE_CLASS || iterator->type == VALUE_INSTANCE) {
                iteratorValue = value_new_string(iterator->object->keys[i]);
            }
            Variable *previousVariable = map_get(loopScope.block->env, node->forinVariable->lhs->string);
            if (previousVariable != NULL) {
                value_free(previousVariable->value);
                previousVariable->value = iteratorValue;
            } else {
                map_set(loopScope.block->env, node->forinVariable->lhs->string,
                        variable_new(node->forinVariable->declarationType, node->forinVariable->type == NODE_LET_ASSIGN, iteratorValue));
            }

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        map_free(loopScope.block->env, (MapFreeFunc *)variable_free);
        value_free(iterator);
        return NULL;
    }
    if (node->type == NODE_CONTINUE) {
        if (!scope->loop->inLoop) {
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string("Continue not in a loop"));
        }
        scope->loop->isContinuing = true;
        return NULL;
    }
    if (node->type == NODE_BREAK) {
        if (!scope->loop->inLoop) {
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string("Break not in a loop"));
        }
        scope->loop->isBreaking = true;
        return NULL;
    }
    if (node->type == NODE_RETURN) {
        scope->function->returnValue = interpreter_node(interpreter, scope, node->unary);
        return NULL;
    }
    if (node->type == NODE_THROW) {
        InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
        return interpreter_throw(&context, interpreter_node(interpreter, scope, node->unary));
    }
    if (node->type == NODE_INCLUDE) {
        Value *pathValue = interpreter_node(interpreter, scope, node->unary);
        if (pathValue->type != VALUE_STRING) {
            ValueType pathValueType = pathValue->type;
            value_free(pathValue);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
            return interpreter_throw(&context, type_error_exception(VALUE_STRING, pathValueType));
        }
        char includePath[255];
        if (strlen(node->token->source->dirname) > 0) {
            sprintf(includePath, "%s/%s", node->token->source->dirname, pathValue->string);
        } else {
            strcpy(includePath, pathValue->string);
        }
        value_free(pathValue);
        char *includeText = file_read(includePath);
        if (includeText == NULL) {
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
            return interpreter_throw(&context, value_new_string_format("Can't read file: %s", includePath));
        }

        List *tokens = lexer(includePath, includeText);
        Node *includeNode = parser(tokens, true);
        interpreter_statement(interpreter, scope, includeNode, {
            node_free(includeNode);
            list_free(tokens, (ListFreeFunc *)token_free);
            free(includeText);
        });
        node_free(includeNode);
        list_free(tokens, (ListFreeFunc *)token_free);
        free(includeText);
        return NULL;
    }

    if (node->type == NODE_VALUE) {
        return value_retrieve(node->value);
    }
    if (node->type == NODE_ARRAY) {
        Value *arrayValue = value_new_array(list_new_with_capacity(node->array->capacity));
        list_foreach(node->array, Node * item, { list_add(arrayValue->array, interpreter_node(interpreter, scope, item)); });
        return arrayValue;
    }
    if (node->type == NODE_OBJECT) {
        Value *objectValue = value_new_object(map_new_with_capacity(node->object->capacity));
        map_foreach(node->object, char *key, Node *value, { map_set(objectValue->object, key, interpreter_node(interpreter, scope, value)); });
        return objectValue;
    }
    if (node->type == NODE_CLASS) {
        Value *classValue = value_new_class(map_new_with_capacity(node->object->capacity),
                                            node->parentClass != NULL ? interpreter_node(interpreter, scope, node->parentClass) : NULL, node->abstract);
        map_foreach(node->object, char *key, Node *value, { map_set(classValue->object, key, interpreter_node(interpreter, scope, value)); });
        return classValue;
    }

    if (node->type == NODE_CONST_ASSIGN || node->type == NODE_LET_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get(scope->block->env, node->lhs->string) != NULL) {
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Can't redeclare variable: '%s'", node->lhs->string));
        }
        if (node->declarationType != VALUE_ANY && node->declarationType != rhs->type) {
            ValueType rhsType = rhs->type;
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("Unexpected variable type: '%s' needed '%s'", value_type_to_string(rhsType),
                                                                       value_type_to_string(node->declarationType)));
        }
        map_set(scope->block->env, node->lhs->string, variable_new(node->declarationType, node->type == NODE_LET_ASSIGN, value_retrieve(rhs)));
        return rhs;
    }
    if (node->type == NODE_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (node->lhs->type == NODE_GET) {
            Value *containerValue = interpreter_node(interpreter, scope, node->lhs->lhs);
            if (containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT && containerValue->type != VALUE_CLASS &&
                containerValue->type != VALUE_INSTANCE) {
                ValueType containerValueType = containerValue->type;
                value_free(rhs);
                value_free(containerValue);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return interpreter_throw(&context, value_new_string_format("Variable is not an array, object, class or instance it is: %s",
                                                                           value_type_to_string(containerValueType)));
            }

            Value *indexOrKey = interpreter_node(interpreter, scope, node->lhs->rhs);
            if (containerValue->type == VALUE_ARRAY) {
                if (indexOrKey->type != VALUE_INT) {
                    ValueType indexOrKeyType = indexOrKey->type;
                    value_free(rhs);
                    value_free(containerValue);
                    value_free(indexOrKey);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_INT, indexOrKeyType));
                }
                Value *previousValue = list_get(containerValue->array, indexOrKey->integer);
                if (previousValue != NULL) value_free(previousValue);
                list_set(containerValue->array, indexOrKey->integer, value_retrieve(rhs));
            }
            if (containerValue->type == VALUE_OBJECT || containerValue->type == VALUE_CLASS || containerValue->type == VALUE_INSTANCE) {
                if (indexOrKey->type != VALUE_STRING) {
                    ValueType indexOrKeyType = indexOrKey->type;
                    value_free(rhs);
                    value_free(containerValue);
                    value_free(indexOrKey);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_STRING, indexOrKeyType));
                }
                Value *previousValue = map_get(containerValue->object, indexOrKey->string);
                if (previousValue != NULL) value_free(previousValue);
                map_set(containerValue->object, indexOrKey->string, value_retrieve(rhs));
            }
            value_free(indexOrKey);
            value_free(containerValue);
            return rhs;
        }

        if (node->lhs->type != NODE_VARIABLE) {
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Is not a variable"));
        }
        Variable *variable = block_scope_get(scope->block, node->lhs->string);
        if (variable == NULL) {
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Variable: '%s' is not declared", node->lhs->string));
        }
        if (!variable->mutable) {
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Can't mutate const variable: '%s'", node->lhs->string));
        }
        if (variable->type != VALUE_ANY && variable->type != rhs->type) {
            ValueType rhsType = rhs->type;
            value_free(rhs);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, type_error_exception(variable->type, rhsType));
        }
        value_free(variable->value);
        variable->value = value_retrieve(rhs);
        return rhs;
    }

    if (node->type == NODE_VARIABLE) {
        Variable *variable = block_scope_get(scope->block, node->string);
        if (variable == NULL) {
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("Can't find variable: '%s'", node->string));
        }
        return value_retrieve(variable->value);
    }
    if (node->type == NODE_GET) {
        Value *containerValue = interpreter_node(interpreter, scope, node->lhs);
        if (containerValue->type != VALUE_STRING && containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT &&
            containerValue->type != VALUE_CLASS && containerValue->type != VALUE_INSTANCE) {
            ValueType containerValueType = containerValue->type;
            value_free(containerValue);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("Variable is not a string, array, object, class or instance it is: %s",
                                                                       value_type_to_string(containerValueType)));
        }

        Value *indexOrKey = interpreter_node(interpreter, scope, node->rhs);
        Value *returnValue = NULL;
        if (containerValue->type == VALUE_STRING) {
            if (indexOrKey->type == VALUE_STRING) {
                Value *stringClass = ((Variable *)map_get(interpreter->env, "String"))->value;
                Value *stringClassItem = map_get(stringClass->object, indexOrKey->string);
                if (stringClassItem != NULL) returnValue = value_retrieve(stringClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_INT) {
                    ValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_INT, indexOrKeyType));
                }
                if (indexOrKey->integer >= 0 && indexOrKey->integer <= (int64_t)strlen(containerValue->string)) {
                    char character[] = {containerValue->string[indexOrKey->integer], '\0'};
                    returnValue = value_new_string(character);
                } else {
                    returnValue = value_new_null();
                }
            }
        }
        if (containerValue->type == VALUE_ARRAY) {
            if (indexOrKey->type == VALUE_STRING) {
                Value *arrayClass = ((Variable *)map_get(interpreter->env, "Array"))->value;
                Value *arrayClassItem = map_get(arrayClass->object, indexOrKey->string);
                if (arrayClassItem != NULL) returnValue = value_retrieve(arrayClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_INT) {
                    ValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_INT, indexOrKeyType));
                }
                Value *value = list_get(containerValue->array, indexOrKey->integer);
                returnValue = value != NULL ? value_retrieve(value) : value_new_null();
            }
        }
        if (containerValue->type == VALUE_OBJECT || containerValue->type == VALUE_CLASS || containerValue->type == VALUE_INSTANCE) {
            if (containerValue->type == VALUE_OBJECT && indexOrKey->type == VALUE_STRING) {
                Value *objectClass = ((Variable *)map_get(interpreter->env, "Object"))->value;
                Value *objectClassItem = map_get(objectClass->object, indexOrKey->string);
                if (objectClassItem != NULL) returnValue = value_retrieve(objectClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_STRING) {
                    ValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_STRING, indexOrKeyType));
                }
                Value *value;
                if (containerValue->type == VALUE_INSTANCE) {
                    value = value_class_get(containerValue, indexOrKey->string);
                } else {
                    value = map_get(containerValue->object, indexOrKey->string);
                }
                if (value == NULL) {
                    char *indexOrKeyString = strdup(indexOrKey->string);
                    value_free(indexOrKey);
                    value_free(containerValue);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    Value *exception = value_new_string_format("Can't find %s in object", indexOrKeyString);
                    free(indexOrKeyString);
                    return interpreter_throw(&context, exception);
                }
                returnValue = value_retrieve(value);
            }
        }

        value_free(indexOrKey);
        value_free(containerValue);
        return returnValue;
    }
    if (node->type == NODE_CALL) {
        Value *thisValue = NULL;
        if (node->function->type == NODE_GET) {
            Value *containerValue = interpreter_node(interpreter, scope, node->function->lhs);
            if (containerValue->type == VALUE_STRING || containerValue->type == VALUE_ARRAY || containerValue->type == VALUE_OBJECT ||
                containerValue->type == VALUE_INSTANCE) {
                thisValue = containerValue;
            } else {
                value_free(containerValue);
            }
        }
        Value *callValue = interpreter_node(interpreter, scope, node->function);
        if (callValue->type != VALUE_FUNCTION && callValue->type != VALUE_NATIVE_FUNCTION && callValue->type != VALUE_CLASS) {
            ValueType callValueType = callValue->type;
            value_free(callValue);
            if (thisValue != NULL) value_free(thisValue);
            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
            return interpreter_throw(&context, value_new_string_format("Variable is not a function or a class but: %s", value_type_to_string(callValueType)));
        }

        List *callArguments = NULL;
        if (callValue->type == VALUE_CLASS) {
            if (callValue->abstract) {
                value_free(callValue);
                if (thisValue != NULL) value_free(thisValue);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
                return interpreter_throw(&context, value_new_string("Can't construct an abstract class"));
            }
            Value *constructorFunction = value_class_get(callValue, "constructor");
            if (constructorFunction != NULL) callArguments = constructorFunction->arguments;
        } else {
            callArguments = callValue->arguments;
        }
        List *arguments = list_new_with_capacity(node->nodes->capacity);
        for (size_t i = 0; i < MAX(callArguments != NULL ? callArguments->size : 0, node->nodes->size); i++) {
            Argument *argument = NULL;
            if (callArguments != NULL) {
                argument = list_get(callArguments, i);
                if (list_get(node->nodes, i) == NULL && argument != NULL) {
                    if (argument->defaultNode != NULL) {
                        Value *defaultValue = interpreter_node(interpreter, scope, argument->defaultNode);
                        if (argument->type != VALUE_ANY && defaultValue->type != argument->type) {
                            ValueType defaultValueType = defaultValue->type;
                            value_free(defaultValue);
                            list_free(arguments, (ListFreeFunc *)value_free);
                            value_free(callValue);
                            if (thisValue != NULL) value_free(thisValue);
                            InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = argument->defaultNode};
                            return interpreter_throw(&context, type_error_exception(argument->type, defaultValueType));
                        }
                        list_add(arguments, defaultValue);
                        continue;
                    }

                    list_free(arguments, (ListFreeFunc *)value_free);
                    value_free(callValue);
                    if (thisValue != NULL) value_free(thisValue);
                    InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    return interpreter_throw(&context, value_new_string("Not all function arguments are given"));
                }
            }

            Value *nodeValue = interpreter_node(interpreter, scope, list_get(node->nodes, i));
            if (argument != NULL && argument->type != VALUE_ANY && nodeValue->type != argument->type) {
                ValueType nodeValueType = nodeValue->type;
                value_free(nodeValue);
                list_free(arguments, (ListFreeFunc *)value_free);
                value_free(callValue);
                if (thisValue != NULL) value_free(thisValue);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return interpreter_throw(&context, type_error_exception(argument->type, nodeValueType));
            }
            list_add(arguments, nodeValue);
        }

        InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        Value *returnValue = interpreter_call(&context, callValue, thisValue, arguments);

        list_free(arguments, (ListFreeFunc *)value_free);
        value_free(callValue);
        if (thisValue != NULL) value_free(thisValue);
        return returnValue;
    }

    if (node->type >= NODE_NEG && node->type <= NODE_CAST) {
        Value *unary = interpreter_node(interpreter, scope, node->unary);
        if (node->type == NODE_NEG) {
            if (unary->type == VALUE_INT) {
                unary->integer = -unary->integer;
                return unary;
            }
            if (unary->type == VALUE_FLOAT) {
                unary->floating = -unary->floating;
                return unary;
            }
        }
        if (node->type == NODE_INC_PRE || node->type == NODE_DEC_PRE || node->type == NODE_INC_POST || node->type == NODE_DEC_POST) {
            if (node->unary->type != NODE_VARIABLE) {
                value_free(unary);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
                return interpreter_throw(&context, value_new_string_format("Is not a variable"));
            }
            Variable *variable = block_scope_get(scope->block, node->lhs->string);
            if (!variable->mutable) {
                value_free(unary);
                InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
                return interpreter_throw(&context, value_new_string_format("Can't mutate const variable: '%s'", node->lhs->string));
            }
            if (unary->type == VALUE_INT) {
                if (node->type == NODE_INC_PRE) unary->integer++;
                if (node->type == NODE_INC_PRE || node->type == NODE_INC_POST) variable->value->integer++;
                if (node->type == NODE_DEC_PRE) unary->integer--;
                if (node->type == NODE_DEC_PRE || node->type == NODE_DEC_POST) variable->value->integer--;
                return unary;
            }
            if (unary->type == VALUE_FLOAT) {
                if (node->type == NODE_INC_PRE) unary->floating++;
                if (node->type == NODE_INC_PRE || node->type == NODE_INC_POST) variable->value->floating++;
                if (node->type == NODE_DEC_PRE) unary->floating--;
                if (node->type == NODE_DEC_PRE || node->type == NODE_DEC_POST) variable->value->floating--;
                return unary;
            }
        }
        if (node->type == NODE_NOT) {
            if (unary->type == VALUE_INT) {
                unary->integer = ~unary->integer;
                return unary;
            }
        }
        if (node->type == NODE_LOGICAL_NOT) {
            if (unary->type == VALUE_BOOL) {
                unary->boolean = !unary->boolean;
                return unary;
            }
        }
        if (node->type == NODE_CAST) {
            if (node->castType == VALUE_BOOL) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = false;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) return unary;
                if (unary->type == VALUE_INT) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = unary->integer != 0;
                    return unary;
                }
                if (unary->type == VALUE_FLOAT) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = unary->floating != 0.0;
                    return unary;
                }
                if (unary->type == VALUE_STRING) {
                    bool result = !(!strcmp(unary->string, "") || !strcmp(unary->string, "0"));
                    value_clear(unary);
                    unary->type = VALUE_BOOL;
                    unary->boolean = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_INT) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_INT;
                    unary->integer = 0;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) {
                    unary->type = VALUE_INT;
                    unary->integer = unary->boolean;
                    return unary;
                }
                if (unary->type == VALUE_INT) return unary;
                if (unary->type == VALUE_FLOAT) {
                    unary->type = VALUE_INT;
                    unary->integer = unary->floating;
                    return unary;
                }
                if (unary->type == VALUE_STRING) {
                    int64_t result = string_to_int(unary->string);
                    value_clear(unary);
                    unary->type = VALUE_INT;
                    unary->integer = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_FLOAT) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = 0;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = unary->boolean;
                    return unary;
                }
                if (unary->type == VALUE_INT) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = unary->integer;
                    return unary;
                }
                if (unary->type == VALUE_FLOAT) return unary;
                if (unary->type == VALUE_STRING) {
                    int64_t result = string_to_float(unary->string);
                    value_clear(unary);
                    unary->type = VALUE_FLOAT;
                    unary->floating = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_STRING) {
                char *string = value_to_string(unary);
                value_clear(unary);
                unary->type = VALUE_STRING;
                unary->string = string;
                return unary;
            }
        }

        value_free(unary);
        InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return interpreter_throw(&context, value_new_string("Type error"));
    }

    if (node->type >= NODE_ADD && node->type <= NODE_LOGICAL_OR) {
        Value *lhs = interpreter_node(interpreter, scope, node->lhs);
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);

        if (node->type == NODE_ADD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer += rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating += rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating += rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer + rhs->floating;
                value_free(rhs);
                return lhs;
            }

            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                char *string = malloc(strlen(lhs->string) + strlen(rhs->string) + 1);
                strcpy(string, lhs->string);
                strcat(string, rhs->string);
                value_clear(lhs);
                lhs->string = string;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_SUB) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer -= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating -= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating -= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer - rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_MUL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer *= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating *= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating *= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer * rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_EXP) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer = pow(lhs->integer, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->floating);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->integer, rhs->floating);
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_DIV) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer = rhs->integer != 0 ? lhs->integer / rhs->integer : 0;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->floating / rhs->floating : 0;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->floating / rhs->integer : 0;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->integer / rhs->floating : 0;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_MOD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer %= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->floating);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->integer, rhs->floating);
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_AND) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer &= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_XOR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer ^= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_OR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer |= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_SHL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer <<= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_SHR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer >>= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_INSTANCEOF) {
            if (lhs->type == VALUE_INSTANCE && rhs->type == VALUE_CLASS) {
                bool result = value_class_instanceof(lhs, rhs);
                value_free(lhs);
                value_free(rhs);
                return value_new_bool(result);
            }
        }

        if (node->type == NODE_EQ) {
            if (lhs->type == VALUE_NULL) {
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = rhs->type == VALUE_NULL;
                value_free(rhs);
                return lhs;
            }
            if (rhs->type == VALUE_NULL) {
                bool result = lhs->type == VALUE_NULL;
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean == rhs->boolean;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                bool result = !strcmp(lhs->string, rhs->string);
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_NEQ) {
            if (lhs->type == VALUE_NULL) {
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = rhs->type != VALUE_NULL;
                value_free(rhs);
                return lhs;
            }
            if (rhs->type == VALUE_NULL) {
                bool result = lhs->type != VALUE_NULL;
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean != rhs->boolean;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                bool result = strcmp(lhs->string, rhs->string);
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_LT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_LTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_GT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_GTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_LOGICAL_AND) {
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean && rhs->boolean;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_LOGICAL_OR) {
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean || rhs->boolean;
                value_free(rhs);
                return lhs;
            }
        }

        value_free(lhs);
        value_free(rhs);
        InterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return interpreter_throw(&context, value_new_string("Type error"));
    }

    fprintf(stderr, "Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

#endif
