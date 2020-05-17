#define _USE_MATH_DEFINES
#include <math.h>

#include "library.h"
#include "map.h"
#include "value.h"

Value *do_floor(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(floor(value->value.number));
}

Value *do_round(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(round(value->value.number));
}

Value *do_ceil(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(ceil(value->value.number));
}

Value *do_sin(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(sin(value->value.number));
}

Value *do_asin(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(asin(value->value.number));
}

Value *do_cos(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(cos(value->value.number));
}

Value *do_acos(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(acos(value->value.number));
}

Value *do_tan(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(tan(value->value.number));
}

Value *do_atan(List *arguments) {
    Value *value = arguments->first->value;
    return value_new_number(atan(value->value.number));
}

Map *get_library(void) {
    Map *library = map_new();

    map_set(library, "pi", value_new_number(M_PI));
    map_set(library, "e", value_new_number(M_E));

    map_set(library, "floor", value_new_native_function(do_floor));
    map_set(library, "round", value_new_native_function(do_round));
    map_set(library, "ceil", value_new_native_function(do_ceil));

    map_set(library, "sin", value_new_native_function(do_sin));
    map_set(library, "asin", value_new_native_function(do_asin));
    map_set(library, "cos", value_new_native_function(do_cos));
    map_set(library, "acos", value_new_native_function(do_acos));
    map_set(library, "tan", value_new_native_function(do_tan));
    map_set(library, "atan", value_new_native_function(do_atan));

    return library;
}
