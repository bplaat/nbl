#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "node.h"
#include "map.h"

double interpreter(Node *node);

double start_interpreter(Node *node, Map *global_vars_map);

#endif
