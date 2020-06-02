#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "value.h"
#include "state.h"
#include "node.h"

Value *interpreter(State *state, Node *node);

#endif
