#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "list.h"

Node *parse_factor(void);

Node *parse_term(void);

Node *parse_expr(void);

Node *parse_equals(void);

Node *parse_bool(void);

Node *parse_stat(void);

List *parser(List *tokens);

#endif
