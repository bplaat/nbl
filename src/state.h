#ifndef STATE_H
#define STATE_H

#include "map.h"
#include "value.h"

typedef struct State {
    struct State *parent;
    Map *env;
} State;

State *state_new(void);

Value *state_get(State *state, char *key);

void state_set(State *state, char *key, Value *value);

State *state_fork(State *state);

void state_free(State *state);

#endif
