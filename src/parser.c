#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "list.h"
#include "token.h"
#include "node.h"
#include "utils.h"

/*
stat = bool | VARIABLE ASSIGN bool
bool = equals ((AND | OR) equals)*
equals = expr ((EQUALS | NOT_EQUALS | GREATER | GREATER_EQUALS | LOWER | LOWER_EQUALS) expr)*
expr = term ((ADD | SUB) term)* |
term = factor ((MUL | EXP | DIV | MOD) factor)*
factor = ADD factor | SUB factor | NOT factor | NULL | NUMBER | STRING | BOOLEAN | VARIABLE | LPAREN bool RPAREN
*/

ListItem *list_item;

Node *parse_factor(void) {
    Token *token = list_item->value;

    if (token->type == TOKEN_TYPE_ADD) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_ADD);
        node->value.child = parse_factor();
        return node;
    }

    else if (token->type == TOKEN_TYPE_SUB) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_SUB);
        node->value.child = parse_factor();
        return node;
    }

    else if (token->type == TOKEN_TYPE_NOT) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_NOT);
        node->value.child = parse_factor();
        return node;
    }

    else if (token->type == TOKEN_TYPE_NULL) {
        list_item = list_item->next;
        return node_new(NODE_TYPE_NULL);
    }

    else if (token->type == TOKEN_TYPE_NUMBER) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_NUMBER);
        node->value.number = token->value.number;
        return node;
    }

    else if (token->type == TOKEN_TYPE_STRING) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_STRING);
        node->value.string = string_copy(token->value.string);
        return node;
    }

    else if (token->type == TOKEN_TYPE_BOOLEAN) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_BOOLEAN);
        node->value.boolean = token->value.boolean;
        return node;
    }

    else if (token->type == TOKEN_TYPE_VARIABLE) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_VARIABLE);
        node->value.string = string_copy(token->value.string);
        return node;
    }

    else if (token->type == TOKEN_TYPE_LPAREN) {
        list_item = list_item->next;
        Node *node = parse_bool();
        list_item = list_item->next;
        return node;
    }

    else {
        char *token_string = token_to_string(token);
        printf("[ERROR] Unexpected token: %s\n", token_string);
        exit(EXIT_FAILURE);
    }
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
        Node *variable_node = node_new(NODE_TYPE_VARIABLE);
        variable_node->value.string = string_copy(((Token *)list_item->value)->value.string);

        list_item = list_item->next;
        list_item = list_item->next;

        Node *new_node = node_new(NODE_TYPE_ASSIGN);
        new_node->value.operation.left = variable_node;
        new_node->value.operation.right = parse_bool();
        return new_node;
    }
    else {
        return parse_bool();
    }
}

List *parser(List *tokens) {
    list_item = tokens->first;

    List *nodes_list = list_new();

    list_add(nodes_list, parse_stat());

    while (list_item != NULL && ((Token *)list_item->value)->type == TOKEN_TYPE_STOP) {
        list_item = list_item->next;
        list_add(nodes_list, parse_stat());
    }

    return nodes_list;
}