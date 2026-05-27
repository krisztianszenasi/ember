#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_IDENTIFIER_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_IDENTIFIER_H

#include <string.h>
#include <ember/core/memory.h>

#include "assembler/internal/models/ast/nodes/base.h"

/**
 * AST node representing an identifier.
 *
 * Identifiers are small value-like nodes that are typically embedded inside
 * other AST nodes rather than allocated separately on the heap.
 */
struct EmberAstIdentifier {
    struct EmberAstNode base;
    char *name;
};

/**
 * Creates and initializes an identifier node.
 *
 * The identifier string is duplicated and owned by the returned node.
 * Because this node type is intended to be embedded in other AST nodes,
 * the returned value should typically be stored by value rather than
 * heap allocated on its own.
 *
 * @param name Identifier text to copy.
 * @param source_span Source span associated with the identifier.
 * @return An initialized identifier node.
 */
static inline struct EmberAstIdentifier ember_ast_identifier(
    const char *name,
    struct EmberSourceSpan source_span
) {
    struct EmberAstIdentifier self = {0};
    ember_ast_node_init(&self.base, AST_NODE_IDENTIFIER, source_span);
    self.name = ember_strdup(name);
    return self;
}

/**
 * Deinitializes an identifier node.
 *
 * This releases resources owned by the identifier, including its copied name.
 * Since identifier nodes are intended to be embedded in other AST nodes, this
 * function deinitializes the node in place and does not free the node object
 * itself.
 *
 * @param self Pointer to the identifier node to deinitialize. If NULL, no
 *             action is taken.
 */
static inline void ember_ast_identifier_deinit(
    struct EmberAstIdentifier *self
) {
    if (self == NULL) {
        return;
    }

    ember_free(self->name);
    *self = (struct EmberAstIdentifier){0};
}

#endif