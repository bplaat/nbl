#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "node.h"
#include "state.h"
#include "value.h"

Value *interpreter(Node *node, State *state);

#endif
