#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_BASE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_BASE_H

#include <ember/core/models/source_span.h>

/**
 * Kinds of assembler AST nodes.
 */
enum EmberAstNodeKind {
    AST_NODE_SECTION_ITEM,
    AST_NODE_CODE_ITEM,
    AST_NODE_IDENTIFIER,
    AST_NODE_PROGRAM,
    AST_NODE_SECTION,
    AST_NODE_LITERAL,
};

/**
 * Base type for all assembler AST nodes.
 *
 * Every AST node stores its runtime node kind and the source span describing
 * where the node originated in the input source.
 */
struct EmberAstNode {
    enum EmberAstNodeKind kind;
    struct EmberSourceSpan source_span;
};

/**
 * Initializes an AST node base.
 *
 * @param self Pointer to the node base to initialize.
 * @param kind Kind of the AST node.
 * @param source_span Source span associated with the node.
 */
void ember_ast_node_init(
    struct EmberAstNode *self,
    enum EmberAstNodeKind kind,
    struct EmberSourceSpan source_span
);

/**
 * Updates the source span of an AST node.
 *
 * @param self Pointer to the node whose source span should be updated.
 * @param source_span New source span to assign to the node.
 */
static inline void ember_ast_node_set_source_span(
    struct EmberAstNode *self,
    struct EmberSourceSpan source_span
) {
    self->source_span = source_span;
}

/**
 * Destroys an AST node using node-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes destroy logic
 * only for node types that are heap allocated. If it is called on a node type
 * that is not heap allocated, no action is taken.
 *
 * The intended use is to call this function on a generic root AST node to
 * clean up the entire tree. When the concrete node type is already known,
 * prefer calling the corresponding node-specific destroy or deinit function
 * directly.
 *
 * @param self Pointer to the AST node to destroy. If NULL, no action is taken.
 */
void ember_ast_node_destroy(struct EmberAstNode *self);

#endif