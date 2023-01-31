#include "interpreter.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

NblValue nbl_interpreter(NblNode *node) {
    NblContext context = {0};
    if (!setjmp(context.exception)) {
        return nbl_interpreter_part(&context, node);
    }
    return (NblValue){.type = NBL_VALUE_NULL};
}

NblValue nbl_interpreter_part(NblContext *context, NblNode *node) {
    if (node->type == NBL_NODE_VALUE) {
        return node->value;
    }
    if (node->type == NBL_NODE_ARRAY) {
        NblValue array = {.type = NBL_VALUE_ARRAY, .array = nbl_value_list_new()};
        for (int32_t i = 0; i < node->array.size; i++) {
            NblNode *valueNode = nbl_list_get(&node->array, i);
            nbl_value_list_add(array.array, nbl_interpreter_part(context, valueNode));
        }
        return array;
    }
    if (node->type == NBL_NODE_OBJECT) {
        NblValue object = {.type = NBL_VALUE_OBJECT, .object = nbl_value_map_new()};
        for (int32_t i = 0; i < node->keys.size; i++) {
            NblNode *keyNode = nbl_list_get(&node->keys, i);
            NblValue key = nbl_interpreter_part(context, keyNode);
            if (key.type != NBL_VALUE_STRING) {
                nbl_source_print_error(keyNode->source, keyNode->offset, "Object key type error");
                longjmp(context->exception, 1);
            }

            NblNode *valueNode = nbl_list_get(&node->values, i);
            nbl_value_map_set(object.object, key.string, nbl_interpreter_part(context, valueNode));
            nbl_value_free(key);
        }
        return object;
    }

    if (node->type == NBL_NODE_VARIABLE) {
        // TODO
    }

    if (node->type == NBL_NODE_TENARY) {
        NblValue condition = nbl_interpreter_part(context, node->condition);
        if (condition.type != NBL_VALUE_BOOL) {
            nbl_source_print_error(node->condition->source, node->condition->offset, "Condition type error");
            longjmp(context->exception, 1);
        }
        return nbl_interpreter_part(context, condition.integer ? node->thenBlock : node->elseBlock);
    }

    if (node->type > NBL_NODE_UNARY_BEGIN && node->type < NBL_NODE_UNARY_END) {
        NblValue unary = nbl_interpreter_part(context, node->unary);

        if (node->type == NBL_NODE_CAST) {
            // TODO
        }
        if (node->type == NBL_NODE_NEG) {
            if (unary.type == NBL_VALUE_INT) return (NblValue){.type = NBL_VALUE_INT, .integer = -unary.integer};
            if (unary.type == NBL_VALUE_FLOAT) return (NblValue){.type = NBL_VALUE_FLOAT, .floating = -unary.floating};
        }
        if (node->type == NBL_NODE_NOT && unary.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = ~unary.integer};
        }
        if (node->type == NBL_NODE_LOGICAL_NOT && unary.type == NBL_VALUE_BOOL) {
            return (NblValue){.type = NBL_VALUE_BOOL, .integer = !unary.integer};
        }

        nbl_source_print_error(node->source, node->offset, "Type error");
        longjmp(context->exception, 1);
    }

    if (node->type > NBL_NODE_OPERATOR_BEGIN && node->type < NBL_NODE_OPERATOR_END) {
        NblValue lhs = nbl_interpreter_part(context, node->lhs);
        NblValue rhs = nbl_interpreter_part(context, node->rhs);

        if (node->type == NBL_NODE_ADD) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer + rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating + rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.integer + rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating + rhs.floating};

            if (lhs.type == NBL_VALUE_STRING || rhs.type == NBL_VALUE_STRING) {
                if (lhs.type == NBL_VALUE_STRING && rhs.type == NBL_VALUE_STRING) {
                    int32_t concatSize = lhs.string_size + rhs.string_size;
                    char *concatString = malloc(concatSize + 1);
                    strcpy(concatString, lhs.string);
                    strcat(concatString, rhs.string);
                    nbl_value_free(lhs);
                    nbl_value_free(rhs);
                    return (NblValue){.type = NBL_VALUE_STRING, .string_size = concatSize, .string = concatString};
                }

                char *lhsString = nbl_value_to_string(lhs);
                char *rhsString = nbl_value_to_string(rhs);
                int32_t concatSize = strlen(lhsString) + strlen(rhsString);
                char *concatString = malloc(concatSize + 1);
                strcpy(concatString, lhsString);
                strcat(concatString, rhsString);
                free(lhsString);
                free(rhsString);
                nbl_value_free(lhs);
                nbl_value_free(rhs);
                return (NblValue){.type = NBL_VALUE_STRING, .string_size = concatSize, .string = concatString};
            }
        }
        if (node->type == NBL_NODE_SUB) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer - rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating - rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.integer - rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating - rhs.floating};
        }
        if (node->type == NBL_NODE_MUL) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer * rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating * rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.integer * rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = lhs.floating * rhs.floating};
        }
        if (node->type == NBL_NODE_EXP) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = pow(lhs.integer, rhs.integer)};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = pow(lhs.floating, rhs.integer)};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = pow(lhs.integer, rhs.floating)};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = pow(lhs.floating, rhs.floating)};
        }
        if (node->type == NBL_NODE_DIV) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = rhs.integer == 0 ? 0 : lhs.integer / rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT,
                                  .floating = rhs.floating == 0 ? 0 : lhs.floating / rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT,
                                  .floating = rhs.floating == 0 ? 0 : lhs.integer / rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT,
                                  .floating = rhs.floating == 0 ? 0 : lhs.floating / rhs.floating};
        }
        if (node->type == NBL_NODE_MOD) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer % rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = fmod(lhs.floating, rhs.integer)};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = fmod(lhs.integer, rhs.floating)};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_FLOAT, .floating = fmod(lhs.floating, rhs.floating)};
        }

        if (node->type == NBL_NODE_AND && lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer & rhs.integer};
        }
        if (node->type == NBL_NODE_XOR && lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer ^ rhs.integer};
        }
        if (node->type == NBL_NODE_OR && lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer | rhs.integer};
        }
        if (node->type == NBL_NODE_SHL && lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer << rhs.integer};
        }
        if (node->type == NBL_NODE_SHR && lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT) {
            return (NblValue){.type = NBL_VALUE_INT, .integer = lhs.integer >> rhs.integer};
        }

        if (node->type == NBL_NODE_EQ) {
            if (lhs.type == NBL_VALUE_NULL)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = rhs.type == NBL_VALUE_NULL};
            if (rhs.type == NBL_VALUE_NULL)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.type == NBL_VALUE_NULL};
            if ((lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) ||
                (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT))
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer == rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating == rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer == rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating == rhs.floating};
        }
        if (node->type == NBL_NODE_NEQ) {
            if (lhs.type == NBL_VALUE_NULL)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = rhs.type != NBL_VALUE_NULL};
            if (rhs.type == NBL_VALUE_NULL)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.type != NBL_VALUE_NULL};
            if ((lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) ||
                (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT))
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer != rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating != rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer != rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating != rhs.floating};
        }

        if (node->type == NBL_NODE_LT) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer < rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating < rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer < rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating < rhs.floating};
        }
        if (node->type == NBL_NODE_LTEQ) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer <= rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating <= rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer <= rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating <= rhs.floating};
        }
        if (node->type == NBL_NODE_GT) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer > rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating > rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer > rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating > rhs.floating};
        }
        if (node->type == NBL_NODE_GTEQ) {
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer >= rhs.integer};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_INT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating >= rhs.integer};
            if (lhs.type == NBL_VALUE_INT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer >= rhs.floating};
            if (lhs.type == NBL_VALUE_FLOAT && rhs.type == NBL_VALUE_FLOAT)
                return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.floating >= rhs.floating};
        }

        if (node->type == NBL_NODE_LOGICAL_AND && lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) {
            return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer && rhs.integer};
        }
        if (node->type == NBL_NODE_LOGICAL_OR && lhs.type == NBL_VALUE_BOOL && rhs.type == NBL_VALUE_BOOL) {
            return (NblValue){.type = NBL_VALUE_BOOL, .integer = lhs.integer || rhs.integer};
        }

        nbl_source_print_error(node->source, node->offset, "Type error");
        longjmp(context->exception, 1);
    }

    nbl_source_print_error(node->source, node->offset, "Unkown node");
    longjmp(context->exception, 1);
}
