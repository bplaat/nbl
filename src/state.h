#ifndef STATE_H
#define STATE_H

#include "map.h"
#include "value.h"

typedef enum StateType {
    STATE_TYPE_RUNNING,
    STATE_TYPE_LOOP_BREAK,
    STATE_TYPE_LOOP_CONTINUE,
    STATE_TYPE_FUNCTION_RETURN
} StateType;

typedef struct State {
    StateType type;
    Map *vars;
} State;

State *state_new(StateType type);

State *state_copy(State *state);

void state_set(State *state, char *key, Value *value);

void state_free(State *state);

#endif
