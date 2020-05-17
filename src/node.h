#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#include "list.h"

typedef enum NodeType {
    NODE_TYPE_NULL,
    NODE_TYPE_NUMBER,
    NODE_TYPE_STRING,
    NODE_TYPE_BOOLEAN,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_CALL,

    NODE_TYPE_ASSIGN,
    NODE_TYPE_ADD_ASSIGN,
    NODE_TYPE_SUB_ASSIGN,
    NODE_TYPE_MUL_ASSIGN,
    NODE_TYPE_EXP_ASSIGN,
    NODE_TYPE_DIV_ASSIGN,
    NODE_TYPE_MOD_ASSIGN,

    NODE_TYPE_UNARY_ADD,
    NODE_TYPE_UNARY_SUB,
    NODE_TYPE_NOT,

    NODE_TYPE_ADD,
    NODE_TYPE_SUB,
    NODE_TYPE_MUL,
    NODE_TYPE_EXP,
    NODE_TYPE_DIV,
    NODE_TYPE_MOD,

    NODE_TYPE_EQUALS,
    NODE_TYPE_NOT_EQUALS,
    NODE_TYPE_GREATER,
    NODE_TYPE_GREATER_EQUALS,
    NODE_TYPE_LOWER,
    NODE_TYPE_LOWER_EQUALS,
    NODE_TYPE_AND,
    NODE_TYPE_OR
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        double number;

        char *string;

        bool boolean;

        struct {
            char *variable;
            List *arguments;
        } call;

        struct {
            char *variable;
            struct Node *node;
        } assign;

        struct Node *node;

        struct {
            struct Node *left;
            struct Node *right;
        } operation;
    } value;
} Node;

Node *node_new(NodeType type);

char *node_to_string(Node *node);

void node_free(Node *node);

#endif
