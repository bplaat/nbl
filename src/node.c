#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "node.h"
#include "utils.h"

Node *node_new(NodeType type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    return node;
}

char *node_to_string(Node *node) {
    if (node->type == NODE_TYPE_NULL) {
        return string_copy("null");
    }

    if (node->type == NODE_TYPE_NUMBER) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %g )", node->value.number);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_STRING) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( '%s' )", node->value.string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_BOOLEAN) {
        if (node->value.boolean) {
            return string_copy("true");
        } else {
            return string_copy("false");
        }
    }

    if (node->type == NODE_TYPE_VARIABLE) {
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s )", node->value.string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_CALL) {
        char *arguments_string_buffer = malloc(BUFFER_SIZE);
        arguments_string_buffer[0] = '\0';

        ListItem *list_item = node->value.call.arguments->first;
        while (list_item != NULL) {
            char *node_string = node_to_string(list_item->value);
            strcat(arguments_string_buffer, node_string);
            free(node_string);

            if (list_item->next != NULL) {
                strcat(arguments_string_buffer, ", ");
            }
            list_item = list_item->next;
        }

        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s(%s) )", node->value.call.variable, arguments_string_buffer);
        free(arguments_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s = %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ADD_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s += %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_SUB_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s -= %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_MUL_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s *= %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_EXP_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s **= %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_DIV_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s /= %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_MOD_ASSIGN) {
        char *left_string = node->value.assign.variable;
        char *right_string = node_to_string(node->value.assign.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s %%= %s )", left_string, right_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_UNARY_ADD) {
        char *node_string = node_to_string(node->value.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( + %s )", node_string);
        free(node_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_UNARY_SUB) {
        char *node_string = node_to_string(node->value.node);
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

    if (node->type == NODE_TYPE_EQUALS) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s == %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_NOT_EQUALS) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s != %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_GREATER) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s > %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_GREATER_EQUALS) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s >= %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_LOWER) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s < %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_LOWER_EQUALS) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s <= %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_NOT) {
        char *node_string = node_to_string(node->value.node);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( ! %s )", node_string);
        free(node_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_AND) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s && %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_OR) {
        char *left_string = node_to_string(node->value.operation.left);
        char *right_string = node_to_string(node->value.operation.right);
        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( %s || %s )", left_string, right_string);
        free(left_string);
        free(right_string);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_IF) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.condition.nodes->first;
        while (list_item != NULL) {
            char *node_string = node_to_string(list_item->value);
            strcat(statements_string_buffer, node_string);
            free(node_string);

            if (list_item->next != NULL) {
                strcat(statements_string_buffer, "; ");
            }
            list_item = list_item->next;
        }

        char *string_buffer = malloc(BUFFER_SIZE);
        char *node_string = node_to_string(node->value.condition.node);
        if (node->value.condition.next != NULL) {
            char *next_string = node_to_string(node->value.condition.next);
            sprintf(string_buffer, "( if (%s) { %s } %s)", node_string, statements_string_buffer, next_string);
            free(next_string);
        } else {
            sprintf(string_buffer, "( if (%s) { %s } )", node_string, statements_string_buffer);
        }
        free(node_string);
        free(statements_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ELSEIF) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.condition.nodes->first;
        while (list_item != NULL) {
            char *node_string = node_to_string(list_item->value);
            strcat(statements_string_buffer, node_string);
            free(node_string);

            if (list_item->next != NULL) {
                strcat(statements_string_buffer, "; ");
            }
            list_item = list_item->next;
        }

        char *string_buffer = malloc(BUFFER_SIZE);
        char *node_string = node_to_string(node->value.condition.node);
        if (node->value.condition.next != NULL) {
            char *next_string = node_to_string(node->value.condition.next);
            sprintf(string_buffer, "( else if (%s) { %s } %s)", node_string, statements_string_buffer, next_string);
            free(next_string);
        } else {
            sprintf(string_buffer, "( else if (%s) { %s } )", node_string, statements_string_buffer);
        }
        free(node_string);
        free(statements_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_ELSE) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.nodes->first;
        while (list_item != NULL) {
            char *node_string = node_to_string(list_item->value);
            strcat(statements_string_buffer, node_string);
            free(node_string);

            if (list_item->next != NULL) {
                strcat(statements_string_buffer, "; ");
            }
            list_item = list_item->next;
        }

        char *string_buffer = malloc(BUFFER_SIZE);
        sprintf(string_buffer, "( else { %s } )", statements_string_buffer);
        free(statements_string_buffer);
        return string_buffer;
    }

    printf("[ERROR] Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

void node_free(Node *node) {
    if (node->type == NODE_TYPE_STRING || node->type == NODE_TYPE_VARIABLE) {
        free(node->value.string);
    }

    if (node->type == NODE_TYPE_CALL) {
        free(node->value.assign.variable);
        ListItem *list_item = node->value.call.arguments->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.call.arguments);
    }

    if (node->type >= NODE_TYPE_ASSIGN && node->type <= NODE_TYPE_MOD_ASSIGN) {
        free(node->value.assign.variable);
        node_free(node->value.assign.node);
    }

    if (node->type >= NODE_TYPE_UNARY_ADD && node->type <= NODE_TYPE_NOT) {
        node_free(node->value.node);
    }

    if (node->type >= NODE_TYPE_ADD && node->type <= NODE_TYPE_OR) {
        node_free(node->value.operation.left);
        node_free(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_IF || node->type == NODE_TYPE_ELSEIF) {
        node_free(node->value.condition.node);
        ListItem *list_item = node->value.condition.nodes->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.condition.nodes);
        node_free(node->value.condition.next);
    }

    if (node->type == NODE_TYPE_ELSE) {
        ListItem *list_item = node->value.nodes->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.nodes);
    }

    free(node);
}
