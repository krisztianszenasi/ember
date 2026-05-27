#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_INSTRUCTION_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_INSTRUCTION_H

#include <stddef.h>
#include "assembler/internal/models/assembly/opcode.h"
#include "assembler/internal/models/ir/nodes/operand.h"
#include "assembler/internal/models/ir/nodes/code_item.h"

/**
 * IR node representing an instruction code item.
 *
 * An instruction stores its opcode together with a dynamically managed array
 * of operand pointers.
 */
struct EmberIrInstruction {
    struct EmberIrCodeItem base;
    enum EmberAssemblyOpcode opcode;
    size_t operand_count;
    size_t operand_capacity;
    struct EmberIrOperand **operands;
};

/**
 * Allocates and initializes an instruction node.
 *
 * @param opcode Opcode of the instruction.
 * @return A newly allocated instruction node, or NULL on failure.
 */
struct EmberIrInstruction *ember_ir_instruction_new(
    enum EmberAssemblyOpcode opcode,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an instruction node and all resources it owns.
 *
 * This destroys all owned operands, releases the dynamic operand array, and
 * frees the instruction node itself.
 *
 * @param self Pointer to the instruction node to destroy. If NULL, no action
 *             is taken.
 */
void ember_ir_instruction_destroy(struct EmberIrInstruction *self);

/**
 * Adds an operand to an instruction.
 *
 * The given operand pointer is appended to the instruction's internal operand
 * array.
 *
 * @param self Pointer to the instruction node.
 * @param operand Pointer to the operand to add.
 */
void ember_ir_instruction_add_operand(
    struct EmberIrInstruction *self,
    struct EmberIrOperand *operand
);

#endif
