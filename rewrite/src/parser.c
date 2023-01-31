#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

NblNode *nbl_node_new(NblNodeType type, NblSource *source, int32_t offset) {
    NblNode *node = calloc(1, sizeof(NblNode));
    node->refs = 1;
    node->type = type;
    node->source = nbl_source_ref(source);
    node->offset = offset;
    return node;
}

NblNode *nbl_node_new_multiple(NblNodeType type, NblSource *source, int32_t offset) {
    NblNode *node = nbl_node_new(type, source, offset);
    nbl_list_init(&node->nodes);
    return node;
}

NblNode *nbl_node_new_value(NblSource *source, int32_t offset, NblValue value) {
    NblNode *node = nbl_node_new(NBL_NODE_VALUE, source, offset);
    node->value = value;
    return node;
}

NblNode *nbl_node_new_unary(NblNodeType type, NblSource *source, int32_t offset, NblNode *unary) {
    NblNode *node = nbl_node_new(type, source, offset);
    node->unary = unary;
    return node;
}

NblNode *nbl_node_new_operation(NblNodeType type, NblSource *source, int32_t offset, NblNode *lhs, NblNode *rhs) {
    NblNode *node = nbl_node_new(type, source, offset);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

NblNode *nbl_node_ref(NblNode *node) {
    node->refs++;
    return node;
}

void nbl_node_print(NblNode *node, FILE *f) {
    if (node->type == NBL_NODE_NODES || node->type == NBL_NODE_BLOCK) {
        if (node->type == NBL_NODE_BLOCK) fprintf(f, "block ");
        fprintf(f, "{\n");
        for (int32_t i = 0; i < node->nodes.size; i++) {
            NblNode *childNode = nbl_list_get(&node->nodes, i);
            nbl_node_print(childNode, f);
            fprintf(f, ";\n");
        }
        fprintf(f, "}\n");
    }

    if (node->type == NBL_NODE_VALUE) {
        if (node->value.type == NBL_VALUE_STRING) fprintf(f, "'");
        char *valueString = nbl_value_to_string(node->value);
        fprintf(f, "%s", valueString);
        free(valueString);
        if (node->value.type == NBL_VALUE_STRING) fprintf(f, "'");
    }
    if (node->type == NBL_NODE_ARRAY) {
        fprintf(f, "[");
        if (node->array.size > 0) fprintf(f, " ");
        for (int32_t i = 0; i < node->array.size; i++) {
            NblNode *valueNode = nbl_list_get(&node->array, i);
            nbl_node_print(valueNode, f);
            if (i != node->array.size - 1) fprintf(f, ", ");
        }
        if (node->array.size > 0) fprintf(f, " ");
        fprintf(f, "]");
    }
    if (node->type == NBL_NODE_OBJECT) {
        fprintf(f, "{");
        if (node->keys.size > 0) fprintf(f, " ");
        for (int32_t i = 0; i < node->keys.size; i++) {
            NblNode *keyNode = nbl_list_get(&node->keys, i);
            nbl_node_print(keyNode, f);
            fprintf(f, ": ");
            NblNode *valueNode = nbl_list_get(&node->values, i);
            nbl_node_print(valueNode, f);
            if (i != node->keys.size - 1) fprintf(f, ", ");
        }
        if (node->keys.size > 0) fprintf(f, " ");
        fprintf(f, "}");
    }

    if (node->type == NBL_NODE_VARIABLE) {
        fprintf(f, "%s", node->variable);
    }

    if (node->type == NBL_NODE_TENARY) {
        fprintf(f, "(");
        nbl_node_print(node->condition, f);
        fprintf(f, " ? ");
        nbl_node_print(node->thenBlock, f);
        fprintf(f, " : ");
        nbl_node_print(node->elseBlock, f);
        fprintf(f, ")");
    }
    if (node->type == NBL_NODE_IF) {
        fprintf(f, "if (");
        nbl_node_print(node->condition, f);
        fprintf(f, ")\n");
        nbl_node_print(node->thenBlock, f);
        if (node->elseBlock) {
            fprintf(f, "else\n");
            nbl_node_print(node->elseBlock, f);
        }
    }
    if (node->type == NBL_NODE_WHILE) {
        fprintf(f, "while (");
        nbl_node_print(node->condition, f);
        fprintf(f, ")\n");
        nbl_node_print(node->thenBlock, f);
    }
    if (node->type == NBL_NODE_DOWHILE) {
        fprintf(f, "do\n");
        nbl_node_print(node->thenBlock, f);
        fprintf(f, "while (");
        nbl_node_print(node->condition, f);
        fprintf(f, ")");
    }

    if (node->type == NBL_NODE_CONTINUE) {
        fprintf(f, "continue");
    }
    if (node->type == NBL_NODE_BREAK) {
        fprintf(f, "break");
    }

    if (node->type > NBL_NODE_UNARY_BEGIN && node->type < NBL_NODE_UNARY_END) {
        fprintf(f, "(");
        if (node->type == NBL_NODE_CAST) fprintf(f, "(%s)", nbl_value_type_to_string(node->castType));
        if (node->type == NBL_NODE_INC_PRE) fprintf(f, "++ ");
        if (node->type == NBL_NODE_DEC_PRE) fprintf(f, "-- ");
        if (node->type == NBL_NODE_NEG) fprintf(f, "- ");
        if (node->type == NBL_NODE_NOT) fprintf(f, "~ ");
        if (node->type == NBL_NODE_LOGICAL_NOT) fprintf(f, "! ");
        if (node->type == NBL_NODE_RETURN) fprintf(f, "return ");
        nbl_node_print(node->unary, f);
        if (node->type == NBL_NODE_INC_POST) fprintf(f, ". ++");
        if (node->type == NBL_NODE_DEC_POST) fprintf(f, " --");
        fprintf(f, ")");
    }

    if (node->type > NBL_NODE_OPERATOR_BEGIN && node->type < NBL_NODE_OPERATOR_END) {
        fprintf(f, "(");
        if (node->type == NBL_NODE_ASSIGN_LET) fprintf(f, "let ");
        if (node->type == NBL_NODE_ASSIGN_CONST) fprintf(f, "const ");
        nbl_node_print(node->lhs, f);
        if (node->type == NBL_NODE_ASSIGN) fprintf(f, " = ");
        if (node->type == NBL_NODE_ASSIGN_LET || node->type == NBL_NODE_ASSIGN_CONST) {
            fprintf(f, " : %s = ", nbl_value_type_to_string(node->declarationType));
        }
        if (node->type == NBL_NODE_ADD) fprintf(f, " + ");
        if (node->type == NBL_NODE_SUB) fprintf(f, " - ");
        if (node->type == NBL_NODE_MUL) fprintf(f, " * ");
        if (node->type == NBL_NODE_EXP) fprintf(f, " ** ");
        if (node->type == NBL_NODE_DIV) fprintf(f, " / ");
        if (node->type == NBL_NODE_MOD) fprintf(f, " %% ");
        if (node->type == NBL_NODE_AND) fprintf(f, " & ");
        if (node->type == NBL_NODE_XOR) fprintf(f, " ^ ");
        if (node->type == NBL_NODE_OR) fprintf(f, " | ");
        if (node->type == NBL_NODE_SHL) fprintf(f, " << ");
        if (node->type == NBL_NODE_SHR) fprintf(f, " >> ");
        if (node->type == NBL_NODE_EQ) fprintf(f, " == ");
        if (node->type == NBL_NODE_NEQ) fprintf(f, " != ");
        if (node->type == NBL_NODE_LT) fprintf(f, " < ");
        if (node->type == NBL_NODE_LTEQ) fprintf(f, " <= ");
        if (node->type == NBL_NODE_GT) fprintf(f, " > ");
        if (node->type == NBL_NODE_GTEQ) fprintf(f, " >= ");
        if (node->type == NBL_NODE_LOGICAL_AND) fprintf(f, " && ");
        if (node->type == NBL_NODE_LOGICAL_OR) fprintf(f, " || ");
        nbl_node_print(node->rhs, f);
        fprintf(f, ")");
    }
}

void nbl_node_release(NblNode *node) {
    node->refs--;
    if (node->refs > 0) return;

    nbl_source_release(node->source);

    if (node->type == NBL_NODE_NODES || node->type == NBL_NODE_BLOCK) {
        nbl_list_release(&node->nodes, (NblListReleaser)nbl_node_release);
    }
    if (node->type == NBL_NODE_ARRAY) {
        nbl_list_release(&node->array, (NblListReleaser)nbl_node_release);
    }
    if (node->type == NBL_NODE_OBJECT) {
        nbl_list_release(&node->keys, (NblListReleaser)nbl_node_release);
        nbl_list_release(&node->values, (NblListReleaser)nbl_node_release);
    }
    if (node->type == NBL_NODE_VARIABLE) {
        free(node->variable);
    }
    if (node->type > NBL_NODE_BLOCKS_BEGIN && node->type < NBL_NODE_BLOCKS_END) {
        nbl_node_release(node->condition);
        nbl_node_release(node->thenBlock);
        if (node->elseBlock) nbl_node_release(node->elseBlock);
    }
    if (node->type > NBL_NODE_UNARY_BEGIN && node->type < NBL_NODE_UNARY_END) {
        nbl_node_release(node->unary);
    }
    if (node->type > NBL_NODE_OPERATOR_BEGIN && node->type < NBL_NODE_OPERATOR_END) {
        nbl_node_release(node->lhs);
        nbl_node_release(node->rhs);
    }
    free(node);
}

#define nbl_parser_current() (&parser->tokens[parser->position])
#define nbl_parser_next(pos) (&parser->tokens[parser->position + 1 + pos])

NblNode *nbl_parser(NblSource *source, NblToken *tokens, int32_t tokensSize) {
    NblParser parser = {.source = source, .tokens = tokens, .tokensSize = tokensSize, .position = 0};
    if (!setjmp(parser.exception)) {
        return nbl_parser_program(&parser);
    }
    return NULL;
}

void nbl_parser_eat(NblParser *parser, NblTokenType type) {
    if (nbl_parser_current()->type == type) {
        parser->position++;
    } else {
        nbl_source_print_error(parser->source, nbl_parser_current()->offset, "Unexpected token: %s needed %s",
                               nbl_token_type_to_string(nbl_parser_current()->type), nbl_token_type_to_string(type));
        longjmp(parser->exception, 1);
    }
}

NblValueType nbl_parser_type(NblParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_ANY) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_ANY);
        return NBL_VALUE_ANY;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_NULL) {
        nbl_parser_eat(parser, NBL_TOKEN_NULL);
        return NBL_VALUE_NULL;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_BOOL) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_BOOL);
        return NBL_VALUE_BOOL;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_INT) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_INT);
        return NBL_VALUE_INT;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_FLOAT) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_FLOAT);
        return NBL_VALUE_FLOAT;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_STRING) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_STRING);
        return NBL_VALUE_STRING;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_ARRAY) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_ARRAY);
        return NBL_VALUE_ARRAY;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TYPE_OBJECT) {
        nbl_parser_eat(parser, NBL_TOKEN_TYPE_OBJECT);
        return NBL_VALUE_OBJECT;
    }
    nbl_source_print_error(parser->source, nbl_parser_current()->offset, "Expected type token");
    longjmp(parser->exception, 1);
}

NblNode *nbl_parser_program(NblParser *parser) {
    NblNode *node = nbl_node_new_multiple(NBL_NODE_NODES, parser->source, nbl_parser_current()->offset);
    while (nbl_parser_current()->type != NBL_TOKEN_EOF) {
        NblNode *childNode = nbl_parser_statement(parser);
        if (childNode) nbl_list_add(&node->nodes, childNode);
    }
    return node;
}

NblNode *nbl_parser_block(NblParser *parser) {
    NblNode *node = nbl_node_new_multiple(NBL_NODE_BLOCK, parser->source, nbl_parser_current()->offset);
    if (nbl_parser_current()->type == NBL_TOKEN_LCURLY) {
        nbl_parser_eat(parser, NBL_TOKEN_LCURLY);
        while (nbl_parser_current()->type != NBL_TOKEN_RCURLY) {
            NblNode *childNode = nbl_parser_statement(parser);
            if (childNode) nbl_list_add(&node->nodes, childNode);
        }
        nbl_parser_eat(parser, NBL_TOKEN_RCURLY);
    } else {
        NblNode *childNode = nbl_parser_statement(parser);
        if (childNode) nbl_list_add(&node->nodes, childNode);
    }
    return node;
}

NblNode *nbl_parser_statement(NblParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_SEMICOLON) {
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return NULL;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_IF) {
        NblNode *node = nbl_node_new(NBL_NODE_IF, parser->source, nbl_parser_current()->offset);
        nbl_parser_eat(parser, NBL_TOKEN_IF);
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        node->thenBlock = nbl_parser_block(parser);
        if (nbl_parser_current()->type == NBL_TOKEN_ELSE) {
            nbl_parser_eat(parser, NBL_TOKEN_ELSE);
            node->elseBlock = nbl_parser_block(parser);
        } else {
            node->elseBlock = NULL;
        }
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_LOOP) {
        NblNode *node = nbl_node_new(NBL_NODE_WHILE, parser->source, nbl_parser_current()->offset);
        node->elseBlock = NULL;
        nbl_parser_eat(parser, NBL_TOKEN_LOOP);
        node->condition = nbl_node_new_value(parser->source, nbl_parser_current()->offset,
                                             (NblValue){.type = NBL_VALUE_BOOL, .integer = 1});
        node->thenBlock = nbl_parser_block(parser);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_WHILE) {
        NblNode *node = nbl_node_new(NBL_NODE_WHILE, parser->source, nbl_parser_current()->offset);
        node->elseBlock = NULL;
        nbl_parser_eat(parser, NBL_TOKEN_WHILE);
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        node->thenBlock = nbl_parser_block(parser);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_DO) {
        NblNode *node = nbl_node_new(NBL_NODE_DOWHILE, parser->source, nbl_parser_current()->offset);
        node->elseBlock = NULL;
        nbl_parser_eat(parser, NBL_TOKEN_DO);
        node->thenBlock = nbl_parser_block(parser);
        nbl_parser_eat(parser, NBL_TOKEN_WHILE);
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        node->condition = nbl_parser_assigns(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_FOR) {
        // TODO

        // nbl_parser_eat(parser, NBL_TOKEN_FOR);
        // nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        // NblNode *declarations;
        // if (current()->type != NBL_TOKEN_SEMICOLON) {
        //     declarations = nbl_parser_declarations(parser);
        // }

        // NblNode *blockNode = nbl_node_new_multiple(NBL_NODE_BLOCK, token);
        // nbl_list_add(blockNode->nodes, declarations);

        // NblNode *node = nbl_node_new(NBL_NODE_FOR, token);
        // nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);
        // if (current()->type != NBL_TOKEN_SEMICOLON) {
        //     node->condition = nbl_parser_tenary(nbl_parser);
        // } else {
        //     node->condition = NULL;
        // }
        // nbl_parser_eat(nbl_parser, NBL_TOKEN_SEMICOLON);

        // if (current()->type != NBL_TOKEN_RPAREN) {
        //     node->incrementBlock = nbl_parser_assigns(nbl_parser);
        // } else {
        //     node->incrementBlock = NULL;
        // }
        // nbl_parser_eat(nbl_parser, NBL_TOKEN_RPAREN);

        // node->thenBlock = nbl_parser_block(nbl_parser);
        // nbl_list_add(blockNode->nodes, node);
        // return blockNode;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_CONTINUE) {
        int32_t offset = nbl_parser_current()->offset;
        nbl_parser_eat(parser, NBL_TOKEN_CONTINUE);
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return nbl_node_new(NBL_NODE_CONTINUE, parser->source, offset);
    }
    if (nbl_parser_current()->type == NBL_TOKEN_BREAK) {
        int32_t offset = nbl_parser_current()->offset;
        nbl_parser_eat(parser, NBL_TOKEN_BREAK);
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return nbl_node_new(NBL_NODE_BREAK, parser->source, offset);
    }
    if (nbl_parser_current()->type == NBL_TOKEN_RETURN) {
        int32_t offset = nbl_parser_current()->offset;
        nbl_parser_eat(parser, NBL_TOKEN_RETURN);
        NblNode *node =
            nbl_node_new_unary(NBL_NODE_RETURN, parser->source, offset,
                               nbl_parser_current()->type != NBL_TOKEN_SEMICOLON
                                   ? nbl_parser_tenary(parser)
                                   : nbl_node_new_value(parser->source, offset, (NblValue){.type = NBL_VALUE_NULL}));
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
        return node;
    }

    NblNode *node = nbl_parser_declarations(parser);
    if (nbl_parser_current()->type == NBL_TOKEN_EOF) {
        nbl_parser_eat(parser, NBL_TOKEN_EOF);
    } else {
        nbl_parser_eat(parser, NBL_TOKEN_SEMICOLON);
    }
    return node;
}

NblNode *nbl_parser_declarations(NblParser *parser) {
    if (nbl_parser_current()->type == NBL_TOKEN_LET || nbl_parser_current()->type == NBL_TOKEN_CONST) {
        NblNode *node = nbl_node_new(NBL_NODE_NODES, parser->source, nbl_parser_current()->offset);

        NblNodeType assignType;
        if (nbl_parser_current()->type == NBL_TOKEN_LET) {
            assignType = NBL_NODE_ASSIGN_LET;
            nbl_parser_eat(parser, NBL_TOKEN_LET);
        }
        if (nbl_parser_current()->type == NBL_TOKEN_CONST) {
            assignType = NBL_NODE_ASSIGN_CONST;
            nbl_parser_eat(parser, NBL_TOKEN_CONST);
        }

        for (;;) {
            NblNode *variable = nbl_node_new(NBL_NODE_VARIABLE, parser->source, nbl_parser_current()->offset);
            variable->variable = strdup(nbl_parser_current()->string);
            nbl_parser_eat(parser, NBL_TOKEN_KEYWORD);

            NblValueType declarationType = NBL_VALUE_ANY;
            if (nbl_parser_current()->type == NBL_TOKEN_COLON) {
                nbl_parser_eat(parser, NBL_TOKEN_COLON);
                declarationType = nbl_parser_type(parser);
            }

            NblNode *childNode;
            if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN) {
                int32_t offset = nbl_parser_current()->offset;
                nbl_parser_eat(parser, NBL_TOKEN_ASSIGN);
                childNode =
                    nbl_node_new_operation(assignType, parser->source, offset, variable, nbl_parser_assign(parser));
            } else {
                childNode = nbl_node_new_operation(assignType, parser->source, nbl_parser_current()->offset, variable,
                                                   nbl_node_new_value(parser->source, nbl_parser_current()->offset,
                                                                      (NblValue){.type = NBL_VALUE_NULL}));
            }
            childNode->declarationType = declarationType;
            nbl_list_add(&node->nodes, childNode);

            if (nbl_parser_current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }

        if (node->nodes.size == 0) {
            nbl_node_release(node);
            return NULL;
        }
        if (node->nodes.size == 1) {
            NblNode *firstNode = nbl_node_ref(nbl_list_get(&node->nodes, 0));
            nbl_node_release(node);
            return firstNode;
        }
        return node;
    }
    return nbl_parser_assigns(parser);
}

NblNode *nbl_parser_assigns(NblParser *parser) {
    NblNode *node = nbl_node_new(NBL_NODE_NODES, parser->source, nbl_parser_current()->offset);
    for (;;) {
        nbl_list_add(&node->nodes, nbl_parser_assign(parser));
        if (nbl_parser_current()->type == NBL_TOKEN_COMMA) {
            nbl_parser_eat(parser, NBL_TOKEN_COMMA);
        } else {
            break;
        }
    }
    if (node->nodes.size == 0) {
        nbl_node_release(node);
        return NULL;
    }
    if (node->nodes.size == 1) {
        NblNode *firstNode = nbl_node_ref(nbl_list_get(&node->nodes, 0));
        nbl_node_release(node);
        return firstNode;
    }
    return node;
}

NblNode *nbl_parser_assign(NblParser *parser) {
    NblNode *lhs = nbl_parser_tenary(parser);
    int32_t offset = nbl_parser_current()->offset;
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN);
        return nbl_node_new_operation(NBL_NODE_ASSIGN, parser->source, offset, lhs, nbl_parser_assign(parser));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_ADD) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_ADD);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_ADD, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_SUB) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_SUB);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_SUB, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_MUL) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_MUL);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_MUL, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_EXP) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_EXP);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_EXP, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_MOD) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_MOD);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_MOD, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_AND) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_AND);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_AND, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_XOR) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_XOR);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_XOR, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_OR) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_OR);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_OR, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_SHL) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_SHL);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_SHL, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ASSIGN_SHR) {
        nbl_parser_eat(parser, NBL_TOKEN_ASSIGN_SHR);
        return nbl_node_new_operation(
            NBL_NODE_ASSIGN, parser->source, offset, lhs,
            nbl_node_new_operation(NBL_NODE_SHR, parser->source, offset, nbl_node_ref(lhs), nbl_parser_assign(parser)));
    }
    return lhs;
}

NblNode *nbl_parser_tenary(NblParser *parser) {
    NblNode *node = nbl_parser_logical(parser);
    if (nbl_parser_current()->type == NBL_TOKEN_QUESTION) {
        NblNode *tenaryNode = nbl_node_new(NBL_NODE_TENARY, parser->source, nbl_parser_current()->offset);
        nbl_parser_eat(parser, NBL_TOKEN_QUESTION);
        tenaryNode->condition = node;
        tenaryNode->thenBlock = nbl_parser_tenary(parser);
        nbl_parser_eat(parser, NBL_TOKEN_COLON);
        tenaryNode->elseBlock = nbl_parser_tenary(parser);
        return tenaryNode;
    }
    return node;
}

NblNode *nbl_parser_logical(NblParser *parser) {
    NblNode *node = nbl_parser_equality(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_AND || nbl_parser_current()->type == NBL_TOKEN_LOGICAL_OR) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_AND) {
            nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_AND);
            node =
                nbl_node_new_operation(NBL_NODE_LOGICAL_AND, parser->source, offset, node, nbl_parser_equality(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_OR) {
            nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_OR);
            node =
                nbl_node_new_operation(NBL_NODE_LOGICAL_OR, parser->source, offset, node, nbl_parser_equality(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_equality(NblParser *parser) {
    NblNode *node = nbl_parser_relational(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_EQ || nbl_parser_current()->type == NBL_TOKEN_NEQ) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_EQ) {
            nbl_parser_eat(parser, NBL_TOKEN_EQ);
            node = nbl_node_new_operation(NBL_NODE_EQ, parser->source, offset, node, nbl_parser_relational(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_NEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_NEQ);
            node = nbl_node_new_operation(NBL_NODE_NEQ, parser->source, offset, node, nbl_parser_relational(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_relational(NblParser *parser) {
    NblNode *node = nbl_parser_bitwise(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_LT || nbl_parser_current()->type == NBL_TOKEN_LTEQ ||
           nbl_parser_current()->type == NBL_TOKEN_GT || nbl_parser_current()->type == NBL_TOKEN_GTEQ) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_LT) {
            nbl_parser_eat(parser, NBL_TOKEN_LT);
            node = nbl_node_new_operation(NBL_NODE_LT, parser->source, offset, node, nbl_parser_bitwise(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_LTEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_LTEQ);
            node = nbl_node_new_operation(NBL_NODE_LTEQ, parser->source, offset, node, nbl_parser_bitwise(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_GT) {
            nbl_parser_eat(parser, NBL_TOKEN_GT);
            node = nbl_node_new_operation(NBL_NODE_GT, parser->source, offset, node, nbl_parser_bitwise(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_GTEQ) {
            nbl_parser_eat(parser, NBL_TOKEN_GTEQ);
            node = nbl_node_new_operation(NBL_NODE_GTEQ, parser->source, offset, node, nbl_parser_bitwise(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_bitwise(NblParser *parser) {
    NblNode *node = nbl_parser_shift(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_AND || nbl_parser_current()->type == NBL_TOKEN_XOR ||
           nbl_parser_current()->type == NBL_TOKEN_OR) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_AND) {
            nbl_parser_eat(parser, NBL_TOKEN_AND);
            node = nbl_node_new_operation(NBL_NODE_AND, parser->source, offset, node, nbl_parser_shift(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_XOR) {
            nbl_parser_eat(parser, NBL_TOKEN_XOR);
            node = nbl_node_new_operation(NBL_NODE_XOR, parser->source, offset, node, nbl_parser_shift(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_OR) {
            nbl_parser_eat(parser, NBL_TOKEN_OR);
            node = nbl_node_new_operation(NBL_NODE_OR, parser->source, offset, node, nbl_parser_shift(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_shift(NblParser *parser) {
    NblNode *node = nbl_parser_add(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_SHL || nbl_parser_current()->type == NBL_TOKEN_SHR) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_SHL) {
            nbl_parser_eat(parser, NBL_TOKEN_SHL);
            node = nbl_node_new_operation(NBL_NODE_SHL, parser->source, offset, node, nbl_parser_add(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_SHR) {
            nbl_parser_eat(parser, NBL_TOKEN_SHR);
            node = nbl_node_new_operation(NBL_NODE_SHR, parser->source, offset, node, nbl_parser_add(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_add(NblParser *parser) {
    NblNode *node = nbl_parser_mul(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_ADD || nbl_parser_current()->type == NBL_TOKEN_SUB) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_ADD) {
            nbl_parser_eat(parser, NBL_TOKEN_ADD);
            node = nbl_node_new_operation(NBL_NODE_ADD, parser->source, offset, node, nbl_parser_mul(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_SUB) {
            nbl_parser_eat(parser, NBL_TOKEN_SUB);
            node = nbl_node_new_operation(NBL_NODE_SUB, parser->source, offset, node, nbl_parser_mul(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_mul(NblParser *parser) {
    NblNode *node = nbl_parser_unary(parser);
    while (nbl_parser_current()->type == NBL_TOKEN_MUL || nbl_parser_current()->type == NBL_TOKEN_EXP ||
           nbl_parser_current()->type == NBL_TOKEN_DIV || nbl_parser_current()->type == NBL_TOKEN_MOD) {
        int32_t offset = nbl_parser_current()->offset;
        if (nbl_parser_current()->type == NBL_TOKEN_MUL) {
            nbl_parser_eat(parser, NBL_TOKEN_MUL);
            node = nbl_node_new_operation(NBL_NODE_MUL, parser->source, offset, node, nbl_parser_unary(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_EXP) {
            nbl_parser_eat(parser, NBL_TOKEN_EXP);
            node = nbl_node_new_operation(NBL_NODE_EXP, parser->source, offset, node, nbl_parser_unary(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_DIV) {
            nbl_parser_eat(parser, NBL_TOKEN_DIV);
            node = nbl_node_new_operation(NBL_NODE_DIV, parser->source, offset, node, nbl_parser_unary(parser));
        }
        if (nbl_parser_current()->type == NBL_TOKEN_MOD) {
            nbl_parser_eat(parser, NBL_TOKEN_MOD);
            node = nbl_node_new_operation(NBL_NODE_MOD, parser->source, offset, node, nbl_parser_unary(parser));
        }
    }
    return node;
}

NblNode *nbl_parser_unary(NblParser *parser) {
    int32_t offset = nbl_parser_current()->offset;
    if (nbl_parser_current()->type == NBL_TOKEN_LPAREN && nbl_parser_next(0)->type > NBL_TOKEN_TYPE_BEGIN &&
        nbl_parser_next(0)->type < NBL_TOKEN_TYPE_END && nbl_parser_next(1)->type == NBL_TOKEN_RPAREN) {
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        NblValueType castType = nbl_parser_type(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        NblNode *node = nbl_node_new_unary(NBL_NODE_CAST, parser->source, offset, nbl_parser_unary(parser));
        node->castType = castType;
        return node;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_ADD) {
        nbl_parser_eat(parser, NBL_TOKEN_ADD);
        return nbl_parser_unary(parser);
    }
    if (nbl_parser_current()->type == NBL_TOKEN_SUB) {
        nbl_parser_eat(parser, NBL_TOKEN_SUB);
        return nbl_node_new_unary(NBL_NODE_NEG, parser->source, offset, nbl_parser_unary(parser));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_NOT) {
        nbl_parser_eat(parser, NBL_TOKEN_NOT);
        return nbl_node_new_unary(NBL_NODE_NOT, parser->source, offset, nbl_parser_unary(parser));
    }
    if (nbl_parser_current()->type == NBL_TOKEN_LOGICAL_NOT) {
        nbl_parser_eat(parser, NBL_TOKEN_LOGICAL_NOT);
        return nbl_node_new_unary(NBL_NODE_LOGICAL_NOT, parser->source, offset, nbl_parser_unary(parser));
    }
    return nbl_parser_primary(parser);
}

NblNode *nbl_parser_primary(NblParser *parser) {
    int32_t offset = nbl_parser_current()->offset;

    if (nbl_parser_current()->type == NBL_TOKEN_LPAREN) {
        nbl_parser_eat(parser, NBL_TOKEN_LPAREN);
        NblNode *node = nbl_parser_add(parser);
        nbl_parser_eat(parser, NBL_TOKEN_RPAREN);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_NULL) {
        nbl_parser_eat(parser, NBL_TOKEN_NULL);
        return nbl_node_new_value(parser->source, offset, (NblValue){.type = NBL_VALUE_NULL});
    }
    if (nbl_parser_current()->type == NBL_TOKEN_TRUE) {
        nbl_parser_eat(parser, NBL_TOKEN_TRUE);
        return nbl_node_new_value(parser->source, offset, (NblValue){.type = NBL_VALUE_BOOL, .integer = 1});
    }
    if (nbl_parser_current()->type == NBL_TOKEN_FALSE) {
        nbl_parser_eat(parser, NBL_TOKEN_FALSE);
        return nbl_node_new_value(parser->source, offset, (NblValue){.type = NBL_VALUE_BOOL, .integer = 0});
    }

    if (nbl_parser_current()->type == NBL_TOKEN_INT) {
        NblNode *node = nbl_node_new_value(parser->source, offset,
                                           (NblValue){.type = NBL_VALUE_INT, .integer = nbl_parser_current()->integer});
        nbl_parser_eat(parser, NBL_TOKEN_INT);
        return node;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_FLOAT) {
        NblNode *node = nbl_node_new_value(
            parser->source, offset, (NblValue){.type = NBL_VALUE_FLOAT, .floating = nbl_parser_current()->floating});
        nbl_parser_eat(parser, NBL_TOKEN_FLOAT);
        return node;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_STRING) {
        NblNode *node = nbl_node_new_value(parser->source, offset,
                                           (NblValue){.type = NBL_VALUE_STRING,
                                                      .string_size = strlen(nbl_parser_current()->string),
                                                      .string = strdup(nbl_parser_current()->string)});
        nbl_parser_eat(parser, NBL_TOKEN_STRING);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_LBRACKET) {
        NblNode *node = nbl_node_new(NBL_NODE_ARRAY, parser->source, nbl_parser_current()->offset);
        nbl_list_init(&node->array);
        nbl_parser_eat(parser, NBL_TOKEN_LBRACKET);
        while (nbl_parser_current()->type != NBL_TOKEN_RBRACKET) {
            nbl_list_add(&node->array, nbl_parser_tenary(parser));
            if (nbl_parser_current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }
        nbl_parser_eat(parser, NBL_TOKEN_RBRACKET);
        return node;
    }
    if (nbl_parser_current()->type == NBL_TOKEN_LCURLY) {
        NblNode *node = nbl_node_new(NBL_NODE_OBJECT, parser->source, nbl_parser_current()->offset);
        nbl_list_init(&node->keys);
        nbl_list_init(&node->values);
        nbl_parser_eat(parser, NBL_TOKEN_LCURLY);
        while (nbl_parser_current()->type != NBL_TOKEN_RCURLY) {
            if (nbl_parser_current()->type == NBL_TOKEN_KEYWORD && nbl_parser_next(0)->type == NBL_TOKEN_COLON) {
                nbl_list_add(&node->keys,
                             nbl_node_new_value(parser->source, offset,
                                                (NblValue){.type = NBL_VALUE_STRING,
                                                           .string_size = strlen(nbl_parser_current()->string),
                                                           .string = strdup(nbl_parser_current()->string)}));
                nbl_parser_eat(parser, NBL_TOKEN_KEYWORD);
            } else {
                nbl_list_add(&node->keys, nbl_parser_tenary(parser));
            }
            nbl_parser_eat(parser, NBL_TOKEN_COLON);
            nbl_list_add(&node->values, nbl_parser_tenary(parser));
            if (nbl_parser_current()->type == NBL_TOKEN_COMMA) {
                nbl_parser_eat(parser, NBL_TOKEN_COMMA);
            } else {
                break;
            }
        }
        nbl_parser_eat(parser, NBL_TOKEN_RCURLY);
        return node;
    }

    if (nbl_parser_current()->type == NBL_TOKEN_KEYWORD) {
        NblNode *node = nbl_node_new(NBL_NODE_VARIABLE, parser->source, offset);
        node->variable = strdup(nbl_parser_current()->string);
        nbl_parser_eat(parser, NBL_TOKEN_KEYWORD);
        return node;
    }

    nbl_source_print_error(parser->source, offset, "Unexpected token");
    longjmp(parser->exception, 1);
}
