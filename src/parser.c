#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "list.h"
#include "token.h"
#include "node.h"
#include "utils.h"

/*
root = (stat | stat STOP)*
stat = bool |
    VARIABLE (ASSIGN | ADD_ASSIGN | SUB_ASSIGN | MUL_ASSIGN | EXP_ASSIGN | DIV_ASSIGN | MOD_ASSIGN) bool |
    IF LPAREN bool RPAREN block (ELSEIF LPAREN bool RPAREN block)* (ELSE block)?
bool = equals ((AND | OR) equals)*
equals = expr ((EQUALS | NOT_EQUALS | GREATER | GREATER_EQUALS | LOWER | LOWER_EQUALS) expr)*
expr = term ((ADD | SUB) term)* |
term = factor ((MUL | EXP | DIV | MOD) factor)*
factor = ADD factor | SUB factor | NOT factor | LPAREN bool RPAREN |
    NULL | NUMBER | STRING | BOOLEAN | VARIABLE | VARIABLE LPAREN args RPAREN
block = stat | LBLOCK (stat STOP?)* RBLOCK
args = (bool COMMA?)*
*/

ListItem *list_item;

List *parse_args(void) {
    List *arguments = list_new();

    while (((Token *)list_item->value)->type != TOKEN_TYPE_RPAREN) {
        list_add(arguments, parse_bool());

        if (((Token *)list_item->value)->type == TOKEN_TYPE_COMMA) {
            list_item = list_item->next;
        }
    }

    return arguments;
}

List *parse_block(void) {
    List *stats = list_new();

    if (((Token *)list_item->value)->type == TOKEN_TYPE_LBLOCK) {
        list_item = list_item->next;
        while (((Token *)list_item->value)->type != TOKEN_TYPE_RBLOCK) {
            list_add(stats, parse_stat());

            if (((Token *)list_item->value)->type == TOKEN_TYPE_STOP) {
                list_item = list_item->next;
            }
        }
        list_item = list_item->next;
    }
    else {
        list_add(stats, parse_stat());
    }

    return stats;
}

Node *parse_factor(void) {
    Token *token = list_item->value;

    if (token->type == TOKEN_TYPE_ADD) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_ADD);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_SUB) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_SUB);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_NOT) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_NOT);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_NULL) {
        list_item = list_item->next;
        return node_new(NODE_TYPE_NULL);
    }

    if (token->type == TOKEN_TYPE_NUMBER) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_NUMBER);
        node->value.number = token->value.number;
        return node;
    }

    if (token->type == TOKEN_TYPE_STRING) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_STRING);
        node->value.string = string_copy(token->value.string);
        return node;
    }

    if (token->type == TOKEN_TYPE_BOOLEAN) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_BOOLEAN);
        node->value.boolean = token->value.boolean;
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE) {
        if (list_item->next != NULL && ((Token *)list_item->next->value)->type == TOKEN_TYPE_LPAREN) {
            char *variable = string_copy(token->value.string);

            list_item = list_item->next;
            list_item = list_item->next;

            Node *node = node_new(NODE_TYPE_CALL);
            node->value.call.variable = variable;
            node->value.call.arguments = parse_args();

            list_item = list_item->next;

            return node;
        }
        else {
            list_item = list_item->next;
            Node *node = node_new(NODE_TYPE_VARIABLE);
            node->value.string = string_copy(token->value.string);
            return node;
        }
    }

    if (token->type == TOKEN_TYPE_LPAREN) {
        list_item = list_item->next;
        Node *node = parse_bool();
        list_item = list_item->next;
        return node;
    }

    if (token->type == TOKEN_TYPE_STOP) {
        return NULL;
    }

    char *token_string = token_to_string(token);
    printf("[ERROR] Unexpected token: %s\n", token_string);
    exit(EXIT_FAILURE);
}

Node *parse_term(void) {
    Node *node = parse_factor();
    while (
        list_item != NULL && (
            ((Token *)list_item->value)->type == TOKEN_TYPE_MUL ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_EXP ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_DIV ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_MOD
        )
    ) {
        if (((Token *)list_item->value)->type == TOKEN_TYPE_MUL) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_MUL);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_EXP) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_EXP);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_DIV) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_DIV);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_MOD) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_MOD);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }
    }
    return node;
}

Node *parse_expr(void) {
    Node *node = parse_term();
    while (
        list_item != NULL && (
            ((Token *)list_item->value)->type == TOKEN_TYPE_ADD ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_SUB
        )
    ) {
        if (((Token *)list_item->value)->type == TOKEN_TYPE_ADD) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_ADD);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_term();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_SUB) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_SUB);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_term();
            node = new_node;
        }
    }
    return node;
}

Node *parse_equals(void) {
    Node *node = parse_expr();
    while (
        list_item != NULL && (
            ((Token *)list_item->value)->type == TOKEN_TYPE_EQUALS ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_NOT_EQUALS ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_GREATER ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_GREATER_EQUALS ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_LOWER ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_LOWER_EQUALS
        )
    ) {
        if (((Token *)list_item->value)->type == TOKEN_TYPE_EQUALS) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_EQUALS);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_NOT_EQUALS) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_NOT_EQUALS);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_GREATER) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_GREATER);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_GREATER_EQUALS) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_GREATER_EQUALS);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_LOWER) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_LOWER);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_LOWER_EQUALS) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_LOWER_EQUALS);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }
    }
    return node;
}

Node *parse_bool(void) {
    Node *node = parse_equals();
    while (
        list_item != NULL && (
            ((Token *)list_item->value)->type == TOKEN_TYPE_AND ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_OR
        )
    ) {
        if (((Token *)list_item->value)->type == TOKEN_TYPE_AND) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_AND);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_equals();
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_OR) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_OR);
            new_node->value.operation.left = node;
            new_node->value.operation.right = parse_equals();
            node = new_node;
        }
    }
    return node;
}

Node *parse_stat(void) {
    if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_ADD_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_ADD_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_SUB_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_SUB_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_MUL_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_MUL_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_EXP_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_EXP_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_DIV_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_DIV_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (
        ((Token *)list_item->value)->type == TOKEN_TYPE_VARIABLE &&
        list_item->next != NULL &&
        ((Token *)list_item->next->value)->type == TOKEN_TYPE_MOD_ASSIGN
    ) {
        char *variable = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_MOD_ASSIGN);
        new_node->value.assign.variable = variable;
        new_node->value.assign.node = parse_bool();
        return new_node;
    }

    else if (((Token *)list_item->value)->type == TOKEN_TYPE_IF) {
        list_item = list_item->next;
        list_item = list_item->next;
        Node *condition = parse_bool();
        list_item = list_item->next;

        Node *node = node_new(NODE_TYPE_IF);
        node->value.condition.node = condition;
        node->value.condition.nodes = parse_block();
        node->value.condition.next = NULL;

        Node *current_node = node;

        while (
            list_item != NULL && (
                ((Token *)list_item->value)->type == TOKEN_TYPE_ELSEIF ||
                ((Token *)list_item->value)->type == TOKEN_TYPE_ELSE
            )
        ) {
            if (((Token *)list_item->value)->type == TOKEN_TYPE_ELSEIF) {
                list_item = list_item->next;
                list_item = list_item->next;
                Node *new_condition = parse_bool();
                list_item = list_item->next;

                Node *new_node = node_new(NODE_TYPE_IF);
                new_node->value.condition.node = new_condition;
                new_node->value.condition.nodes = parse_block();
                new_node->value.condition.next = NULL;
                current_node->value.condition.next = new_node;
                current_node = new_node;
            }

            if (((Token *)list_item->value)->type == TOKEN_TYPE_ELSE) {
                list_item = list_item->next;

                Node *new_node = node_new(NODE_TYPE_ELSE);
                new_node->value.nodes = parse_block();
                current_node->value.condition.next = new_node;
                current_node = new_node;
            }
        }
        return node;
    }

    else {
        return parse_bool();
    }
}

List *parser(List *tokens) {
    list_item = tokens->first;

    List *nodes_list = list_new();

    if (list_item != NULL) {
        Node *node = parse_stat();
        if (node != NULL) list_add(nodes_list, node);

        while (list_item != NULL) {
            node = parse_stat();
            if (node != NULL) list_add(nodes_list, node);

            if (list_item != NULL && ((Token *)list_item->value)->type == TOKEN_TYPE_STOP) {
                list_item = list_item->next;
            }
        }
    }

    return nodes_list;
}
