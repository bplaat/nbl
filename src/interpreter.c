#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "interpreter.h"
#include "node.h"
#include "map.h"
#include "value.h"
#include "utils.h"

Map *vars_map;

Value *interpreter(Node *node) {
    if (node->type == NODE_TYPE_NULL) {
        return value_new_null();
    }

    if (node->type == NODE_TYPE_NUMBER) {
        return value_new_number(node->value.number);
    }

    if (node->type == NODE_TYPE_STRING) {
        return value_new_string(node->value.string);
    }

    if (node->type == NODE_TYPE_BOOLEAN) {
        return value_new_boolean(node->value.boolean);
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        Value *value = map_get(vars_map, node->value.string);
        if (value != NULL) {
            return value_copy(value);
        } else {
            return value_new_null();
        }
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        Value *value = interpreter(node->value.operation.right);
        map_set(vars_map, node->value.operation.left->value.string, value);
        return value;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        Value *value = interpreter(node->value.child);

        if (value->type == VALUE_TYPE_NUMBER) {
            return value_new_number(+value->value.number);
        }

        else {
            printf("[ERROR] Type error by unary add\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        Value *value = interpreter(node->value.child);

        if (value->type == VALUE_TYPE_NUMBER) {
            return value_new_number(-value->value.number);
        }

        else {
            printf("[ERROR] Type error by unary sub\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_NOT) {
        Value *value = interpreter(node->value.child);

        if (value->type == VALUE_TYPE_BOOLEAN) {
            return value_new_boolean(!value->value.boolean);
        }

        else {
            printf("[ERROR] Type error by not\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_ADD) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(left->value.number + right->value.number);
        }

        else if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            return value_new_string(string_concat(left->value.string, right->value.string));
        }

        else if (left->type == VALUE_TYPE_STRING && right->type != VALUE_TYPE_STRING) {
            return value_new_string(string_concat(left->value.string, value_to_string(right)));
        }

        else if (left->type != VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            return value_new_string(string_concat(value_to_string(left), right->value.string));
        }

        else {
            printf("[ERROR] Type error by add\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_SUB) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(left->value.number - right->value.number);
        }

        else {
            printf("[ERROR] Type error by sub\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_MUL) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(left->value.number * right->value.number);
        }

        else {
            printf("[ERROR] Type error by mul\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_EXP) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(pow(left->value.number, right->value.number));
        }

        else {
            printf("[ERROR] Type error by exp\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_DIV) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(left->value.number / right->value.number);
        }

        else {
            printf("[ERROR] Type error by div\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_MOD) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_number(fmod(left->value.number, right->value.number));
        }

        else {
            printf("[ERROR] Type error by mod\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NULL && right->type == VALUE_TYPE_NULL) {
            return value_new_boolean(true);
        }

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number == right->value.number);
        }

        if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            return value_new_boolean(!strcmp(left->value.string, right->value.string));
        }

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            return value_new_boolean(left->value.boolean == right->value.boolean);
        }

        return value_new_boolean(false);
    }

    if (node->type == NODE_TYPE_NOT_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NULL && right->type == VALUE_TYPE_NULL) {
            return value_new_boolean(false);
        }

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number != right->value.number);
        }

        if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            return value_new_boolean(strcmp(left->value.string, right->value.string));
        }

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            return value_new_boolean(left->value.boolean != right->value.boolean);
        }

        return value_new_boolean(true);
    }

    if (node->type == NODE_TYPE_GREATER) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number > right->value.number);
        }

        else {
            printf("[ERROR] Type error by greater\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_GREATER_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number >= right->value.number);
        }

        else {
            printf("[ERROR] Type error by greater equals\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_LOWER) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number < right->value.number);
        }

        else {
            printf("[ERROR] Type error by lower\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_LOWER_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            return value_new_boolean(left->value.number <= right->value.number);
        }

        else {
            printf("[ERROR] Type error by lower equals\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_AND) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            return value_new_boolean(left->value.boolean && right->value.boolean);
        }

        else {
            printf("[ERROR] Type error by and\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_OR) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            return value_new_boolean(left->value.boolean || right->value.boolean);
        }

        else {
            printf("[ERROR] Type error by or\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("[ERROR] Unkown node type\n");
    exit(EXIT_FAILURE);
}

Value *start_interpreter(Node *node, Map *global_vars_map) {
    vars_map = global_vars_map;
    return interpreter(node);
}
