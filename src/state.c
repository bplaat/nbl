#include <stdlib.h>

#include "state.h"
#include "utils.h"

State *state_new(StateType type) {
    State *state = malloc(sizeof(State));
    state->type = type;
    return state;
}

State *state_copy(State *state) {
    State *new_state = malloc(sizeof(State));
    new_state->type = state->type;

    new_state->vars = map_new();
    MapItem *map_item = state->vars->first;
    while (map_item != NULL) {
        map_set(new_state->vars, map_item->key, value_copy(map_item->value));
        map_item = map_item->next;
    }

    return new_state;
}

void state_set(State *state, char *key, Value *value) {
    Value *old_value = map_get(state->vars, key);
    if (old_value == NULL) {
        map_set(state->vars, string_copy(key), value_copy(value));
    } else {
        value_free(old_value);
        map_set(state->vars, key, value_copy(value));
    }
}

void state_free(State *state) {
    MapItem *map_item = state->vars->first;
    while (map_item != NULL) {
        free(map_item->key);
        value_free(map_item->value);
        map_item = map_item->next;
    }

    free(state);
}
