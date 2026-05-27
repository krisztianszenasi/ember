#include "assembler/internal/models/ir/nodes/instruction.h"

#include <ember/core/memory.h>

struct EmberIrInstruction *ember_ir_instruction_new(
    enum EmberAssemblyOpcode opcode,
    struct EmberSourceSpan source_span
) {
    struct EmberIrInstruction *self = ember_alloc(sizeof(struct EmberIrInstruction));
    ember_ir_code_item_init(&self->base, IR_CODE_ITEM_INSTRUCTION, source_span);
    self->opcode = opcode;
    self->operand_count = 0;
    self->operand_capacity = 0;
    self->operands = NULL;
    return self;
}

void ember_ir_instruction_destroy(struct EmberIrInstruction *self) {
    for (size_t i = 0; i < self->operand_count; i++) {
        ember_ir_operand_destroy(self->operands[i]);
    }
    EMBER_FREE_ARRAY(struct EmberIrOperand *, self->operands, self->operand_capacity);
    ember_free(self);
}

void ember_ir_instruction_add_operand(
    struct EmberIrInstruction *self,
    struct EmberIrOperand *operand
) {
    if (self->operand_count + 1 > self->operand_capacity) {
        size_t old_capcity = self->operand_capacity;
        self->operand_capacity = EMBER_GROW_CAPACITY(old_capcity);
        self->operands = EMBER_GROW_ARRAY(
            struct EmberIrOperand *,
            self->operands,
            old_capcity,
            self->operand_capacity
        );
    }
    self->operands[self->operand_count++] = operand;
}
