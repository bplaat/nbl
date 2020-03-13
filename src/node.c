#include <stdio.h>
#include <stdlib.h>
#include "node.h"

Node *node_new(NodeType type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    return node;
}

void node_dump(Node *node) {
    if (node->type == NODE_TYPE_NUMBER) {
        printf("%.15g", node->value.number);
    }
    if (node->type == NODE_TYPE_VARIABLE) {
        printf("%s", node->value.string);
    }

    if (node->type == NODE_TYPE_BLOCK) {
        ListItem *list_item = node->value.children->first;
        while (list_item != NULL) {
            printf("- ");
            node_dump(list_item->value);
            putchar('\n');
            list_item = list_item->next;
        }
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        node_dump(node->value.children->first->value);
        printf(" = ");
        node_dump(node->value.children->first->next->value);
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        printf("(+ ");
        node_dump(node->value.children->first->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        printf("(- ");
        node_dump(node->value.children->first->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_ADD) {
        putchar('(');
        node_dump(node->value.children->first->value);
        printf(" + ");
        node_dump(node->value.children->first->next->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_SUB) {
        putchar('(');
        node_dump(node->value.children->first->value);
        printf(" - ");
        node_dump(node->value.children->first->next->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_MUL) {
        putchar('(');
        node_dump(node->value.children->first->value);
        printf(" * ");
        node_dump(node->value.children->first->next->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_DIV) {
        putchar('(');
        node_dump(node->value.children->first->value);
        printf(" / ");
        node_dump(node->value.children->first->next->value);
        putchar(')');
    }

    if (node->type == NODE_TYPE_MOD) {
        putchar('(');
        node_dump(node->value.children->first->value);
        printf(" %% ");
        node_dump(node->value.children->first->next->value);
        putchar(')');
    }
}

void node_free(Node *node) {
    if (node->type == NODE_TYPE_VARIABLE) {
        free(node->value.string);
    }

    if (node->type >= NODE_TYPE_BLOCK && node->type <= NODE_TYPE_MOD) {
        ListItem *list_item = node->value.children->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.children);
    }

    free(node);
}
