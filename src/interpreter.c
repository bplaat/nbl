#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "interpreter.h"
#include "node.h"
#include "map.h"
#include "value.h"

Map *vars_map;

double interpreter(Node *node) {
    if (node->type == NODE_TYPE_NUMBER) {
        return node->value.number;
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        Value *value = map_get(vars_map, node->value.string);
        return value->value.number;
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        double number = interpreter(node->value.operation.right);
        map_set(vars_map, node->value.operation.left->value.string, value_new_number(number));
        return number;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        return interpreter(node->value.child);
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        return -interpreter(node->value.child);
    }

    if (node->type == NODE_TYPE_ADD) {
        return interpreter(node->value.operation.left) + interpreter(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_SUB) {
        return interpreter(node->value.operation.left) - interpreter(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_MUL) {
        return interpreter(node->value.operation.left) * interpreter(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_EXP) {
        return pow(interpreter(node->value.operation.left), interpreter(node->value.operation.right));
    }

    if (node->type == NODE_TYPE_DIV) {
        return interpreter(node->value.operation.left) / interpreter(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_MOD) {
        return fmod(interpreter(node->value.operation.left), interpreter(node->value.operation.right));
    }

    printf("[ERROR] Unkown node type");
    exit(EXIT_FAILURE);
}

double start_interpreter(Node *node, Map *global_vars_map) {
    vars_map = global_vars_map;
    return interpreter(node);
}
