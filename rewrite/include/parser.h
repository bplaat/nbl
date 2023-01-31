#pragma once

#include <setjmp.h>
#include <stdio.h>

#include "containers.h"
#include "lexer.h"
#include "value.h"

// Node
typedef enum NblNodeType {
    NBL_NODE_NODES,
    NBL_NODE_BLOCK,

    NBL_NODE_VALUE,
    NBL_NODE_ARRAY,
    NBL_NODE_OBJECT,

    NBL_NODE_VARIABLE,

    NBL_NODE_BLOCKS_BEGIN,
    NBL_NODE_TENARY,
    NBL_NODE_IF,
    NBL_NODE_WHILE,
    NBL_NODE_DOWHILE,
    NBL_NODE_BLOCKS_END,

    NBL_NODE_CONTINUE,
    NBL_NODE_BREAK,

    NBL_NODE_UNARY_BEGIN,
    NBL_NODE_CAST,
    NBL_NODE_INC_PRE,
    NBL_NODE_INC_POST,
    NBL_NODE_DEC_PRE,
    NBL_NODE_DEC_POST,
    NBL_NODE_NEG,
    NBL_NODE_NOT,
    NBL_NODE_LOGICAL_NOT,
    NBL_NODE_RETURN,
    NBL_NODE_UNARY_END,

    NBL_NODE_OPERATOR_BEGIN,
    NBL_NODE_ASSIGN,
    NBL_NODE_ASSIGN_LET,
    NBL_NODE_ASSIGN_CONST,
    NBL_NODE_ADD,
    NBL_NODE_SUB,
    NBL_NODE_MUL,
    NBL_NODE_EXP,
    NBL_NODE_DIV,
    NBL_NODE_MOD,
    NBL_NODE_AND,
    NBL_NODE_XOR,
    NBL_NODE_OR,
    NBL_NODE_SHL,
    NBL_NODE_SHR,
    NBL_NODE_EQ,
    NBL_NODE_NEQ,
    NBL_NODE_LT,
    NBL_NODE_LTEQ,
    NBL_NODE_GT,
    NBL_NODE_GTEQ,
    NBL_NODE_LOGICAL_AND,
    NBL_NODE_LOGICAL_OR,
    NBL_NODE_OPERATOR_END
} NblNodeType;

typedef struct NblNode NblNode;
struct NblNode {
    int32_t refs;
    NblNodeType type;
    NblSource *source;
    int32_t offset;
    union {
        NblList nodes;
        NblValue value;
        NblList array;
        struct {
            NblList keys;
            NblList values;
        };
        char *variable;
        struct {
            NblNode *condition;
            NblNode *thenBlock;
            NblNode *elseBlock;
        };
        struct {
            NblValueType castType;
            NblNode *unary;
        };
        struct {
            NblValueType declarationType;
            NblNode *lhs;
            NblNode *rhs;
        };
    };
};

NblNode *nbl_node_new(NblNodeType type, NblSource *source, int32_t offset);

NblNode *nbl_node_new_multiple(NblNodeType type, NblSource *source, int32_t offset);

NblNode *nbl_node_new_value(NblSource *source, int32_t offset, NblValue value);

NblNode *nbl_node_new_unary(NblNodeType type, NblSource *source, int32_t offset, NblNode *unary);

NblNode *nbl_node_new_operation(NblNodeType type, NblSource *source, int32_t offset, NblNode *lhs, NblNode *rhs);

NblNode *nbl_node_refs(NblNode *node);

void nbl_node_print(NblNode *node, FILE *f);

void nbl_node_release(NblNode *node);

// Parser
typedef struct NblParser {
    NblSource *source;
    NblToken *tokens;
    int32_t tokensSize;
    int32_t position;
    jmp_buf exception;
} NblParser;

NblNode *nbl_parser(NblSource *source, NblToken *tokens, int32_t tokensSize);

void nbl_parser_eat(NblParser *parser, NblTokenType type);

NblValueType nbl_parser_type(NblParser *parser);

NblNode *nbl_parser_program(NblParser *parser);
NblNode *nbl_parser_block(NblParser *parser);
NblNode *nbl_parser_statement(NblParser *parser);
NblNode *nbl_parser_declarations(NblParser *parser);
NblNode *nbl_parser_assigns(NblParser *parser);
NblNode *nbl_parser_assign(NblParser *parser);
NblNode *nbl_parser_tenary(NblParser *parser);
NblNode *nbl_parser_logical(NblParser *parser);
NblNode *nbl_parser_equality(NblParser *parser);
NblNode *nbl_parser_relational(NblParser *parser);
NblNode *nbl_parser_bitwise(NblParser *parser);
NblNode *nbl_parser_shift(NblParser *parser);
NblNode *nbl_parser_add(NblParser *parser);
NblNode *nbl_parser_mul(NblParser *parser);
NblNode *nbl_parser_unary(NblParser *parser);
NblNode *nbl_parser_primary(NblParser *parser);
