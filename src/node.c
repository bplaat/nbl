#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "utils.h"
#include "list.h"

Node *node_new(NodeType type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    return node;
}

char *nodes_to_string(List *nodes) {
    char *nodes_string = string_copy("");
    for (ListItem *list_item = nodes->first; list_item != NULL; list_item = list_item->next) {
        char *node_string = node_to_string(list_item->value);
        nodes_string = string_append(nodes_string, node_string);
        free(node_string);
        if (list_item->next != NULL) {
            nodes_string = string_append(nodes_string, "; ");
        }
    }
    return nodes_string;
}

char *node_to_string(Node *node) {
    if (node->type == NODE_TYPE_NULL) {
        return string_copy("( null )");
    }

    if (node->type == NODE_TYPE_NUMBER) {
        return string_format("( %g )", node->value.number);
    }

    if (node->type == NODE_TYPE_BOOLEAN) {
        return string_copy(node->value.boolean ? "( true )" : "( false )");
    }

    if (node->type == NODE_TYPE_STRING) {
        return string_format("( '%s' )", node->value.string);
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        return string_format("( %s )", node->value.string);
    }

    if (node->type == NODE_TYPE_CALL) {
        char *args_string = NULL;

        for (ListItem *list_item = node->value.call.args->first; list_item != NULL; list_item = list_item->next) {
            if (args_string == NULL) {
                args_string = node_to_string(list_item->value);
            } else {
                char *node_string = node_to_string(list_item->value);
                args_string = string_append(args_string, node_string);
                free(node_string);
            }

            if (list_item->next != NULL) {
                args_string = string_append(args_string, ", ");
            }
        }

        char *return_string = string_format("( %s(%s) )", node->value.call.variable, args_string != NULL ? args_string : "");
        free(args_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) = %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_ADD_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) += %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_SUB_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) -= %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_MUL_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) *= %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_EXP_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) **= %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_DIV_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) /= %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_MOD_ASSIGN) {
        char *node_string = node_to_string(node->value.assign.node);
        char *return_string = string_format("( ( %s ) %%= %s )", node->value.assign.variable, node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_IF) {
        char *condition_string = node_to_string(node->value.condition.condition);
        char *nodes_string = nodes_to_string(node->value.condition.nodes);
        char *next_node_string = node->value.condition.next != NULL ? node_to_string(node->value.condition.next) : string_copy("");
        char *return_string = string_format("( if (%s) { %s } %s )", condition_string, nodes_string, next_node_string);
        free(condition_string);
        free(nodes_string);
        free(next_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_ELSE_IF) {
        char *condition_string = node_to_string(node->value.condition.condition);
        char *nodes_string = nodes_to_string(node->value.condition.nodes);
        char *next_node_string = node->value.condition.next != NULL ? node_to_string(node->value.condition.next) : string_copy("");
        char *return_string = string_format("( else if (%s) { %s } %s )", condition_string, nodes_string, next_node_string);
        free(condition_string);
        free(nodes_string);
        free(next_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_ELSE) {
        char *nodes_string = nodes_to_string(node->value.nodes);
        char *return_string = string_format("( else { %s } )", nodes_string);
        free(nodes_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        char *node_string = node_to_string(node->value.node);
        char *return_string = string_format("( + %s )", node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        char *node_string = node_to_string(node->value.node);
        char *return_string = string_format("( - %s )", node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_NOT) {
        char *node_string = node_to_string(node->value.node);
        char *return_string = string_format("( ! %s )", node_string);
        free(node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_ADD) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s + %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_SUB) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s - %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_MUL) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s * %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_EXP) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s ** %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_DIV) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s / %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_MOD) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s % %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_EQUALS) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s == %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_NOT_EQUALS) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s != %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_GREATER) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s > %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_GREATER_EQUALS) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s >= %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_LOWER) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s < %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_LOWER_EQUALS) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s <= %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_AND) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s && %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    if (node->type == NODE_TYPE_OR) {
        char *left_node_string = node_to_string(node->value.operation.left);
        char *right_node_string = node_to_string(node->value.operation.right);
        char *return_string = string_format("( %s || %s )", left_node_string, right_node_string);
        free(left_node_string);
        free(right_node_string);
        return return_string;
    }

    fprintf(stderr, "[ERROR] node_to_string(): Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

void node_free(Node *node) {
    if (node->type == NODE_TYPE_STRING || node->type == NODE_TYPE_VARIABLE) {
        free(node->value.string);
    }

    if (node->type == NODE_TYPE_CALL) {
        free(node->value.call.variable);
        list_free(node->value.call.args, node_free);
    }

    if (node->type >= NODE_TYPE_ASSIGN && node->type <= NODE_TYPE_MOD_ASSIGN) {
        free(node->value.assign.variable);
        node_free(node->value.assign.node);
    }

    if (node->type == NODE_TYPE_IF || node->type == NODE_TYPE_ELSE_IF) {
        node_free(node->value.condition.condition);
        list_free(node->value.condition.nodes, node_free);
        if (node->value.condition.next != NULL) {
            node_free(node->value.condition.next);
        }
    }

    if (node->type == NODE_TYPE_ELSE) {
        list_free(node->value.nodes, node_free);
    }

    if ((node->type >= NODE_TYPE_UNARY_ADD && node->type <= NODE_TYPE_NOT)) {
        node_free(node->value.node);
    }

    if (node->type >= NODE_TYPE_ADD && node->type <= NODE_TYPE_OR) {
        node_free(node->value.operation.left);
        node_free(node->value.operation.right);
    }

    free(node);
}
