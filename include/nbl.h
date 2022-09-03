// New Bastiaan Language Interpreter Header
#ifndef NBL_H
#define NBL_H

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

char *format(char *fmt, ...);

void error(char *text, size_t line, size_t position, char *fmt, ...);

// List header
typedef struct List {
    int32_t refs;
    void **items;
    size_t capacity;
    size_t size;
} List;

#define list_foreach(list, item, block)       \
    for (size_t i = 0; i < list->size; i++) { \
        item = list_get(list, i);             \
        block                                 \
    }

List *list_new(void);

List *list_new_with_capacity(size_t capacity);

List *list_ref(List *list);

void *list_get(List *list, size_t index);

void list_set(List *list, size_t index, void *item);

void list_add(List *list, void *item);

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

#define map_foreach(map, key, value, block)  \
    for (size_t i = 0; i < map->size; i++) { \
        key = map->keys[i];                  \
        value = map->values[i];              \
        block                                \
    }

Map *map_new(void);

Map *map_new_with_capacity(size_t capacity);

Map *map_ref(Map *map);

void *map_get(Map *map, char *key);

void map_set(Map *map, char *key, void *item);

typedef void MapFreeFunc(void *item);

void map_free(Map *map, MapFreeFunc *freeFunc);

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
    TOKEN_ASSIGN_ADD,
    TOKEN_SUB,
    TOKEN_ASSIGN_SUB,
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
    int32_t refs;
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

List *lexer(char *text);

// Value
typedef struct Node Node;  // Forward define

typedef enum ValueType {
    VALUE_ANY,
    VALUE_NULL,
    VALUE_BOOL,
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
    int32_t refs;
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

Value *value_new_bool(bool boolean);

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

Value *value_ref(Value *value);

Value *value_copy(Value *value);

void value_clear(Value *value);

void value_free(Value *value);

// Parser
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

    NODE_NEG,
    NODE_NOT,
    NODE_LOGICAL_NOT,
    NODE_CAST,

    NODE_VARIABLE,
    NODE_GET,
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
    int32_t refs;
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

Node *node_ref(Node *node);

void node_free(Node *node);

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

// Interpreter
typedef struct Variable {
    ValueType type;
    bool mutable;
    Value *value;
} Variable;

Variable *variable_new(ValueType type, bool mutable, Value *value);

void variable_free(Variable *variable);

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

typedef struct BlockScope BlockScope;

struct BlockScope {
    BlockScope *parentBlock;
    Map *env;
};

typedef struct Scope {
    FunctionScope *function;
    LoopScope *loop;
    BlockScope *block;
} Scope;

Value *interpreter(char *text, Map *env, Node *node);

Variable *block_scope_get(BlockScope *block, char *key);

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node);

#endif