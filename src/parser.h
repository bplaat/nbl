#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"
#include "list.h"

void parser_eat(TokenType type);

List *parse_args(void);

List *parse_block(void);

Node *parse_factor(void);

Node *parse_term(void);

Node *parse_expr(void);

Node *parse_comp(void);

Node *parse_logic(void);

Node *parse_stat(void);

List *parser(List *tokens);

#endif
