#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/instruction.h"

#include <ember/core/memory.h>

struct EmberAstInstruction *ember_ast_instruction_new(enum EmberAssemblyOpcode opcode) {
    struct EmberAstInstruction *self = ember_alloc(sizeof(struct EmberAstInstruction));
    
    ember_ast_code_item_init(&self->base, AST_CODE_ITEM_INSTRUCTION, EMBER_SOURCE_SPAN_NONE);
    self->opcode = opcode;
    
    self->operand_capacity = 0;
    self->operand_count = 0;
    self->operands = NULL;
    
    return self;
}

void ember_ast_instruction_destroy(struct EmberAstInstruction *self) {
    for (size_t i = 0; i < self->operand_count; i++) {
        ember_ast_operand_deinit(&self->operands[i]);
    }
    EMBER_FREE_ARRAY(
        struct EmberAstOperand,
        self->operands,
        self->operand_capacity
    );
    ember_free(self);
}

void ember_ast_instruction_add_operand(
    struct EmberAstInstruction *self,
    struct EmberAstOperand operand
) {
    if (self->operand_count + 1 > self->operand_capacity) {
        size_t old_capacity = self->operand_capacity;
        self->operand_capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->operands = EMBER_GROW_ARRAY(
            struct EmberAstOperand,
            self->operands,
            old_capacity,
            self->operand_capacity
        );
    }
    self->operands[self->operand_count++] = operand;
}
