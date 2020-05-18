#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "node.h"
#include "state.h"
#include "value.h"
#include "map.h"

void interpreter_set(Map *map, char *key, Value *value);

Value *interpreter(Node *node, State *state);

Value *start_interpreter(Node *node, Map *global_vars_map);

#endif
