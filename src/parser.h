#ifndef PARSER_H
#define PARSER_H

#include "list.h"
#include "node.h"

List *parse_args(void);

List *parse_block(void);

Node *parse_factor(void);

Node *parse_term(void);

Node *parse_expr(void);

Node *parse_equals(void);

Node *parse_bool(void);

Node *parse_stat(void);

List *parser(List *tokens);

#endif
