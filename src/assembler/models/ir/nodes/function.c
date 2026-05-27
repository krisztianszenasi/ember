#include "assembler/internal/models/ir/nodes/function.h"

#include <ember/core/memory.h>

void ember_ir_function_init(
    struct EmberIrFunction *self,
    struct EmberSourceSpan source_span
) {
    ember_ir_node_init(&self->base, IR_NODE_FUNCTION, source_span);
    self->code_item_capacity = 0;
    self->code_item_count = 0;
    self->code_items = NULL;
    self->local_count = 0;
    self->name = NULL;
}

void ember_ir_function_deinit(struct EmberIrFunction *self) {
    for (size_t i = 0; i < self->code_item_count; i++) {
        ember_ir_code_item_destroy(self->code_items[i]);
    }
    EMBER_FREE_ARRAY(struct EmberIrCodeItem *, self->code_items, self->code_item_capacity);
    ember_ir_function_init(self, EMBER_SOURCE_SPAN_NONE);
}

void ember_ir_function_add_code_item(
    struct EmberIrFunction *self,
    struct EmberIrCodeItem *code_item
) {
    if (self->code_item_count + 1 > self->code_item_capacity) {
        size_t old_capacity = self->code_item_capacity;
        self->code_item_capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->code_items = EMBER_GROW_ARRAY(
            struct EmberIrCodeItem *,
            self->code_items,
            old_capacity,
            self->code_item_capacity
        );
    }
    self->code_items[self->code_item_count++] = code_item;
}

struct EmberIrLabelDef *ember_ir_function_add_label(
    struct EmberIrFunction *self,
    const char *name,
    struct EmberSourceSpan source_span
) {
    struct EmberIrLabelDef *label = ember_ir_label_def_new(name, source_span);
    ember_ir_function_add_code_item(self, &label->base);
    return label;
}

struct EmberIrInstruction *ember_ir_function_add_instruction(
    struct EmberIrFunction *self,
    enum EmberAssemblyOpcode opcode,
    struct EmberSourceSpan source_span
) {
    struct EmberIrInstruction *instruction = ember_ir_instruction_new(opcode, source_span);
    ember_ir_function_add_code_item(self, &instruction->base);
    return instruction;
}
