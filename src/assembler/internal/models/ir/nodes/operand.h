#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_OPERAND_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_OPERAND_H

#include <stdint.h>

#include "assembler/internal/models/ir/nodes/base.h"
#include "assembler/internal/models/ir/nodes/value.h"

/**
 * @brief Kind of IR operand used by assembler IR instructions.
 */
enum EmberIrOperandKind {
    IR_OPERAND_IMMEDIATE,
    IR_OPERAND_CONSTANT,
    IR_OPERAND_ARG_INDEX,
    IR_OPERAND_LOCAL_INDEX,
    IR_OPERAND_GLOBAL_REF,
    IR_OPERAND_FUNC_REF,
    IR_OPERAND_LABEL_REF,
};

/**
 * @brief Base type for all IR operands.
 */
struct EmberIrOperand {
    struct EmberIrNode base;
    enum EmberIrOperandKind kind;
};

/**
 * Initializes an IR operand base.
 *
 * @param self Pointer to the operand base to initialize.
 * @param kind Concrete operand kind.
 */
void ember_ir_operand_init(
    struct EmberIrOperand *self,
    enum EmberIrOperandKind kind,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an operand using operand-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes the appropriate
 * destroy function for the concrete heap-allocated operand node.
 *
 * @param self Pointer to the operand to destroy. If NULL, no action is taken.
 */
void ember_ir_operand_destroy(struct EmberIrOperand *self);

/**
 * IR operand containing an immediate integer value.
 */
struct EmberIrImmediateOperand {
    struct EmberIrOperand base;
    int64_t value;
};

/**
 * Allocates and initializes an immediate operand.
 *
 * @param value Immediate value stored in the operand.
 * @return A newly allocated immediate operand, or NULL on failure.
 */
struct EmberIrImmediateOperand *ember_ir_immediate_operand_new(
    int64_t value,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an immediate operand.
 *
 * @param self Pointer to the immediate operand to destroy. If NULL, no action
 *             is taken.
 */
void ember_ir_immediate_operand_destroy(struct EmberIrImmediateOperand *self);

/**
 * @brief IR operand containing a value for the constant pool.
 */
struct EmberIrConstantOperand {
    struct EmberIrOperand base;
    struct EmberIrValue value;
};

/**
 * Allocates and initializes a constant operand.
 *
 * The returned operand takes ownership of the given IR value.
 *
 * @param value IR value stored in the operand.
 * @return A newly allocated constant operand, or NULL on failure.
 */
struct EmberIrConstantOperand *ember_ir_constant_operand_new(
    struct EmberIrValue value,
    struct EmberSourceSpan source_span
);

/**
 * Destroys a constant operand and the value it owns.
 *
 * @param self Pointer to the constant operand to destroy. If NULL, no action
 *             is taken.
 */
void ember_ir_constant_operand_destroy(struct EmberIrConstantOperand *self);

/**
 * @brief IR operand containing an argument or local slot index.
 *
 * The concrete meaning is determined by `base.kind`, which must be either
 * `IR_OPERAND_ARG_INDEX` or `IR_OPERAND_LOCAL_INDEX`.
 */
struct EmberIrIndexOperand {
    struct EmberIrOperand base;
    uint32_t index;
};

/**
 * Allocates and initializes an argument index operand.
 *
 * @param index Argument slot index stored in the operand.
 * @return A newly allocated argument index operand, or NULL on failure.
 */
struct EmberIrIndexOperand *ember_ir_arg_index_operand_new(
    uint32_t index,
    struct EmberSourceSpan source_span
);

/**
 * Allocates and initializes a local index operand.
 *
 * @param index Local slot index stored in the operand.
 * @return A newly allocated local index operand, or NULL on failure.
 */
struct EmberIrIndexOperand *ember_ir_local_index_operand_new(
    uint32_t index,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an index operand.
 *
 * @param self Pointer to the index operand to destroy. If NULL, no action is
 *             taken.
 */
void ember_ir_index_operand_destroy(struct EmberIrIndexOperand *self);

/**
 * @brief IR operand referencing a named symbol.
 *
 * The concrete reference type is determined by `base.kind`, which must be one
 * of `IR_OPERAND_GLOBAL_REF`, `IR_OPERAND_FUNC_REF`, or `IR_OPERAND_LABEL_REF`.
 */
struct EmberIrRefOperand {
    struct EmberIrOperand base;
    char *name;
};

/**
 * Allocates and initializes a named reference operand.
 *
 * The given `kind` must be one of the reference operand kinds. The name is
 * copied into the returned operand.
 *
 * @param kind Concrete reference operand kind.
 * @param name Referenced symbol name.
 * @return A newly allocated reference operand, or NULL on failure or if
 *         `kind` is invalid.
 */
struct EmberIrRefOperand *ember_ir_ref_operand_new(
    enum EmberIrOperandKind kind,
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Allocates and initializes a global reference operand.
 *
 * @param name Referenced global symbol name.
 * @return A newly allocated global reference operand, or NULL on failure.
 */
struct EmberIrRefOperand *ember_ir_global_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Allocates and initializes a function reference operand.
 *
 * @param name Referenced function name.
 * @return A newly allocated function reference operand, or NULL on failure.
 */
struct EmberIrRefOperand *ember_ir_func_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Allocates and initializes a label reference operand.
 *
 * @param name Referenced label name.
 * @return A newly allocated label reference operand, or NULL on failure.
 */
struct EmberIrRefOperand *ember_ir_label_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Destroys a reference operand and the copied name it owns.
 *
 * @param self Pointer to the reference operand to destroy. If NULL, no action
 *             is taken.
 */
void ember_ir_ref_operand_destroy(struct EmberIrRefOperand *self);

#endif
