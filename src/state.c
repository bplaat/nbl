#include <stdlib.h>

#include "state.h"
#include "value.h"

State *state_new(void) {
    State *state = malloc(sizeof(State));
    state->parent = NULL;
    state->env = map_new();
    return state;
}

Value *state_get(State *state, char *key) {
    Value *value = map_get(state->env, key);
    if (value != NULL) {
        return value;
    }

    if (state->parent != NULL) {
        value = state_get(state->parent, key);
        if (value != NULL) {
            return value;
        }
    }

    return NULL;
}

void state_set(State *state, char *key, Value *value) {
    if (state->parent != NULL) {
        Value *old_value = state_get(state->parent, key);
        if (old_value != NULL) {
            state_set(state->parent, key, value);
            return;
        }
    }

    Value *old_value = map_get(state->env, key);
    if (old_value != NULL) {
        value_free(old_value);
    }

    map_set(state->env, key, value);
}

State *state_fork(State *state) {
    State *new_state = malloc(sizeof(State));
    new_state->parent = state;
    new_state->env = map_new();
    return new_state;
}

void state_free(State *state) {
    map_free(state->env, value_free);

    free(state);
}
