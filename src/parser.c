#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "token.h"
#include "node.h"
#include "parser.h"

/*
block = stat (STOP stat)*
stat = VARIABLE ASSIGN expr
expr = term ((ADD | SUB) term)*
term = factor ((MUL | DIV | MOD) factor)*
factor = ADD factor | SUB factor | NUMBER | VARIABLE | LPAREN expr RPAREN
*/

ListItem *list_item;

Node *parse_factor(void) {
    Token *token = list_item->value;

    if (token->type == TOKEN_TYPE_ADD) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_ADD);
        node->value.children = list_new();
        list_add(node->value.children, parse_factor());
        return node;
    }

    else if (token->type == TOKEN_TYPE_SUB) {
        list_item = list_item->next;
        Node *node = node_new(NODE_TYPE_UNARY_SUB);
        node->value.children = list_new();
        list_add(node->value.children, parse_factor());
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
        char *string_copy = malloc(strlen(token->value.string) + 1);
        strcpy(string_copy, token->value.string);
        node->value.string = string_copy;
        return node;
    }

    else if (token->type == TOKEN_TYPE_PAREN_LEFT) {
        list_item = list_item->next;
        Node *node = parse_expr();
        list_item = list_item->next;
        return node;
    }

    else {
        printf("Unexpected token: ");
        token_dump(token);
        putchar('\n');
        return NULL;
    }
}

Node *parse_term(void) {
    Node *node = parse_factor();
    while (
        list_item != NULL && (
            ((Token *)list_item->value)->type == TOKEN_TYPE_MUL ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_DIV ||
            ((Token *)list_item->value)->type == TOKEN_TYPE_MOD
        )
    ) {
        if (((Token *)list_item->value)->type == TOKEN_TYPE_MUL) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_MUL);
            new_node->value.children = list_new();
            list_add(new_node->value.children, node);
            list_add(new_node->value.children, parse_factor());
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_DIV) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_DIV);
            new_node->value.children = list_new();
            list_add(new_node->value.children, node);
            list_add(new_node->value.children, parse_factor());
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_MOD) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_MOD);
            new_node->value.children = list_new();
            list_add(new_node->value.children, node);
            list_add(new_node->value.children, parse_factor());
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
            new_node->value.children = list_new();
            list_add(new_node->value.children, node);
            list_add(new_node->value.children, parse_term());
            node = new_node;
        }

        else if (((Token *)list_item->value)->type == TOKEN_TYPE_SUB) {
            list_item = list_item->next;
            Node *new_node = node_new(NODE_TYPE_SUB);
            new_node->value.children = list_new();
            list_add(new_node->value.children, node);
            list_add(new_node->value.children, parse_term());
            node = new_node;
        }
    }
    return node;
}

Node *parse_stat(void) {
    Node *node = parse_expr();
    while (list_item != NULL && ((Token *)list_item->value)->type == TOKEN_TYPE_ASSIGN) {
        list_item = list_item->next;
        Node *new_node = node_new(NODE_TYPE_ASSIGN);
        new_node->value.children = list_new();
        list_add(new_node->value.children, node);
        list_add(new_node->value.children, parse_expr());
        node = new_node;
    }
    return node;
}

Node *parse_block(void) {
    Node *node = node_new(NODE_TYPE_BLOCK);
    node->value.children = list_new();
    list_add(node->value.children, parse_stat());
    while (list_item != NULL && ((Token *)list_item->value)->type == TOKEN_TYPE_STOP) {
        list_item = list_item->next;
        if (list_item != NULL && ((Token *)list_item->value)->type == TOKEN_TYPE_STOP) continue;
        list_add(node->value.children, parse_stat());
    }
    return node;
}

Node *parser(List *tokens) {
    list_item = tokens->first;
    return parse_block();
}
