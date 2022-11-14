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
#ifdef _WIN32
#include <windows.h>
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
extern int64_t nbl_random_seed;

double nbl_random_random(void);

int64_t nbl_time_ms(void);

char *nbl_file_read(char *path);

typedef struct NblToken NblToken;  // Forward define

void nbl_print_error(NblToken *token, char *fmt, ...);

// List header
typedef struct NblList {
    int32_t refs;
    void **items;
    size_t capacity;
    size_t size;
} NblList;

#define nbl_list_foreach(list, item, block)                   \
    for (size_t index = 0; index < list->size; index++) { \
        item = nbl_list_get(list, index);                     \
        block                                             \
    }

NblList *nbl_list_new(void);

NblList *nbl_list_new_with_capacity(size_t capacity);

NblList *nbl_list_ref(NblList *list);

void *nbl_list_get(NblList *list, size_t index);

void nbl_list_set(NblList *list, size_t index, void *item);

void nbl_list_add(NblList *list, void *item);

char *nbl_list_to_string(NblList *list);

typedef void NblListFreeFunc(void *item);

void nbl_list_free(NblList *list, NblListFreeFunc *freeFunc);

// Map header
typedef struct NblMap {
    int32_t refs;
    char **keys;
    void **values;
    size_t capacity;
    size_t size;
} NblMap;

#define nbl_map_foreach(map, key, value, block)              \
    for (size_t index = 0; index < map->size; index++) { \
        key = map->keys[index];                          \
        value = map->values[index];                      \
        block                                            \
    }

NblMap *nbl_map_new(void);

NblMap *nbl_map_new_with_capacity(size_t capacity);

NblMap *nbl_map_ref(NblMap *map);

void *nbl_map_get(NblMap *map, char *key);

void nbl_map_set(NblMap *map, char *key, void *item);

typedef void NblMapFreeFunc(void *item);

void nbl_map_free(NblMap *map, NblMapFreeFunc *freeFunc);

// Lexer header
typedef struct NblSource {
    int32_t refs;
    char *path;
    char *basename;
    char *dirname;
    char *text;
} NblSource;

NblSource *nbl_source_new(char *path, char *text);

NblSource *nbl_source_ref(NblSource *source);

void nbl_source_free(NblSource *source);

typedef enum NblTokenType {
    NBL_TOKEN_EOF,
    NBL_TOKEN_UNKNOWN,
    NBL_TOKEN_LPAREN,
    NBL_TOKEN_RPAREN,
    NBL_TOKEN_LCURLY,
    NBL_TOKEN_RCURLY,
    NBL_TOKEN_LBRACKET,
    NBL_TOKEN_RBRACKET,
    NBL_TOKEN_QUESTION,
    NBL_TOKEN_SEMICOLON,
    NBL_TOKEN_COLON,
    NBL_TOKEN_COMMA,
    NBL_TOKEN_POINT,
    NBL_TOKEN_FAT_ARROW,

    NBL_TOKEN_KEYWORD,
    NBL_TOKEN_INT,
    NBL_TOKEN_FLOAT,
    NBL_TOKEN_STRING,

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
    NBL_TOKEN_INSTANCEOF,
    NBL_TOKEN_EQ,
    NBL_TOKEN_NEQ,
    NBL_TOKEN_LT,
    NBL_TOKEN_LTEQ,
    NBL_TOKEN_GT,
    NBL_TOKEN_GTEQ,
    NBL_TOKEN_LOGICAL_AND,
    NBL_TOKEN_LOGICAL_OR,
    NBL_TOKEN_LOGICAL_NOT,

    NBL_TOKEN_TYPE_ANY,
    NBL_TOKEN_NULL,
    NBL_TOKEN_TYPE_BOOL,
    NBL_TOKEN_TRUE,
    NBL_TOKEN_FALSE,
    NBL_TOKEN_TYPE_INT,
    NBL_TOKEN_TYPE_FLOAT,
    NBL_TOKEN_TYPE_STRING,
    NBL_TOKEN_TYPE_ARRAY,
    NBL_TOKEN_TYPE_OBJECT,
    NBL_TOKEN_CLASS,
    NBL_TOKEN_EXTENDS,
    NBL_TOKEN_ABSTRACT,
    NBL_TOKEN_TYPE_INSTANCE,
    NBL_TOKEN_TYPE_FUNCTION,
    NBL_TOKEN_FUNCTION,

    NBL_TOKEN_CONST,
    NBL_TOKEN_LET,
    NBL_TOKEN_IF,
    NBL_TOKEN_ELSE,
    NBL_TOKEN_LOOP,
    NBL_TOKEN_WHILE,
    NBL_TOKEN_DO,
    NBL_TOKEN_FOR,
    NBL_TOKEN_IN,
    NBL_TOKEN_CONTINUE,
    NBL_TOKEN_BREAK,
    NBL_TOKEN_RETURN,
    NBL_TOKEN_THROW,
    NBL_TOKEN_TRY,
    NBL_TOKEN_CATCH,
    NBL_TOKEN_FINALLY,
    NBL_TOKEN_INCLUDE
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

NblToken *nbl_token_new(NblTokenType type, NblSource *source, int32_t line, int32_t column);

NblToken *nbl_token_new_int(NblTokenType type, NblSource *source, int32_t line, int32_t column, int64_t integer);

NblToken *nbl_token_new_float(NblSource *source, int32_t line, int32_t column, double floating);

NblToken *nbl_token_new_string(NblTokenType type, NblSource *source, int32_t line, int32_t column, char *string);

NblToken *nbl_token_ref(NblToken *token);

bool nbl_token_type_is_type(NblTokenType type);

char *nbl_token_type_to_string(NblTokenType type);

void nbl_token_free(NblToken *token);

int64_t nbl_string_to_int(char *string);

double nbl_string_to_float(char *string);

typedef struct NblKeyword {
    char *keyword;
    NblTokenType type;
} NblKeyword;

NblList *nbl_lexer(char *path, char *text);

// Value
typedef struct NblNode NblNode;                              // Forward define
typedef struct NblInterpreterContext NblInterpreterContext;  // Forward define

typedef enum NblValueType {
    NBL_VALUE_ANY,
    NBL_VALUE_NULL,
    NBL_VALUE_BOOL,
    NBL_VALUE_INT,
    NBL_VALUE_FLOAT,
    NBL_VALUE_STRING,
    NBL_VALUE_ARRAY,
    NBL_VALUE_OBJECT,
    NBL_VALUE_CLASS,
    NBL_VALUE_INSTANCE,
    NBL_VALUE_FUNCTION,
    NBL_VALUE_NATIVE_FUNCTION
} NblValueType;

typedef struct NblArgument {
    char *name;
    NblValueType type;
    NblNode *defaultNode;
} NblArgument;

NblArgument *nbl_argument_new(char *name, NblValueType type, NblNode *defaultNode);

void nbl_argument_free(NblArgument *argument);

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

NblValue *nbl_value_new(NblValueType type);

NblValue *nbl_value_new_null(void);

NblValue *nbl_value_new_bool(bool boolean);

NblValue *nbl_value_new_int(int64_t integer);

NblValue *nbl_value_new_float(double integer);

NblValue *nbl_value_new_string(char *string);

NblValue *nbl_value_new_string_format(char *format, ...);

NblValue *nbl_value_new_array(NblList *array);

NblValue *nbl_value_new_object(NblMap *object);

NblValue *nbl_value_new_class(NblMap *object, NblValue *parentClass, bool abstract);

NblValue *nbl_value_new_instance(NblMap *object, NblValue *instanceClass);

NblValue *nbl_value_new_function(NblList *args, NblValueType returnType, NblNode *node);

NblValue *nbl_value_new_native_function(NblList *args, NblValueType returnType,
                                    NblValue *(*nativeFunc)(NblInterpreterContext *context, NblValue *this, NblList *values));

char *nbl_value_type_to_string(NblValueType type);

NblValueType nbl_token_type_to_value_type(NblTokenType type);

char *nbl_value_to_string(NblValue *value);

NblValue *nbl_value_class_get(NblValue *instance, char *key);

bool nbl_value_class_instanceof(NblValue *instance, NblValue *class);

NblValue *nbl_value_ref(NblValue *value);

NblValue *nbl_value_retrieve(NblValue *value);

void nbl_value_clear(NblValue *value);

void nbl_value_free(NblValue *value);

// Parser
typedef enum NblNodeType {
    NBL_NODE_PROGRAM,
    NBL_NODE_NODES,
    NBL_NODE_BLOCK,
    NBL_NODE_IF,
    NBL_NODE_TRY,
    NBL_NODE_TENARY,
    NBL_NODE_LOOP,
    NBL_NODE_WHILE,
    NBL_NODE_DOWHILE,
    NBL_NODE_FOR,
    NBL_NODE_FORIN,
    NBL_NODE_CONTINUE,
    NBL_NODE_BREAK,
    NBL_NODE_RETURN,
    NBL_NODE_THROW,
    NBL_NODE_INCLUDE,

    NBL_NODE_VALUE,
    NBL_NODE_ARRAY,
    NBL_NODE_OBJECT,
    NBL_NODE_CLASS,
    NBL_NODE_CALL,

    NBL_NODE_NEG,
    NBL_NODE_INC_PRE,
    NBL_NODE_DEC_PRE,
    NBL_NODE_INC_POST,
    NBL_NODE_DEC_POST,
    NBL_NODE_NOT,
    NBL_NODE_LOGICAL_NOT,
    NBL_NODE_CAST,

    NBL_NODE_VARIABLE,
    NBL_NODE_CONST_ASSIGN,
    NBL_NODE_LET_ASSIGN,
    NBL_NODE_ASSIGN,
    NBL_NODE_GET,
    NBL_NODE_ADD,
    NBL_NODE_SUB,
    NBL_NODE_MUL,
    NBL_NODE_EXP,
    NBL_NODE_DIV,
    NBL_NODE_MOD,
    NBL_NODE_AND,
    NBL_NODE_XOR,
    NBL_NODE_OR,
    NBL_NODE_SHL,
    NBL_NODE_SHR,
    NBL_NODE_INSTANCEOF,
    NBL_NODE_EQ,
    NBL_NODE_NEQ,
    NBL_NODE_LT,
    NBL_NODE_LTEQ,
    NBL_NODE_GT,
    NBL_NODE_GTEQ,
    NBL_NODE_LOGICAL_AND,
    NBL_NODE_LOGICAL_OR
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

NblNode *nbl_node_new(NblNodeType type, NblToken *token);

NblNode *nbl_node_new_value(NblToken *token, NblValue *value);

NblNode *nbl_node_new_string(NblNodeType type, NblToken *token, char *string);

NblNode *nbl_node_new_unary(NblNodeType type, NblToken *token, NblNode *unary);

NblNode *nbl_node_new_cast(NblToken *token, NblValueType castType, NblNode *unary);

NblNode *nbl_node_new_operation(NblNodeType type, NblToken *token, NblNode *lhs, NblNode *rhs);

NblNode *nbl_node_new_multiple(NblNodeType type, NblToken *token);

NblNode *nbl_node_ref(NblNode *node);

void nbl_node_free(NblNode *node);

typedef struct NblParser {
    NblList *tokens;
    int32_t position;
} NblParser;

NblNode *nbl_parser(NblList *tokens, bool included);

void nbl_parser_eat(NblParser *nbl_parser, NblTokenType type);

NblValueType nbl_parser_eat_type(NblParser *nbl_parser);

NblNode *nbl_parser_program(NblParser *nbl_parser, bool included);
NblNode *nbl_parser_block(NblParser *nbl_parser);
NblNode *nbl_parser_statement(NblParser *nbl_parser);
NblNode *nbl_parser_declarations(NblParser *nbl_parser);
NblNode *nbl_parser_assigns(NblParser *nbl_parser);
NblNode *nbl_parser_assign(NblParser *nbl_parser);
NblNode *nbl_parser_tenary(NblParser *nbl_parser);
NblNode *nbl_parser_logical(NblParser *nbl_parser);
NblNode *nbl_parser_equality(NblParser *nbl_parser);
NblNode *nbl_parser_relational(NblParser *nbl_parser);
NblNode *nbl_parser_instanceof(NblParser *nbl_parser);
NblNode *nbl_parser_bitwise(NblParser *nbl_parser);
NblNode *nbl_parser_shift(NblParser *nbl_parser);
NblNode *nbl_parser_add(NblParser *nbl_parser);
NblNode *nbl_parser_mul(NblParser *nbl_parser);
NblNode *nbl_parser_unary(NblParser *nbl_parser);
NblNode *nbl_parser_primary(NblParser *nbl_parser);
NblNode *nbl_parser_primary_suffix(NblParser *nbl_parser, NblNode *node);
NblNode *nbl_parser_function(NblParser *nbl_parser, NblToken *token);
NblNode *nbl_parser_class(NblParser *nbl_parser, NblToken *token, bool abstract);
NblArgument *nbl_parser_argument(NblParser *nbl_parser);

// Standard library
NblMap *nbl_std_env(void);

// Interpreter
typedef struct NblVariable {
    NblValueType type;
    bool mutable;
    NblValue *value;
} NblVariable;

NblVariable *nbl_variable_new(NblValueType type, bool mutable, NblValue *value);

void nbl_variable_free(NblVariable *variable);

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

NblVariable *nbl_block_scope_get(NblBlockScope *block, char *key);

NblValue *nbl_interpreter(NblMap *env, NblNode *node);

NblValue *nbl_type_error_exception(NblValueType expected, NblValueType got);

NblValue *nbl_interpreter_call(NblInterpreterContext *context, NblValue *callValue, NblValue *this, NblList *arguments);

NblValue *nbl_interpreter_throw(NblInterpreterContext *context, NblValue *exception);

NblValue *nbl_interpreter_node(NblInterpreter *interpreter, NblScope *scope, NblNode *node);

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
int64_t nbl_random_seed;

double nbl_random_random(void) {
    double x = sin(nbl_random_seed++ * 10000);
    return x - floor(x);
}

int64_t nbl_time_ms(void) {
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

void nbl_print_error(NblToken *token, char *fmt, ...) {
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
NblList *nbl_list_new(void) { return nbl_list_new_with_capacity(8); }

NblList *nbl_list_new_with_capacity(size_t capacity) {
    NblList *list = malloc(sizeof(NblList));
    list->refs = 1;
    list->items = malloc(sizeof(void *) * capacity);
    list->capacity = capacity;
    list->size = 0;
    return list;
}

NblList *nbl_list_ref(NblList *list) {
    list->refs++;
    return list;
}

void *nbl_list_get(NblList *list, size_t index) {
    if (index < list->size) {
        return list->items[index];
    }
    return NULL;
}

void nbl_list_set(NblList *list, size_t index, void *item) {
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

void nbl_list_add(NblList *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

char *nbl_list_to_string(NblList *list) {
    size_t size = 0;
    for (size_t i = 0; i < list->size; i++) {
        size += strlen(nbl_list_get(list, i));
    }
    char *string = malloc(size + 1);
    string[0] = '\0';
    for (size_t i = 0; i < list->size; i++) {
        strcat(string, nbl_list_get(list, i));
    }
    return string;
}

void nbl_list_free(NblList *list, NblListFreeFunc *freeFunc) {
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
NblMap *nbl_map_new(void) { return nbl_map_new_with_capacity(8); }

NblMap *nbl_map_new_with_capacity(size_t capacity) {
    NblMap *map = malloc(sizeof(NblMap));
    map->refs = 1;
    map->keys = malloc(sizeof(char *) * capacity);
    map->values = malloc(sizeof(void *) * capacity);
    map->capacity = capacity;
    map->size = 0;
    return map;
}

NblMap *nbl_map_ref(NblMap *map) {
    map->refs++;
    return map;
}

void *nbl_map_get(NblMap *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->values[i];
        }
    }
    return NULL;
}

void nbl_map_set(NblMap *map, char *key, void *item) {
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

void nbl_map_free(NblMap *map, NblMapFreeFunc *freeFunc) {
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

void nbl_source_free(NblSource *source) {
    source->refs--;
    if (source->refs > 0) return;

    free(source->path);
    free(source->text);
    free(source->dirname);
    free(source);
}

NblToken *nbl_token_new(NblTokenType type, NblSource *source, int32_t line, int32_t column) {
    NblToken *token = malloc(sizeof(NblToken));
    token->refs = 1;
    token->source = nbl_source_ref(source);
    token->type = type;
    token->line = line;
    token->column = column;
    return token;
}

NblToken *nbl_token_new_int(NblTokenType type, NblSource *source, int32_t line, int32_t column, int64_t integer) {
    NblToken *token = nbl_token_new(type, source, line, column);
    token->integer = integer;
    return token;
}

NblToken *nbl_token_new_float(NblSource *source, int32_t line, int32_t column, double floating) {
    NblToken *token = nbl_token_new(NBL_TOKEN_FLOAT, source, line, column);
    token->floating = floating;
    return token;
}

NblToken *nbl_token_new_string(NblTokenType type, NblSource *source, int32_t line, int32_t column, char *string) {
    NblToken *token = nbl_token_new(type, source, line, column);
    token->string = string;
    return token;
}

NblToken *nbl_token_ref(NblToken *token) {
    token->refs++;
    return token;
}

bool nbl_token_type_is_type(NblTokenType type) {
    return type == NBL_TOKEN_TYPE_ANY || type == NBL_TOKEN_NULL || type == NBL_TOKEN_TYPE_BOOL || type == NBL_TOKEN_TYPE_INT || type == NBL_TOKEN_TYPE_FLOAT ||
           type == NBL_TOKEN_TYPE_STRING || type == NBL_TOKEN_TYPE_ARRAY || type == NBL_TOKEN_TYPE_OBJECT || type == NBL_TOKEN_TYPE_FUNCTION ||
           type == NBL_TOKEN_CLASS || type == NBL_TOKEN_TYPE_INSTANCE;
}

char *nbl_token_type_to_string(NblTokenType type) {
    if (type == NBL_TOKEN_EOF) return "EOF";
    if (type == NBL_TOKEN_UNKNOWN) return "Unknown character";
    if (type == NBL_TOKEN_LPAREN) return "(";
    if (type == NBL_TOKEN_RPAREN) return ")";
    if (type == NBL_TOKEN_LCURLY) return "{";
    if (type == NBL_TOKEN_RCURLY) return "}";
    if (type == NBL_TOKEN_LBRACKET) return "[";
    if (type == NBL_TOKEN_RBRACKET) return "]";
    if (type == NBL_TOKEN_QUESTION) return "?";
    if (type == NBL_TOKEN_SEMICOLON) return ";";
    if (type == NBL_TOKEN_COLON) return ":";
    if (type == NBL_TOKEN_COMMA) return ",";
    if (type == NBL_TOKEN_POINT) return ".";
    if (type == NBL_TOKEN_FAT_ARROW) return "=>";

    if (type == NBL_TOKEN_KEYWORD) return "keyword";
    if (type == NBL_TOKEN_INT) return "int";
    if (type == NBL_TOKEN_FLOAT) return "float";
    if (type == NBL_TOKEN_STRING) return "string";

    if (type == NBL_TOKEN_ASSIGN) return "=";
    if (type == NBL_TOKEN_ADD) return "+";
    if (type == NBL_TOKEN_INC) return "++";
    if (type == NBL_TOKEN_ASSIGN_ADD) return "+=";
    if (type == NBL_TOKEN_SUB) return "-";
    if (type == NBL_TOKEN_DEC) return "--";
    if (type == NBL_TOKEN_ASSIGN_SUB) return "-=";
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
    if (type == NBL_TOKEN_INSTANCEOF) return "instanceof";

    if (type == NBL_TOKEN_TYPE_ANY) return "any";
    if (type == NBL_TOKEN_NULL) return "null";
    if (type == NBL_TOKEN_TYPE_BOOL) return "bool";
    if (type == NBL_TOKEN_TRUE) return "true";
    if (type == NBL_TOKEN_FALSE) return "false";
    if (type == NBL_TOKEN_TYPE_INT) return "int";
    if (type == NBL_TOKEN_TYPE_FLOAT) return "float";
    if (type == NBL_TOKEN_TYPE_STRING) return "string";
    if (type == NBL_TOKEN_TYPE_ARRAY) return "array";
    if (type == NBL_TOKEN_TYPE_OBJECT) return "object";
    if (type == NBL_TOKEN_CLASS) return "class";
    if (type == NBL_TOKEN_EXTENDS) return "extends";
    if (type == NBL_TOKEN_ABSTRACT) return "abstract";
    if (type == NBL_TOKEN_TYPE_INSTANCE) return "instance";
    if (type == NBL_TOKEN_TYPE_FUNCTION) return "function";
    if (type == NBL_TOKEN_FUNCTION) return "fn";

    if (type == NBL_TOKEN_CONST) return "const";
    if (type == NBL_TOKEN_LET) return "let";
    if (type == NBL_TOKEN_IF) return "if";
    if (type == NBL_TOKEN_ELSE) return "else";
    if (type == NBL_TOKEN_LOOP) return "loop";
    if (type == NBL_TOKEN_WHILE) return "while";
    if (type == NBL_TOKEN_DO) return "do";
    if (type == NBL_TOKEN_FOR) return "for";
    if (type == NBL_TOKEN_IN) return "in";
    if (type == NBL_TOKEN_CONTINUE) return "continue";
    if (type == NBL_TOKEN_BREAK) return "break";
    if (type == NBL_TOKEN_RETURN) return "return";
    if (type == NBL_TOKEN_THROW) return "throw";
    if (type == NBL_TOKEN_TRY) return "try";
    if (type == NBL_TOKEN_CATCH) return "catch";
    if (type == NBL_TOKEN_FINALLY) return "finally";
    if (type == NBL_TOKEN_INCLUDE) return "include";
    return NULL;
}

void nbl_token_free(NblToken *token) {
    token->refs--;
    if (token->refs > 0) return;

    nbl_source_free(token->source);
    if (token->type == NBL_TOKEN_KEYWORD || token->type == NBL_TOKEN_STRING) free(token->string);
    free(token);
}

int64_t nbl_string_to_int(char *string) {
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

double nbl_string_to_float(char *string) { return strtod(string, NULL); }

NblList *nbl_lexer(char *path, char *text) {
    NblKeyword keywords[] = {{"instanceof", NBL_TOKEN_INSTANCEOF},
                             {"any", NBL_TOKEN_TYPE_ANY},
                             {"null", NBL_TOKEN_NULL},
                             {"bool", NBL_TOKEN_TYPE_BOOL},
                             {"true", NBL_TOKEN_TRUE},
                             {"false", NBL_TOKEN_FALSE},
                             {"int", NBL_TOKEN_TYPE_INT},
                             {"float", NBL_TOKEN_TYPE_FLOAT},
                             {"string", NBL_TOKEN_TYPE_STRING},
                             {"array", NBL_TOKEN_TYPE_ARRAY},
                             {"object", NBL_TOKEN_TYPE_OBJECT},
                             {"class", NBL_TOKEN_CLASS},
                             {"extends", NBL_TOKEN_EXTENDS},
                             {"abstract", NBL_TOKEN_ABSTRACT},
                             {"instance", NBL_TOKEN_TYPE_INSTANCE},
                             {"function", NBL_TOKEN_TYPE_FUNCTION},
                             {"fn", NBL_TOKEN_FUNCTION},

                             {"const", NBL_TOKEN_CONST},
                             {"let", NBL_TOKEN_LET},
                             {"if", NBL_TOKEN_IF},
                             {"else", NBL_TOKEN_ELSE},
                             {"loop", NBL_TOKEN_LOOP},
                             {"while", NBL_TOKEN_WHILE},
                             {"do", NBL_TOKEN_DO},
                             {"for", NBL_TOKEN_FOR},
                             {"in", NBL_TOKEN_IN},
                             {"continue", NBL_TOKEN_CONTINUE},
                             {"break", NBL_TOKEN_BREAK},
                             {"return", NBL_TOKEN_RETURN},
                             {"throw", NBL_TOKEN_THROW},
                             {"try", NBL_TOKEN_TRY},
                             {"catch", NBL_TOKEN_CATCH},
                             {"finally", NBL_TOKEN_FINALLY},
                             {"include", NBL_TOKEN_INCLUDE}};

    NblSource *source = nbl_source_new(path, text);
    NblList *tokens = nbl_list_new_with_capacity(512);
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
            nbl_list_add(tokens, nbl_token_new_int(NBL_TOKEN_INT, source, line, column, strtol(c, &c, 2)));
            continue;
        }
        if (*c == '0' && (isdigit(*(c + 1)) || *(c + 1) == 'o')) {
            if (*(c + 1) == 'o') c++;
            c++;
            nbl_list_add(tokens, nbl_token_new_int(NBL_TOKEN_INT, source, line, column, strtol(c, &c, 8)));
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            nbl_list_add(tokens, nbl_token_new_int(NBL_TOKEN_INT, source, line, column, strtol(c, &c, 16)));
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
                nbl_list_add(tokens, nbl_token_new_float(source, line, column, strtod(start, &c)));
            } else {
                nbl_list_add(tokens, nbl_token_new_int(NBL_TOKEN_INT, source, line, column, strtol(start, &c, 10)));
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
            nbl_list_add(tokens, nbl_token_new_string(NBL_TOKEN_STRING, source, line, column, string));
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
                    nbl_list_add(tokens, nbl_token_new(keyword->type, source, line, column));
                    found = true;
                    break;
                }
            }
            if (!found) {
                nbl_list_add(tokens, nbl_token_new_string(NBL_TOKEN_KEYWORD, source, line, column, strndup(ptr, size)));
            }
            continue;
        }

        // Syntax
        if (*c == '(') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LPAREN, source, line, column));
            c++;
            continue;
        }
        if (*c == ')') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_RPAREN, source, line, column));
            c++;
            continue;
        }
        if (*c == '{') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LCURLY, source, line, column));
            c++;
            continue;
        }
        if (*c == '}') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_RCURLY, source, line, column));
            c++;
            continue;
        }
        if (*c == '[') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LBRACKET, source, line, column));
            c++;
            continue;
        }
        if (*c == ']') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_RBRACKET, source, line, column));
            c++;
            continue;
        }
        if (*c == '?') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_QUESTION, source, line, column));
            c++;
            continue;
        }
        if (*c == ';') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_SEMICOLON, source, line, column));
            c++;
            continue;
        }
        if (*c == ':') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_COLON, source, line, column));
            c++;
            continue;
        }
        if (*c == ',') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_COMMA, source, line, column));
            c++;
            continue;
        }
        if (*c == '.') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_POINT, source, line, column));
            c++;
            continue;
        }

        // Operators
        if (*c == '=') {
            if (*(c + 1) == '>') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_FAT_ARROW, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_EQ, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN, source, line, column));
            c++;
            continue;
        }
        if (*c == '+') {
            if (*(c + 1) == '+') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_INC, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_ADD, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ADD, source, line, column));
            c++;
            continue;
        }
        if (*c == '-') {
            if (*(c + 1) == '-') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_DEC, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_SUB, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_SUB, source, line, column));
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_MUL, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '*') {
                if (*(c + 2) == '=') {
                    nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_EXP, source, line, column));
                    c += 3;
                    continue;
                }
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_EXP, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_MUL, source, line, column));
            c++;
            continue;
        }
        if (*c == '/') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_DIV, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_DIV, source, line, column));
            c++;
            continue;
        }
        if (*c == '%') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_MOD, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_MOD, source, line, column));
            c++;
            continue;
        }
        if (*c == '^') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_XOR, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_XOR, source, line, column));
            c++;
            continue;
        }
        if (*c == '~') {
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_NOT, source, line, column));
            c++;
            continue;
        }
        if (*c == '<') {
            if (*(c + 1) == '<') {
                if (*(c + 2) == '=') {
                    nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_SHL, source, line, column));
                    c += 3;
                    continue;
                }
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_SHL, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LTEQ, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LT, source, line, column));
            c++;
            continue;
        }
        if (*c == '>') {
            if (*(c + 1) == '>') {
                if (*(c + 2) == '=') {
                    nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_SHR, source, line, column));
                    c += 3;
                    continue;
                }
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_SHR, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_GTEQ, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_GT, source, line, column));
            c++;
            continue;
        }
        if (*c == '!') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_NEQ, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LOGICAL_NOT, source, line, column));
            c++;
            continue;
        }
        if (*c == '|') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_OR, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '|') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LOGICAL_OR, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_OR, source, line, column));
            c++;
            continue;
        }
        if (*c == '&') {
            if (*(c + 1) == '=') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_ASSIGN_AND, source, line, column));
                c += 2;
                continue;
            }
            if (*(c + 1) == '&') {
                nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_LOGICAL_AND, source, line, column));
                c += 2;
                continue;
            }
            nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_AND, source, line, column));
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

        nbl_list_add(tokens, nbl_token_new_int(NBL_TOKEN_UNKNOWN, source, line, column, *c));
        c++;
    }
    nbl_list_add(tokens, nbl_token_new(NBL_TOKEN_EOF, source, line, c - lineStart));
    nbl_source_free(source);
    return tokens;
}

// Value
NblArgument *nbl_argument_new(char *name, NblValueType type, NblNode *defaultNode) {
    NblArgument *argument = malloc(sizeof(NblArgument));
    argument->name = strdup(name);
    argument->type = type;
    argument->defaultNode = defaultNode;
    return argument;
}

void nbl_argument_free(NblArgument *argument) {
    free(argument->name);
    if (argument->defaultNode != NULL) {
        nbl_node_free(argument->defaultNode);
    }
    free(argument);
}

NblValue *nbl_value_new(NblValueType type) {
    NblValue *value = malloc(sizeof(NblValue));
    value->refs = 1;
    value->type = type;
    return value;
}

NblValue *nbl_value_new_null(void) { return nbl_value_new(NBL_VALUE_NULL); }

NblValue *nbl_value_new_bool(bool boolean) {
    NblValue *value = nbl_value_new(NBL_VALUE_BOOL);
    value->boolean = boolean;
    return value;
}

NblValue *nbl_value_new_int(int64_t integer) {
    NblValue *value = nbl_value_new(NBL_VALUE_INT);
    value->integer = integer;
    return value;
}

NblValue *nbl_value_new_float(double floating) {
    NblValue *value = nbl_value_new(NBL_VALUE_FLOAT);
    value->floating = floating;
    return value;
}

NblValue *nbl_value_new_string(char *string) {
    NblValue *value = nbl_value_new(NBL_VALUE_STRING);
    value->string = strdup(string);
    return value;
}

NblValue *nbl_value_new_string_format(char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return nbl_value_new_string(buffer);
}

NblValue *nbl_value_new_array(NblList *array) {
    NblValue *value = nbl_value_new(NBL_VALUE_ARRAY);
    value->array = array;
    return value;
}

NblValue *nbl_value_new_object(NblMap *object) {
    NblValue *value = nbl_value_new(NBL_VALUE_OBJECT);
    value->object = object;
    return value;
}

NblValue *nbl_value_new_class(NblMap *object, NblValue *parentClass, bool abstract) {
    NblValue *value = nbl_value_new(NBL_VALUE_CLASS);
    value->object = object;
    value->parentClass = parentClass;
    value->abstract = abstract;
    return value;
}

NblValue *nbl_value_new_instance(NblMap *object, NblValue *instanceClass) {
    NblValue *value = nbl_value_new(NBL_VALUE_INSTANCE);
    value->object = object;
    value->instanceClass = instanceClass;
    return value;
}

NblValue *nbl_value_new_function(NblList *args, NblValueType returnType, NblNode *functionNode) {
    NblValue *value = nbl_value_new(NBL_VALUE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->functionNode = functionNode;
    return value;
}

NblValue *nbl_value_new_native_function(NblList *args, NblValueType returnType,
                                    NblValue *(*nativeFunc)(NblInterpreterContext *context, NblValue *this, NblList *values)) {
    NblValue *value = nbl_value_new(NBL_VALUE_NATIVE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->nativeFunc = nativeFunc;
    return value;
}

char *nbl_value_type_to_string(NblValueType type) {
    if (type == NBL_VALUE_ANY) return "any";
    if (type == NBL_VALUE_NULL) return "null";
    if (type == NBL_VALUE_BOOL) return "bool";
    if (type == NBL_VALUE_INT) return "int";
    if (type == NBL_VALUE_FLOAT) return "float";
    if (type == NBL_VALUE_STRING) return "string";
    if (type == NBL_VALUE_ARRAY) return "array";
    if (type == NBL_VALUE_OBJECT) return "object";
    if (type == NBL_VALUE_CLASS) return "class";
    if (type == NBL_VALUE_INSTANCE) return "instance";
    if (type == NBL_VALUE_FUNCTION || type == NBL_VALUE_NATIVE_FUNCTION) return "function";
    return NULL;
}

char *nbl_value_to_string(NblValue *value) {
    if (value->type == NBL_VALUE_NULL) {
        return strdup("null");
    }
    if (value->type == NBL_VALUE_BOOL) {
        return strdup(value->boolean ? "true" : "false");
    }
    if (value->type == NBL_VALUE_INT) {
        char buffer[255];
        sprintf(buffer, "%" PRIi64, value->integer);
        return strdup(buffer);
    }
    if (value->type == NBL_VALUE_FLOAT) {
        char buffer[255];
        sprintf(buffer, "%g", value->floating);
        return strdup(buffer);
    }
    if (value->type == NBL_VALUE_STRING) {
        return strdup(value->string);
    }
    if (value->type == NBL_VALUE_ARRAY) {
        NblList *sb = nbl_list_new();
        nbl_list_add(sb, strdup("["));
        if (value->array->size > 0) nbl_list_add(sb, strdup(" "));
        for (size_t i = 0; i < value->array->size; i++) {
            NblValue *item = nbl_list_get(value->array, i);
            nbl_list_add(sb, item != NULL ? nbl_value_to_string(item) : strdup("null"));
            if (i != value->array->size - 1) nbl_list_add(sb, strdup(", "));
        }
        if (value->array->size > 0) nbl_list_add(sb, strdup(" "));
        nbl_list_add(sb, strdup("]"));
        char *string = nbl_list_to_string(sb);
        nbl_list_free(sb, free);
        return string;
    }
    if (value->type == NBL_VALUE_OBJECT || value->type == NBL_VALUE_CLASS || value->type == NBL_VALUE_INSTANCE) {
        NblList *sb = nbl_list_new();
        nbl_list_add(sb, strdup("{"));
        if (value->object->size > 0) nbl_list_add(sb, strdup(" "));
        for (size_t i = 0; i < value->object->size; i++) {
            nbl_list_add(sb, strdup(value->object->keys[i]));
            nbl_list_add(sb, strdup(" = "));
            nbl_list_add(sb, nbl_value_to_string(value->object->values[i]));
            if (i != value->object->size - 1) nbl_list_add(sb, strdup(", "));
        }
        if (value->object->size > 0) nbl_list_add(sb, strdup(" "));
        nbl_list_add(sb, strdup("}"));
        char *string = nbl_list_to_string(sb);
        nbl_list_free(sb, free);
        return string;
    }
    if (value->type == NBL_VALUE_FUNCTION || value->type == NBL_VALUE_NATIVE_FUNCTION) {
        NblList *sb = nbl_list_new();
        nbl_list_add(sb, "fn (");
        for (size_t i = 0; i < value->arguments->size; i++) {
            NblArgument *argument = nbl_list_get(value->arguments, i);
            nbl_list_add(sb, argument->name);
            if (argument->type != NBL_VALUE_ANY) {
                nbl_list_add(sb, ": ");
                nbl_list_add(sb, nbl_value_type_to_string(argument->type));
            }
            if (i != value->arguments->size - 1) nbl_list_add(sb, ", ");
        }
        nbl_list_add(sb, ")");
        if (value->returnType != NBL_VALUE_ANY) {
            nbl_list_add(sb, ": ");
            nbl_list_add(sb, nbl_value_type_to_string(value->returnType));
        }
        char *string = nbl_list_to_string(sb);
        nbl_list_free(sb, NULL);
        return string;
    }
    return NULL;
}

NblValueType nbl_token_type_to_value_type(NblTokenType type) {
    if (type == NBL_TOKEN_TYPE_ANY) return NBL_VALUE_ANY;
    if (type == NBL_TOKEN_NULL) return NBL_VALUE_NULL;
    if (type == NBL_TOKEN_TYPE_BOOL) return NBL_VALUE_BOOL;
    if (type == NBL_TOKEN_TYPE_INT) return NBL_VALUE_INT;
    if (type == NBL_TOKEN_TYPE_FLOAT) return NBL_VALUE_FLOAT;
    if (type == NBL_TOKEN_TYPE_STRING) return NBL_VALUE_STRING;
    if (type == NBL_TOKEN_TYPE_ARRAY) return NBL_VALUE_ARRAY;
    if (type == NBL_TOKEN_TYPE_OBJECT) return NBL_VALUE_OBJECT;
    if (type == NBL_TOKEN_CLASS) return NBL_VALUE_CLASS;
    if (type == NBL_TOKEN_TYPE_INSTANCE) return NBL_VALUE_INSTANCE;
    if (type == NBL_TOKEN_TYPE_FUNCTION) return NBL_VALUE_FUNCTION;
    return 0;
}

NblValue *nbl_value_class_get(NblValue *instance, char *key) {
    NblValue *value = nbl_map_get(instance->object, key);
    if (value != NULL) {
        return value;
    }
    if (instance->parentClass != NULL) {
        return nbl_value_class_get(instance->parentClass, key);
    }
    return NULL;
}

bool nbl_value_class_instanceof(NblValue *instance, NblValue *class) {
    bool result = instance->instanceClass == class;
    if (result) {
        return true;
    }
    if (instance->instanceClass->parentClass != NULL) {
        return nbl_value_class_instanceof(instance->instanceClass, class);
    }
    return false;
}

NblValue *nbl_value_ref(NblValue *value) {
    value->refs++;
    return value;
}

NblValue *nbl_value_retrieve(NblValue *value) {
    if (value->type == NBL_VALUE_NULL) return nbl_value_new_null();
    if (value->type == NBL_VALUE_BOOL) return nbl_value_new_bool(value->boolean);
    if (value->type == NBL_VALUE_INT) return nbl_value_new_int(value->integer);
    if (value->type == NBL_VALUE_FLOAT) return nbl_value_new_float(value->floating);
    if (value->type == NBL_VALUE_STRING) return nbl_value_new_string(value->string);
    return nbl_value_ref(value);
}

void nbl_value_clear(NblValue *value) {
    if (value->type == NBL_VALUE_STRING) {
        free(value->string);
    }
    if (value->type == NBL_VALUE_ARRAY) {
        nbl_list_free(value->array, (NblListFreeFunc *)nbl_value_free);
    }
    if (value->type == NBL_VALUE_OBJECT || value->type == NBL_VALUE_CLASS || value->type == NBL_VALUE_INSTANCE) {
        nbl_map_free(value->object, (NblMapFreeFunc *)nbl_value_free);
        if ((value->type == NBL_VALUE_CLASS || value->type == NBL_VALUE_INSTANCE) && value->parentClass != NULL) {
            nbl_value_free(value->parentClass);
        }
    }
    if (value->type == NBL_VALUE_FUNCTION || value->type == NBL_VALUE_NATIVE_FUNCTION) {
        nbl_list_free(value->arguments, (NblListFreeFunc *)nbl_argument_free);
    }
    if (value->type == NBL_VALUE_FUNCTION) {
        nbl_node_free(value->functionNode);
    }
}

void nbl_value_free(NblValue *value) {
    value->refs--;
    if (value->refs > 0) return;
    nbl_value_clear(value);
    free(value);
}

// Parser
NblNode *nbl_node_new(NblNodeType type, NblToken *token) {
    NblNode *node = malloc(sizeof(NblNode));
    node->refs = 1;
    node->type = type;
    if (token != NULL) {
        node->token = nbl_token_ref(token);
    }
    return node;
}

NblNode *nbl_node_new_value(NblToken *token, NblValue *value) {
    NblNode *node = nbl_node_new(NBL_NODE_VALUE, token);
    node->value = value;
    return node;
}

NblNode *nbl_node_new_string(NblNodeType type, NblToken *token, char *string) {
    NblNode *node = nbl_node_new(type, token);
    node->string = strdup(string);
    return node;
}

NblNode *nbl_node_new_unary(NblNodeType type, NblToken *token, NblNode *unary) {
    NblNode *node = nbl_node_new(type, token);
    node->unary = unary;
    return node;
}

NblNode *nbl_node_new_cast(NblToken *token, NblValueType castType, NblNode *unary) {
    NblNode *node = nbl_node_new(NBL_NODE_CAST, token);
    node->castType = castType;
    node->unary = unary;
    return node;
}

NblNode *nbl_node_new_operation(NblNodeType type, NblToken *token, NblNode *lhs, NblNode *rhs) {
    NblNode *node = nbl_node_new(type, token);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

NblNode *nbl_node_new_multiple(NblNodeType type, NblToken *token) {
    NblNode *node = nbl_node_new(type, token);
    node->nodes = nbl_list_new();
    return node;
}

NblNode *nbl_node_ref(NblNode *node) {
    node->refs++;
    return node;
}

void nbl_node_free(NblNode *node) {
    node->refs--;
    if (node->refs > 0) return;

    if (node->token != NULL) {
        nbl_token_free(node->token);
    }
    if (node->type == NBL_NODE_VALUE) {
        nbl_value_free(node->value);
    }
    if (node->type == NBL_NODE_VARIABLE) {
        free(node->string);
    }
    if (node->type == NBL_NODE_ARRAY) {
        nbl_list_free(node->array, (NblListFreeFunc *)nbl_node_free);
    }
    if (node->type == NBL_NODE_OBJECT || node->type == NBL_NODE_CLASS) {
        nbl_map_free(node->object, (NblMapFreeFunc *)nbl_node_free);
        if (node->type == NBL_NODE_CLASS && node->parentClass != NULL) {
            nbl_node_free(node->parentClass);
        }
    }
    if ((node->type >= NBL_NODE_RETURN && node->type <= NBL_NODE_INCLUDE) || (node->type >= NBL_NODE_NEG && node->type <= NBL_NODE_CAST)) {
        nbl_node_free(node->unary);
    }
    if (node->type >= NBL_NODE_GET && node->type <= NBL_NODE_LOGICAL_OR) {
        nbl_node_free(node->lhs);
        nbl_node_free(node->rhs);
    }
    if (node->type >= NBL_NODE_IF && node->type <= NBL_NODE_FORIN) {
        if (node->condition != NULL) nbl_node_free(node->condition);
        nbl_node_free(node->thenBlock);
        if (node->elseBlock != NULL) nbl_node_free(node->elseBlock);
        if (node->type == NBL_NODE_TRY && node->finallyBlock != NULL) nbl_node_free(node->finallyBlock);
    }
    if ((node->type >= NBL_NODE_PROGRAM && node->type <= NBL_NODE_BLOCK) || node->type == NBL_NODE_CALL) {
        if (node->type == NBL_NODE_CALL) nbl_node_free(node->function);
        nbl_list_free(node->nodes, (NblListFreeFunc *)nbl_node_free);
    }
    free(node);
}

NblNode *nbl_parser(NblList *tokens, bool included) {
    NblParser nbl_parser = {.tokens = tokens, .position = 0};
    return nbl_parser_program(&nbl_parser, included);
}

#define current() ((NblToken *)nbl_list_get(nbl_parser->tokens, nbl_parser->position))
#define next(pos) ((NblToken *)nbl_list_get(nbl_parser->tokens, nbl_parser->position + 1 + pos))

void nbl_parser_eat(NblParser *nbl_parser, NblTokenType type) {
    if (current()->type == type) {
        nbl_parser->position++;
    } else {
        nbl_print_error(current(), "Unexpected token: '%s' needed '%s'", nbl_token_type_to_string(current()->type), nbl_token_type_to_string(type));
        exit(EXIT_FAILURE);
    }
}

NblValueType nbl_parser_eat_type(NblParser *nbl_parser) {
    if (nbl_token_type_is_type(current()->type)) {
        NblValueType type = nbl_token_type_to_value_type(current()->type);
        nbl_parser->position++;
        return type;
    }
    nbl_print_error(current(), "Unexpected token: '%s' needed type token", nbl_token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
    return NBL_VALUE_ANY;
}

NblNode *nbl_parser_program(NblParser *nbl_parser, bool included) {
    NblNode *programNode = nbl_node_new_multiple(included ? NBL_NODE_NODES : NBL_NODE_PROGRAM, current());
    while (current()->type != NBL_TOKEN_EOF) {
        NblNode *node = nbl_parser_statement(nbl_parser);
        if (node != NULL) nbl_list_add(programNode->nodes, node);
    }
    return programNode;
}

NblNode *nbl_parser_block(NblParser *nbl_parser) {
    NblNode *blockNode = nbl_node_new_multiple(NBL_NODE_BLOCK, current());
    if (current()->type == NBL_TOKEN_LCURLY) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LCURLY);
        while (current()->type != NBL_TOKEN_RCURLY) {
            NblNode *node = nbl_parser_statement(nbl_parser);
            if (node != NULL) nbl_list_add(blockNode->nodes, node);
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RCURLY);
    } else {
        NblNode *node = nbl_parser_statement(nbl_parser);
        if (node != NULL) nbl_list_add(blockNode->nodes, node);
    }
    return blockNode;
}

NblNode *nbl_parser_statement(NblParser *nbl_parser) {
    if (current()->type == NBL_TOKEN_SEMICOLON) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return NULL;
    }

    if (current()->type == NBL_TOKEN_LCURLY) {
        return nbl_parser_block(nbl_parser);
    }

    if (current()->type == NBL_TOKEN_IF) {
        NblNode *node = nbl_node_new(NBL_NODE_IF, current());
        nbl_parser_eat(nbl_parser, NBL_TOKEN_IF);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        node->thenBlock = nbl_parser_block(nbl_parser);
        if (current()->type == NBL_TOKEN_ELSE) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_ELSE);
            node->elseBlock = nbl_parser_block(nbl_parser);
        } else {
            node->elseBlock = NULL;
        }
        return node;
    }

    if (current()->type == NBL_TOKEN_TRY) {
        NblNode *node = nbl_node_new(NBL_NODE_TRY, current());
        nbl_parser_eat(nbl_parser, NBL_TOKEN_TRY);
        node->tryBlock = nbl_parser_block(nbl_parser);

        nbl_parser_eat(nbl_parser, NBL_TOKEN_CATCH);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        NblNode *declarations = nbl_parser_declarations(nbl_parser);
        if (declarations->type != NBL_NODE_CONST_ASSIGN && declarations->type != NBL_NODE_LET_ASSIGN) {
            nbl_print_error(declarations->token, "You can only declare one variable in a catch block");
            exit(EXIT_FAILURE);
        }
        node->catchVariable = declarations;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        node->catchBlock = nbl_parser_block(nbl_parser);

        if (current()->type == NBL_TOKEN_FINALLY) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_FINALLY);
            node->finallyBlock = nbl_parser_block(nbl_parser);
        } else {
            node->finallyBlock = NULL;
        }
        return node;
    }

    if (current()->type == NBL_TOKEN_LOOP) {
        NblNode *node = nbl_node_new(NBL_NODE_LOOP, current());
        node->elseBlock = NULL;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LOOP);
        node->condition = NULL;
        node->thenBlock = nbl_parser_block(nbl_parser);
        return node;
    }

    if (current()->type == NBL_TOKEN_WHILE) {
        NblNode *node = nbl_node_new(NBL_NODE_WHILE, current());
        node->elseBlock = NULL;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_WHILE);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        node->thenBlock = nbl_parser_block(nbl_parser);
        return node;
    }

    if (current()->type == NBL_TOKEN_DO) {
        NblNode *node = nbl_node_new(NBL_NODE_DOWHILE, current());
        node->elseBlock = NULL;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_DO);
        node->thenBlock = nbl_parser_block(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_WHILE);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == NBL_TOKEN_FOR) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FOR);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        NblNode *declarations;
        if (current()->type != NBL_TOKEN_SEMICOLON) {
            declarations = nbl_parser_declarations(nbl_parser);
        }

        if (current()->type == NBL_TOKEN_IN) {
            NblNode *node = nbl_node_new(NBL_NODE_FORIN, token);
            if (declarations->type != NBL_NODE_CONST_ASSIGN && declarations->type != NBL_NODE_LET_ASSIGN) {
                nbl_print_error(declarations->token, "You can only declare one variable in a for in loop");
                exit(EXIT_FAILURE);
            }
            node->forinVariable = declarations;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_IN);
            node->iterator = nbl_parser_tenary(nbl_parser);
            nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
            node->thenBlock = nbl_parser_block(nbl_parser);
            return node;
        }

        NblNode *blockNode = nbl_node_new_multiple(NBL_NODE_BLOCK, token);
        nbl_list_add(blockNode->nodes, declarations);

        NblNode *node = nbl_node_new(NBL_NODE_FOR, token);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        if (current()->type != NBL_TOKEN_SEMICOLON) {
            node->condition = nbl_parser_tenary(nbl_parser);
        } else {
            node->condition = NULL;
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);

        if (current()->type != NBL_TOKEN_RPAREN) {
            node->incrementBlock = nbl_parser_assigns(nbl_parser);
        } else {
            node->incrementBlock = NULL;
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);

        node->thenBlock = nbl_parser_block(nbl_parser);
        nbl_list_add(blockNode->nodes, node);
        return blockNode;
    }

    if (current()->type == NBL_TOKEN_CONTINUE) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_CONTINUE);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return nbl_node_new(NBL_NODE_CONTINUE, token);
    }
    if (current()->type == NBL_TOKEN_BREAK) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_BREAK);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return nbl_node_new(NBL_NODE_BREAK, token);
    }
    if (current()->type == NBL_TOKEN_RETURN) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RETURN);
        NblNode *node = nbl_node_new_unary(NBL_NODE_RETURN, token, nbl_parser_tenary(nbl_parser));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == NBL_TOKEN_THROW) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_THROW);
        NblNode *node = nbl_node_new_unary(NBL_NODE_THROW, token, nbl_parser_tenary(nbl_parser));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return node;
    }
    if (current()->type == NBL_TOKEN_INCLUDE) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_INCLUDE);
        NblNode *node = nbl_node_new_unary(NBL_NODE_INCLUDE, token, nbl_parser_tenary(nbl_parser));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == NBL_TOKEN_FUNCTION) {
        NblToken *functionToken = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FUNCTION);
        NblToken *nameToken = current();
        char *name = current()->string;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
        NblNode *node = nbl_node_new_operation(NBL_NODE_CONST_ASSIGN, functionToken, nbl_node_new_string(NBL_NODE_VARIABLE, nameToken, name),
                                           nbl_parser_function(nbl_parser, functionToken));
        node->declarationType = NBL_VALUE_FUNCTION;
        return node;
    }
    if (current()->type == NBL_TOKEN_ABSTRACT || current()->type == NBL_TOKEN_CLASS) {
        NblToken *classToken = current();
        bool abstract = false;
        if (current()->type == NBL_TOKEN_ABSTRACT) {
            abstract = true;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_ABSTRACT);
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_CLASS);
        NblToken *nameToken = current();
        char *name = current()->string;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
        NblNode *node = nbl_node_new_operation(NBL_NODE_CONST_ASSIGN, classToken, nbl_node_new_string(NBL_NODE_VARIABLE, nameToken, name),
                                           nbl_parser_class(nbl_parser, classToken, abstract));
        node->declarationType = NBL_VALUE_CLASS;
        return node;
    }

    NblNode *node = nbl_parser_declarations(nbl_parser);
    nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
    return node;
}

NblNode *nbl_parser_declarations(NblParser *nbl_parser) {
    if (current()->type == NBL_TOKEN_CONST || current()->type == NBL_TOKEN_LET) {
        NblList *nodes = nbl_list_new();
        NblNodeType assignType;
        if (current()->type == NBL_TOKEN_CONST) {
            assignType = NBL_NODE_CONST_ASSIGN;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_CONST);
        }
        if (current()->type == NBL_TOKEN_LET) {
            assignType = NBL_NODE_LET_ASSIGN;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LET);
        }

        for (;;) {
            char *name = current()->string;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
            NblNode *variable = nbl_node_new_string(NBL_NODE_VARIABLE, current(), name);

            NblValueType declarationType = NBL_VALUE_ANY;
            if (current()->type == NBL_TOKEN_COLON) {
                nbl_parser_eat(nbl_parser, NBL_TOKEN_COLON);
                declarationType = nbl_parser_eat_type(nbl_parser);
            }

            NblNode *node;
            if (current()->type == NBL_TOKEN_ASSIGN) {
                NblToken *token = current();
                nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN);
                node = nbl_node_new_operation(assignType, token, variable, nbl_parser_assign(nbl_parser));
            } else {
                node = nbl_node_new_operation(assignType, variable->token, variable, nbl_node_new_value(variable->token, nbl_value_new_null()));
            }
            node->declarationType = declarationType;
            nbl_list_add(nodes, node);

            if (current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }
        if (nodes->size == 0) {
            nbl_list_free(nodes, NULL);
            return NULL;
        }
        if (nodes->size == 1) {
            NblNode *firstNode = nbl_list_get(nodes, 0);
            nbl_list_free(nodes, NULL);
            return firstNode;
        }
        NblNode *nodesNode = nbl_node_new(NBL_NODE_NODES, current());
        nodesNode->nodes = nodes;
        return nodesNode;
    }
    return nbl_parser_assigns(nbl_parser);
}

NblNode *nbl_parser_assigns(NblParser *nbl_parser) {
    NblList *nodes = nbl_list_new();
    for (;;) {
        NblNode *node = nbl_parser_assign(nbl_parser);
        nbl_list_add(nodes, node);
        if (current()->type == NBL_TOKEN_COMMA) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
        } else {
            break;
        }
    }
    if (nodes->size == 0) {
        nbl_list_free(nodes, NULL);
        return NULL;
    }
    if (nodes->size == 1) {
        NblNode *firstNode = nbl_list_get(nodes, 0);
        nbl_list_free(nodes, NULL);
        return firstNode;
    }
    NblNode *nodesNode = nbl_node_new(NBL_NODE_NODES, current());
    nodesNode->nodes = nodes;
    return nodesNode;
}

NblNode *nbl_parser_assign(NblParser *nbl_parser) {
    NblNode *lhs = nbl_parser_tenary(nbl_parser);  // TODO
    if (current()->type == NBL_TOKEN_ASSIGN) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_parser_assign(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_ADD) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_ADD);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_ADD, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_SUB) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_SUB);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_SUB, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_MUL) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_MUL);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_MUL, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_EXP) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_EXP);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_EXP, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_MOD) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_MOD);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_MOD, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_AND) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_AND);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_AND, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_XOR) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_XOR);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_XOR, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_OR) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_OR);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_OR, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_SHL) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_SHL);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_SHL, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    if (current()->type == NBL_TOKEN_ASSIGN_SHR) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN_SHR);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, token, lhs, nbl_node_new_operation(NBL_NODE_SHR, token, nbl_node_ref(lhs), nbl_parser_assign(nbl_parser)));
    }
    return lhs;
}

NblNode *nbl_parser_tenary(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_logical(nbl_parser);
    if (current()->type == NBL_TOKEN_QUESTION) {
        NblNode *tenaryNode = nbl_node_new(NBL_NODE_TENARY, current());
        nbl_parser_eat(nbl_parser, NBL_TOKEN_QUESTION);
        tenaryNode->condition = node;
        tenaryNode->thenBlock = nbl_parser_tenary(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_COLON);
        tenaryNode->elseBlock = nbl_parser_tenary(nbl_parser);
        return tenaryNode;
    }
    return node;
}

NblNode *nbl_parser_logical(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_equality(nbl_parser);
    while (current()->type == NBL_TOKEN_LOGICAL_AND || current()->type == NBL_TOKEN_LOGICAL_OR) {
        if (current()->type == NBL_TOKEN_LOGICAL_AND) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LOGICAL_AND);
            node = nbl_node_new_operation(NBL_NODE_LOGICAL_AND, token, node, nbl_parser_equality(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_LOGICAL_OR) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LOGICAL_OR);
            node = nbl_node_new_operation(NBL_NODE_LOGICAL_OR, token, node, nbl_parser_equality(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_equality(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_relational(nbl_parser);
    while (current()->type == NBL_TOKEN_EQ || current()->type == NBL_TOKEN_NEQ) {
        if (current()->type == NBL_TOKEN_EQ) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_EQ);
            node = nbl_node_new_operation(NBL_NODE_EQ, token, node, nbl_parser_relational(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_NEQ) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_NEQ);
            node = nbl_node_new_operation(NBL_NODE_NEQ, token, node, nbl_parser_relational(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_relational(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_instanceof(nbl_parser);
    while (current()->type == NBL_TOKEN_LT || current()->type == NBL_TOKEN_LTEQ || current()->type == NBL_TOKEN_GT || current()->type == NBL_TOKEN_GTEQ) {
        if (current()->type == NBL_TOKEN_LT) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LT);
            node = nbl_node_new_operation(NBL_NODE_LT, token, node, nbl_parser_instanceof(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_LTEQ) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LTEQ);
            node = nbl_node_new_operation(NBL_NODE_LTEQ, token, node, nbl_parser_instanceof(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_GT) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_GT);
            node = nbl_node_new_operation(NBL_NODE_GT, token, node, nbl_parser_instanceof(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_GTEQ) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_GTEQ);
            node = nbl_node_new_operation(NBL_NODE_GTEQ, token, node, nbl_parser_instanceof(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_instanceof(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_bitwise(nbl_parser);
    while (current()->type == NBL_TOKEN_INSTANCEOF) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_INSTANCEOF);
        node = nbl_node_new_operation(NBL_NODE_INSTANCEOF, token, node, nbl_parser_bitwise(nbl_parser));
    }
    return node;
}

NblNode *nbl_parser_bitwise(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_shift(nbl_parser);
    while (current()->type == NBL_TOKEN_AND || current()->type == NBL_TOKEN_XOR || current()->type == NBL_TOKEN_OR) {
        if (current()->type == NBL_TOKEN_AND) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_AND);
            node = nbl_node_new_operation(NBL_NODE_AND, token, node, nbl_parser_shift(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_XOR) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_XOR);
            node = nbl_node_new_operation(NBL_NODE_XOR, token, node, nbl_parser_shift(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_OR) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_OR);
            node = nbl_node_new_operation(NBL_NODE_OR, token, node, nbl_parser_shift(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_shift(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_add(nbl_parser);
    while (current()->type == NBL_TOKEN_SHL || current()->type == NBL_TOKEN_SHR) {
        if (current()->type == NBL_TOKEN_SHL) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_SHL);
            node = nbl_node_new_operation(NBL_NODE_SHL, token, node, nbl_parser_add(nbl_parser));
        }

        if (current()->type == NBL_TOKEN_SHR) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_SHR);
            node = nbl_node_new_operation(NBL_NODE_SHR, token, node, nbl_parser_add(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_add(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_mul(nbl_parser);
    while (current()->type == NBL_TOKEN_ADD || current()->type == NBL_TOKEN_SUB) {
        if (current()->type == NBL_TOKEN_ADD) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_ADD);
            node = nbl_node_new_operation(NBL_NODE_ADD, token, node, nbl_parser_mul(nbl_parser));
        }

        if (current()->type == NBL_TOKEN_SUB) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_SUB);
            node = nbl_node_new_operation(NBL_NODE_SUB, token, node, nbl_parser_mul(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_mul(NblParser *nbl_parser) {
    NblNode *node = nbl_parser_unary(nbl_parser);
    while (current()->type == NBL_TOKEN_MUL || current()->type == NBL_TOKEN_EXP || current()->type == NBL_TOKEN_DIV || current()->type == NBL_TOKEN_MOD) {
        if (current()->type == NBL_TOKEN_MUL) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_MUL);
            node = nbl_node_new_operation(NBL_NODE_MUL, token, node, nbl_parser_unary(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_EXP) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_EXP);
            node = nbl_node_new_operation(NBL_NODE_EXP, token, node, nbl_parser_unary(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_DIV) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_DIV);
            node = nbl_node_new_operation(NBL_NODE_DIV, token, node, nbl_parser_unary(nbl_parser));
        }
        if (current()->type == NBL_TOKEN_MOD) {
            NblToken *token = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_MOD);
            node = nbl_node_new_operation(NBL_NODE_MOD, token, node, nbl_parser_unary(nbl_parser));
        }
    }
    return node;
}

NblNode *nbl_parser_unary(NblParser *nbl_parser) {
    if (current()->type == NBL_TOKEN_ADD) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ADD);
        return nbl_parser_unary(nbl_parser);
    }
    if (current()->type == NBL_TOKEN_SUB) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_SUB);
        return nbl_node_new_unary(NBL_NODE_NEG, token, nbl_parser_unary(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_INC) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_INC);
        return nbl_node_new_unary(NBL_NODE_INC_PRE, token, nbl_parser_unary(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_DEC) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_DEC);
        return nbl_node_new_unary(NBL_NODE_DEC_PRE, token, nbl_parser_unary(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_NOT) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_NOT);
        return nbl_node_new_unary(NBL_NODE_NOT, token, nbl_parser_unary(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_LOGICAL_NOT) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LOGICAL_NOT);
        return nbl_node_new_unary(NBL_NODE_LOGICAL_NOT, token, nbl_parser_unary(nbl_parser));
    }
    if (current()->type == NBL_TOKEN_LPAREN && nbl_token_type_is_type(next(0)->type) && next(1)->type == NBL_TOKEN_RPAREN) {
        NblToken *token = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        NblValueType castType = nbl_parser_eat_type(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        return nbl_node_new_cast(token, castType, nbl_parser_unary(nbl_parser));
    }
    return nbl_parser_primary(nbl_parser);
}

NblNode *nbl_parser_primary(NblParser *nbl_parser) {
    if (current()->type == NBL_TOKEN_LPAREN) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
        NblNode *node = nbl_parser_tenary(nbl_parser);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_NULL) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new(NBL_VALUE_NULL));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_NULL);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_TRUE) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new_bool(true));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_TRUE);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_FALSE) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new_bool(false));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FALSE);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_INT) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new_int(current()->integer));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_INT);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_FLOAT) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new_float(current()->floating));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FLOAT);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_STRING) {
        NblNode *node = nbl_node_new_value(current(), nbl_value_new_string(current()->string));
        nbl_parser_eat(nbl_parser, NBL_TOKEN_STRING);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_KEYWORD) {
        NblToken *nameToken = current();
        char *name = current()->string;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
        return nbl_parser_primary_suffix(nbl_parser, nbl_node_new_string(NBL_NODE_VARIABLE, nameToken, name));
    }
    if (current()->type == NBL_TOKEN_LBRACKET) {
        NblNode *node = nbl_node_new(NBL_NODE_ARRAY, current());
        node->array = nbl_list_new();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LBRACKET);
        while (current()->type != NBL_TOKEN_RBRACKET) {
            nbl_list_add(node->array, nbl_parser_assign(nbl_parser));
            if (current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RBRACKET);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_LCURLY) {
        NblNode *node = nbl_node_new(NBL_NODE_OBJECT, current());
        node->object = nbl_map_new();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_LCURLY);
        while (current()->type != NBL_TOKEN_RCURLY) {
            if (current()->type == NBL_TOKEN_FUNCTION) {
                NblToken *functionToken = current();
                nbl_parser_eat(nbl_parser, NBL_TOKEN_FUNCTION);
                char *keyName = current()->string;
                nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
                nbl_map_set(node->object, keyName, nbl_parser_function(nbl_parser, functionToken));
                continue;
            }

            NblToken *keyToken = current();
            char *keyName = current()->string;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
            if (current()->type == NBL_TOKEN_ASSIGN) {
                nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN);
                nbl_map_set(node->object, keyName, nbl_parser_tenary(nbl_parser));
            } else {
                nbl_map_set(node->object, keyName, nbl_node_new_string(NBL_NODE_VARIABLE, keyToken, keyName));
            }
            if (current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_RCURLY);
        return nbl_parser_primary_suffix(nbl_parser, node);
    }
    if (current()->type == NBL_TOKEN_FUNCTION) {
        NblToken *functionToken = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FUNCTION);
        return nbl_parser_primary_suffix(nbl_parser, nbl_parser_function(nbl_parser, functionToken));
    }
    if (current()->type == NBL_TOKEN_ABSTRACT || current()->type == NBL_TOKEN_CLASS) {
        NblToken *classToken = current();
        bool abstract = false;
        if (current()->type == NBL_TOKEN_ABSTRACT) {
            abstract = true;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_ABSTRACT);
        }
        nbl_parser_eat(nbl_parser, NBL_TOKEN_CLASS);
        return nbl_parser_class(nbl_parser, classToken, abstract);
    }

    nbl_print_error(current(), "Unexpected token: '%s'", nbl_token_type_to_string(current()->type));
    exit(EXIT_FAILURE);
    return NULL;
}

NblNode *nbl_parser_primary_suffix(NblParser *nbl_parser, NblNode *node) {
    while (current()->type == NBL_TOKEN_LBRACKET || current()->type == NBL_TOKEN_POINT || current()->type == NBL_TOKEN_LPAREN ||
           current()->type == NBL_TOKEN_INC || current()->type == NBL_TOKEN_DEC) {
        NblToken *token = current();
        if (current()->type == NBL_TOKEN_LBRACKET) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LBRACKET);
            NblNode *indexOrKey = nbl_parser_assign(nbl_parser);
            nbl_parser_eat(nbl_parser, NBL_TOKEN_RBRACKET);
            node = nbl_node_new_operation(NBL_NODE_GET, token, node, indexOrKey);
        }
        if (current()->type == NBL_TOKEN_POINT) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_POINT);
            NblToken *keyToken = current();
            char *key = current()->string;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
            node = nbl_node_new_operation(NBL_NODE_GET, token, node, nbl_node_new_value(keyToken, nbl_value_new_string(key)));
        }
        if (current()->type == NBL_TOKEN_LPAREN) {
            NblNode *callNode = nbl_node_new_multiple(NBL_NODE_CALL, current());
            callNode->function = node;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
            while (current()->type != NBL_TOKEN_RPAREN) {
                nbl_list_add(callNode->nodes, nbl_parser_assign(nbl_parser));
                if (current()->type == NBL_TOKEN_COMMA) {
                    nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
                } else {
                    break;
                }
            }
            nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);
            node = callNode;
        }
        if (current()->type == NBL_TOKEN_INC) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_INC);
            node = nbl_node_new_unary(NBL_NODE_INC_POST, token, node);
        }
        if (current()->type == NBL_TOKEN_DEC) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_DEC);
            node = nbl_node_new_unary(NBL_NODE_DEC_POST, token, node);
        }
    }
    return node;
}

NblNode *nbl_parser_function(NblParser *nbl_parser, NblToken *token) {
    NblList *arguments = nbl_list_new();
    nbl_parser_eat(nbl_parser, NBL_TOKEN_LPAREN);
    while (current()->type != NBL_TOKEN_RPAREN) {
        nbl_list_add(arguments, nbl_parser_argument(nbl_parser));
        if (current()->type == NBL_TOKEN_COMMA) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
        } else {
            break;
        }
    }
    nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);

    NblValueType returnType = NBL_VALUE_ANY;
    if (current()->type == NBL_TOKEN_COLON) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_COLON);
        returnType = nbl_parser_eat_type(nbl_parser);
    }

    if (current()->type == NBL_TOKEN_FAT_ARROW) {
        NblToken *fatArrowToken = current();
        nbl_parser_eat(nbl_parser, NBL_TOKEN_FAT_ARROW);
        return nbl_node_new_value(token, nbl_value_new_function(arguments, returnType, nbl_node_new_unary(NBL_NODE_RETURN, fatArrowToken, nbl_parser_tenary(nbl_parser))));
    }
    return nbl_node_new_value(token, nbl_value_new_function(arguments, returnType, nbl_parser_block(nbl_parser)));
}

NblNode *nbl_parser_class(NblParser *nbl_parser, NblToken *token, bool abstract) {
    NblNode *parentClass = NULL;
    if (current()->type == NBL_TOKEN_EXTENDS) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_EXTENDS);
        parentClass = nbl_parser_tenary(nbl_parser);
    }

    NblMap *object = nbl_map_new();
    nbl_parser_eat(nbl_parser, NBL_TOKEN_LCURLY);
    while (current()->type != NBL_TOKEN_RCURLY) {
        if (current()->type == NBL_TOKEN_FUNCTION) {
            NblToken *functionToken = current();
            nbl_parser_eat(nbl_parser, NBL_TOKEN_FUNCTION);
            char *keyName = current()->string;
            nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
            nbl_map_set(object, keyName, nbl_parser_function(nbl_parser, functionToken));
            continue;
        }

        char *keyName = current()->string;
        nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN);
        nbl_map_set(object, keyName, nbl_parser_tenary(nbl_parser));

        if (current()->type == NBL_TOKEN_COMMA) {
            nbl_parser_eat(nbl_parser, NBL_TOKEN_COMMA);
        } else {
            break;
        }
    }
    nbl_parser_eat(nbl_parser, NBL_TOKEN_RCURLY);

    NblNode *classNode = nbl_node_new(NBL_NODE_CLASS, token);
    classNode->object = object;
    classNode->parentClass = parentClass;
    classNode->abstract = abstract;
    return classNode;
}

NblArgument *nbl_parser_argument(NblParser *nbl_parser) {
    char *name = current()->string;
    nbl_parser_eat(nbl_parser, NBL_TOKEN_KEYWORD);
    NblValueType type = NBL_VALUE_ANY;
    if (current()->type == NBL_TOKEN_COLON) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_COLON);
        type = nbl_parser_eat_type(nbl_parser);
    }
    NblNode *defaultNode = NULL;
    if (current()->type == NBL_TOKEN_ASSIGN) {
        nbl_parser_eat(nbl_parser, NBL_TOKEN_ASSIGN);
        defaultNode = nbl_parser_tenary(nbl_parser);
    }
    return nbl_argument_new(name, type, defaultNode);
}

// Standard library

// Math
static NblValue *env_math_abs(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    if (x->type == NBL_VALUE_INT) {
        return nbl_value_new_int(x->integer < 0 ? -x->integer : x->integer);
    }
    if (x->type == NBL_VALUE_FLOAT) {
        return nbl_value_new_float(x->floating < 0 ? -x->floating : x->floating);
    }
    return nbl_value_new_null();
}
static NblValue *env_math_sin(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(sin(x->floating));
}
static NblValue *env_math_cos(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(cos(x->floating));
}
static NblValue *env_math_tan(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(tan(x->floating));
}
static NblValue *env_math_asin(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(asin(x->floating));
}
static NblValue *env_math_acos(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(acos(x->floating));
}
static NblValue *env_math_atan(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(atan(x->floating));
}
static NblValue *env_math_atan2(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *y = nbl_list_get(values, 0);
    NblValue *x = nbl_list_get(values, 1);
    return nbl_value_new_float(atan2(y->floating, x->floating));
}
static NblValue *env_math_pow(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    NblValue *y = nbl_list_get(values, 1);
    return nbl_value_new_float(pow(x->floating, y->floating));
}
static NblValue *env_math_sqrt(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(sqrt(x->floating));
}
static NblValue *env_math_floor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(floor(x->floating));
}
static NblValue *env_math_ceil(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(ceil(x->floating));
}
static NblValue *env_math_round(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(round(x->floating));
}
static NblValue *env_math_min(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = nbl_list_get(values, 0);
    int64_t minInteger;
    double minFloating;
    if (first->type == NBL_VALUE_INT) {
        minInteger = first->integer;
        minFloating = first->integer;
    }
    if (first->type == NBL_VALUE_FLOAT) {
        minInteger = first->floating;
        minFloating = first->floating;
    }

    bool onlyInteger = true;
    nbl_list_foreach(values, NblValue * value, {
        if (value->type != NBL_VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == NBL_VALUE_INT) {
                minInteger = MIN(minInteger, value->integer);
            }
            if (value->type == NBL_VALUE_FLOAT) {
                minInteger = MIN(minInteger, value->floating);
            }
        }
        if (value->type == NBL_VALUE_INT) {
            minFloating = MIN(minFloating, value->integer);
        }
        if (value->type == NBL_VALUE_FLOAT) {
            minFloating = MIN(minFloating, value->floating);
        }
    });
    return onlyInteger ? nbl_value_new_int(minInteger) : nbl_value_new_float(minFloating);
}
static NblValue *env_math_max(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = nbl_list_get(values, 0);
    int64_t maxInteger;
    double maxFloating;
    if (first->type == NBL_VALUE_INT) {
        maxInteger = first->integer;
        maxFloating = first->integer;
    }
    if (first->type == NBL_VALUE_FLOAT) {
        maxInteger = first->floating;
        maxFloating = first->floating;
    }

    bool onlyInteger = true;
    nbl_list_foreach(values, NblValue * value, {
        if (value->type != NBL_VALUE_INT) onlyInteger = false;
        if (onlyInteger) {
            if (value->type == NBL_VALUE_INT) {
                maxInteger = MAX(maxInteger, value->integer);
            }
            if (value->type == NBL_VALUE_FLOAT) {
                maxInteger = MAX(maxInteger, value->floating);
            }
        }
        if (value->type == NBL_VALUE_INT) {
            maxFloating = MAX(maxFloating, value->integer);
        }
        if (value->type == NBL_VALUE_FLOAT) {
            maxFloating = MAX(maxFloating, value->floating);
        }
    });
    return onlyInteger ? nbl_value_new_int(maxInteger) : nbl_value_new_float(maxFloating);
}
static NblValue *env_math_exp(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(exp(x->floating));
}
static NblValue *env_math_log(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *x = nbl_list_get(values, 0);
    return nbl_value_new_float(log(x->floating));
}
static NblValue *env_math_random(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    (void)values;
    return nbl_value_new_float(nbl_random_random());
}

// Exception
static NblValue *env_exception_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *error = nbl_list_get(values, 0);
    nbl_map_set(this->object, "error", nbl_value_retrieve(error));
    nbl_map_set(this->object, "path", nbl_value_new_string(context->node->token->source->path));
    nbl_map_set(this->object, "text", nbl_value_new_string(context->node->token->source->text));
    nbl_map_set(this->object, "line", nbl_value_new_int(context->node->token->line));
    nbl_map_set(this->object, "column", nbl_value_new_int(context->node->token->column));
    return nbl_value_new_null();
}

// String
static NblValue *env_string_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = nbl_list_get(values, 0);
    char *string = nbl_value_to_string(first);
    NblValue *value = nbl_value_new_string(string);
    free(string);
    return value;
}
static NblValue *env_string_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return nbl_value_new_int(strlen(this->string));
}

// Array
static NblValue *env_array_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = nbl_list_get(values, 0);
    if (first != NULL && first->type == NBL_VALUE_ARRAY) {
        return nbl_value_ref(first);
    }
    return nbl_value_new_array(nbl_list_new());
}
static NblValue *env_array_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return nbl_value_new_int(this->array->size);
}
static NblValue *env_array_push(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    nbl_list_foreach(values, NblValue * value, { nbl_list_add(this->array, nbl_value_retrieve(value)); });
    return nbl_value_new_int(this->array->size);
}
static NblValue *env_array_foreach(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = nbl_list_get(values, 0);
    nbl_list_foreach(this->array, NblValue * value, {
        NblList *arguments = nbl_list_new();
        nbl_list_add(arguments, nbl_value_retrieve(value));
        nbl_list_add(arguments, nbl_value_new_int(index));
        nbl_list_add(arguments, nbl_value_ref(this));
        nbl_interpreter_call(context, function, NULL, arguments);
        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
    });
    return nbl_value_new_null();
}
static NblValue *env_array_map(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = nbl_list_get(values, 0);
    NblList *items = nbl_list_new();
    nbl_list_foreach(this->array, NblValue * value, {
        NblList *arguments = nbl_list_new();
        nbl_list_add(arguments, nbl_value_retrieve(value));
        nbl_list_add(arguments, nbl_value_new_int(index));
        nbl_list_add(arguments, nbl_value_ref(this));
        nbl_list_add(items, nbl_interpreter_call(context, function, NULL, arguments));
        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
    });
    return nbl_value_new_array(items);
}
static NblValue *env_array_filter(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = nbl_list_get(values, 0);
    NblList *items = nbl_list_new();
    nbl_list_foreach(this->array, NblValue * value, {
        NblList *arguments = nbl_list_new();
        nbl_list_add(arguments, nbl_value_retrieve(value));
        nbl_list_add(arguments, nbl_value_new_int(index));
        nbl_list_add(arguments, nbl_value_ref(this));
        NblValue *returnValue = nbl_interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != NBL_VALUE_BOOL) {
            return nbl_interpreter_throw(context,
                                     nbl_value_new_string_format("Array filter condition type is not a bool it is: %s", nbl_value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            nbl_list_add(items, value);
        }
        nbl_value_free(returnValue);
        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
    });
    return nbl_value_new_array(items);
}
static NblValue *env_array_find(NblInterpreterContext *context, NblValue *this, NblList *values) {
    NblValue *function = nbl_list_get(values, 0);
    nbl_list_foreach(this->array, NblValue * value, {
        NblList *arguments = nbl_list_new();
        nbl_list_add(arguments, nbl_value_retrieve(value));
        nbl_list_add(arguments, nbl_value_new_int(index));
        nbl_list_add(arguments, nbl_value_ref(this));
        NblValue *returnValue = nbl_interpreter_call(context, function, NULL, arguments);
        if (returnValue->type != NBL_VALUE_BOOL) {
            return nbl_interpreter_throw(context,
                                     nbl_value_new_string_format("Array find condition type is not a bool it is: %s", nbl_value_type_to_string(returnValue->type)));
        }
        if (returnValue->boolean) {
            nbl_value_free(returnValue);
            nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
            return nbl_value_retrieve(value);
        }
        nbl_value_free(returnValue);
        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
    });
    return nbl_value_new_null();
}

// Object
static NblValue *env_object_constructor(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *first = nbl_list_get(values, 0);
    if (first != NULL && first->type == NBL_VALUE_OBJECT) {
        return nbl_value_ref(first);
    }
    return nbl_value_new_object(nbl_map_new());
}
static NblValue *env_object_length(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    return nbl_value_new_int(this->object->size);
}
static NblValue *env_object_keys(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    NblList *items = nbl_list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        nbl_list_add(items, nbl_value_new_string(this->object->keys[i]));
    }
    return nbl_value_new_array(items);
}
static NblValue *env_object_values(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)values;
    NblList *items = nbl_list_new_with_capacity(this->object->capacity);
    for (size_t i = 0; i < this->object->size; i++) {
        nbl_list_add(items, nbl_value_retrieve(this->object->values[i]));
    }
    return nbl_value_new_array(items);
}

// Date
static NblValue *env_date_now(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    (void)values;
    return nbl_value_new_int(nbl_time_ms());
}

// Root
static NblValue *env_type(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *value = nbl_list_get(values, 0);
    return nbl_value_new_string(nbl_value_type_to_string(value->type));
}

static NblValue *env_assert(NblInterpreterContext *context, NblValue *this, NblList *values) {
    (void)context;
    (void)this;
    NblValue *assertion = nbl_list_get(values, 0);
    if (!assertion->boolean) {
        return nbl_interpreter_throw(context, nbl_value_new_string("Assertion failed"));
    }
    return nbl_value_new_null();
}

NblMap *nbl_std_env(void) {
    NblMap *env = nbl_map_new();

    NblList *empty_args = nbl_list_new();

    // Math
    NblMap *math = nbl_map_new();
    nbl_map_set(env, "Math", nbl_variable_new(NBL_VALUE_OBJECT, false, nbl_value_new_object(math)));
    nbl_map_set(math, "E", nbl_value_new_float(M_E));
    nbl_map_set(math, "LN2", nbl_value_new_float(M_LN2));
    nbl_map_set(math, "LN10", nbl_value_new_float(M_LN10));
    nbl_map_set(math, "LOG2E", nbl_value_new_float(M_LOG2E));
    nbl_map_set(math, "LOG10E", nbl_value_new_float(M_LOG10E));
    nbl_map_set(math, "PI", nbl_value_new_float(M_PI));
    nbl_map_set(math, "SQRT1_2", nbl_value_new_float(M_SQRT1_2));
    nbl_map_set(math, "SQRT2", nbl_value_new_float(M_SQRT2));

    NblList *math_float_args = nbl_list_new();
    nbl_list_add(math_float_args, nbl_argument_new("x", NBL_VALUE_FLOAT, NULL));
    NblList *math_float_float_args = nbl_list_new();
    nbl_list_add(math_float_float_args, nbl_argument_new("x", NBL_VALUE_FLOAT, NULL));
    nbl_list_add(math_float_float_args, nbl_argument_new("y", NBL_VALUE_FLOAT, NULL));
    NblList *math_float_float_reverse_args = nbl_list_new();
    nbl_list_add(math_float_float_reverse_args, nbl_argument_new("y", NBL_VALUE_FLOAT, NULL));
    nbl_list_add(math_float_float_reverse_args, nbl_argument_new("x", NBL_VALUE_FLOAT, NULL));
    nbl_map_set(math, "abs", nbl_value_new_native_function(math_float_args, NBL_VALUE_ANY, env_math_abs));
    nbl_map_set(math, "sin", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_sin));
    nbl_map_set(math, "cos", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_cos));
    nbl_map_set(math, "tan", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_tan));
    nbl_map_set(math, "asin", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_asin));
    nbl_map_set(math, "acos", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_acos));
    nbl_map_set(math, "atan", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_atan));
    nbl_map_set(math, "atan2", nbl_value_new_native_function(math_float_float_reverse_args, NBL_VALUE_FLOAT, env_math_atan2));
    nbl_map_set(math, "pow", nbl_value_new_native_function(math_float_float_args, NBL_VALUE_FLOAT, env_math_pow));
    nbl_map_set(math, "sqrt", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_sqrt));
    nbl_map_set(math, "floor", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_floor));
    nbl_map_set(math, "ceil", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_ceil));
    nbl_map_set(math, "round", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_round));
    nbl_map_set(math, "min", nbl_value_new_native_function(empty_args, NBL_VALUE_ANY, env_math_min));
    nbl_map_set(math, "max", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_ANY, env_math_max));
    nbl_map_set(math, "exp", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_exp));
    nbl_map_set(math, "log", nbl_value_new_native_function(nbl_list_ref(math_float_args), NBL_VALUE_FLOAT, env_math_log));
    nbl_random_seed = nbl_time_ms();
    nbl_map_set(math, "random", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_FLOAT, env_math_random));

    // Exception
    NblMap *exception = nbl_map_new();
    NblList *exception_constructor_args = nbl_list_new();
    nbl_list_add(exception_constructor_args, nbl_argument_new("error", NBL_VALUE_STRING, NULL));
    nbl_map_set(env, "Exception", nbl_variable_new(NBL_VALUE_CLASS, false, nbl_value_new_class(exception, NULL, false)));
    nbl_map_set(exception, "constructor", nbl_value_new_native_function(exception_constructor_args, NBL_VALUE_ANY, env_exception_constructor));

    // String
    NblMap *string = nbl_map_new();
    nbl_map_set(env, "String", nbl_variable_new(NBL_VALUE_CLASS, false, nbl_value_new_class(string, NULL, false)));
    nbl_map_set(string, "constructor", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_STRING, env_string_constructor));
    nbl_map_set(string, "length", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_INT, env_string_length));

    // Array
    NblMap *array = nbl_map_new();
    NblList *array_function_args = nbl_list_new();
    nbl_list_add(array_function_args, nbl_argument_new("function", NBL_VALUE_FUNCTION, NULL));
    nbl_map_set(env, "Array", nbl_variable_new(NBL_VALUE_CLASS, false, nbl_value_new_class(array, NULL, false)));
    nbl_map_set(array, "constructor", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_ARRAY, env_array_constructor));
    nbl_map_set(array, "length", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_INT, env_array_length));
    nbl_map_set(array, "push", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_INT, env_array_push));
    nbl_map_set(array, "foreach", nbl_value_new_native_function(array_function_args, NBL_VALUE_NULL, env_array_foreach));
    nbl_map_set(array, "map", nbl_value_new_native_function(nbl_list_ref(array_function_args), NBL_VALUE_ARRAY, env_array_map));
    nbl_map_set(array, "filter", nbl_value_new_native_function(nbl_list_ref(array_function_args), NBL_VALUE_ARRAY, env_array_filter));
    nbl_map_set(array, "find", nbl_value_new_native_function(nbl_list_ref(array_function_args), NBL_VALUE_ANY, env_array_find));

    // Object
    NblMap *object = nbl_map_new();
    nbl_map_set(env, "Object", nbl_variable_new(NBL_VALUE_CLASS, false, nbl_value_new_class(object, NULL, false)));
    nbl_map_set(object, "constructor", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_OBJECT, env_object_constructor));
    nbl_map_set(object, "length", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_INT, env_object_length));
    nbl_map_set(object, "keys", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_ARRAY, env_object_keys));
    nbl_map_set(object, "values", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_ARRAY, env_object_values));

    // Date
    NblMap *date = nbl_map_new();
    nbl_map_set(env, "Date", nbl_variable_new(NBL_VALUE_CLASS, false, nbl_value_new_class(date, NULL, false)));
    nbl_map_set(date, "now", nbl_value_new_native_function(nbl_list_ref(empty_args), NBL_VALUE_INT, env_date_now));

    // Root
    NblList *type_args = nbl_list_new();
    nbl_list_add(type_args, nbl_argument_new("value", NBL_VALUE_ANY, NULL));
    nbl_map_set(env, "type", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(type_args, NBL_VALUE_ANY, env_type)));

    NblList *assertion_args = nbl_list_new();
    nbl_list_add(assertion_args, nbl_argument_new("assertion", NBL_VALUE_ANY, NULL));
    nbl_map_set(env, "assert", nbl_variable_new(NBL_VALUE_NATIVE_FUNCTION, false, nbl_value_new_native_function(assertion_args, NBL_VALUE_NULL, env_assert)));

    return env;
}

// Interpreter
NblVariable *nbl_variable_new(NblValueType type, bool mutable, NblValue *value) {
    NblVariable *variable = malloc(sizeof(NblVariable));
    variable->type = type;
    variable->mutable = mutable;
    variable->value = value;
    return variable;
}

void nbl_variable_free(NblVariable *variable) {
    nbl_value_free(variable->value);
    free(variable);
}

NblVariable *nbl_block_scope_get(NblBlockScope *block, char *key) {
    NblVariable *variable = nbl_map_get(block->env, key);
    if (variable == NULL && block->parentBlock != NULL) {
        return nbl_block_scope_get(block->parentBlock, key);
    }
    return variable;
}

NblValue *nbl_interpreter(NblMap *env, NblNode *node) {
    NblInterpreter interpreter = {.env = env};
    NblScope scope = {.exception = &(NblExceptionScope){.exceptionValue = NULL},
                      .function = &(NblFunctionScope){.returnValue = NULL},
                      .loop = &(NblLoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                      .block = &(NblBlockScope){.parentBlock = NULL, .env = env}};
    NblValue *returnValue = nbl_interpreter_node(&interpreter, &scope, node);
    if (scope.exception->exceptionValue != NULL) {
        NblValue *path = nbl_value_class_get(scope.exception->exceptionValue, "path");
        NblValue *text = nbl_value_class_get(scope.exception->exceptionValue, "text");
        NblValue *line = nbl_value_class_get(scope.exception->exceptionValue, "line");
        NblValue *column = nbl_value_class_get(scope.exception->exceptionValue, "column");
        NblToken *token = nbl_token_new(NBL_TOKEN_THROW, nbl_source_new(path->string, text->string), line->integer, column->integer);
        NblValue *error = nbl_value_class_get(scope.exception->exceptionValue, "error");
        nbl_print_error(token, "Uncatched exception: %s", error->string);
        nbl_token_free(token);
    }
    if (returnValue != NULL) {
        return returnValue;
    }
    if (scope.function->returnValue != NULL) {
        return scope.function->returnValue;
    } else {
        return nbl_value_new_null();
    }
}

NblValue *nbl_type_error_exception(NblValueType expected, NblValueType got) {
    return nbl_value_new_string_format("Unexpected type: '%s' expected '%s'", nbl_value_type_to_string(got), nbl_value_type_to_string(expected));
}

#define interpreter_statement_in_try(interpreter, scope, node, cleanup)   \
    {                                                                     \
        NblValue *nodeValue = nbl_interpreter_node(interpreter, scope, node); \
        if (nodeValue != NULL) nbl_value_free(nodeValue);                     \
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

NblValue *nbl_interpreter_call(NblInterpreterContext *context, NblValue *callValue, NblValue *this, NblList *arguments) {
    if (callValue->type == NBL_VALUE_FUNCTION) {
        NblScope functionScope = {.exception = context->scope->exception,
                                  .function = &(NblFunctionScope){.returnValue = NULL},
                                  .loop = &(NblLoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                                  .block = &(NblBlockScope){.parentBlock = context->scope->block, .env = nbl_map_new()}};
        if (this != NULL) {
            if (this->instanceClass->parentClass != NULL) {
                nbl_map_set(functionScope.block->env, "super",
                        nbl_variable_new(NBL_VALUE_INSTANCE, false, nbl_value_new_instance(nbl_map_ref(this->object), nbl_value_ref(this->instanceClass->parentClass))));
            }
            nbl_map_set(functionScope.block->env, "this", nbl_variable_new(NBL_VALUE_INSTANCE, false, nbl_value_ref(this)));
        }
        nbl_map_set(functionScope.block->env, "arguments", nbl_variable_new(NBL_VALUE_ARRAY, false, nbl_value_new_array(nbl_list_ref(arguments))));
        for (size_t i = 0; i < callValue->arguments->size; i++) {
            NblArgument *argument = nbl_list_get(callValue->arguments, i);
            nbl_map_set(functionScope.block->env, argument->name, nbl_variable_new(argument->type, true, nbl_value_ref(nbl_list_get(arguments, i))));
        }
        NblInterpreter interpreter = {.env = context->env};
        nbl_interpreter_node(&interpreter, &functionScope, callValue->functionNode);
        nbl_map_free(functionScope.block->env, (NblMapFreeFunc *)nbl_variable_free);
        if (callValue->returnType != NBL_VALUE_ANY && context->scope->exception->exceptionValue == NULL &&
            functionScope.function->returnValue->type != callValue->returnType) {
            NblValueType returnValueType = functionScope.function->returnValue->type;
            nbl_value_free(functionScope.function->returnValue);
            return nbl_interpreter_throw(context, nbl_type_error_exception(callValue->returnType, returnValueType));
        }

        if (functionScope.function->returnValue != NULL) {
            return functionScope.function->returnValue;
        }
        return nbl_value_new_null();
    }

    if (callValue->type == NBL_VALUE_NATIVE_FUNCTION) {
        NblValue *returnValue = callValue->nativeFunc(context, this, arguments);
        if (callValue->returnType != NBL_VALUE_ANY && context->scope->exception->exceptionValue == NULL && returnValue->type != callValue->returnType) {
            return nbl_interpreter_throw(context, nbl_type_error_exception(callValue->returnType, returnValue->type));
        }
        return returnValue;
    }

    if (callValue->type == NBL_VALUE_CLASS) {
        NblValue *instance = nbl_value_new_instance(nbl_map_new(), nbl_value_ref(callValue));
        NblValue *constructorFunction = nbl_value_class_get(callValue, "constructor");
        if (constructorFunction != NULL) {
            NblValue *newReturnValue = nbl_interpreter_call(context, constructorFunction, instance, arguments);
            if (newReturnValue->type == NBL_VALUE_NULL) {
                nbl_value_free(newReturnValue);
            } else {
                nbl_value_free(instance);
                return newReturnValue;
            }
        }
        return instance;
    }
    return NULL;
}

NblValue *nbl_interpreter_throw(NblInterpreterContext *context, NblValue *exception) {
    if (exception->type == NBL_VALUE_STRING) {
        NblValue *exceptionClass = ((NblVariable *)nbl_map_get(context->env, "Exception"))->value;
        NblList *arguments = nbl_list_new();
        nbl_list_add(arguments, exception);
        exception = nbl_interpreter_call(context, exceptionClass, NULL, arguments);
        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
    }
    if (exception->type != NBL_VALUE_INSTANCE) {
        return nbl_interpreter_throw(context, nbl_type_error_exception(NBL_VALUE_INSTANCE, exception->type));
    }
    context->scope->exception->exceptionValue = exception;
    return nbl_value_new_null();
}

NblValue *nbl_interpreter_node(NblInterpreter *interpreter, NblScope *scope, NblNode *node) {
    if (node->type == NBL_NODE_PROGRAM) {
        nbl_list_foreach(node->nodes, NblNode * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NBL_NODE_NODES) {
        nbl_list_foreach(node->nodes, NblNode * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NBL_NODE_BLOCK) {
        NblScope blockScope = {.exception = scope->exception,
                               .function = scope->function,
                               .loop = scope->loop,
                               .block = &(NblBlockScope){.parentBlock = scope->block, .env = nbl_map_new()}};
        nbl_list_foreach(node->nodes, NblNode * child,
                     { interpreter_statement(interpreter, &blockScope, child, { nbl_map_free(blockScope.block->env, (NblMapFreeFunc *)nbl_variable_free); }); });
        nbl_map_free(blockScope.block->env, (NblMapFreeFunc *)nbl_variable_free);
        return NULL;
    }
    if (node->type == NBL_NODE_IF) {
        NblValue *condition = nbl_interpreter_node(interpreter, scope, node->condition);
        if (condition->type != NBL_VALUE_BOOL) {
            NblValueType conditionType = condition->type;
            nbl_value_free(condition);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
            return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_BOOL, conditionType));
        }
        if (condition->boolean) {
            interpreter_statement(interpreter, scope, node->thenBlock, { nbl_value_free(condition); });
        } else if (node->elseBlock != NULL) {
            interpreter_statement(interpreter, scope, node->elseBlock, { nbl_value_free(condition); });
        }
        nbl_value_free(condition);
        return NULL;
    }
    if (node->type == NBL_NODE_TENARY) {
        NblValue *condition = nbl_interpreter_node(interpreter, scope, node->condition);
        if (condition->type != NBL_VALUE_BOOL) {
            NblValueType conditionType = condition->type;
            nbl_value_free(condition);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
            return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_BOOL, conditionType));
        }
        if (condition->boolean) {
            nbl_value_free(condition);
            return nbl_interpreter_node(interpreter, scope, node->thenBlock);
        }
        nbl_value_free(condition);
        return nbl_interpreter_node(interpreter, scope, node->elseBlock);
    }
    if (node->type == NBL_NODE_TRY) {
        NblScope tryScope = {
            .exception = &(NblExceptionScope){.exceptionValue = NULL}, .function = scope->function, .loop = scope->loop, .block = scope->block};
        interpreter_statement_in_try(interpreter, &tryScope, node->tryBlock, {});
        if (tryScope.exception->exceptionValue != NULL) {
            NblScope catchScope = {.exception = scope->exception,
                                   .function = scope->function,
                                   .loop = scope->loop,
                                   .block = &(NblBlockScope){.parentBlock = scope->block, .env = nbl_map_new()}};
            nbl_map_set(catchScope.block->env, node->catchVariable->lhs->string,
                    nbl_variable_new(node->catchVariable->declarationType, node->catchVariable->type == NBL_NODE_LET_ASSIGN,
                                 nbl_value_ref(tryScope.exception->exceptionValue)));
            interpreter_statement(interpreter, &catchScope, node->catchBlock, { nbl_map_free(catchScope.block->env, (NblMapFreeFunc *)nbl_variable_free); });
            nbl_map_free(catchScope.block->env, (NblMapFreeFunc *)nbl_variable_free);
            nbl_value_free(tryScope.exception->exceptionValue);
        }
        if (node->finallyBlock != NULL) {
            interpreter_statement(interpreter, scope, node->finallyBlock, {});
        }
        return NULL;
    }
    if (node->type == NBL_NODE_LOOP) {
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
    if (node->type == NBL_NODE_WHILE) {
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = scope->block};
        for (;;) {
            NblValue *condition = nbl_interpreter_node(interpreter, scope, node->condition);
            if (condition->type != NBL_VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                nbl_value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                nbl_value_free(condition);
                break;
            }
            nbl_value_free(condition);

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
    if (node->type == NBL_NODE_DOWHILE) {
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

            NblValue *condition = nbl_interpreter_node(interpreter, scope, node->condition);
            if (condition->type != NBL_VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                nbl_value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                nbl_value_free(condition);
                break;
            }
            nbl_value_free(condition);
        }
        return NULL;
    }
    if (node->type == NBL_NODE_FOR) {
        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = scope->block};
        for (;;) {
            NblValue *condition = nbl_interpreter_node(interpreter, scope, node->condition);
            if (condition->type != NBL_VALUE_BOOL) {
                NblValueType conditionType = condition->type;
                nbl_value_free(condition);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->condition};
                return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_BOOL, conditionType));
            }
            if (!condition->boolean) {
                nbl_value_free(condition);
                break;
            }
            nbl_value_free(condition);

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
    if (node->type == NBL_NODE_FORIN) {
        NblValue *iterator = nbl_interpreter_node(interpreter, scope, node->iterator);
        if (iterator->type != NBL_VALUE_STRING && iterator->type != NBL_VALUE_ARRAY && iterator->type != NBL_VALUE_OBJECT &&
            iterator->type != NBL_VALUE_CLASS && iterator->type != NBL_VALUE_INSTANCE) {
            NblValueType iteratorType = iterator->type;
            nbl_value_free(iterator);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->iterator};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("NblVariable is not a string, array, object, class or instance it is: %s",
                                                                       nbl_value_type_to_string(iteratorType)));
        }

        NblScope loopScope = {.exception = scope->exception,
                              .function = scope->function,
                              .loop = &(NblLoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                              .block = &(NblBlockScope){.parentBlock = scope->block, .env = nbl_map_new()}};
        size_t size;
        if (iterator->type == NBL_VALUE_STRING) {
            size = strlen(iterator->string);
        }
        if (iterator->type == NBL_VALUE_ARRAY) {
            size = iterator->array->size;
        }
        if (iterator->type == NBL_VALUE_OBJECT || iterator->type == NBL_VALUE_CLASS || iterator->type == NBL_VALUE_INSTANCE) {
            size = iterator->object->size;
        }

        for (size_t i = 0; i < size; i++) {
            NblValue *iteratorValue;
            if (iterator->type == NBL_VALUE_STRING) {
                char character[] = {iterator->string[i], '\0'};
                iteratorValue = nbl_value_new_string(character);
            }
            if (iterator->type == NBL_VALUE_ARRAY) {
                NblValue *value = nbl_list_get(iterator->array, i);
                iteratorValue = value != NULL ? nbl_value_retrieve(value) : nbl_value_new_null();
            }
            if (iterator->type == NBL_VALUE_OBJECT || iterator->type == NBL_VALUE_CLASS || iterator->type == NBL_VALUE_INSTANCE) {
                iteratorValue = nbl_value_new_string(iterator->object->keys[i]);
            }
            NblVariable *previousVariable = nbl_map_get(loopScope.block->env, node->forinVariable->lhs->string);
            if (previousVariable != NULL) {
                nbl_value_free(previousVariable->value);
                previousVariable->value = iteratorValue;
            } else {
                nbl_map_set(loopScope.block->env, node->forinVariable->lhs->string,
                        nbl_variable_new(node->forinVariable->declarationType, node->forinVariable->type == NBL_NODE_LET_ASSIGN, iteratorValue));
            }

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        nbl_map_free(loopScope.block->env, (NblMapFreeFunc *)nbl_variable_free);
        nbl_value_free(iterator);
        return NULL;
    }
    if (node->type == NBL_NODE_CONTINUE) {
        if (!scope->loop->inLoop) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return nbl_interpreter_throw(&context, nbl_value_new_string("Continue not in a loop"));
        }
        scope->loop->isContinuing = true;
        return NULL;
    }
    if (node->type == NBL_NODE_BREAK) {
        if (!scope->loop->inLoop) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return nbl_interpreter_throw(&context, nbl_value_new_string("Break not in a loop"));
        }
        scope->loop->isBreaking = true;
        return NULL;
    }
    if (node->type == NBL_NODE_RETURN) {
        scope->function->returnValue = nbl_interpreter_node(interpreter, scope, node->unary);
        return NULL;
    }
    if (node->type == NBL_NODE_THROW) {
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
        return nbl_interpreter_throw(&context, nbl_interpreter_node(interpreter, scope, node->unary));
    }
    if (node->type == NBL_NODE_INCLUDE) {
        NblValue *pathValue = nbl_interpreter_node(interpreter, scope, node->unary);
        if (pathValue->type != NBL_VALUE_STRING) {
            NblValueType pathValueType = pathValue->type;
            nbl_value_free(pathValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
            return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_STRING, pathValueType));
        }
        char includePath[255];
        if (strlen(node->token->source->dirname) > 0) {
            sprintf(includePath, "%s/%s", node->token->source->dirname, pathValue->string);
        } else {
            strcpy(includePath, pathValue->string);
        }
        nbl_value_free(pathValue);
        char *includeText = nbl_file_read(includePath);
        if (includeText == NULL) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->unary};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Can't read file: %s", includePath));
        }

        NblList *tokens = nbl_lexer(includePath, includeText);
        NblNode *includeNode = nbl_parser(tokens, true);
        interpreter_statement(interpreter, scope, includeNode, {
            nbl_node_free(includeNode);
            nbl_list_free(tokens, (NblListFreeFunc *)nbl_token_free);
            free(includeText);
        });
        nbl_node_free(includeNode);
        nbl_list_free(tokens, (NblListFreeFunc *)nbl_token_free);
        free(includeText);
        return NULL;
    }

    if (node->type == NBL_NODE_VALUE) {
        return nbl_value_retrieve(node->value);
    }
    if (node->type == NBL_NODE_ARRAY) {
        NblValue *arrayValue = nbl_value_new_array(nbl_list_new_with_capacity(node->array->capacity));
        nbl_list_foreach(node->array, NblNode * item, { nbl_list_add(arrayValue->array, nbl_interpreter_node(interpreter, scope, item)); });
        return arrayValue;
    }
    if (node->type == NBL_NODE_OBJECT) {
        NblValue *objectValue = nbl_value_new_object(nbl_map_new_with_capacity(node->object->capacity));
        nbl_map_foreach(node->object, char *key, NblNode *value, { nbl_map_set(objectValue->object, key, nbl_interpreter_node(interpreter, scope, value)); });
        return objectValue;
    }
    if (node->type == NBL_NODE_CLASS) {
        NblValue *classValue = nbl_value_new_class(nbl_map_new_with_capacity(node->object->capacity),
                                               node->parentClass != NULL ? nbl_interpreter_node(interpreter, scope, node->parentClass) : NULL, node->abstract);
        nbl_map_foreach(node->object, char *key, NblNode *value, { nbl_map_set(classValue->object, key, nbl_interpreter_node(interpreter, scope, value)); });
        return classValue;
    }

    if (node->type == NBL_NODE_CONST_ASSIGN || node->type == NBL_NODE_LET_ASSIGN) {
        NblValue *rhs = nbl_interpreter_node(interpreter, scope, node->rhs);
        if (nbl_map_get(scope->block->env, node->lhs->string) != NULL) {
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Can't redeclare variable: '%s'", node->lhs->string));
        }
        if (node->declarationType != NBL_VALUE_ANY && node->declarationType != rhs->type) {
            NblValueType rhsType = rhs->type;
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Unexpected variable type: '%s' needed '%s'", nbl_value_type_to_string(rhsType),
                                                                       nbl_value_type_to_string(node->declarationType)));
        }
        nbl_map_set(scope->block->env, node->lhs->string, nbl_variable_new(node->declarationType, node->type == NBL_NODE_LET_ASSIGN, nbl_value_retrieve(rhs)));
        return rhs;
    }
    if (node->type == NBL_NODE_ASSIGN) {
        NblValue *rhs = nbl_interpreter_node(interpreter, scope, node->rhs);
        if (node->lhs->type == NBL_NODE_GET) {
            NblValue *containerValue = nbl_interpreter_node(interpreter, scope, node->lhs->lhs);
            if (containerValue->type != NBL_VALUE_ARRAY && containerValue->type != NBL_VALUE_OBJECT && containerValue->type != NBL_VALUE_CLASS &&
                containerValue->type != NBL_VALUE_INSTANCE) {
                NblValueType containerValueType = containerValue->type;
                nbl_value_free(rhs);
                nbl_value_free(containerValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return nbl_interpreter_throw(&context, nbl_value_new_string_format("NblVariable is not an array, object, class or instance it is: %s",
                                                                           nbl_value_type_to_string(containerValueType)));
            }

            NblValue *indexOrKey = nbl_interpreter_node(interpreter, scope, node->lhs->rhs);
            if (containerValue->type == NBL_VALUE_ARRAY) {
                if (indexOrKey->type != NBL_VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    nbl_value_free(rhs);
                    nbl_value_free(containerValue);
                    nbl_value_free(indexOrKey);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_INT, indexOrKeyType));
                }
                NblValue *previousValue = nbl_list_get(containerValue->array, indexOrKey->integer);
                if (previousValue != NULL) nbl_value_free(previousValue);
                nbl_list_set(containerValue->array, indexOrKey->integer, nbl_value_retrieve(rhs));
            }
            if (containerValue->type == NBL_VALUE_OBJECT || containerValue->type == NBL_VALUE_CLASS || containerValue->type == NBL_VALUE_INSTANCE) {
                if (indexOrKey->type != NBL_VALUE_STRING) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    nbl_value_free(rhs);
                    nbl_value_free(containerValue);
                    nbl_value_free(indexOrKey);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_STRING, indexOrKeyType));
                }
                NblValue *previousValue = nbl_map_get(containerValue->object, indexOrKey->string);
                if (previousValue != NULL) nbl_value_free(previousValue);
                nbl_map_set(containerValue->object, indexOrKey->string, nbl_value_retrieve(rhs));
            }
            nbl_value_free(indexOrKey);
            nbl_value_free(containerValue);
            return rhs;
        }

        if (node->lhs->type != NBL_NODE_VARIABLE) {
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Is not a variable"));
        }
        NblVariable *variable = nbl_block_scope_get(scope->block, node->lhs->string);
        if (variable == NULL) {
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("NblVariable: '%s' is not declared", node->lhs->string));
        }
        if (!variable->mutable) {
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Can't mutate const variable: '%s'", node->lhs->string));
        }
        if (variable->type != NBL_VALUE_ANY && variable->type != rhs->type) {
            NblValueType rhsType = rhs->type;
            nbl_value_free(rhs);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
            return nbl_interpreter_throw(&context, nbl_type_error_exception(variable->type, rhsType));
        }
        nbl_value_free(variable->value);
        variable->value = nbl_value_retrieve(rhs);
        return rhs;
    }

    if (node->type == NBL_NODE_VARIABLE) {
        NblVariable *variable = nbl_block_scope_get(scope->block, node->string);
        if (variable == NULL) {
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("Can't find variable: '%s'", node->string));
        }
        return nbl_value_retrieve(variable->value);
    }
    if (node->type == NBL_NODE_GET) {
        NblValue *containerValue = nbl_interpreter_node(interpreter, scope, node->lhs);
        if (containerValue->type != NBL_VALUE_STRING && containerValue->type != NBL_VALUE_ARRAY && containerValue->type != NBL_VALUE_OBJECT &&
            containerValue->type != NBL_VALUE_CLASS && containerValue->type != NBL_VALUE_INSTANCE) {
            NblValueType containerValueType = containerValue->type;
            nbl_value_free(containerValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
            return nbl_interpreter_throw(&context, nbl_value_new_string_format("NblVariable is not a string, array, object, class or instance it is: %s",
                                                                       nbl_value_type_to_string(containerValueType)));
        }

        NblValue *indexOrKey = nbl_interpreter_node(interpreter, scope, node->rhs);
        NblValue *returnValue = NULL;
        if (containerValue->type == NBL_VALUE_STRING) {
            if (indexOrKey->type == NBL_VALUE_STRING) {
                NblValue *stringClass = ((NblVariable *)nbl_map_get(interpreter->env, "String"))->value;
                NblValue *stringClassItem = nbl_map_get(stringClass->object, indexOrKey->string);
                if (stringClassItem != NULL) returnValue = nbl_value_retrieve(stringClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != NBL_VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    nbl_value_free(indexOrKey);
                    nbl_value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_INT, indexOrKeyType));
                }
                if (indexOrKey->integer >= 0 && indexOrKey->integer <= (int64_t)strlen(containerValue->string)) {
                    char character[] = {containerValue->string[indexOrKey->integer], '\0'};
                    returnValue = nbl_value_new_string(character);
                } else {
                    returnValue = nbl_value_new_null();
                }
            }
        }
        if (containerValue->type == NBL_VALUE_ARRAY) {
            if (indexOrKey->type == NBL_VALUE_STRING) {
                NblValue *arrayClass = ((NblVariable *)nbl_map_get(interpreter->env, "Array"))->value;
                NblValue *arrayClassItem = nbl_map_get(arrayClass->object, indexOrKey->string);
                if (arrayClassItem != NULL) returnValue = nbl_value_retrieve(arrayClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != NBL_VALUE_INT) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    nbl_value_free(indexOrKey);
                    nbl_value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_INT, indexOrKeyType));
                }
                NblValue *value = nbl_list_get(containerValue->array, indexOrKey->integer);
                returnValue = value != NULL ? nbl_value_retrieve(value) : nbl_value_new_null();
            }
        }
        if (containerValue->type == NBL_VALUE_OBJECT || containerValue->type == NBL_VALUE_CLASS || containerValue->type == NBL_VALUE_INSTANCE) {
            if (containerValue->type == NBL_VALUE_OBJECT && indexOrKey->type == NBL_VALUE_STRING) {
                NblValue *objectClass = ((NblVariable *)nbl_map_get(interpreter->env, "Object"))->value;
                NblValue *objectClassItem = nbl_map_get(objectClass->object, indexOrKey->string);
                if (objectClassItem != NULL) returnValue = nbl_value_retrieve(objectClassItem);
            }
            if (returnValue == NULL) {
                if (indexOrKey->type != NBL_VALUE_STRING) {
                    NblValueType indexOrKeyType = indexOrKey->type;
                    nbl_value_free(indexOrKey);
                    nbl_value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->rhs};
                    return nbl_interpreter_throw(&context, nbl_type_error_exception(NBL_VALUE_STRING, indexOrKeyType));
                }
                NblValue *value;
                if (containerValue->type == NBL_VALUE_INSTANCE) {
                    value = nbl_value_class_get(containerValue, indexOrKey->string);
                } else {
                    value = nbl_map_get(containerValue->object, indexOrKey->string);
                }
                if (value == NULL) {
                    char *indexOrKeyString = strdup(indexOrKey->string);
                    nbl_value_free(indexOrKey);
                    nbl_value_free(containerValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    NblValue *exception = nbl_value_new_string_format("Can't find %s in object", indexOrKeyString);
                    free(indexOrKeyString);
                    return nbl_interpreter_throw(&context, exception);
                }
                returnValue = nbl_value_retrieve(value);
            }
        }

        nbl_value_free(indexOrKey);
        nbl_value_free(containerValue);
        return returnValue;
    }
    if (node->type == NBL_NODE_CALL) {
        NblValue *thisValue = NULL;
        if (node->function->type == NBL_NODE_GET) {
            NblValue *containerValue = nbl_interpreter_node(interpreter, scope, node->function->lhs);
            if (containerValue->type == NBL_VALUE_STRING || containerValue->type == NBL_VALUE_ARRAY || containerValue->type == NBL_VALUE_OBJECT ||
                containerValue->type == NBL_VALUE_INSTANCE) {
                thisValue = containerValue;
            } else {
                nbl_value_free(containerValue);
            }
        }
        NblValue *callValue = nbl_interpreter_node(interpreter, scope, node->function);
        if (callValue->type != NBL_VALUE_FUNCTION && callValue->type != NBL_VALUE_NATIVE_FUNCTION && callValue->type != NBL_VALUE_CLASS) {
            NblValueType callValueType = callValue->type;
            nbl_value_free(callValue);
            if (thisValue != NULL) nbl_value_free(thisValue);
            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
            return nbl_interpreter_throw(&context,
                                     nbl_value_new_string_format("NblVariable is not a function or a class but: %s", nbl_value_type_to_string(callValueType)));
        }

        NblList *callArguments = NULL;
        if (callValue->type == NBL_VALUE_CLASS) {
            if (callValue->abstract) {
                nbl_value_free(callValue);
                if (thisValue != NULL) nbl_value_free(thisValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->function};
                return nbl_interpreter_throw(&context, nbl_value_new_string("Can't construct an abstract class"));
            }
            NblValue *constructorFunction = nbl_value_class_get(callValue, "constructor");
            if (constructorFunction != NULL) callArguments = constructorFunction->arguments;
        } else {
            callArguments = callValue->arguments;
        }
        NblList *arguments = nbl_list_new_with_capacity(node->nodes->capacity);
        for (size_t i = 0; i < MAX(callArguments != NULL ? callArguments->size : 0, node->nodes->size); i++) {
            NblArgument *argument = NULL;
            if (callArguments != NULL) {
                argument = nbl_list_get(callArguments, i);
                if (nbl_list_get(node->nodes, i) == NULL && argument != NULL) {
                    if (argument->defaultNode != NULL) {
                        NblValue *defaultValue = nbl_interpreter_node(interpreter, scope, argument->defaultNode);
                        if (argument->type != NBL_VALUE_ANY && defaultValue->type != argument->type) {
                            NblValueType defaultValueType = defaultValue->type;
                            nbl_value_free(defaultValue);
                            nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
                            nbl_value_free(callValue);
                            if (thisValue != NULL) nbl_value_free(thisValue);
                            NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = argument->defaultNode};
                            return nbl_interpreter_throw(&context, nbl_type_error_exception(argument->type, defaultValueType));
                        }
                        nbl_list_add(arguments, defaultValue);
                        continue;
                    }

                    nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
                    nbl_value_free(callValue);
                    if (thisValue != NULL) nbl_value_free(thisValue);
                    NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                    return nbl_interpreter_throw(&context, nbl_value_new_string("Not all function arguments are given"));
                }
            }

            NblValue *nodeValue = nbl_interpreter_node(interpreter, scope, nbl_list_get(node->nodes, i));
            if (argument != NULL && argument->type != NBL_VALUE_ANY && nodeValue->type != argument->type) {
                NblValueType nodeValueType = nodeValue->type;
                nbl_value_free(nodeValue);
                nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
                nbl_value_free(callValue);
                if (thisValue != NULL) nbl_value_free(thisValue);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
                return nbl_interpreter_throw(&context, nbl_type_error_exception(argument->type, nodeValueType));
            }
            nbl_list_add(arguments, nodeValue);
        }

        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        NblValue *returnValue = nbl_interpreter_call(&context, callValue, thisValue, arguments);

        nbl_list_free(arguments, (NblListFreeFunc *)nbl_value_free);
        nbl_value_free(callValue);
        if (thisValue != NULL) nbl_value_free(thisValue);
        return returnValue;
    }

    if (node->type >= NBL_NODE_NEG && node->type <= NBL_NODE_CAST) {
        NblValue *unary = nbl_interpreter_node(interpreter, scope, node->unary);
        if (node->type == NBL_NODE_NEG) {
            if (unary->type == NBL_VALUE_INT) {
                unary->integer = -unary->integer;
                return unary;
            }
            if (unary->type == NBL_VALUE_FLOAT) {
                unary->floating = -unary->floating;
                return unary;
            }
        }
        if (node->type == NBL_NODE_INC_PRE || node->type == NBL_NODE_DEC_PRE || node->type == NBL_NODE_INC_POST || node->type == NBL_NODE_DEC_POST) {
            if (node->unary->type != NBL_NODE_VARIABLE) {
                nbl_value_free(unary);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
                return nbl_interpreter_throw(&context, nbl_value_new_string_format("Is not a variable"));
            }
            NblVariable *variable = nbl_block_scope_get(scope->block, node->lhs->string);
            if (!variable->mutable) {
                nbl_value_free(unary);
                NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node->lhs};
                return nbl_interpreter_throw(&context, nbl_value_new_string_format("Can't mutate const variable: '%s'", node->lhs->string));
            }
            if (unary->type == NBL_VALUE_INT) {
                if (node->type == NBL_NODE_INC_PRE) unary->integer++;
                if (node->type == NBL_NODE_INC_PRE || node->type == NBL_NODE_INC_POST) variable->value->integer++;
                if (node->type == NBL_NODE_DEC_PRE) unary->integer--;
                if (node->type == NBL_NODE_DEC_PRE || node->type == NBL_NODE_DEC_POST) variable->value->integer--;
                return unary;
            }
            if (unary->type == NBL_VALUE_FLOAT) {
                if (node->type == NBL_NODE_INC_PRE) unary->floating++;
                if (node->type == NBL_NODE_INC_PRE || node->type == NBL_NODE_INC_POST) variable->value->floating++;
                if (node->type == NBL_NODE_DEC_PRE) unary->floating--;
                if (node->type == NBL_NODE_DEC_PRE || node->type == NBL_NODE_DEC_POST) variable->value->floating--;
                return unary;
            }
        }
        if (node->type == NBL_NODE_NOT) {
            if (unary->type == NBL_VALUE_INT) {
                unary->integer = ~unary->integer;
                return unary;
            }
        }
        if (node->type == NBL_NODE_LOGICAL_NOT) {
            if (unary->type == NBL_VALUE_BOOL) {
                unary->boolean = !unary->boolean;
                return unary;
            }
        }
        if (node->type == NBL_NODE_CAST) {
            if (node->castType == NBL_VALUE_BOOL) {
                if (unary->type == NBL_VALUE_NULL) {
                    unary->type = NBL_VALUE_BOOL;
                    unary->boolean = false;
                    return unary;
                }
                if (unary->type == NBL_VALUE_BOOL) return unary;
                if (unary->type == NBL_VALUE_INT) {
                    unary->type = NBL_VALUE_BOOL;
                    unary->boolean = unary->integer != 0;
                    return unary;
                }
                if (unary->type == NBL_VALUE_FLOAT) {
                    unary->type = NBL_VALUE_BOOL;
                    unary->boolean = unary->floating != 0.0;
                    return unary;
                }
                if (unary->type == NBL_VALUE_STRING) {
                    bool result = !(!strcmp(unary->string, "") || !strcmp(unary->string, "0"));
                    nbl_value_clear(unary);
                    unary->type = NBL_VALUE_BOOL;
                    unary->boolean = result;
                    return unary;
                }
            }

            if (node->castType == NBL_VALUE_INT) {
                if (unary->type == NBL_VALUE_NULL) {
                    unary->type = NBL_VALUE_INT;
                    unary->integer = 0;
                    return unary;
                }
                if (unary->type == NBL_VALUE_BOOL) {
                    unary->type = NBL_VALUE_INT;
                    unary->integer = unary->boolean;
                    return unary;
                }
                if (unary->type == NBL_VALUE_INT) return unary;
                if (unary->type == NBL_VALUE_FLOAT) {
                    unary->type = NBL_VALUE_INT;
                    unary->integer = unary->floating;
                    return unary;
                }
                if (unary->type == NBL_VALUE_STRING) {
                    int64_t result = nbl_string_to_int(unary->string);
                    nbl_value_clear(unary);
                    unary->type = NBL_VALUE_INT;
                    unary->integer = result;
                    return unary;
                }
            }

            if (node->castType == NBL_VALUE_FLOAT) {
                if (unary->type == NBL_VALUE_NULL) {
                    unary->type = NBL_VALUE_FLOAT;
                    unary->floating = 0;
                    return unary;
                }
                if (unary->type == NBL_VALUE_BOOL) {
                    unary->type = NBL_VALUE_FLOAT;
                    unary->floating = unary->boolean;
                    return unary;
                }
                if (unary->type == NBL_VALUE_INT) {
                    unary->type = NBL_VALUE_FLOAT;
                    unary->floating = unary->integer;
                    return unary;
                }
                if (unary->type == NBL_VALUE_FLOAT) return unary;
                if (unary->type == NBL_VALUE_STRING) {
                    int64_t result = nbl_string_to_float(unary->string);
                    nbl_value_clear(unary);
                    unary->type = NBL_VALUE_FLOAT;
                    unary->floating = result;
                    return unary;
                }
            }

            if (node->castType == NBL_VALUE_STRING) {
                char *string = nbl_value_to_string(unary);
                nbl_value_clear(unary);
                unary->type = NBL_VALUE_STRING;
                unary->string = string;
                return unary;
            }
        }

        nbl_value_free(unary);
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return nbl_interpreter_throw(&context, nbl_value_new_string("Type error"));
    }

    if (node->type >= NBL_NODE_ADD && node->type <= NBL_NODE_LOGICAL_OR) {
        NblValue *lhs = nbl_interpreter_node(interpreter, scope, node->lhs);
        NblValue *rhs = nbl_interpreter_node(interpreter, scope, node->rhs);

        if (node->type == NBL_NODE_ADD) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer += rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating += rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating += rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = lhs->integer + rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }

            if (lhs->type == NBL_VALUE_STRING && rhs->type == NBL_VALUE_STRING) {
                char *string = malloc(strlen(lhs->string) + strlen(rhs->string) + 1);
                strcpy(string, lhs->string);
                strcat(string, rhs->string);
                nbl_value_clear(lhs);
                lhs->string = string;
                nbl_value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NBL_NODE_SUB) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer -= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating -= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating -= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = lhs->integer - rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_MUL) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer *= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating *= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating *= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = lhs->integer * rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_EXP) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer = pow(lhs->integer, rhs->integer);
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->floating);
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->integer);
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = pow(lhs->integer, rhs->floating);
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_DIV) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer = rhs->integer != 0 ? lhs->integer / rhs->integer : 0;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->floating / rhs->floating : 0;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->floating / rhs->integer : 0;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = rhs->integer != 0 ? lhs->integer / rhs->floating : 0;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_MOD) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer %= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->floating);
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->integer);
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_FLOAT;
                lhs->floating = fmod(lhs->integer, rhs->floating);
                nbl_value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NBL_NODE_AND) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer &= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_XOR) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer ^= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_OR) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer |= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_SHL) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer <<= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_SHR) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->integer >>= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NBL_NODE_INSTANCEOF) {
            if (lhs->type == NBL_VALUE_INSTANCE && rhs->type == NBL_VALUE_CLASS) {
                bool result = nbl_value_class_instanceof(lhs, rhs);
                nbl_value_free(lhs);
                nbl_value_free(rhs);
                return nbl_value_new_bool(result);
            }
        }

        if (node->type == NBL_NODE_EQ) {
            if (lhs->type == NBL_VALUE_NULL) {
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = rhs->type == NBL_VALUE_NULL;
                nbl_value_free(rhs);
                return lhs;
            }
            if (rhs->type == NBL_VALUE_NULL) {
                bool result = lhs->type == NBL_VALUE_NULL;
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = result;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_BOOL && rhs->type == NBL_VALUE_BOOL) {
                lhs->boolean = lhs->boolean == rhs->boolean;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_STRING && rhs->type == NBL_VALUE_STRING) {
                bool result = !strcmp(lhs->string, rhs->string);
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = result;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_NEQ) {
            if (lhs->type == NBL_VALUE_NULL) {
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = rhs->type != NBL_VALUE_NULL;
                nbl_value_free(rhs);
                return lhs;
            }
            if (rhs->type == NBL_VALUE_NULL) {
                bool result = lhs->type != NBL_VALUE_NULL;
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = result;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_BOOL && rhs->type == NBL_VALUE_BOOL) {
                lhs->boolean = lhs->boolean != rhs->boolean;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_STRING && rhs->type == NBL_VALUE_STRING) {
                bool result = strcmp(lhs->string, rhs->string);
                nbl_value_clear(lhs);
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = result;
                nbl_value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NBL_NODE_LT) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_LTEQ) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_GT) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_GTEQ) {
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_FLOAT && rhs->type == NBL_VALUE_INT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->integer;
                nbl_value_free(rhs);
                return lhs;
            }
            if (lhs->type == NBL_VALUE_INT && rhs->type == NBL_VALUE_FLOAT) {
                lhs->type = NBL_VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->floating;
                nbl_value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NBL_NODE_LOGICAL_AND) {
            if (lhs->type == NBL_VALUE_BOOL && rhs->type == NBL_VALUE_BOOL) {
                lhs->boolean = lhs->boolean && rhs->boolean;
                nbl_value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NBL_NODE_LOGICAL_OR) {
            if (lhs->type == NBL_VALUE_BOOL && rhs->type == NBL_VALUE_BOOL) {
                lhs->boolean = lhs->boolean || rhs->boolean;
                nbl_value_free(rhs);
                return lhs;
            }
        }

        nbl_value_free(lhs);
        nbl_value_free(rhs);
        NblInterpreterContext context = {.env = interpreter->env, .scope = scope, .node = node};
        return nbl_interpreter_throw(&context, nbl_value_new_string("Type error"));
    }

    fprintf(stderr, "Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

#endif
