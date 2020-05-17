#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "list.h"
#include "token.h"
#include "node.h"
#include "utils.h"

/*
stat = VARIABLE ASSIGN expr | expr
expr = term ((ADD | SUB) term)*
term = factor ((MUL | EXP | DIV | MOD) factor)*
factor = ADD factor | SUB factor | NUMBER | VARIABLE | LPAREN expr RPAREN
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

    else if (token->type == TOKEN_TYPE_NUMBER) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_NUMBER);
        node->value.number = token->value.number;
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
        Node *node = parse_expr();
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
        new_node->value.operation.right = parse_expr();
        return new_node;
    }
    else {
        return parse_expr();
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
