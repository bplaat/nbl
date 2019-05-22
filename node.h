#pragma once
#include "list.h"

typedef enum NodeType {
    NODE_TYPE_NUMBER,
    NODE_TYPE_VARIABLE,

    NODE_TYPE_BLOCK,
    NODE_TYPE_UNARY_ADD,
    NODE_TYPE_UNARY_SUB,
    NODE_TYPE_SET,
    NODE_TYPE_ADD,
    NODE_TYPE_SUB,
    NODE_TYPE_MUL,
    NODE_TYPE_DIV,
    NODE_TYPE_MOD
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        double number;
        char *string;
        List *children;
    } value;
} Node;

Node *node_new(NodeType type);

void node_dump(Node *node);

void node_free(Node *node);
