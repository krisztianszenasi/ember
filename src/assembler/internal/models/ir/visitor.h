#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_VISITOR_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_VISITOR_H

#include "assembler/internal/models/ir/nodes/all.h"

/**
 * Opaque visitor context payload.
 *
 * This wrapper allows callers to associate arbitrary visitor-specific state
 * with a visitor instance.
 */
struct EmberIrVisitorCtx {
    void *data;
};

struct EmberIrVisitor;

/**
 * Visitor callback table for IR traversal.
 *
 * Any callback may be set to NULL when constructing a visitor. In that case,
 * the visitor initialization logic is expected to substitute the default
 * implementation for that callback.
 *
 * The callbacks are ordered by IR structure rather than alphabetically:
 * generic node first, then root/container nodes, then code items, operand
 * variants, and value nodes.
 */
struct EmberIrVisitorVTable {
    void (*visit_ir_node)                   (struct EmberIrVisitor *self, struct EmberIrNode *node);
    void (*visit_ir_program)                (struct EmberIrVisitor *self, struct EmberIrProgram *node);
    void (*visit_ir_function)               (struct EmberIrVisitor *self, struct EmberIrFunction *node);
    void (*visit_ir_code_item)              (struct EmberIrVisitor *self, struct EmberIrCodeItem *node);
    void (*visit_ir_label_def)              (struct EmberIrVisitor *self, struct EmberIrLabelDef *node);
    void (*visit_ir_instruction)            (struct EmberIrVisitor *self, struct EmberIrInstruction *node);

    void (*visit_ir_operand)                (struct EmberIrVisitor *self, struct EmberIrOperand *node);
    void (*visit_ir_immediate_operand)      (struct EmberIrVisitor *self, struct EmberIrImmediateOperand *node);
    void (*visit_ir_constant_operand)       (struct EmberIrVisitor *self, struct EmberIrConstantOperand *node);
    void (*visit_ir_arg_index_operand)      (struct EmberIrVisitor *self, struct EmberIrIndexOperand *node);
    void (*visit_ir_local_index_operand)    (struct EmberIrVisitor *self, struct EmberIrIndexOperand *node);
    void (*visit_ir_func_ref_operand)       (struct EmberIrVisitor *self, struct EmberIrRefOperand *node);
    void (*visit_ir_global_ref_operand)     (struct EmberIrVisitor *self, struct EmberIrRefOperand *node);
    void (*visit_ir_label_ref_operand)      (struct EmberIrVisitor *self, struct EmberIrRefOperand *node);

    void (*visit_ir_value)(struct EmberIrVisitor *self, struct EmberIrValue *value);
};

/**
 * IR visitor object.
 *
 * A visitor stores caller-provided context together with the effective
 * visitor callback table used during traversal.
 */
struct EmberIrVisitor {
    struct EmberIrVisitorCtx context;
    struct EmberIrVisitorVTable vtable;
};

/**
 * Initializes an IR visitor.
 *
 * The visitor stores the given context and initializes its effective vtable
 * from the provided callback table. Any NULL callback entries in `vtable`
 * may be replaced by default visitor implementations.
 *
 * @param self Pointer to the visitor to initialize.
 * @param context Caller-provided visitor context.
 * @param vtable Visitor callback table to use as overrides, or NULL to use
 *               only default visitor behavior.
 */
void ember_ir_visitor_init(
    struct EmberIrVisitor *self,
    struct EmberIrVisitorCtx context,
    const struct EmberIrVisitorVTable *vtable
);

/**
 * Updates the visitor's callback table.
 *
 * The given vtable is used to override the visitor's current callbacks. Any
 * NULL callback entries may be replaced with the corresponding default
 * visitor implementations.
 *
 * @param self Pointer to the visitor to update.
 * @param vtable Visitor callback table containing the new overrides.
 */
void ember_ir_visitor_set_vtable(
    struct EmberIrVisitor *self,
    const struct EmberIrVisitorVTable *vtable
);

#endif
