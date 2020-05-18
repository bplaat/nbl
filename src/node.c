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

    // if (node->type == NODE_TYPE_FUNCTION) {
    //     char *variables_string_buffer = malloc(BUFFER_SIZE);
    //     variables_string_buffer[0] = '\0';

    //     ListItem *list_item = node->value.function.variables->first;
    //     while (list_item != NULL) {
    //         strcat(variables_string_buffer, list_item->value);

    //         if (list_item->next != NULL) {
    //             strcat(variables_string_buffer, ", ");
    //         }
    //         list_item = list_item->next;
    //     }

    //     char *statements_string_buffer = malloc(BUFFER_SIZE);
    //     statements_string_buffer[0] = '\0';

    //     list_item = node->value.function.nodes->first;
    //     while (list_item != NULL) {
    //         char *node_string = node_to_string(list_item->value);
    //         strcat(statements_string_buffer, node_string);
    //         free(node_string);

    //         if (list_item->next != NULL) {
    //             strcat(statements_string_buffer, "; ");
    //         }
    //         list_item = list_item->next;
    //     }

    //     char *string_buffer = malloc(BUFFER_SIZE);
    //     sprintf(string_buffer, "( function (%s) { %s } )", variables_string_buffer, statements_string_buffer);
    //     free(variables_string_buffer);
    //     free(statements_string_buffer);
    //     return string_buffer;
    // }

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
        char *node_string = node_to_string(node->value.condition.cond);
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

    if (node->type == NODE_TYPE_ELSE_IF) {
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
        char *node_string = node_to_string(node->value.condition.cond);
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

    if (node->type == NODE_TYPE_WHILE) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.while_loop.nodes->first;
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
        char *node_string = node_to_string(node->value.while_loop.cond);
        sprintf(string_buffer, "( while (%s) { %s } )", node_string, statements_string_buffer);
        free(node_string);
        free(statements_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_DO_WHILE) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.while_loop.nodes->first;
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
        char *node_string = node_to_string(node->value.while_loop.cond);
        sprintf(string_buffer, "( do { %s } while (%s) )", statements_string_buffer, node_string);
        free(node_string);
        free(statements_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_FOR) {
        char *statements_string_buffer = malloc(BUFFER_SIZE);
        statements_string_buffer[0] = '\0';

        ListItem *list_item = node->value.for_loop.nodes->first;
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
        char *init_string = node_to_string(node->value.for_loop.init);
        char *cond_string = node_to_string(node->value.for_loop.cond);
        char *inc_string = node_to_string(node->value.for_loop.inc);
        sprintf(string_buffer, "( for (%s; %s; %s) { %s } )", init_string, cond_string, inc_string, statements_string_buffer);
        free(init_string);
        free(cond_string);
        free(inc_string);
        free(statements_string_buffer);
        return string_buffer;
    }

    if (node->type == NODE_TYPE_BREAK) {
        return string_copy("( break )");
    }

    if (node->type == NODE_TYPE_CONTINUE) {
        return string_copy("( continue )");
    }

    // if (node->type == NODE_TYPE_RETURN) {
    //     char *node_string = node_to_string(node->value.node);
    //     char *string_buffer = malloc(BUFFER_SIZE);
    //     sprintf(string_buffer, "( return %s )", node_string);
    //     free(node_string);
    //     return string_buffer;
    // }

    printf("[ERROR] Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}

Node *node_copy(Node *node) {
    Node *new_node = malloc(sizeof(Node));
    new_node->type = node->type;

    if (node->type == NODE_TYPE_NUMBER) {
        new_node->value.number = node->value.number;
    }

    if (node->type == NODE_TYPE_STRING || node->type == NODE_TYPE_VARIABLE) {
        new_node->value.string = string_copy(node->value.string);
    }

    if (node->type == NODE_TYPE_BOOLEAN) {
        new_node->value.boolean = node->value.boolean;
    }

    // if (node->type == NODE_TYPE_FUNCTION) {
    //     new_node->value.function.variables = list_new();
    //     ListItem *list_item = node->value.function.variables->first;
    //     while (list_item != NULL) {
    //         list_add(new_node->value.function.variables, string_copy(list_item->value));
    //         list_item = list_item->next;
    //     }

    //     new_node->value.function.nodes = list_new();
    //     list_item = node->value.function.nodes->first;
    //     while (list_item != NULL) {
    //         list_add(new_node->value.function.nodes, node_copy(list_item->value));
    //         list_item = list_item->next;
    //     }
    // }

    if (node->type == NODE_TYPE_CALL) {
        new_node->value.assign.variable = string_copy(node->value.assign.variable);

        new_node->value.call.arguments = list_new();
        ListItem *list_item = node->value.call.arguments->first;
        while (list_item != NULL) {
            list_add(new_node->value.call.arguments, node_copy(list_item->value));
            list_item = list_item->next;
        }
    }

    if (node->type >= NODE_TYPE_ASSIGN && node->type <= NODE_TYPE_MOD_ASSIGN) {
        new_node->value.assign.variable = string_copy(node->value.assign.variable);
    }

    if ((node->type >= NODE_TYPE_UNARY_ADD && node->type <= NODE_TYPE_NOT)) { // || node->type == NODE_TYPE_RETURN) {
        new_node->value.node = node_copy(node->value.node);
    }

    if (node->type >= NODE_TYPE_ADD && node->type <= NODE_TYPE_OR) {
        new_node->value.operation.left = node_copy(node->value.operation.left);
        new_node->value.operation.right = node_copy(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_IF || node->type == NODE_TYPE_ELSE_IF) {
        new_node->value.condition.cond = node_copy(node->value.condition.cond);

        new_node->value.condition.nodes = list_new();
        ListItem *list_item = node->value.condition.nodes->first;
        while (list_item != NULL) {
            list_add(new_node->value.condition.nodes, node_copy(list_item->value));
            list_item = list_item->next;
        }

        new_node->value.condition.next = node_copy(node->value.condition.next);
    }

    if (node->type == NODE_TYPE_ELSE) {
        new_node->value.nodes = list_new();
        ListItem *list_item = node->value.nodes->first;
        while (list_item != NULL) {
            list_add(new_node->value.nodes, node_copy(list_item->value));
            list_item = list_item->next;
        }
    }

    if (node->type == NODE_TYPE_WHILE || node->type == NODE_TYPE_DO_WHILE) {
        new_node->value.while_loop.cond = node_copy(node->value.while_loop.cond);

        new_node->value.while_loop.nodes = list_new();
        ListItem *list_item = node->value.while_loop.nodes->first;
        while (list_item != NULL) {
            list_add(new_node->value.while_loop.nodes, node_copy(list_item->value));
            list_item = list_item->next;
        }
    }

    if (node->type == NODE_TYPE_FOR) {
        new_node->value.for_loop.init = node_copy(node->value.for_loop.init);
        new_node->value.for_loop.cond = node_copy(node->value.for_loop.cond);
        new_node->value.for_loop.inc = node_copy(node->value.for_loop.inc);

        new_node->value.for_loop.nodes = list_new();
        ListItem *list_item = node->value.for_loop.nodes->first;
        while (list_item != NULL) {
            list_add(new_node->value.for_loop.nodes, node_copy(list_item->value));
            list_item = list_item->next;
        }
    }

    return new_node;
}

void node_free(Node *node) {
    if (node->type == NODE_TYPE_STRING || node->type == NODE_TYPE_VARIABLE) {
        free(node->value.string);
    }

    // if (node->type == NODE_TYPE_FUNCTION) {
    //     ListItem *list_item = node->value.function.variables->first;
    //     while (list_item != NULL) {
    //         free(list_item->value);
    //         list_item = list_item->next;
    //     }
    //     list_free(node->value.function.variables);

    //     list_item = node->value.function.nodes->first;
    //     while (list_item != NULL) {
    //         node_free(list_item->value);
    //         list_item = list_item->next;
    //     }
    //     list_free(node->value.function.nodes);
    // }

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

    if ((node->type >= NODE_TYPE_UNARY_ADD && node->type <= NODE_TYPE_NOT)) { // || node->type == NODE_TYPE_RETURN) {
        node_free(node->value.node);
    }

    if (node->type >= NODE_TYPE_ADD && node->type <= NODE_TYPE_OR) {
        node_free(node->value.operation.left);
        node_free(node->value.operation.right);
    }

    if (node->type == NODE_TYPE_IF || node->type == NODE_TYPE_ELSE_IF) {
        node_free(node->value.condition.cond);

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

    if (node->type == NODE_TYPE_WHILE || node->type == NODE_TYPE_DO_WHILE) {
        node_free(node->value.while_loop.cond);

        ListItem *list_item = node->value.while_loop.nodes->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.while_loop.nodes);
    }

    if (node->type == NODE_TYPE_FOR) {
        node_free(node->value.for_loop.init);
        node_free(node->value.for_loop.cond);
        node_free(node->value.for_loop.inc);

        ListItem *list_item = node->value.for_loop.nodes->first;
        while (list_item != NULL) {
            node_free(list_item->value);
            list_item = list_item->next;
        }
        list_free(node->value.for_loop.nodes);
    }

    free(node);
}
