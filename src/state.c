#include <stdlib.h>

#include "state.h"

State *state_new(StateType type) {
    State *state = malloc(sizeof(State));
    state->type = type;
    return state;
}

void state_free(State *state) {
    free(state);
}
