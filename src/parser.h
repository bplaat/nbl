#ifndef PARSER_H
#define PARSER_H

#include "list.h"

Node *parse_factor(void);

Node *parse_term(void);

Node *parse_expr(void);

Node *parse_stat(void);

Node *parse_block(void);

Node *parser(List *tokens);

#endif
