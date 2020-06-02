#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "interpreter.h"
#include "value.h"
#include "state.h"
#include "node.h"
#include "utils.h"

Value *interpreter(State *state, Node *node) {
    // Values
    if (node->type == NODE_TYPE_NULL) {
        return value_new_null();
    }

    if (node->type == NODE_TYPE_NUMBER) {
        return value_new_number(node->value.number);
    }

    if (node->type == NODE_TYPE_BOOLEAN) {
        return value_new_boolean(node->value.boolean);
    }

    if (node->type == NODE_TYPE_STRING) {
        return value_new_string(node->value.string);
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        Value *value = state_get(state, node->value.string);
        if (value != NULL) {
            return value_copy(value);
        } else {
            return value_new_null();
        }
    }

    // Call
    if (node->type == NODE_TYPE_CALL) {
        Value *value = state_get(state, node->value.call.variable);

        if (value != NULL && value->type == VALUE_TYPE_NATIVE_FUNCTION) {
            List *args = list_new();
            for (ListItem *list_item = node->value.call.args->first; list_item != NULL; list_item = list_item->next) {
                list_add(args, interpreter(state, list_item->value));
            }

            Value *return_value = value->value.native_function(args);

            list_free(args, node_free);

            return return_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Call unexpected value type: %d\n", value->type);
        exit(EXIT_FAILURE);
    }

    // Assign
    if (node->type == NODE_TYPE_ASSIGN) {
        Value *value = interpreter(state, node->value.assign.node);
        state_set(state, node->value.assign.variable, value_copy(value));
        return value;
    }

    if (node->type == NODE_TYPE_ADD_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number += value->value.number;
            value_free(value);
            return value_copy(old_value);
        }

        if (old_value->type == VALUE_TYPE_STRING && value->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            new_value->value.string = string_concat(old_value->value.string, value->value.string);
            value_free(value);
            state_set(state, node->value.assign.variable, value_copy(new_value));
            return new_value;
        }

        if (old_value->type == VALUE_TYPE_STRING && value->type != VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            char *value_string = value_to_string(value);
            new_value->value.string = string_concat(old_value->value.string, value_string);
            free(value_string);
            value_free(value);
            state_set(state, node->value.assign.variable, value_copy(new_value));
            return new_value;
        }

        if (old_value->type != VALUE_TYPE_STRING && value->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            char *old_value_string = value_to_string(old_value);
            new_value->value.string = string_concat(old_value_string, value->value.string);
            free(old_value_string);
            value_free(value);
            state_set(state, node->value.assign.variable, value_copy(new_value));
            return new_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Addition assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_SUB_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number -= value->value.number;
            value_free(value);
            return value_copy(old_value);
        }

        fprintf(stderr, "[ERROR] interpreter(): Subtract assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_MUL_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number *= value->value.number;
            value_free(value);
            return value_copy(old_value);
        }

        fprintf(stderr, "[ERROR] interpreter(): Multiply assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_EXP_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number = pow(old_value->value.number, value->value.number);
            value_free(value);
            return value_copy(old_value);
        }

        fprintf(stderr, "[ERROR] interpreter(): Power assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_DIV_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number /= value->value.number;
            value_free(value);
            return value_copy(old_value);
        }

        fprintf(stderr, "[ERROR] interpreter(): Divide assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_MOD_ASSIGN) {
        Value *old_value = state_get(state, node->value.assign.variable);
        Value *value = interpreter(state, node->value.assign.node);

        if (old_value->type == VALUE_TYPE_NUMBER && value->type == VALUE_TYPE_NUMBER) {
            old_value->value.number = fmod(old_value->value.number, value->value.number);
            value_free(value);
            return value_copy(old_value);
        }

        fprintf(stderr, "[ERROR] interpreter(): Modulo assign unexpected value type: %d and %d\n", old_value->type, value->type);
        exit(EXIT_FAILURE);
    }

    // Conditions
    if (node->type == NODE_TYPE_IF || node->type == NODE_TYPE_ELSE_IF) {
        State *new_state = state_fork(state);

        Value *value = interpreter(new_state, node->value.condition.condition);
        if (value->type == VALUE_TYPE_BOOLEAN) {
            if (value->value.boolean) {
                for (ListItem *list_item = node->value.condition.nodes->first; list_item != NULL; list_item = list_item->next) {
                    Value *return_value = interpreter(new_state, list_item->value);
                    if (return_value != NULL) {
                        value_free(return_value);
                    }
                }
            }

            else if (node->value.condition.next != NULL) {
                interpreter(state, node->value.condition.next);
            }

            value_free(value);
            state_free(new_state);
            return NULL;
        }

        fprintf(stderr, "[ERROR] interpreter(): Condition unexpected value type: %d\n", value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_ELSE) {
        State *new_state = state_fork(state);
        for (ListItem *list_item = node->value.nodes->first; list_item != NULL; list_item = list_item->next) {
            Value *return_value = interpreter(new_state, list_item->value);
            if (return_value != NULL) {
                value_free(return_value);
            }
        }
        state_free(new_state);
        return NULL;
    }

    // Unary operations
    if (node->type == NODE_TYPE_UNARY_ADD) {
        Value *value = interpreter(state, node->value.node);

        if (value->type == VALUE_TYPE_NUMBER) {
            value->value.number = +value->value.number;
            return value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Unary addition unexpected value type: %d\n", value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        Value *value = interpreter(state, node->value.node);

        if (value->type == VALUE_TYPE_NUMBER) {
            value->value.number = -value->value.number;
            return value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Unary subtract unexpected value type: %d\n", value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_NOT) {
        Value *value = interpreter(state, node->value.node);

        if (value->type == VALUE_TYPE_BOOLEAN) {
            value->value.boolean = !value->value.boolean;
            return value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Not unexpected value type: %d\n", value->type);
        exit(EXIT_FAILURE);
    }

    // Operations
    if (node->type == NODE_TYPE_ADD) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number += right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_STRING && right_value->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            new_value->value.string = string_concat(left_value->value.string, right_value->value.string);
            value_free(left_value);
            value_free(right_value);
            return new_value;
        }

        if (left_value->type == VALUE_TYPE_STRING && right_value->type != VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            char *right_value_string = value_to_string(right_value);
            new_value->value.string = string_concat(left_value->value.string, right_value_string);
            free(right_value_string);
            value_free(left_value);
            value_free(right_value);
            return new_value;
        }

        if (left_value->type != VALUE_TYPE_STRING && right_value->type == VALUE_TYPE_STRING) {
            Value *new_value = value_new(VALUE_TYPE_STRING);
            char *left_value_string = value_to_string(left_value);
            new_value->value.string = string_concat(left_value_string, right_value->value.string);
            free(left_value_string);
            value_free(left_value);
            value_free(right_value);
            return new_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Addition unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_SUB) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number -= right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Subtract unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_MUL) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number *= right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Multiply unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_EXP) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number = pow(left_value->value.number, right_value->value.number);
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Power unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_DIV) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number /= right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Divide unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_MOD) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->value.number = fmod(left_value->value.number, right_value->value.number);
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Modulo unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_EQUALS) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NULL && right_value->type == VALUE_TYPE_NULL) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = true;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number == right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_BOOLEAN && right_value->type == VALUE_TYPE_BOOLEAN) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.boolean == right_value->value.boolean;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_STRING && right_value->type == VALUE_TYPE_STRING) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = !strcmp(left_value->value.string, right_value->value.string);
            value_free(right_value);
            return left_value;
        }

        left_value->type = VALUE_TYPE_BOOLEAN;
        left_value->value.boolean = false;
        value_free(right_value);
        return left_value;
    }

    if (node->type == NODE_TYPE_NOT_EQUALS) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NULL && right_value->type == VALUE_TYPE_NULL) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = false;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number != right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_BOOLEAN && right_value->type == VALUE_TYPE_BOOLEAN) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.boolean != right_value->value.boolean;
            value_free(right_value);
            return left_value;
        }

        if (left_value->type == VALUE_TYPE_STRING && right_value->type == VALUE_TYPE_STRING) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = strcmp(left_value->value.string, right_value->value.string);
            value_free(right_value);
            return left_value;
        }

        left_value->type = VALUE_TYPE_BOOLEAN;
        left_value->value.boolean = true;
        value_free(right_value);
        return left_value;
    }

    if (node->type == NODE_TYPE_GREATER) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number > right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Greater unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_GREATER_EQUALS) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number >= right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Greater equals unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_LOWER) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number < right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Lower unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_LOWER_EQUALS) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_NUMBER && right_value->type == VALUE_TYPE_NUMBER) {
            left_value->type = VALUE_TYPE_BOOLEAN;
            left_value->value.boolean = left_value->value.number <= right_value->value.number;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Lower equals unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_AND) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_BOOLEAN && right_value->type == VALUE_TYPE_BOOLEAN) {
            left_value->value.boolean = left_value->value.boolean && right_value->value.boolean;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): And unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    if (node->type == NODE_TYPE_OR) {
        Value *left_value = interpreter(state, node->value.operation.left);
        Value *right_value = interpreter(state, node->value.operation.right);

        if (left_value->type == VALUE_TYPE_BOOLEAN && right_value->type == VALUE_TYPE_BOOLEAN) {
            left_value->value.boolean = left_value->value.boolean || right_value->value.boolean;
            value_free(right_value);
            return left_value;
        }

        fprintf(stderr, "[ERROR] interpreter(): Or unexpected value type: %d and %d\n", left_value->type, right_value->type);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[ERROR] interpreter(): Unexpected node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}
