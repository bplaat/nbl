#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "map.h"
#include "node.h"
#include "interpreter.h"

Map *vars_map;

double interpreter(Node *node) {
    if (node->type == NODE_TYPE_NUMBER) {
        return node->value.number;
    }
    if (node->type == NODE_TYPE_VARIABLE) {
        Node *var = map_get(vars_map, node->value.string);
        return var == NULL ? 0 : var->value.number;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        return interpreter(node->value.children->first->value);
    }
    if (node->type == NODE_TYPE_UNARY_SUB) {
        return -interpreter(node->value.children->first->value);
    }
    if (node->type == NODE_TYPE_SET) {
        Node *var = node_new(NODE_TYPE_NUMBER);
        var->value.number = interpreter(node->value.children->first->next->value);
        map_set(vars_map, ((Node *)node->value.children->first->value)->value.string, var);
        return var->value.number;
    }
    if (node->type == NODE_TYPE_ADD) {
        return interpreter(node->value.children->first->value) + interpreter(node->value.children->first->next->value);
    }
    if (node->type == NODE_TYPE_SUB) {
        return interpreter(node->value.children->first->value) - interpreter(node->value.children->first->next->value);
    }
    if (node->type == NODE_TYPE_MUL) {
        return interpreter(node->value.children->first->value) * interpreter(node->value.children->first->next->value);
    }
    if (node->type == NODE_TYPE_DIV) {
        return interpreter(node->value.children->first->value) / interpreter(node->value.children->first->next->value);
    }
    if (node->type == NODE_TYPE_MOD) {
        return fmod(interpreter(node->value.children->first->value), interpreter(node->value.children->first->next->value));
    }

    return 0;
}


void start_interpreter(Node *node, Map *vars) {
    vars_map = vars;

    ListItem *list_item = node->value.children->first;
    while (list_item != NULL) {
        printf("- %.15g\n", interpreter(list_item->value));
        list_item = list_item->next;
    }
}