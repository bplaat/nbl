#ifndef NODE_H
#define NODE_H

typedef enum NodeType {
    NODE_TYPE_NUMBER,
    NODE_TYPE_VARIABLE,

    NODE_TYPE_ASSIGN,

    NODE_TYPE_UNARY_ADD,
    NODE_TYPE_UNARY_SUB,

    NODE_TYPE_ADD,
    NODE_TYPE_SUB,
    NODE_TYPE_MUL,
    NODE_TYPE_EXP,
    NODE_TYPE_DIV,
    NODE_TYPE_MOD
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        double number;

        char *string;

        struct Node *child;

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
