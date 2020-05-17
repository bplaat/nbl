#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "node.h"
#include "utils.h"

Node *node_new(NodeType type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    return node;
}

char *node_to_string(Node *node) {
    if (node->type == NODE_TYPE_NUMBER) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %g )", node->value.number);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s )", node->value.string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_STRING) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( '%s' )", node->value.string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s = %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        char *node_string = node_to_string(node->value.child);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( + %s )", node_string);
        free(node_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        char *node_string = node_to_string(node->value.child);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( - %s )", node_string);
        free(node_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ADD) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s + %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_SUB) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s - %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_MUL) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s * %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_EXP) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s ** %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_DIV) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s / %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_MOD) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s %% %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    printf("[ERROR] Unkown node type\n");
    exit(EXIT_FAILURE);
}

void node_free(Node *node) {
    if (node->type == NODE_TYPE_VARIABLE) {
        free(node->value.string);
    }

    if (node->type >= NODE_TYPE_UNARY_ADD && node->type <= NODE_TYPE_UNARY_SUB) {
        free(node->value.child);
    }

    if (node->type >= NODE_TYPE_ADD && node->type <= NODE_TYPE_MOD) {
        node_free(node->value.operation.left);
        node_free(node->value.operation.right);
    }

    free(node);
}
