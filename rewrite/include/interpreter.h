#pragma once

#include <setjmp.h>

#include "parser.h"
#include "value.h"

typedef struct NblContext {
    jmp_buf exception;
} NblContext;

NblValue nbl_interpreter(NblNode *node);

NblValue nbl_interpreter_part(NblContext *context, NblNode *node);
