#ifndef STATE_H
#define STATE_H

typedef enum StateType {
    STATE_TYPE_RUNNING,
    STATE_TYPE_LOOP_BREAK,
    STATE_TYPE_LOOP_CONTINUE
} StateType;

typedef struct State {
    StateType type;
} State;

State *state_new(StateType type);

void state_free(State *state);

#endif
