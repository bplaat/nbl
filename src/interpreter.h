#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "node.h"
#include "value.h"
#include "map.h"

Value *interpreter(Node *node);

Value *start_interpreter(Node *node, Map *global_vars_map);

#endif
