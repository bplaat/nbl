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
    // Value nodes
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

    if (node->type == NODE_TYPE_CALL) {
        Value *value = map_get(vars_map, node->value.call.variable);
        if (value != NULL) {
            List *arguments = node->value.call.arguments;

            List *value_arguments = list_new();

            // Interpreter the arguments
            ListItem *list_item = arguments->first;
            while (list_item != NULL) {
                list_add(value_arguments, interpreter(list_item->value));
                list_item = list_item->next;
            }

            // Run native function with arguments
            Value *new_value = value->value.native_function(value_arguments);

            // Free value arguments
            list_item = value_arguments->first;
            while (list_item != NULL) {
                value_free(list_item->value);
                list_item = list_item->next;
            }
            list_free(value_arguments);

            // Return the returend value
            return new_value;
        } else {
            printf("[ERROR] Function call error\n");
            exit(EXIT_FAILURE);
        }
    }

    // Assignments
    if (node->type == NODE_TYPE_ASSIGN) {
        Value *value = interpreter(node->value.assign.node);
        map_set(vars_map, node->value.assign.variable, value_copy(value));
        return value;
    }

    if (node->type == NODE_TYPE_ADD_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(old_value->value.number + value->value.number);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        if (old_value != NULL && old_value->type == VALUE_TYPE_STRING && value->type == VALUE_TYPE_STRING) {
            char *concat_string = string_concat(old_value->value.string, value->value.string);
            Value *new_value = value_new_string(concat_string);
            free(concat_string);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        if (old_value != NULL && old_value->type == VALUE_TYPE_STRING && value->type != VALUE_TYPE_STRING) {
            char *value_string = value_to_string(value);
            char *concat_string = string_concat(old_value->value.string, value_string);
            Value *new_value = value_new_string(concat_string);
            free(value_string);
            free(concat_string);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        if (old_value != NULL && old_value->type != VALUE_TYPE_STRING && value->type == VALUE_TYPE_STRING) {
            char *value_string = value_to_string(old_value);
            char *concat_string = string_concat(value_string, value->value.string);
            Value *new_value = value_new_string(concat_string);
            free(value_string);
            free(concat_string);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        printf("[ERROR] Type error by add assign\n");
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_SUB_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(old_value->value.number - value->value.number);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        else {
            printf("[ERROR] Type error by sub assign\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_MUL_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(old_value->value.number * value->value.number);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        else {
            printf("[ERROR] Type error by mul assign\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_EXP_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(pow(old_value->value.number, value->value.number));
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        else {
            printf("[ERROR] Type error by exp assign\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_DIV_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(old_value->value.number / value->value.number);
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        else {
            printf("[ERROR] Type error by div assign\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_MOD_ASSIGN) {
        char *variable = node->value.assign.variable;
        Value *value = interpreter(node->value.assign.node);
        Value *old_value = map_get(vars_map, variable);

        if (old_value != NULL && old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(fmod(old_value->value.number, value->value.number));
            value_free(value);
            value_free(old_value);
            map_set(vars_map, variable, value_copy(new_value));
            return new_value;
        }

        else {
            printf("[ERROR] Type error by mod assign\n");
            exit(EXIT_FAILURE);
        }
    }

    // Unaries
    if (node->type == NODE_TYPE_UNARY_ADD) {
        Value *value = interpreter(node->value.node);

        if (value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(+value->value.number);
            value_free(value);
            return new_value;
        }

        else {
            printf("[ERROR] Type error by unary add\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        Value *value = interpreter(node->value.node);

        if (value->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(-value->value.number);
            value_free(value);
            return new_value;
        }

        else {
            printf("[ERROR] Type error by unary sub\n");
            exit(EXIT_FAILURE);
        }
    }

    if (node->type == NODE_TYPE_NOT) {
        Value *value = interpreter(node->value.node);

        if (value->type == VALUE_TYPE_BOOLEAN) {
            Value *new_value = value_new_boolean(!value->value.boolean);
            value_free(value);
            return new_value;
        }

        else {
            printf("[ERROR] Type error by not\n");
            exit(EXIT_FAILURE);
        }
    }

    // Number operations
    if (node->type == NODE_TYPE_ADD) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(left->value.number + right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            char *concat_string = string_concat(left->value.string, right->value.string);
            Value *new_value = value_new_string(concat_string);
            free(concat_string);
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_STRING && right->type != VALUE_TYPE_STRING) {
            char *right_string = value_to_string(right);
            char *concat_string = string_concat(left->value.string, right_string);
            Value *new_value = value_new_string(concat_string);
            free(right_string);
            free(concat_string);
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type != VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            char *left_string = value_to_string(left);
            char *concat_string = string_concat(left_string, right->value.string);
            Value *new_value = value_new_string(concat_string);
            free(left_string);
            free(concat_string);
            value_free(left);
            value_free(right);
            return new_value;
        }

        printf("[ERROR] Type error by add\n");
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_SUB) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_number(left->value.number - right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_number(left->value.number * right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_number(pow(left->value.number, right->value.number));
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_number(left->value.number / right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_number(fmod(left->value.number, right->value.number));
            value_free(left);
            value_free(right);
            return new_value;
        }

        else {
            printf("[ERROR] Type error by mod\n");
            exit(EXIT_FAILURE);
        }
    }

    // Boolean operations
    if (node->type == NODE_TYPE_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NULL && right->type == VALUE_TYPE_NULL) {
            value_free(left);
            value_free(right);
            return value_new_boolean(true);
        }

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_boolean(left->value.number == right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new_boolean(!strcmp(left->value.string, right->value.string));
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            Value *new_value = value_new_boolean(left->value.boolean == right->value.boolean);
            value_free(left);
            value_free(right);
            return new_value;
        }

        value_free(left);
        value_free(right);
        return value_new_boolean(false);
    }

    if (node->type == NODE_TYPE_NOT_EQUALS) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NULL && right->type == VALUE_TYPE_NULL) {
            value_free(left);
            value_free(right);
            return value_new_boolean(false);
        }

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_boolean(left->value.number != right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_STRING && right->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new_boolean(strcmp(left->value.string, right->value.string));
            value_free(left);
            value_free(right);
            return new_value;
        }

        if (left->type == VALUE_TYPE_BOOLEAN && right->type == VALUE_TYPE_BOOLEAN) {
            Value *new_value = value_new_boolean(left->value.boolean != right->value.boolean);
            value_free(left);
            value_free(right);
            return new_value;
        }

        value_free(left);
        value_free(right);
        return value_new_boolean(true);
    }

    if (node->type == NODE_TYPE_GREATER) {
        Value *left = interpreter(node->value.operation.left);
        Value *right = interpreter(node->value.operation.right);

        if (left->type == VALUE_TYPE_NUMBER && right->type == VALUE_TYPE_NUMBER) {
            Value *new_value = value_new_boolean(left->value.number > right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_boolean(left->value.number >= right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_boolean(left->value.number < right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_boolean(left->value.number <= right->value.number);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_boolean(left->value.boolean && right->value.boolean);
            value_free(left);
            value_free(right);
            return new_value;
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
            Value *new_value = value_new_boolean(left->value.boolean || right->value.boolean);
            value_free(left);
            value_free(right);
            return new_value;
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
