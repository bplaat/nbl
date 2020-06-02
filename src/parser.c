#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "utils.h"

/*
root = stat (STOP stat)*
stat = logic | VARIABLE (ASSIGN | ADD_ASSIGN | SUB_ASSIGN | MUL_ASSIGN | EXP_ASSIGN | DIV_ASSIGN | MOD_ASSIGN) logic |
    IF LPAREN logic RPAREN block (ELSE_IF LPAREN logic RPAREN block )* (ELSE block)
logic = comp ((AND | OR) comp)*
comp = expr ((EQUALS | NOT_EQUALS | GREATER | GREATER_EQUALS | LOWER | LOWER_EQUALS) expr)*
expr = term ((ADD | SUB) term)*
term = factor ((MUL | EXP | DIV | MOD) factor)*
factor = ADD factor | SUB factor | NOT factor | LPAREN logic RPAREN | NULL | NUMBER | BOOLEAN | STRING | VARIABLE | -
block = stat | LBLOCK stat (STOP stat)* RBLOCK
args =  - | logic (COMMA logic)*
*/

ListItem *list_item;
Token *prev;
Token *token;
Token *next;

void parser_eat(TokenType type) {
    if (token->type == type) {
        list_item = list_item->next;
        prev = token;
        token = list_item->value;
        next = list_item->next != NULL ? list_item->next->value : NULL;
    } else {
        fprintf(stderr, "[ERROR] parser_eat(): Unexpected token type: %d\n", type);
        exit(EXIT_FAILURE);
    }
}

List *parse_args(void) {
    List *args = list_new();

    if (token->type != TOKEN_TYPE_RPAREN) {
        for (;;) {
            list_add(args, parse_logic());

            if (token->type == TOKEN_TYPE_COMMA) {
                parser_eat(TOKEN_TYPE_COMMA);
                continue;
            }

            if (token->type == TOKEN_TYPE_RPAREN) {
                break;
            }

            fprintf(stderr, "[ERROR] parse_args(): Unexpected token type: %d\n", token->type);
            exit(EXIT_FAILURE);
        }
    }

    return args;
}

List *parse_block(void) {
    List *nodes = list_new();

    if (token->type == TOKEN_TYPE_LBLOCK) {
        parser_eat(TOKEN_TYPE_LBLOCK);

        for (;;) {
            Node *node = parse_stat();
            if (node != NULL) {
                list_add(nodes, node);
            }

            if (token->type == TOKEN_TYPE_STOP) {
                parser_eat(TOKEN_TYPE_STOP);
                continue;
            }

            if (token->type == TOKEN_TYPE_RBLOCK) {
                break;
            }

            fprintf(stderr, "[ERROR] parse_block(): Unexpected token type: %d\n", token->type);
            exit(EXIT_FAILURE);
        }

        parser_eat(TOKEN_TYPE_RBLOCK);
    }

    else {
        list_add(nodes, parse_stat());
    }

    return nodes;
}

Node *parse_factor(void) {
    if (token->type == TOKEN_TYPE_ADD) {
        Node *node = node_new(NODE_TYPE_UNARY_ADD);
        parser_eat(TOKEN_TYPE_ADD);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_SUB) {
        Node *node = node_new(NODE_TYPE_UNARY_SUB);
        parser_eat(TOKEN_TYPE_SUB);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_NOT) {
        Node *node = node_new(NODE_TYPE_NOT);
        parser_eat(TOKEN_TYPE_NOT);
        node->value.node = parse_factor();
        return node;
    }

    if (token->type == TOKEN_TYPE_LPAREN) {
        parser_eat(TOKEN_TYPE_LPAREN);
        Node *node = parse_logic();
        parser_eat(TOKEN_TYPE_RPAREN);
        return node;
    }

    if (token->type == TOKEN_TYPE_NULL) {
        Node *node = node_new(NODE_TYPE_NULL);
        parser_eat(TOKEN_TYPE_NULL);
        return node;
    }

    if (token->type == TOKEN_TYPE_NUMBER) {
        Node *node = node_new(NODE_TYPE_NUMBER);
        node->value.number = token->value.number;
        parser_eat(TOKEN_TYPE_NUMBER);
        return node;
    }

    if (token->type == TOKEN_TYPE_BOOLEAN) {
        Node *node = node_new(NODE_TYPE_BOOLEAN);
        node->value.boolean = token->value.boolean;
        parser_eat(TOKEN_TYPE_BOOLEAN);
        return node;
    }

    if (token->type == TOKEN_TYPE_STRING) {
        Node *node = node_new(NODE_TYPE_STRING);
        node->value.string = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_STRING);
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_LPAREN) {
        Node *node = node_new(NODE_TYPE_CALL);
        node->value.call.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_LPAREN);
        node->value.call.args = parse_args();
        parser_eat(TOKEN_TYPE_RPAREN);
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE) {
        Node *node = node_new(NODE_TYPE_VARIABLE);
        node->value.string = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        return node;
    }

    return NULL;
}

Node *parse_term(void) {
    Node *node = parse_factor();

    while (
        token->type == TOKEN_TYPE_MUL ||
        token->type == TOKEN_TYPE_EXP ||
        token->type == TOKEN_TYPE_DIV ||
        token->type == TOKEN_TYPE_MOD
    ) {
        if (token->type == TOKEN_TYPE_MUL) {
            Node *new_node = node_new(NODE_TYPE_MUL);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_MUL);
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_EXP) {
            Node *new_node = node_new(NODE_TYPE_EXP);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_EXP);
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_DIV) {
            Node *new_node = node_new(NODE_TYPE_DIV);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_DIV);
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_MOD) {
            Node *new_node = node_new(NODE_TYPE_MOD);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_MOD);
            new_node->value.operation.right = parse_factor();
            node = new_node;
        }
    }

    return node;
}

Node *parse_expr(void) {
    Node *node = parse_term();

    while (
        token->type == TOKEN_TYPE_ADD ||
        token->type == TOKEN_TYPE_SUB
    ) {
        if (token->type == TOKEN_TYPE_ADD) {
            Node *new_node = node_new(NODE_TYPE_ADD);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_ADD);
            new_node->value.operation.right = parse_term();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_SUB) {
            Node *new_node = node_new(NODE_TYPE_SUB);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_SUB);
            new_node->value.operation.right = parse_term();
            node = new_node;
        }
    }

    return node;
}

Node *parse_comp(void) {
    Node *node = parse_expr();

    while (
        token->type == TOKEN_TYPE_EQUALS ||
        token->type == TOKEN_TYPE_NOT_EQUALS ||
        token->type == TOKEN_TYPE_GREATER ||
        token->type == TOKEN_TYPE_GREATER_EQUALS ||
        token->type == TOKEN_TYPE_LOWER ||
        token->type == TOKEN_TYPE_LOWER_EQUALS
    ) {
        if (token->type == TOKEN_TYPE_EQUALS) {
            Node *new_node = node_new(NODE_TYPE_EQUALS);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_EQUALS);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_NOT_EQUALS) {
            Node *new_node = node_new(NODE_TYPE_NOT_EQUALS);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_NOT_EQUALS);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_GREATER) {
            Node *new_node = node_new(NODE_TYPE_GREATER);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_GREATER);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_GREATER_EQUALS) {
            Node *new_node = node_new(NODE_TYPE_GREATER_EQUALS);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_GREATER_EQUALS);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_LOWER) {
            Node *new_node = node_new(NODE_TYPE_LOWER);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_LOWER);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_LOWER_EQUALS) {
            Node *new_node = node_new(NODE_TYPE_LOWER_EQUALS);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_LOWER_EQUALS);
            new_node->value.operation.right = parse_expr();
            node = new_node;
        }
    }

    return node;
}

Node *parse_logic(void) {
    Node *node = parse_comp();

    while (
        token->type == TOKEN_TYPE_AND ||
        token->type == TOKEN_TYPE_OR
    ) {
        if (token->type == TOKEN_TYPE_AND) {
            Node *new_node = node_new(NODE_TYPE_AND);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_AND);
            new_node->value.operation.right = parse_comp();
            node = new_node;
        }

        if (token->type == TOKEN_TYPE_OR) {
            Node *new_node = node_new(NODE_TYPE_OR);
            new_node->value.operation.left = node;
            parser_eat(TOKEN_TYPE_OR);
            new_node->value.operation.right = parse_comp();
            node = new_node;
        }
    }

    return node;
}

Node *parse_stat(void) {
    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_ASSIGN) {
        Node *node = node_new(NODE_TYPE_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_ADD_ASSIGN) {
        Node *node = node_new(NODE_TYPE_ADD_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_ADD_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_SUB_ASSIGN) {
        Node *node = node_new(NODE_TYPE_SUB_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_SUB_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_MUL_ASSIGN) {
        Node *node = node_new(NODE_TYPE_MUL_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_MUL_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_EXP_ASSIGN) {
        Node *node = node_new(NODE_TYPE_EXP_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_EXP_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_DIV_ASSIGN) {
        Node *node = node_new(NODE_TYPE_DIV_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_DIV_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_VARIABLE && next != NULL && next->type == TOKEN_TYPE_MOD_ASSIGN) {
        Node *node = node_new(NODE_TYPE_MOD_ASSIGN);
        node->value.assign.variable = string_copy(token->value.string);
        parser_eat(TOKEN_TYPE_VARIABLE);
        parser_eat(TOKEN_TYPE_MOD_ASSIGN);
        node->value.assign.node = parse_logic();
        return node;
    }

    if (token->type == TOKEN_TYPE_IF) {
        Node *node = node_new(NODE_TYPE_IF);
        parser_eat(TOKEN_TYPE_IF);
        parser_eat(TOKEN_TYPE_LPAREN);
        node->value.condition.condition = parse_logic();
        parser_eat(TOKEN_TYPE_RPAREN);
        node->value.condition.nodes = parse_block();
        node->value.condition.next = NULL;

        Node *current_node = node;

        while (token->type == TOKEN_TYPE_ELSE_IF) {
            Node *new_node = node_new(NODE_TYPE_ELSE_IF);
            parser_eat(TOKEN_TYPE_ELSE_IF);
            parser_eat(TOKEN_TYPE_LPAREN);
            new_node->value.condition.condition = parse_logic();
            parser_eat(TOKEN_TYPE_RPAREN);
            new_node->value.condition.nodes = parse_block();
            new_node->value.condition.next = NULL;
            current_node->value.condition.next = new_node;
            current_node = new_node;
        }

        if (token->type == TOKEN_TYPE_ELSE) {
            Node *new_node = node_new(NODE_TYPE_ELSE);
            parser_eat(TOKEN_TYPE_ELSE);
            new_node->value.nodes = parse_block();
            current_node->value.condition.next = new_node;
        }

        return node;
    }

    return parse_logic();
}

List *parser(List *tokens) {
    List *nodes = list_new();

    list_item = tokens->first;
    prev = NULL;
    token = list_item->value;
    next = list_item->next != NULL ? list_item->next->value : NULL;

    for (;;) {
        Node *node = parse_stat();
        if (node != NULL) {
            list_add(nodes, node);
        }

        if (prev != NULL && prev->type == TOKEN_TYPE_RBLOCK) {
            continue;
        }

        if (token->type == TOKEN_TYPE_STOP) {
            parser_eat(TOKEN_TYPE_STOP);
            continue;
        }

        if (token->type == TOKEN_TYPE_END) {
            break;
        }

        fprintf(stderr, "[ERROR] parser(): Unexpected token type: %d\n", token->type);
        exit(EXIT_FAILURE);
    }

    return nodes;
}
