// New Bastiaan Language AST NblInterpreter
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

// Custom windows headers because NblTokenType name conflict :(
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

typedef struct NblToken NblToken;  // Forward define

void print_error(NblToken *token, char *fmt, ...);

// NblList header
typedef struct NblList {
    int32_t refs;
    void **items;
    size_t capacity;
    size_t size;
} NblList;

#define list_foreach(list, item, block)                   \
    for (size_t index = 0; index < list->size; index++) { \
        item = list_get(list, index);                     \
        block                                             \
    }

NblList *list_new(void);

NblList *list_new_with_capacity(size_t capacity);

NblList *list_ref(NblList *list);

void *list_get(NblList *list, size_t index);

void list_set(NblList *list, size_t index, void *item);

void list_add(NblList *list, void *item);

char *list_to_string(NblList *list);

typedef void NblListFreeFunc(void *item);

void list_free(NblList *list, NblListFreeFunc *freeFunc);

// NblMap header
typedef struct NblMap {
    int32_t refs;
    char **keys;
    void **values;
    size_t capacity;
    size_t size;
} NblMap;

#define map_foreach(map, key, value, block)              \
    for (size_t index = 0; index < map->size; index++) { \
        key = map->keys[index];                          \
        value = map->values[index];                      \
        block                                            \
    }

NblMap *map_new(void);

NblMap *map_new_with_capacity(size_t capacity);

NblMap *map_ref(NblMap *map);

void *map_get(NblMap *map, char *key);

void map_set(NblMap *map, char *key, void *item);

typedef void NblMapFreeFunc(void *item);

void map_free(NblMap *map, NblMapFreeFunc *freeFunc);

// Lexer header
typedef struct NblSource {
    int32_t refs;
    char *path;
    char *basename;
    char *dirname;
    char *text;
} NblSource;

NblSource *source_new(char *path, char *text);

NblSource *source_ref(NblSource *source);

void source_free(NblSource *source);

typedef enum NblTokenType {
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
} NblTokenType;

struct NblToken {
    int32_t refs;
    NblTokenType type;
    NblSource *source;
    int32_t line;
    int32_t column;
    union {
        int64_t integer;
        double floating;
        char *string;
    };
};

NblToken *token_new(NblTokenType type, NblSource *source, int32_t line, int32_t column);

NblToken *token_new_int(NblTokenType type, NblSource *source, int32_t line, int32_t column, int64_t integer);

NblToken *token_new_float(NblSource *source, int32_t line, int32_t column, double floating);

NblToken *token_new_string(NblTokenType type, NblSource *source, int32_t line, int32_t column, char *string);

NblToken *token_ref(NblToken *token);

bool token_type_is_type(NblTokenType type);

char *token_type_to_string(NblTokenType type);

void token_free(NblToken *token);

int64_t string_to_int(char *string);

double string_to_float(char *string);

typedef struct NblKeyword {
    char *keyword;
    NblTokenType type;
} NblKeyword;

NblList *lexer(char *path, char *text);

// NblValue
typedef struct NblNode NblNode;                              // Forward define
typedef struct NblInterpreterContext NblInterpreterContext;  // Forward define

typedef enum NblValueType {
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
} NblValueType;

typedef struct NblArgument {
    char *name;
    NblValueType type;
    NblNode *defaultNode;
} NblArgument;

NblArgument *argument_new(char *name, NblValueType type, NblNode *defaultNode);

void argument_free(NblArgument *argument);

typedef struct NblValue NblValue;

struct NblValue {
    int32_t refs;
    NblValueType type;
    union {
        bool boolean;
        int64_t integer;
        double floating;
        char *string;
        NblList *array;
        struct {
            NblMap *object;
            union {
                NblValue *parentClass;
                NblValue *instanceClass;
            };
            bool abstract;
        };
        struct {
            NblList *arguments;
            NblValueType returnType;
            union {
                NblNode *functionNode;
                NblValue *(*nativeFunc)(NblInterpreterContext *context, NblValue *this, NblList *values);
            };
        };
    };
};

NblValue *value_new(NblValueType type);

NblValue *value_new_null(void);

NblValue *value_new_bool(bool boolean);

NblValue *value_new_int(int64_t integer);

NblValue *value_new_float(double integer);

NblValue *value_new_string(char *string);

NblValue *value_new_string_format(char *format, ...);

NblValue *value_new_array(NblList *array);

NblValue *value_new_object(NblMap *object);

NblValue *value_new_class(NblMap *object, NblValue *parentClass, bool abstract);

NblValue *value_new_instance(NblMap *object, NblValue *instanceClass);

NblValue *value_new_function(NblList *args, NblValueType returnType, NblNode *node);

NblValue *value_new_native_function(NblList *args, NblValueType returnType,
                                    NblValue *(*nativeFunc)(NblInterpreterContext *context, NblValue *this, NblList *values));

char *value_type_to_string(NblValueType type);

NblValueType token_type_to_value_type(NblTokenType type);

char *value_to_string(NblValue *value);

NblValue *value_class_get(NblValue *instance, char *key);

bool value_class_instanceof(NblValue *instance, NblValue *class);

NblValue *value_ref(NblValue *value);

NblValue *value_retrieve(NblValue *value);

void value_clear(NblValue *value);

void value_free(NblValue *value);

// NblParser
typedef enum NblNodeType {
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
} NblNodeType;

struct NblNode {
    int32_t refs;
    NblNodeType type;
    NblToken *token;
    union {
        NblValue *value;
        char *string;
        NblList *array;
        struct {
            NblMap *object;
            NblNode *parentClass;
            bool abstract;
        };
        struct {
            NblValueType castType;
            NblNode *unary;
        };
        struct {
            NblValueType declarationType;
            NblNode *lhs;
            NblNode *rhs;
        };
        struct {
            union {
                NblNode *condition;
                NblNode *iterator;
                NblNode *catchVariable;
            };
            union {
                NblNode *thenBlock;
                NblNode *tryBlock;
            };
            union {
                NblNode *elseBlock;
                NblNode *incrementBlock;
                NblNode *forinVariable;
                NblNode *catchBlock;
            };
            NblNode *finallyBlock;
        };
        struct {
            NblNode *function;
            NblList *keys;
            NblList *nodes;
        };
    };
};

NblNode *node_new(NblNodeType type, NblToken *token);

NblNode *node_new_value(NblToken *token, NblValue *value);

NblNode *node_new_string(NblNodeType type, NblToken *token, char *string);

NblNode *node_new_unary(NblNodeType type, NblToken *token, NblNode *unary);

NblNode *node_new_cast(NblToken *token, NblValueType castType, NblNode *unary);

NblNode *node_new_operation(NblNodeType type, NblToken *token, NblNode *lhs, NblNode *rhs);

NblNode *node_new_multiple(NblNodeType type, NblToken *token);

NblNode *node_ref(NblNode *node);

void node_free(NblNode *node);

typedef struct NblParser {
    NblList *tokens;
    int32_t position;
} NblParser;

NblNode *parser(NblList *tokens, bool included);

void parser_eat(NblParser *parser, NblTokenType type);

NblValueType parser_eat_type(NblParser *parser);

NblNode *parser_program(NblParser *parser, bool included);
NblNode *parser_block(NblParser *parser);
NblNode *parser_statement(NblParser *parser);
NblNode *parser_declarations(NblParser *parser);
NblNode *parser_assigns(NblParser *parser);
NblNode *parser_assign(NblParser *parser);
NblNode *parser_tenary(NblParser *parser);
NblNode *parser_logical(NblParser *parser);
NblNode *parser_equality(NblParser *parser);
NblNode *parser_relational(NblParser *parser);
NblNode *parser_instanceof(NblParser *parser);
NblNode *parser_bitwise(NblParser *parser);
NblNode *parser_shift(NblParser *parser);
NblNode *parser_add(NblParser *parser);
NblNode *parser_mul(NblParser *parser);
NblNode *parser_unary(NblParser *parser);
NblNode *parser_primary(NblParser *parser);
NblNode *parser_primary_suffix(NblParser *parser, NblNode *node);
NblNode *parser_function(NblParser *parser, NblToken *token);
NblNode *parser_class(NblParser *parser, NblToken *token, bool abstract);
NblArgument *parser_argument(NblParser *parser);

// Standard library
NblMap *std_env(void);

// NblInterpreter
typedef struct NblVariable {
    NblValueType type;
    bool mutable;
    NblValue *value;
} NblVariable;

NblVariable *variable_new(NblValueType type, bool mutable, NblValue *value);

void variable_free(NblVariable *variable);

typedef struct NblExceptionScope {
    NblValue *exceptionValue;
} NblExceptionScope;

typedef struct NblFunctionScope {
    NblValue *returnValue;
} NblFunctionScope;

typedef struct NblLoopScope {
    bool inLoop;
    bool isContinuing;
    bool isBreaking;
} NblLoopScope;

typedef struct NblBlockScope NblBlockScope;

struct NblBlockScope {
    NblBlockScope *parentBlock;
    NblMap *env;
};

typedef struct NblScope {
    NblExceptionScope *exception;
    NblFunctionScope *function;
    NblLoopScope *loop;
    NblBlockScope *block;
} NblScope;

typedef struct NblInterpreter {
    NblMap *env;
} NblInterpreter;

struct NblInterpreterContext {
    NblMap *env;
    NblScope *scope;
    NblNode *node;
};

NblVariable *block_scope_get(NblBlockScope *block, char *key);

NblValue *interpreter(NblMap *env, NblNode *node);

NblValue *type_error_exception(NblValueType expected, NblValueType got);

NblValue *interpreter_call(NblInterpreterContext *context, NblValue *callValue, NblValue *this, NblList *arguments);

NblValue *interpreter_throw(NblInterpreterContext *context, NblValue *exception);

NblValue *interpreter_node(NblInterpreter *interpreter, NblScope *scope, NblNode *node);

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

void print_error(NblToken *token, char *fmt, ...) {
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

// NblList
NblList *list_new(void) { return list_new_with_capacity(8); }

NblList *list_new_with_capacity(size_t capacity) {
    NblList *list = malloc(sizeof(NblList));
    list->refs = 1;
    list->items = malloc(sizeof(void *) * capacity);
    list->capacity = capacity;
    list->size = 0;
    return list;
}

NblList *list_ref(NblList *list) {
    list->refs++;
    return list;
}

void *list_get(NblList *list, size_t index) {
    if (index < list->size) {
        return list->items[index];
    }
    return NULL;
}

void list_set(NblList *list, size_t index, void *item) {
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

void list_add(NblList *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

char *list_to_string(NblList *list) {
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

void list_free(NblList *list, NblListFreeFunc *freeFunc) {
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

// NblMap
NblMap *map_new(void) { return map_new_with_capacity(8); }

NblMap *map_new_with_capacity(size_t capacity) {
    NblMap *map = malloc(sizeof(NblMap));
    map->refs = 1;
    map->keys = malloc(sizeof(char *) * capacity);
    map->values = malloc(sizeof(void *) * capacity);
    map->capacity = capacity;
    map->size = 0;
    return map;
}

NblMap *map_ref(NblMap *map) {
    map->refs++;
    return map;
}

void *map_get(NblMap *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->values[i];
        }
    }
    return NULL;
}

void map_set(NblMap *map, char *key, void *item) {
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

void map_free(NblMap *map, NblMapFreeFunc *freeFunc) {
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
NblSource *source_new(char *path, char *text) {
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

NblSource *source_ref(NblSource *source) {
    source->refs++;
    return source;
}

void source_free(NblSource *source) {
    source->refs--;
    if (source->refs > 0) return;

    free(source->path);
    free(source->text);
    free(source->dirname);
    free(source);
}

NblToken *token_new(NblTokenType type, NblSource *source, int32_t line, int32_t column) {
    NblToken *token = malloc(sizeof(NblToken));
    token->refs = 1;
    token->source = source_ref(source);
    token->type = type;
    token->line = line;
    token->column = column;
    return token;
}

NblToken *token_new_int(NblTokenType type, NblSource *source, int32_t line, int32_t column, int64_t integer) {
    NblToken *token = token_new(type, source, line, column);
    token->integer = integer;
    return token;
}

NblToken *token_new_float(NblSource *source, int32_t line, int32_t column, double floating) {
    NblToken *token = token_new(TOKEN_FLOAT, source, line, column);
    token->floating = floating;
    return token;
}

NblToken *token_new_string(NblTokenType type, NblSource *source, int32_t line, int32_t column, char *string) {
    NblToken *token = token_new(type, source, line, column);
    token->string = string;
    return token;
}

NblToken *token_ref(NblToken *token) {
    token->refs++;
    return token;
}

bool token_type_is_type(NblTokenType type) {
    return type == TOKEN_TYPE_ANY || type == TOKEN_NULL || type == TOKEN_TYPE_BOOL || type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT ||
           type == TOKEN_TYPE_STRING || type == TOKEN_TYPE_ARRAY || type == TOKEN_TYPE_OBJECT || type == TOKEN_TYPE_FUNCTION || type == TOKEN_CLASS ||
           type == TOKEN_TYPE_INSTANCE;
}

char *token_type_to_string(NblTokenType type) {
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

void token_free(NblToken *token) {
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

NblList *lexer(char *path, char *text) {
    NblKeyword keywords[] = {{"instanceof", TOKEN_INSTANCEOF},
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

    NblSource *source = source_new(path, text);
    NblList *tokens = list_new_with_capacity(512);
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
            for (size_t i = 0; i < sizeof(keywords) / sizeof(NblKeyword); i++) {
                NblKeyword *keyword = &keywords[i];
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

// NblValue
NblArgument *argument_new(char *name, NblValueType type, NblNode *defaultNode) {
    NblArgument *argument = malloc(sizeof(NblArgument));
    argument->name = strdup(name);
    argument->type = type;
    argument->defaultNode = defaultNode;
    return argument;
}

void argument_free(NblArgument *argument) {
    free(argument->name);
    if (argument->defaultNode != NULL) {
        node_free(argument->defaultNode);
    }
    free(argument);
}

NblValue *value_new(NblValueType type) {
    NblValue *value = malloc(sizeof(NblValue));
    value->refs = 1;
    value->type = type;
    return value;
}

NblValue *value_new_null(void) { return value_new(VALUE_NULL); }

NblValue *value_new_bool(bool boolean) {
    NblValue *value = value_new(VALUE_BOOL);
    value->boolean = boolean;
    return value;
}

NblValue *value_new_int(int64_t integer) {
    NblValue *value = value_new(VALUE_INT);
    value->integer = integer;
    return value;
}

NblValue *value_new_float(double floating) {
    NblValue *value = value_new(VALUE_FLOAT);
    value->floating = floating;
    return value;
}

NblValue *value_new_string(char *string) {
    NblValue *value = value_new(VALUE_STRING);
    value->string = strdup(string);
    return value;
}

NblValue *value_new_string_format(char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return value_new_string(buffer);
}

NblValue *value_new_array(NblList *array) {
    NblValue *value = value_new(VALUE_ARRAY);
    value->array = array;
    return value;
}

NblValue *value_new_object(NblMap *object) {
    NblValue *value = value_new(VALUE_OBJECT);
    value->object = object;
    return value;
}

NblValue *value_new_class(NblMap *object, NblValue *parentClass, bool abstract) {
    NblValue *value = value_new(VALUE_CLASS);
    value->object = object;
    value->parentClass = parentClass;
    value->abstract = abstract;
    return value;
}

NblValue *value_new_instance(NblMap *object, NblValue *instanceClass) {
    NblValue *value = value_new(VALUE_INSTANCE);
    value->object = object;
    value->instanceClass = instanceClass;
    return value;
}

NblValue *value_new_function(NblList *args, NblValueType returnType, NblNode *functionNode) {
    NblValue *value = value_new(VALUE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->functionNode = functionNode;
    return value;
}

NblValue *value_new_native_function(NblList *args, NblValueType returnType,
                                    NblValue *(*nativeFunc)(NblInterpreterContext *context, NblValue *this, NblList *values)) {
    NblValue *value = value_new(VALUE_NATIVE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->nativeFunc = nativeFunc;
    return value;
}

char *value_type_to_string(NblValueType type) {
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

char *value_to_string(NblValue *value) {
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
        NblList *sb = list_new();
        list_add(sb, strdup("["));
        if (value->array->size > 0) list_add(sb, strdup(" "));
        for (size_t i = 0; i < value->array->size; i++) {
            NblValue *item = list_get(value->array, i);
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
        NblList *sb = list_new();
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
        NblList *sb = list_new();
        list_add(sb, "fn (");
        for (size_t i = 0; i < value->arguments->size; i++) {
            NblArgument *argument = list_get(value->arguments, i);
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

NblValueType token_type_to_value_type(NblTokenType type) {
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

NblValue *value_class_get(NblValue *instance, char *key) {
    NblValue *value = map_get(instance->object, key);
    if (value != NULL) {
        return value;
    }
    if (instance->parentClass != NULL) {
        return value_class_get(instance->parentClass, key);
    }
    return NULL;
}

bool value_class_instanceof(NblValue *instance, NblValue *class) {
    bool result = instance->instanceClass == class;
    if (result) {
        return true;
    }
    if (instance->instanceClass->parentClass != NULL) {
        return value_class_instanceof(instance->instanceClass, class);
    }
    return false;
}

NblValue *value_ref(NblValue *value) {
    value->refs++;
    return value;
}

NblValue *value_retrieve(NblValue *value) {
    if (value->type == VALUE_NULL) return value_new_null();
    if (value->type == VALUE_BOOL) return value_new_bool(value->boolean);
    if (value->type == VALUE_INT) return value_new_int(value->integer);
    if (value->type == VALUE_FLOAT) return value_new_float(value->floating);
    if (value->type == VALUE_STRING) return value_new_string(value->string);
    return value_ref(value);
}

void value_clear(NblValue *value) {
    if (value->type == VALUE_STRING) {
        free(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        list_free(value->array, (NblListFreeFunc *)value_free);
    }
    if (value->type == VALUE_OBJECT || value->type == VALUE_CLASS || value->type == VALUE_INSTANCE) {
        map_free(value->object, (NblMapFreeFunc *)value_free);
        if ((value->type == VALUE_CLASS || value->type == VALUE_INSTANCE) && value->parentClass != NULL) {
            value_free(value->parentClass);
        }
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        list_free(value->arguments, (NblListFreeFunc *)argument_free);
    }
    if (value->type == VALUE_FUNCTION) {
        node_free(value->functionNode);
    }
}

void value_free(NblValue *value) {
    value->refs--;
    if (value->refs > 0) return;
    value_clear(value);
    free(value);
}

// NblParser
NblNode *node_new(NblNodeType type, NblToken *token) {
    NblNode *node = malloc(sizeof(NblNode));
    node->refs = 1;
    node->type = type;
    if (token != NULL) {
        node->token = token_ref(token);
    }
    return node;
}

NblNode *node_new_value(NblToken *token, NblValue *value) {
    NblNode *node = node_new(NODE_VALUE, token);
    node->value = value;
    return node;
}

NblNode *node_new_string(NblNodeType type, NblToken *token, char *string) {
    NblNode *node = node_new(type, token);
    node->string = strdup(string);
    return node;
}

NblNode *node_new_unary(NblNodeType type, NblToken *token, NblNode *unary) {
    NblNode *node = node_new(type, token);
    node->unary = unary;
    return node;
}

NblNode *node_new_cast(NblToken *token, NblValueType castType, NblNode *unary) {
    NblNode *node = node_new(NODE_CAST, token);
    node->castType = castType;
    node->unary = unary;
    return node;
}

NblNode *node_new_operation(NblNodeType type, NblToken *token, NblNode *lhs, NblNode *rhs) {
    NblNode *node = node_new(type, token);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

NblNode *node_new_multiple(NblNodeType type, NblToken *token) {
    NblNode *node = node_new(type, token);
    node->nodes = list_new();
    return node;
}

NblNode *node_ref(NblNode *node) {
    node->refs++;
    return node;
}

void node_free(NblNode *node) {
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
        list_free(node->array, (NblListFreeFunc *)node_free);
    }
    if (node->type == NODE_OBJECT || node->type == NODE_CLASS) {
        map_free(node->object, (NblMapFreeFunc *)node_free);
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
        list_free(node->nodes, (NblListFreeFunc *)node_free);
    }
    free(node);
}

NblNode *parser(NblList *tokens, bool included) {
    NblParser parser = {.tokens = tokens, .position = 0};
    return parser_program(&parser, included);
}

#define current() ((NblToken *)list_get(parser->tokens, parser->position))
#define next(pos) ((NblToken *)list_get(parser->tokens, parser->position + 1 + pos))

void parser_eat(NblParser *parser, NblTokenType type) {
    if (current()->type == type) {
        parser->position++;
    } else {
        print_error(current(), "Unexpected token: '%s' needed '%s'", token_type_to_string(current()->type), token_type_to_string(type));
        exit(EXIT_FAILURE);
    }
}

NblValueType parser_eat_type(NblParser *parser) {
    if (token_type_is_type(current()->type)) {
        NblValueType type = token_type_to_value_type(current()->type);
        parser->position++;
        return type;
    }
    print_error(current(), "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
    return VALUE_ANY;
}

NblNode *parser_program(NblParser *parser, bool included) {
    NblNode *programNode = node_new_multiple(included ? NODE_NODES : NODE_PROGRAM, current());
    while (current()->type != TOKEN_EOF) {
        NblNode *node = parser_statement(parser);
        if (node != NULL) list_add(programNode->nodes, node);
    }
    return programNode;
}

NblNode *parser_block(NblParser *parser) {
    NblNode *blockNode = node_new_multiple(NODE_BLOCK, current());
    if (current()->type == TOKEN_LCURLY) {
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            NblNode *node = parser_statement(parser);
            if (node != NULL) list_add(blockNode->nodes, node);
        }
        parser_eat(parser, TOKEN_RCURLY);
    } else {
        NblNode *node = parser_statement(parser);
        if (node != NULL) list_add(blockNode->nodes, node);
    }
    return blockNode;
}

NblNode *parser_statement(NblParser *parser) {
    if (current()->type == TOKEN_SEMICOLON) {
        parser_eat(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    if (current()->type == TOKEN_LCURLY) {
        return parser_block(parser);
    }

    if (current()->type == TOKEN_IF) {
        NblNode *node = node_new(NODE_IF, current());
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
        NblNode *node = node_new(NODE_TRY, current());
        parser_eat(parser, TOKEN_TRY);
        node->tryBlock = parser_block(parser);

        parser_eat(parser, TOKEN_CATCH);
        parser_eat(parser, TOKEN_LPAREN);
        NblNode *declarations = parser_declarations(parser);
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
        NblNode *node = node_new(NODE_LOOP, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_LOOP);
        node->condition = NULL;
        node->thenBlock = parser_block(parser);
        return node;
    }

    if (current()->type == TOKEN_WHILE) {
        NblNode *node = node_new(NODE_WHILE, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_WHILE);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        node->thenBlock = parser_block(parser);
        return node;
    }

    if (current()->type == TOKEN_DO) {
        NblNode *node = node_new(NODE_DOWHILE, current());
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
        NblToken *token = current();
        parser_eat(parser, TOKEN_FOR);
        parser_eat(parser, TOKEN_LPAREN);
        NblNode *declarations;
        if (current()->type != TOKEN_SEMICOLON) {
            declarations = parser_declarations(parser);
        }

        if (current()->type == TOKEN_IN) {
            NblNode *node = node_new(NODE_FORIN, token);
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

        NblNode *blockNode = node_new_multiple(NODE_BLOCK, token);
        list_add(blockNode->nodes, declarations);

        NblNode *node = node_new(NODE_FOR, token);
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
        NblToken *token = current();
        parser_eat(parser, TOKEN_CONTINUE);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_CONTINUE, token);
    }
    if (current()->type == TOKEN_BREAK) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_BREAK);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_BREAK, token);
    }
    if (current()->type == TOKEN_RETURN) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_RETURN);
        NblNode *node = node_new_unary(NODE_RETURN, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == TOKEN_THROW) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_THROW);
        NblNode *node = node_new_unary(NODE_THROW, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == TOKEN_INCLUDE) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_INCLUDE);
        NblNode *node = node_new_unary(NODE_INCLUDE, token, parser_tenary(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FUNCTION) {
        NblToken *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        NblToken *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        NblNode *node =
            node_new_operation(NODE_CONST_ASSIGN, functionToken, node_new_string(NODE_VARIABLE, nameToken, name), parser_function(parser, functionToken));
        node->declarationType = VALUE_FUNCTION;
        return node;
    }
    if (current()->type == TOKEN_ABSTRACT || current()->type == TOKEN_CLASS) {
        NblToken *classToken = current();
        bool abstract = false;
        if (current()->type == TOKEN_ABSTRACT) {
            abstract = true;
            parser_eat(parser, TOKEN_ABSTRACT);
        }
        parser_eat(parser, TOKEN_CLASS);
        NblToken *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        NblNode *node =
            node_new_operation(NODE_CONST_ASSIGN, classToken, node_new_string(NODE_VARIABLE, nameToken, name), parser_class(parser, classToken, abstract));
        node->declarationType = VALUE_CLASS;
        return node;
    }

    NblNode *node = parser_declarations(parser);
    parser_eat(parser, TOKEN_SEMICOLON);
    return node;
}

NblNode *parser_declarations(NblParser *parser) {
    if (current()->type == TOKEN_CONST || current()->type == TOKEN_LET) {
        NblList *nodes = list_new();
        NblNodeType assignType;
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
            NblNode *variable = node_new_string(NODE_VARIABLE, current(), name);

            NblValueType declarationType = VALUE_ANY;
            if (current()->type == TOKEN_COLON) {
                parser_eat(parser, TOKEN_COLON);
                declarationType = parser_eat_type(parser);
            }

            NblNode *node;
            if (current()->type == TOKEN_ASSIGN) {
                NblToken *token = current();
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
            NblNode *firstNode = list_get(nodes, 0);
            list_free(nodes, NULL);
            return firstNode;
        }
        NblNode *nodesNode = node_new(NODE_NODES, current());
        nodesNode->nodes = nodes;
        return nodesNode;
    }
    return parser_assigns(parser);
}

NblNode *parser_assigns(NblParser *parser) {
    NblList *nodes = list_new();
    for (;;) {
        NblNode *node = parser_assign(parser);
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
        NblNode *firstNode = list_get(nodes, 0);
        list_free(nodes, NULL);
        return firstNode;
    }
    NblNode *nodesNode = node_new(NODE_NODES, current());
    nodesNode->nodes = nodes;
    return nodesNode;
}

NblNode *parser_assign(NblParser *parser) {
    NblNode *lhs = parser_tenary(parser);  // TODO
    if (current()->type == TOKEN_ASSIGN) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN);
        return node_new_operation(NODE_ASSIGN, token, lhs, parser_assign(parser));
    }
    if (current()->type == TOKEN_ASSIGN_ADD) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_ADD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_ADD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SUB) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SUB);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SUB, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MUL) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MUL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MUL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_EXP) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_EXP);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_EXP, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MOD) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MOD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MOD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_AND) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_AND);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_AND, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_XOR) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_XOR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_XOR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_OR) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_OR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_OR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHL) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHR) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHR, token, node_ref(lhs), parser_assign(parser)));
    }
    return lhs;
}

NblNode *parser_tenary(NblParser *parser) {
    NblNode *node = parser_logical(parser);
    if (current()->type == TOKEN_QUESTION) {
        NblNode *tenaryNode = node_new(NODE_TENARY, current());
        parser_eat(parser, TOKEN_QUESTION);
        tenaryNode->condition = node;
        tenaryNode->thenBlock = parser_tenary(parser);
        parser_eat(parser, TOKEN_COLON);
        tenaryNode->elseBlock = parser_tenary(parser);
        return tenaryNode;
    }
    return node;
}

NblNode *parser_logical(NblParser *parser) {
    NblNode *node = parser_equality(parser);
    while (current()->type == TOKEN_LOGICAL_AND || current()->type == TOKEN_LOGICAL_OR) {
        if (current()->type == TOKEN_LOGICAL_AND) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_LOGICAL_AND);
            node = node_new_operation(NODE_LOGICAL_AND, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_LOGICAL_OR) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_LOGICAL_OR);
            node = node_new_operation(NODE_LOGICAL_OR, token, node, parser_equality(parser));
        }
    }
    return node;
}

NblNode *parser_equality(NblParser *parser) {
    NblNode *node = parser_relational(parser);
    while (current()->type == TOKEN_EQ || current()->type == TOKEN_NEQ) {
        if (current()->type == TOKEN_EQ) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_EQ);
            node = node_new_operation(NODE_EQ, token, node, parser_relational(parser));
        }
        if (current()->type == TOKEN_NEQ) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_NEQ);
            node = node_new_operation(NODE_NEQ, token, node, parser_relational(parser));
        }
    }
    return node;
}

NblNode *parser_relational(NblParser *parser) {
    NblNode *node = parser_instanceof(parser);
    while (current()->type == TOKEN_LT || current()->type == TOKEN_LTEQ || current()->type == TOKEN_GT || current()->type == TOKEN_GTEQ) {
        if (current()->type == TOKEN_LT) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_LT);
            node = node_new_operation(NODE_LT, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_LTEQ) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_LTEQ);
            node = node_new_operation(NODE_LTEQ, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_GT) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_GT);
            node = node_new_operation(NODE_GT, token, node, parser_instanceof(parser));
        }
        if (current()->type == TOKEN_GTEQ) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_GTEQ);
            node = node_new_operation(NODE_GTEQ, token, node, parser_instanceof(parser));
        }
    }
    return node;
}

NblNode *parser_instanceof(NblParser *parser) {
    NblNode *node = parser_bitwise(parser);
    while (current()->type == TOKEN_INSTANCEOF) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_INSTANCEOF);
        node = node_new_operation(NODE_INSTANCEOF, token, node, parser_bitwise(parser));
    }
    return node;
}

NblNode *parser_bitwise(NblParser *parser) {
    NblNode *node = parser_shift(parser);
    while (current()->type == TOKEN_AND || current()->type == TOKEN_XOR || current()->type == TOKEN_OR) {
        if (current()->type == TOKEN_AND) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_AND);
            node = node_new_operation(NODE_AND, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_XOR) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_XOR);
            node = node_new_operation(NODE_XOR, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_OR) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_OR);
            node = node_new_operation(NODE_OR, token, node, parser_shift(parser));
        }
    }
    return node;
}

NblNode *parser_shift(NblParser *parser) {
    NblNode *node = parser_add(parser);
    while (current()->type == TOKEN_SHL || current()->type == TOKEN_SHR) {
        if (current()->type == TOKEN_SHL) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_SHL);
            node = node_new_operation(NODE_SHL, token, node, parser_add(parser));
        }

        if (current()->type == TOKEN_SHR) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_SHR);
            node = node_new_operation(NODE_SHR, token, node, parser_add(parser));
        }
    }
    return node;
}

NblNode *parser_add(NblParser *parser) {
    NblNode *node = parser_mul(parser);
    while (current()->type == TOKEN_ADD || current()->type == TOKEN_SUB) {
        if (current()->type == TOKEN_ADD) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_ADD);
            node = node_new_operation(NODE_ADD, token, node, parser_mul(parser));
        }

        if (current()->type == TOKEN_SUB) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_SUB);
            node = node_new_operation(NODE_SUB, token, node, parser_mul(parser));
        }
    }
    return node;
}

NblNode *parser_mul(NblParser *parser) {
    NblNode *node = parser_unary(parser);
    while (current()->type == TOKEN_MUL || current()->type == TOKEN_EXP || current()->type == TOKEN_DIV || current()->type == TOKEN_MOD) {
        if (current()->type == TOKEN_MUL) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_MUL);
            node = node_new_operation(NODE_MUL, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_EXP) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_EXP);
            node = node_new_operation(NODE_EXP, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_DIV) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_DIV);
            node = node_new_operation(NODE_DIV, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_MOD) {
            NblToken *token = current();
            parser_eat(parser, TOKEN_MOD);
            node = node_new_operation(NODE_MOD, token, node, parser_unary(parser));
        }
    }
    return node;
}

NblNode *parser_unary(NblParser *parser) {
    if (current()->type == TOKEN_ADD) {
        parser_eat(parser, TOKEN_ADD);
        return parser_unary(parser);
    }
    if (current()->type == TOKEN_SUB) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_SUB);
        return node_new_unary(NODE_NEG, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_INC) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_INC);
        return node_new_unary(NODE_INC_PRE, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_DEC) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_DEC);
        return node_new_unary(NODE_DEC_PRE, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_NOT) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_NOT);
        return node_new_unary(NODE_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LOGICAL_NOT) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_LOGICAL_NOT);
        return node_new_unary(NODE_LOGICAL_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LPAREN && token_type_is_type(next(0)->type) && next(1)->type == TOKEN_RPAREN) {
        NblToken *token = current();
        parser_eat(parser, TOKEN_LPAREN);
        NblValueType castType = parser_eat_type(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return node_new_cast(token, castType, parser_unary(parser));
    }
    return parser_primary(parser);
}

NblNode *parser_primary(NblParser *parser) {
    if (current()->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        NblNode *node = parser_tenary(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_NULL) {
        NblNode *node = node_new_value(current(), value_new(VALUE_NULL));
        parser_eat(parser, TOKEN_NULL);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_TRUE) {
        NblNode *node = node_new_value(current(), value_new_bool(true));
        parser_eat(parser, TOKEN_TRUE);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_FALSE) {
        NblNode *node = node_new_value(current(), value_new_bool(false));
        parser_eat(parser, TOKEN_FALSE);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_INT) {
        NblNode *node = node_new_value(current(), value_new_int(current()->integer));
        parser_eat(parser, TOKEN_INT);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_FLOAT) {
        NblNode *node = node_new_value(current(), value_new_float(current()->floating));
        parser_eat(parser, TOKEN_FLOAT);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_STRING) {
        NblNode *node = node_new_value(current(), value_new_string(current()->string));
        parser_eat(parser, TOKEN_STRING);
        return parser_primary_suffix(parser, node);
    }
    if (current()->type == TOKEN_KEYWORD) {
        NblToken *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);
        return parser_primary_suffix(parser, node_new_string(NODE_VARIABLE, nameToken, name));
    }
    if (current()->type == TOKEN_LBRACKET) {
        NblNode *node = node_new(NODE_ARRAY, current());
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
        NblNode *node = node_new(NODE_OBJECT, current());
        node->object = map_new();
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            if (current()->type == TOKEN_FUNCTION) {
                NblToken *functionToken = current();
                parser_eat(parser, TOKEN_FUNCTION);
                char *keyName = current()->string;
                parser_eat(parser, TOKEN_KEYWORD);
                map_set(node->object, keyName, parser_function(parser, functionToken));
                continue;
            }

            NblToken *keyToken = current();
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
        NblToken *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        return parser_primary_suffix(parser, parser_function(parser, functionToken));
    }
    if (current()->type == TOKEN_ABSTRACT || current()->type == TOKEN_CLASS) {
        NblToken *classToken = current();
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

NblNode *parser_primary_suffix(NblParser *parser, NblNode *node) {
    while (current()->type == TOKEN_LBRACKET || current()->type == TOKEN_POINT || current()->type == TOKEN_LPAREN || current()->type == TOKEN_INC ||
           current()->type == TOKEN_DEC) {
        NblToken *token = current();
        if (current()->type == TOKEN_LBRACKET) {
            parser_eat(parser, TOKEN_LBRACKET);
            NblNode *indexOrKey = parser_assign(parser);
            parser_eat(parser, TOKEN_RBRACKET);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
        if (current()->type == TOKEN_POINT) {
            parser_eat(parser, TOKEN_POINT);
            NblToken *keyToken = current();
            char *key = current()->string;
            parser_eat(parser, TOKEN_KEYWORD);
            node = node_new_operation(NODE_GET, token, node, node_new_value(keyToken, value_new_string(key)));
        }
        if (current()->type == TOKEN_LPAREN) {
            NblNode *callNode = node_new_multiple(NODE_CALL, current());
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

NblNode *parser_function(NblParser *parser, NblToken *token) {
    NblList *arguments = list_new();
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

    NblValueType returnType = VALUE_ANY;
    if (current()->type == TOKEN_COLON) {
        parser_eat(parser, TOKEN_COLON);
        returnType = parser_eat_type(parser);
    }

    if (current()->type == TOKEN_FAT_ARROW) {
        NblToken *fatArrowToken = current();
        parser_eat(parser, TOKEN_FAT_ARROW);
        return node_new_value(token, value_new_function(arguments, returnType, node_new_unary(NODE_RETURN, fatArrowToken, parser_tenary(parser))));
    }
    return node_new_value(token, value_new_function(arguments, returnType, parser_block(parser)));
}

NblNode *parser_class(NblParser *parser, NblToken *token, bool abstract) {
    NblNode *parentClass = NULL;
    if (current()->type == TOKEN_EXTENDS) {
        parser_eat(parser, TOKEN_EXTENDS);
        parentClass = parser_tenary(parser);
    }

    NblMap *object = map_new();
    parser_eat(parser, TOKEN_LCURLY);
    while (current()->type != TOKEN_RCURLY) {
        if (current()->type == TOKEN_FUNCTION) {
            NblToken *functionToken = current();
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

    NblNode *classNode = node_new(NODE_CLASS, token);
    classNode->object = object;
    classNode->parentClass = parentClass;
    classNode->abstract = abstract;
    return classNode;
}

NblArgument *parser_argument(NblParser *parser) {
    char *name = current()->string;
    parser_eat(parser, TOKEN_KEYWORD);
    NblValueType type = VALUE_ANY;
    if (current()->type == TOKEN_COLON) {
        parser_eat(parser, TOKEN_COLON);
        type = parser_eat_type(parser);
    }
    NblNode *defaultNode = NULL;
    if (current()->type == TOKEN_ASSIGN) {
        parser_eat(parser, TOKEN_ASSIGN);
        defaultNode = parser_tenary(parser);
    }
    return argument_new(name, type, defaultNode);
}

// Standard library

// Math
static NblValue *env_math_abs(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    if (x->type == VALUE_INT) {
        return value_new_int(x->integer < 0 ? -x->integer : x->integer);
    }
    if (x->type == VALUE_FLOAT) {
        return value_new_float(x->floating < 0 ? -x->floating : x->floating);
    }
    return value_new_null();
}
static NblValue *env_math_sin(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(sin(x->floating));
}
static NblValue *env_math_cos(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(cos(x->floating));
}
static NblValue *env_math_tan(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(tan(x->floating));
}
static NblValue *env_math_asin(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(asin(x->floating));
}
static NblValue *env_math_acos(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(acos(x->floating));
}
static NblValue *env_math_atan(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(atan(x->floating));
}
static NblValue *env_math_atan2(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *y = list_get(values, 0);
    NblValue *x = list_get(values, 1);
    return value_new_float(atan2(y->floating, x->floating));
}
static NblValue *env_math_pow(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    NblValue *y = list_get(values, 1);
    return value_new_float(pow(x->floating, y->floating));
}
static NblValue *env_math_sqrt(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(sqrt(x->floating));
}
static NblValue *env_math_floor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(floor(x->floating));
}
static NblValue *env_math_ceil(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(ceil(x->floating));
}
static NblValue *env_math_round(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(round(x->floating));
}
static NblValue *env_math_min(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = list_get(values, 0);
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
    list_foreach(values, NblValue * value, {
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
static NblValue *env_math_max(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = list_get(values, 0);
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
    list_foreach(values, NblValue * value, {
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
static NblValue *env_math_exp(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(exp(x->floating));
}
static NblValue *env_math_log(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = list_get(values, 0);
    return value_new_float(log(x->floating));
}
static NblValue *env_math_random(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    (void)values;
    return value_new_float(random_random());
}

// Exception
static NblValue *env_exception_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *error = list_get(values, 0);
    map_set(this->object, "error", value_retrieve(error));
    map_set(this->object, "path", value_new_string(context->node->token->source->path));
    map_set(this->object, "text", value_new_string(context->node->token->source->text));
    map_set(this->object, "line", value_new_int(context->node->token->line));
    map_set(this->object, "column", value_new_int(context->node->token->column));
    return value_new_null();
}

// String
static NblValue *env_string_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = list_get(values, 0);
    char *string = value_to_string(first);
    NblValue *value = value_new_string(string);
    free(string);
    return value;
}
static NblValue *env_string_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return value_new_int(strlen(this->string));
}

// Array
static NblValue *env_array_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = list_get(values, 0);
    if (first != NULL && first->type == VALUE_ARRAY) {
        return value_ref(first);
    }
    return value_new_array(list_new());
}
static NblValue *env_array_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return value_new_int(this->array->size);
}
static NblValue *env_array_push(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    list_foreach(values, NblValue * value, { list_add(this->array, value_retrieve(value)); });
    return value_new_int(this->array->size);
}
static NblValue *env_array_foreach(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = list_get(values, 0);
    list_foreach(this->array, NblValue * value, {
        NblList *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        interpreter_call(context, function, NULL, arguments);
        list_free(arguments, (NblListFreeFunc *)value_free);
    });
    return value_new_null();
}
static NblValue *env_array_map(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = list_get(values, 0);
    NblList *items = list_new();
    list_foreach(this->array, NblValue * value, {
        NblList *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        list_add(items, interpreter_call(context, function, NULL, arguments));
        list_free(arguments, (NblListFreeFunc *)value_free);
    });
    return value_new_array(items);
}
static NblValue *env_array_filter(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = list_get(values, 0);
    NblList *items = list_new();
    list_foreach(this->array, NblValue * value, {
        NblList *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        NblValue *returnValue = interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != VALUE_BOOL) {
            return interpreter_throw(context,
                                     value_new_string_format("Array filter condition type is not a bool it is: %s", value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            list_add(items, value);
        }
        value_free(returnValue);
        list_free(arguments, (NblListFreeFunc *)value_free);
    });
    return value_new_array(items);
}
static NblValue *env_array_find(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = list_get(values, 0);
    list_foreach(this->array, NblValue * value, {
        NblList *arguments = list_new();
        list_add(arguments, value_retrieve(value));
        list_add(arguments, value_new_int(index));
        list_add(arguments, value_ref(this));
        NblValue *returnValue = interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != VALUE_BOOL) {
            return interpreter_throw(context,
                                     value_new_string_format("Array find condition type is not a bool it is: %s", value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            value_free(returnValue);
            list_free(arguments, (NblListFreeFunc *)value_free);
            return value_retrieve(value);
        }
        value_free(returnValue);
        list_free(arguments, (NblListFreeFunc *)value_free);
    });
    return value_new_null();
}

// Object
static NblValue *env_object_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = list_get(values, 0);
    if (first != NULL && first->type == VALUE_OBJECT) {
        return value_ref(first);
    }
    return value_new_object(map_new());
}
static NblValue *env_object_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return value_new_int(this->object->size);
}
static NblValue *env_object_keys(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    NblList *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_new_string(this->object->keys[i]));
    }
    return value_new_array(items);
}
static NblValue *env_object_values(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    NblList *items = list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        list_add(items, value_retrieve(this->object->values[i]));
    }
    return value_new_array(items);
}

// Date
static NblValue *env_date_now(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    (void)values;
    return value_new_int(time_ms());
}

// Root
static NblValue *env_type(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *value = list_get(values, 0);
    return value_new_string(value_type_to_string(value->type));
}

static NblValue *env_assert(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *assertion = list_get(values, 0);
    if (!assertion->boolean) {
        return interpreter_throw(context, value_new_string("Assertion failed"));
    }
    return value_new_null();
}

NblMap *std_env(void) {
    NblMap *env = map_new();

    NblList *empty_args = list_new();

    // Math
    NblMap *math = map_new();
    map_set(env, "Math", variable_new(VALUE_OBJECT, false, value_new_object(math)));
    map_set(math, "E", value_new_float(M_E));
    map_set(math, "LN2", value_new_float(M_LN2));
    map_set(math, "LN10", value_new_float(M_LN10));
    map_set(math, "LOG2E", value_new_float(M_LOG2E));
    map_set(math, "LOG10E", value_new_float(M_LOG10E));
    map_set(math, "PI", value_new_float(M_PI));
    map_set(math, "SQRT1_2", value_new_float(M_SQRT1_2));
    map_set(math, "SQRT2", value_new_float(M_SQRT2));

    NblList *math_float_args = list_new();
    list_add(math_float_args, argument_new("x", VALUE_FLOAT, NULL));
    NblList *math_float_float_args = list_new();
    list_add(math_float_float_args, argument_new("x", VALUE_FLOAT, NULL));
    list_add(math_float_float_args, argument_new("y", VALUE_FLOAT, NULL));
    NblList *math_float_float_reverse_args = list_new();
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
    NblMap *exception = map_new();
    NblList *exception_constructor_args = list_new();
    list_add(exception_constructor_args, argument_new("error", VALUE_STRING, NULL));
    map_set(env, "Exception", variable_new(VALUE_CLASS, false, value_new_class(exception, NULL, false)));
    map_set(exception, "constructor", value_new_native_function(exception_constructor_args, VALUE_ANY, env_exception_constructor));

    // String
    NblMap *string = map_new();
    map_set(env, "String", variable_new(VALUE_CLASS, false, value_new_class(string, NULL, false)));
    map_set(string, "constructor", value_new_native_function(list_ref(empty_args), VALUE_STRING, env_string_constructor));
    map_set(string, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_string_length));

    // Array
    NblMap *array = map_new();
    NblList *array_function_args = list_new();
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
    NblMap *object = map_new();
    map_set(env, "Object", variable_new(VALUE_CLASS, false, value_new_class(object, NULL, false)));
    map_set(object, "constructor", value_new_native_function(list_ref(empty_args), VALUE_OBJECT, env_object_constructor));
    map_set(object, "length", value_new_native_function(list_ref(empty_args), VALUE_INT, env_object_length));
    map_set(object, "keys", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_keys));
    map_set(object, "values", value_new_native_function(list_ref(empty_args), VALUE_ARRAY, env_object_values));

    // Date
    NblMap *date = map_new();
    map_set(env, "Date", variable_new(VALUE_CLASS, false, value_new_class(date, NULL, false)));
    map_set(date, "now", value_new_native_function(list_ref(empty_args), VALUE_INT, env_date_now));

    // Root
    NblList *type_args = list_new();
    list_add(type_args, argument_new("value", VALUE_ANY, NULL));
    map_set(env, "type", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(type_args, VALUE_ANY, env_type)));

    NblList *assertion_args = list_new();
    list_add(assertion_args, argument_new("assertion", VALUE_ANY, NULL));
    map_set(env, "assert", variable_new(VALUE_NATIVE_FUNCTION, false, value_new_native_function(assertion_args, VALUE_NULL, env_assert)));

    return env;
}

// NblInterpreter
NblVariable *variable_new(NblValueType type, bool mutable, NblValue *value) {
    NblVariable *variable = malloc(sizeof(NblVariable));
    variable->type = type;
    variable->mutable = mutable;
    variable->value = value;
    return variable;
}

void variable_free(NblVariable *variable) {
    value_free(variable->value);
    free(variable);
}

NblVariable *block_scope_get(NblBlockScope *block, char *key) {
    NblVariable *variable = map_get(block->env, key);
    if (variable == NULL && block->parentBlock != NULL) {
        return block_scope_get(block->parentBlock, key);
    }
    return variable;
}

NblValue *interpreter(NblMap *env, NblNode *node) {
    NblInterpreter interpreter = {.env = env};
    NblScope scope = {.exception = &(NblExceptionScope){.exceptionValue = NULL},
                      .function = &(NblFunctionScope){.returnValue = NULL},
                      .loop = &(NblLoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                      .block = &(NblBlockScope){.parentBlock = NULL, .env = env}};
    NblValue *returnValue = interpreter_node(&interpreter, &scope, node);
    if (scope.exception->exceptionValue != NULL) {
        NblValue *path = value_class_get(scope.exception->exceptionValue, "path");
        NblValue *text = value_class_get(scope.exception->exceptionValue, "text");
        NblValue *line = value_class_get(scope.exception->exceptionValue, "line");
        NblValue *column = value_class_get(scope.exception->exceptionValue, "column");
        NblToken *token = token_new(TOKEN_THROW, source_new(path->string, text->string), line->integer, column->integer);
        NblValue *error = value_class_get(scope.exception->exceptionValue, "error");
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

NblValue *type_error_exception(NblValueType expected, NblValueType got) {
    return value_new_string_format("Unexpected type: '%s' expected '%s'", value_type_to_string(got), value_type_to_string(expected));
}

#define interpreter_statement_in_try(interpreter, scope, node, cleanup)   \
    {                                                                     \
        NblValue *nodeValue = interpreter_node(interpreter, scope, node); \
        if (nodeValue != NULL) value_free(nodeValue);                     \
        if ((scope)->function->returnValue != NULL) {                     \
            cleanup;                                                      \
            return NULL;                                                  \
        }                                                                 \
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

NblValue *interpreter_call(NblInterpreterContext *context, NblValue *callValue, NblValue *this, NblList *arguments) {
    if (callValue->type == VALUE_FUNCTION) {
        NblScope functionScope = {.exception = context->scope->exception,
                                  .function = &(NblFunctionScope){.returnValue = NULL},
                                  .loop = &(NblLoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                                  .block = &(NblBlockScope){.parentBlock = context->scope->block, .env = map_new()}};
        if (this != NULL) {
            if (this->instanceClass->parentClass != NULL) {
                map_set(functionScope.block->env, "super",
                        variable_new(VALUE_INSTANCE, false, value_new_instance(map_ref(this->object), value_ref(this->instanceClass->parentClass))));
            }
            map_set(functionScope.block->env, "this", variable_new(VALUE_INSTANCE, false, value_ref(this)));
        }
        map_set(functionScope.block->env, "arguments", variable_new(VALUE_ARRAY, false, value_new_array(list_ref(arguments))));
        for (size_t i = 0; i < callValue->arguments->size; i++) {
            NblArgument *argument = list_get(callValue->arguments, i);
            map_set(functionScope.block->env, argument->name, variable_new(argument->type, true, value_ref(list_get(arguments, i))));
        }
        NblInterpreter interpreter = {.env = context->env};
        interpreter_node(&interpreter, &functionScope, callValue->functionNode);
        map_free(functionScope.block->env, (NblMapFreeFunc *)variable_free);
        if (callValue->returnType != VALUE_ANY && context->scope->exception->exceptionValue == NULL &&
            functionScope.function->returnValue->type != callValue->returnType) {
            NblValueType returnValueType = functionScope.function->returnValue->type;
            value_free(functionScope.function->returnValue);
            return interpreter_throw(context, type_error_exception(callValue->returnType, returnValueType));
        }

        if (functionScope.function->returnValue != NULL) {
            return functionScope.function->returnValue;
        }
        return value_new_null();
    }

    if (callValue->type == VALUE_NATIVE_FUNCTION) {
        NblValue *returnValue = callValue->nativeFunc(context, this, arguments);
        if (callValue->returnType != VALUE_ANY && context->scope->exception->exceptionValue == NULL && returnValue->type != callValue->returnType) {
            return interpreter_throw(context, type_error_exception(callValue->returnType, returnValue->type));
        }
        return returnValue;
    }

    if (callValue->type == VALUE_CLASS) {
        NblValue *instance = value_new_instance(map_new(), value_ref(callValue));
        NblValue *constructorFunction = value_class_get(callValue, "constructor");
        if (constructorFunction != NULL) {
            NblValue *newReturnValue = interpreter_call(context, constructorFunction, instance, arguments);
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

NblValue *interpreter_throw(NblInterpreterContext *context, NblValue *exception) {
    if (exception->type == VALUE_STRING) {
        NblValue *exceptionClass = ((NblVariable *)map_get(context->env, "Exception"))->value;
        NblList *arguments = list_new();
        list_add(arguments, exception);
        exception = interpreter_call(context, exceptionClass, NULL, arguments);
        list_free(arguments, (NblListFreeFunc *)value_free);
    }
    if (exception->type != VALUE_INSTANCE) {
        return interpreter_throw(context, type_error_exception(VALUE_INSTANCE, exception->type));
    }
    context->scope->exception->exceptionValue = exception;
    return value_new_null();
}

NblValue *interpreter_node(NblInterpreter *interpreter, NblScope *scope, NblNode *node) {
    if (node->type == NODE_PROGRAM) {
        list_foreach(node->nodes, NblNode * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NODE_NODES) {
        list_foreach(node->nodes, NblNode * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NODE_BLOCK) {
        NblScope blockScope = {.exception = scope->exception,
                               .function = scope->function,
                               .loop = scope->loop,
                               .block = &(NblBlockScope){.parentBlock = scope->block, .env = map_new()}};
        list_foreach(node->nodes, NblNode * child,
                     { interpreter_statement(interpreter, &blockScope, child, { map_free(blockScope.block->env, (NblMapFreeFunc *)variable_free); }); });
        map_free(blockScope.block->env, (NblMapFreeFunc *)variable_free);
        return NULL;
    }
    if (node->type == NODE_IF) {
        NblValue *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOL) {
            NblValueType conditionType = condition->type;
            value_free(condition);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
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
        NblValue *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOL) {
            NblValueType conditionType = condition->type;
            value_free(condition);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
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
        NblScope tryScope = {
            .exception = &(NblExceptionScope){.exceptionValue = NULL}, .function = scope->function, .loop = scope->loop, .block = scope->block};
        interpreter_statement_in_try(interpreter, &tryScope, node->tryBlock, {});
        if (tryScope.exception->exceptionValue != NULL) {
            NblScope catchScope = {.exception = scope->exception,
                                   .function = scope->function,
                                   .loop = scope->loop,
                                   .block = &(NblBlockScope){.parentBlock = scope->block, .env = map_new()}};
            map_set(catchScope.block->env, node->catchVariable->lhs->string,
                    variable_new(node->catchVariable->declarationType, node->catchVariable->type == NODE_LET_ASSIGN,
                                 value_ref(tryScope.exception->exceptionValue)));
            interpreter_statement(interpreter, &catchScope, node->catchBlock, { map_free(catchScope.block->env, (NblMapFreeFunc *)variable_free); });
            map_free(catchScope.block->env, (NblMapFreeFunc *)variable_free);
            value_free(tryScope.exception->exceptionValue);
        }
        if (node->finallyBlock != NULL) {
            interpreter_statement(interpreter, scope, node->finallyBlock, {});
        }
        return NULL;
    }
    if (node->type == NODE_LOOP) {
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
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
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = scope->block};
        for (;;) {
            NblValue *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
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
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = scope->block};
        for (;;) {
            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }

            NblValue *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
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
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = scope->block};
        for (;;) {
            NblValue *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
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
        NblValue *iterator = interpreter_node(interpreter, scope, node->iterator);
        if (iterator->type != VALUE_STRING && iterator->type != VALUE_ARRAY && iterator->type != VALUE_OBJECT && iterator->type != VALUE_CLASS &&
            iterator->type != VALUE_INSTANCE) {
            NblValueType iteratorType = iterator->type;
            value_free(iterator);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->iterator};
            return interpreter_throw(&context, value_new_string_format("NblVariable is not a string, array, object, class or instance it is: %s",
                                                                       value_type_to_string(iteratorType)));
        }

        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = &(NblBlockScope){.parentBlock = scope->block, .env = map_new()}};
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
            NblValue *iteratorValue;
            if (iterator->type == VALUE_STRING) {
                char character[] = {iterator->string[i], '\0'};
                iteratorValue = value_new_string(character);
            }
            if (iterator->type == VALUE_ARRAY) {
                NblValue *value = list_get(iterator->array, i);
                iteratorValue = value != NULL ? value_retrieve(value) : value_new_null();
            }
            if (iterator->type == VALUE_OBJECT || iterator->type == VALUE_CLASS || iterator->type == VALUE_INSTANCE) {
                iteratorValue = value_new_string(iterator->object->keys[i]);
            }
            NblVariable *previousVariable = map_get(loopScope.block->env, node->forinVariable->lhs->string);
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
        map_free(loopScope.block->env, (NblMapFreeFunc *)variable_free);
        value_free(iterator);
        return NULL;
    }
    if (node->type == NODE_CONTINUE) {
        if (!scope->loop->inLoop) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string("Continue not in a loop"));
        }
        scope->loop->isContinuing = true;
        return NULL;
    }
    if (node->type == NODE_BREAK) {
        if (!scope->loop->inLoop) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
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
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
        return interpreter_throw(&context, interpreter_node(interpreter, scope, node->unary));
    }
    if (node->type == NODE_INCLUDE) {
        NblValue *pathValue = interpreter_node(interpreter, scope, node->unary);
        if (pathValue->type != VALUE_STRING) {
            NblValueType pathValueType = pathValue->type;
            value_free(pathValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
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
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
            return interpreter_throw(&context, value_new_string_format("Can't read file: %s", includePath));
        }

        NblList *tokens = lexer(includePath, includeText);
        NblNode *includeNode = parser(tokens, true);
        interpreter_statement(interpreter, scope, includeNode, {
            node_free(includeNode);
            list_free(tokens, (NblListFreeFunc *)token_free);
            free(includeText);
        });
        node_free(includeNode);
        list_free(tokens, (NblListFreeFunc *)token_free);
        free(includeText);
        return NULL;
    }

    if (node->type == NODE_VALUE) {
        return value_retrieve(node->value);
    }
    if (node->type == NODE_ARRAY) {
        NblValue *arrayValue = value_new_array(list_new_with_capacity(node->array->capacity));
        list_foreach(node->array, NblNode * item, { list_add(arrayValue->array, interpreter_node(interpreter, scope, item)); });
        return arrayValue;
    }
    if (node->type == NODE_OBJECT) {
        NblValue *objectValue = value_new_object(map_new_with_capacity(node->object->capacity));
        map_foreach(node->object, char *key, NblNode *value, { map_set(objectValue->object, key, interpreter_node(interpreter, scope, value)); });
        return objectValue;
    }
    if (node->type == NODE_CLASS) {
        NblValue *classValue = value_new_class(map_new_with_capacity(node->object->capacity),
                                               node->parentClass != NULL ? interpreter_node(interpreter, scope, node->parentClass) : NULL, node->abstract);
        map_foreach(node->object, char *key, NblNode *value, { map_set(classValue->object, key, interpreter_node(interpreter, scope, value)); });
        return classValue;
    }

    if (node->type == NODE_CONST_ASSIGN || node->type == NODE_LET_ASSIGN) {
        NblValue *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get(scope->block->env, node->lhs->string) != NULL) {
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Can't redeclare variable: '%s'", node->lhs->string));
        }
        if (node->declarationType != VALUE_ANY && node->declarationType != rhs->type) {
            NblValueType rhsType = rhs->type;
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("Unexpected variable type: '%s' needed '%s'", value_type_to_string(rhsType),
                                                                       value_type_to_string(node->declarationType)));
        }
        map_set(scope->block->env, node->lhs->string, variable_new(node->declarationType, node->type == NODE_LET_ASSIGN, value_retrieve(rhs)));
        return rhs;
    }
    if (node->type == NODE_ASSIGN) {
        NblValue *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (node->lhs->type == NODE_GET) {
            NblValue *containerValue = interpreter_node(interpreter, scope, node->lhs->lhs);
            if (containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT && containerValue->type != VALUE_CLASS &&
                containerValue->type != VALUE_INSTANCE) {
                NblValueType containerValueType = containerValue->type;
                value_free(rhs);
                value_free(containerValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return interpreter_throw(&context, value_new_string_format("NblVariable is not an array, object, class or instance it is: %s",
                                                                           value_type_to_string(containerValueType)));
            }

            NblValue *indexOrKey = interpreter_node(interpreter, scope, node->lhs->rhs);
            if (containerValue->type == VALUE_ARRAY) {
                if (indexOrKey->type != VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    value_free(rhs);
                    value_free(containerValue);
                    value_free(indexOrKey);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_INT, indexOrKeyType));
                }
                NblValue *previousValue = list_get(containerValue->array, indexOrKey->integer);
                if (previousValue != NULL) value_free(previousValue);
                list_set(containerValue->array, indexOrKey->integer, value_retrieve(rhs));
            }
            if (containerValue->type == VALUE_OBJECT || containerValue->type == VALUE_CLASS || containerValue->type == VALUE_INSTANCE) {
                if (indexOrKey->type != VALUE_STRING) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    value_free(rhs);
                    value_free(containerValue);
                    value_free(indexOrKey);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_STRING, indexOrKeyType));
                }
                NblValue *previousValue = map_get(containerValue->object, indexOrKey->string);
                if (previousValue != NULL) value_free(previousValue);
                map_set(containerValue->object, indexOrKey->string, value_retrieve(rhs));
            }
            value_free(indexOrKey);
            value_free(containerValue);
            return rhs;
        }

        if (node->lhs->type != NODE_VARIABLE) {
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Is not a variable"));
        }
        NblVariable *variable = block_scope_get(scope->block, node->lhs->string);
        if (variable == NULL) {
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("NblVariable: '%s' is not declared", node->lhs->string));
        }
        if (!variable->mutable) {
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, value_new_string_format("Can't mutate const variable: '%s'", node->lhs->string));
        }
        if (variable->type != VALUE_ANY && variable->type != rhs->type) {
            NblValueType rhsType = rhs->type;
            value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return interpreter_throw(&context, type_error_exception(variable->type, rhsType));
        }
        value_free(variable->value);
        variable->value = value_retrieve(rhs);
        return rhs;
    }

    if (node->type == NODE_VARIABLE) {
        NblVariable *variable = block_scope_get(scope->block, node->string);
        if (variable == NULL) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("Can't find variable: '%s'", node->string));
        }
        return value_retrieve(variable->value);
    }
    if (node->type == NODE_GET) {
        NblValue *containerValue = interpreter_node(interpreter, scope, node->lhs);
        if (containerValue->type != VALUE_STRING && containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT &&
            containerValue->type != VALUE_CLASS && containerValue->type != VALUE_INSTANCE) {
            NblValueType containerValueType = containerValue->type;
            value_free(containerValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return interpreter_throw(&context, value_new_string_format("NblVariable is not a string, array, object, class or instance it is: %s",
                                                                       value_type_to_string(containerValueType)));
        }

        NblValue *indexOrKey = interpreter_node(interpreter, scope, node->rhs);
        NblValue *returnValue = NULL;
        if (containerValue->type == VALUE_STRING) {
            if (indexOrKey->type == VALUE_STRING) {
                NblValue *stringClass = ((NblVariable *)map_get(interpreter->env, "String"))->value;
                NblValue *stringClassItem = map_get(stringClass->object, indexOrKey->string);
                if (stringClassItem != NULL) returnValue = value_retrieve(stringClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
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
                NblValue *arrayClass = ((NblVariable *)map_get(interpreter->env, "Array"))->value;
                NblValue *arrayClassItem = map_get(arrayClass->object, indexOrKey->string);
                if (arrayClassItem != NULL) returnValue = value_retrieve(arrayClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_INT, indexOrKeyType));
                }
                NblValue *value = list_get(containerValue->array, indexOrKey->integer);
                returnValue = value != NULL ? value_retrieve(value) : value_new_null();
            }
        }
        if (containerValue->type == VALUE_OBJECT || containerValue->type == VALUE_CLASS || containerValue->type == VALUE_INSTANCE) {
            if (containerValue->type == VALUE_OBJECT && indexOrKey->type == VALUE_STRING) {
                NblValue *objectClass = ((NblVariable *)map_get(interpreter->env, "Object"))->value;
                NblValue *objectClassItem = map_get(objectClass->object, indexOrKey->string);
                if (objectClassItem != NULL) returnValue = value_retrieve(objectClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != VALUE_STRING) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    value_free(indexOrKey);
                    value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return interpreter_throw(&context, type_error_exception(VALUE_STRING, indexOrKeyType));
                }
                NblValue *value;
                if (containerValue->type == VALUE_INSTANCE) {
                    value = value_class_get(containerValue, indexOrKey->string);
                } else {
                    value = map_get(containerValue->object, indexOrKey->string);
                }
                if (value == NULL) {
                    char *indexOrKeyString = strdup(indexOrKey->string);
                    value_free(indexOrKey);
                    value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    NblValue *exception = value_new_string_format("Can't find %s in object", indexOrKeyString);
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
        NblValue *thisValue = NULL;
        if (node->function->type == NODE_GET) {
            NblValue *containerValue = interpreter_node(interpreter, scope, node->function->lhs);
            if (containerValue->type == VALUE_STRING || containerValue->type == VALUE_ARRAY || containerValue->type == VALUE_OBJECT ||
                containerValue->type == VALUE_INSTANCE) {
                thisValue = containerValue;
            } else {
                value_free(containerValue);
            }
        }
        NblValue *callValue = interpreter_node(interpreter, scope, node->function);
        if (callValue->type != VALUE_FUNCTION && callValue->type != VALUE_NATIVE_FUNCTION && callValue->type != VALUE_CLASS) {
            NblValueType callValueType = callValue->type;
            value_free(callValue);
            if (thisValue != NULL) value_free(thisValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
            return interpreter_throw(&context,
                                     value_new_string_format("NblVariable is not a function or a class but: %s", value_type_to_string(callValueType)));
        }

        NblList *callArguments = NULL;
        if (callValue->type == VALUE_CLASS) {
            if (callValue->abstract) {
                value_free(callValue);
                if (thisValue != NULL) value_free(thisValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
                return interpreter_throw(&context, value_new_string("Can't construct an abstract class"));
            }
            NblValue *constructorFunction = value_class_get(callValue, "constructor");
            if (constructorFunction != NULL) callArguments = constructorFunction->arguments;
        } else {
            callArguments = callValue->arguments;
        }
        NblList *arguments = list_new_with_capacity(node->nodes->capacity);
        for (size_t i = 0; i < MAX(callArguments != NULL ? callArguments->size : 0, node->nodes->size); i++) {
            NblArgument *argument = NULL;
            if (callArguments != NULL) {
                argument = list_get(callArguments, i);
                if (list_get(node->nodes, i) == NULL && argument != NULL) {
                    if (argument->defaultNode != NULL) {
                        NblValue *defaultValue = interpreter_node(interpreter, scope, argument->defaultNode);
                        if (argument->type != VALUE_ANY && defaultValue->type != argument->type) {
                            NblValueType defaultValueType = defaultValue->type;
                            value_free(defaultValue);
                            list_free(arguments, (NblListFreeFunc *)value_free);
                            value_free(callValue);
                            if (thisValue != NULL) value_free(thisValue);
                            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = argument->defaultNode};
                            return interpreter_throw(&context, type_error_exception(argument->type, defaultValueType));
                        }
                        list_add(arguments, defaultValue);
                        continue;
                    }

                    list_free(arguments, (NblListFreeFunc *)value_free);
                    value_free(callValue);
                    if (thisValue != NULL) value_free(thisValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    return interpreter_throw(&context, value_new_string("Not all function arguments are given"));
                }
            }

            NblValue *nodeValue = interpreter_node(interpreter, scope, list_get(node->nodes, i));
            if (argument != NULL && argument->type != VALUE_ANY && nodeValue->type != argument->type) {
                NblValueType nodeValueType = nodeValue->type;
                value_free(nodeValue);
                list_free(arguments, (NblListFreeFunc *)value_free);
                value_free(callValue);
                if (thisValue != NULL) value_free(thisValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return interpreter_throw(&context, type_error_exception(argument->type, nodeValueType));
            }
            list_add(arguments, nodeValue);
        }

        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        NblValue *returnValue = interpreter_call(&context, callValue, thisValue, arguments);

        list_free(arguments, (NblListFreeFunc *)value_free);
        value_free(callValue);
        if (thisValue != NULL) value_free(thisValue);
        return returnValue;
    }

    if (node->type >= NODE_NEG && node->type <= NODE_CAST) {
        NblValue *unary = interpreter_node(interpreter, scope, node->unary);
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
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
                return interpreter_throw(&context, value_new_string_format("Is not a variable"));
            }
            NblVariable *variable = block_scope_get(scope->block, node->lhs->string);
            if (!variable->mutable) {
                value_free(unary);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
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
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return interpreter_throw(&context, value_new_string("Type error"));
    }

    if (node->type >= NODE_ADD && node->type <= NODE_LOGICAL_OR) {
        NblValue *lhs = interpreter_node(interpreter, scope, node->lhs);
        NblValue *rhs = interpreter_node(interpreter, scope, node->rhs);

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
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return interpreter_throw(&context, value_new_string("Type error"));
    }

    fprintf(stderr, "Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

#endif
