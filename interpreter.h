#pragma once
#include "map.h"
#include "node.h"

double interpreter(Node *node);

void start_interpreter(Node *node, Map *vars);
