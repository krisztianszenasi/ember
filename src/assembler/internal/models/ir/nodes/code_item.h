#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_CODE_ITEM_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_CODE_ITEM_H

#include "assembler/internal/models/ir/nodes/base.h"

/**
 * Kinds of function body code item IR nodes.
 */
enum EmberIrCodeItemKind {
    IR_CODE_ITEM_INSTRUCTION,
    IR_CODE_ITEM_LABEL_DEF,
};

/**
 * Base type for function body code item IR nodes.
 *
 * Code items are polymorphic nodes stored through base pointers inside
 * functions. The concrete code item kind is stored in `kind`.
 */
struct EmberIrCodeItem {
    struct EmberIrNode base;
    enum EmberIrCodeItemKind kind;
};

/**
 * Initializes a code item base.
 *
 * @param self Pointer to the code item base to initialize.
 * @param kind Concrete code item kind.
 */
void ember_ir_code_item_init(
    struct EmberIrCodeItem *self,
    enum EmberIrCodeItemKind kind,
    struct EmberSourceSpan source_span
);

/**
 * Destroys a code item node using code item-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes the appropriate
 * destroy function for the concrete heap-allocated code item node.
 *
 * @param self Pointer to the code item node to destroy. If NULL, no action is
 *             taken.
 */
void ember_ir_code_item_destroy(struct EmberIrCodeItem *self);

#endif
