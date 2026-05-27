#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_BASE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_BASE_H

#include <ember/core/models/source_span.h>

/**
 * Kinds of assembler IR nodes.
 */
enum EmberIrNodeKind {
    IR_NODE_PROGRAM,
    IR_NODE_FUNCTION,
    IR_NODE_CODE_ITEM,
    IR_NODE_VALUE,
    IR_NODE_OPERAND,
};

/**
 * Base type for all assembler IR nodes.
 *
 * Every IR node stores its runtime node kind together with the source span
 * associated with the originating AST construct, when available.
 */
struct EmberIrNode {
    enum EmberIrNodeKind kind;
    struct EmberSourceSpan source_span;
};

/**
 * Initializes an IR node base.
 *
 * @param self Pointer to the node base to initialize.
 * @param kind Kind of the IR node.
 */
void ember_ir_node_init(
    struct EmberIrNode *self,
    enum EmberIrNodeKind kind,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an IR node using node-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes destroy logic
 * only for node types that are heap allocated. If it is called on a node type
 * that is not heap allocated, no action is taken.
 *
 * The intended use is to call this function on a generic root IR node to
 * clean up the entire structure. When the concrete node type is already known,
 * prefer calling the corresponding node-specific destroy or deinit function
 * directly.
 *
 * @param self Pointer to the IR node to destroy. If NULL, no action is taken.
 */
void ember_ir_node_destroy(struct EmberIrNode *self);

#endif
