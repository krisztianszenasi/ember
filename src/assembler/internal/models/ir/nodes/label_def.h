#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_LABEL_DEF_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_LABEL_DEF_H

#include "assembler/internal/models/ir/nodes/code_item.h"

/**
 * IR node representing a label definition code item.
 *
 * A label definition introduces a function-local label identified by name.
 * This node is heap allocated because code items are polymorphic and are
 * stored through base pointers inside functions.
 */
struct EmberIrLabelDef {
    struct EmberIrCodeItem base;
    char *name;
};

/**
 * Allocates and initializes a label definition node.
 *
 * The given name is copied into the returned node.
 *
 * @param name Name of the label.
 * @return A newly allocated label definition node, or NULL on failure.
 */
struct EmberIrLabelDef *ember_ir_label_def_new(
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Destroys a label definition node and all resources it owns.
 *
 * This releases the copied label name and frees the label definition node
 * itself.
 *
 * @param self Pointer to the label definition node to destroy. If NULL, no
 *             action is taken.
 */
void ember_ir_label_def_destroy(struct EmberIrLabelDef *self);

#endif
