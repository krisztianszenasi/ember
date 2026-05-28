#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_LITERAL_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_LITERAL_H

#include <stdint.h>
#include <stdbool.h>
#include <ember/core/memory.h>

#include "assembler/internal/models/ast/nodes/base.h"

/**
 * Kinds of literal AST nodes.
 */
enum EmberAstLiteralKind {
    AST_LITERAL_BOOLEAN,
    AST_LITERAL_FLOAT,
    AST_LITERAL_INT,
    AST_LITERAL_NONE,
    AST_LITERAL_STR,
};

/**
 * AST node representing a literal value.
 *
 * Literals are small value-like nodes that are typically embedded inside other
 * AST nodes rather than allocated separately on the heap.
 */
struct EmberAstLiteral {
    struct EmberAstNode base;
    enum EmberAstLiteralKind kind;
    union {
        bool boolean_val;
        double float_val;
        int64_t int_val;
        char *str_val;
    } as;
};

static inline void ember_ast_literal_deinit(struct EmberAstLiteral *self) {
    if (self->kind == AST_LITERAL_STR) {
        ember_free(self->as.str_val);
    }
}

/**
 * Creates and initializes a boolean literal node.
 *
 * This node type is intended to be embedded in other AST nodes and is usually
 * stored by value rather than heap allocated on its own.
 *
 * @param value Boolean literal value.
 * @param source_span Source span associated with the literal.
 * @return An initialized boolean literal node.
 */
static inline struct EmberAstLiteral ember_ast_boolean_literal(
    bool value,
    struct EmberSourceSpan source_span
) {
    return (struct EmberAstLiteral){
        .base = {
            .kind = AST_NODE_LITERAL,
            .source_span = source_span
        },
        .kind = AST_LITERAL_BOOLEAN,
        .as = {
            .boolean_val = value
        }
    };
}

/**
 * Creates and initializes a floating-point literal node.
 *
 * This node type is intended to be embedded in other AST nodes and is usually
 * stored by value rather than heap allocated on its own.
 *
 * @param value Floating-point literal value.
 * @param source_span Source span associated with the literal.
 * @return An initialized floating-point literal node.
 */
static inline struct EmberAstLiteral ember_ast_float_literal(
    double value,
    struct EmberSourceSpan source_span
) {
    return (struct EmberAstLiteral){
        .base = {
            .kind = AST_NODE_LITERAL,
            .source_span = source_span
        },
        .kind = AST_LITERAL_FLOAT,
        .as = {
            .float_val = value
        }
    };
}

/**
 * Creates and initializes an integer literal node.
 *
 * This node type is intended to be embedded in other AST nodes and is usually
 * stored by value rather than heap allocated on its own.
 *
 * @param value Integer literal value.
 * @param source_span Source span associated with the literal.
 * @return An initialized integer literal node.
 */
static inline struct EmberAstLiteral ember_ast_int_literal(
    int value,
    struct EmberSourceSpan source_span
) {
    return (struct EmberAstLiteral){
        .base = {
            .kind = AST_NODE_LITERAL,
            .source_span = source_span
        },
        .kind = AST_LITERAL_INT,
        .as = {
            .int_val = value
        }
    };
}

static inline struct EmberAstLiteral *ember_ast_int_literal_new(
    int value,
    struct EmberSourceSpan source_span
) {
    struct EmberAstLiteral *self = ember_alloc(sizeof(struct EmberAstLiteral));
    self->base.kind = AST_NODE_LITERAL;
    self->base.source_span = source_span;
    self->kind = AST_LITERAL_INT;
    self->as.int_val = value;
    return self;
}

/**
 * Creates and initializes a none literal node.
 *
 * @param source_span Source span associated with the literal.
 * @return An initialized none literal node.
 */
static inline struct EmberAstLiteral ember_ast_none_literal(
    struct EmberSourceSpan source_span
) {
    return (struct EmberAstLiteral){
        .base = {
            .kind = AST_NODE_LITERAL,
            .source_span = source_span
        },
        .kind = AST_LITERAL_NONE,
    };
}

/**
 * Creates and initializes a string literal node.
 *
 * This node type is intended to be embedded in other AST nodes and is usually
 * stored by value rather than heap allocated on its own.
 *
 * The string pointer is stored as provided.
 *
 * @param value String literal value.
 * @param source_span Source span associated with the literal.
 * @return An initialized string literal node.
 */
static inline struct EmberAstLiteral ember_ast_str_literal(
    char *value,
    struct EmberSourceSpan source_span
) {
    return (struct EmberAstLiteral){
        .base = {
            .kind = AST_NODE_LITERAL,
            .source_span = source_span
        },
        .kind = AST_LITERAL_STR,
        .as = {
            .str_val = value
        }
    };
}

#endif
