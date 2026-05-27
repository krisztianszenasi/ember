#include "assembler/internal/models/ir/nodes/code_item.h"

#include "assembler/internal/models/ir/nodes/label_def.h"
#include "assembler/internal/models/ir/nodes/instruction.h"

void ember_ir_code_item_init(
    struct EmberIrCodeItem *self,
    enum EmberIrCodeItemKind kind,
    struct EmberSourceSpan source_span
) {
    ember_ir_node_init(&self->base, IR_NODE_CODE_ITEM, source_span);
    self->kind = kind;
}

void ember_ir_code_item_destroy(struct EmberIrCodeItem *self) {
    switch(self->kind) {
        case IR_CODE_ITEM_INSTRUCTION:
            ember_ir_instruction_destroy((struct EmberIrInstruction *)self);
            break;
        case IR_CODE_ITEM_LABEL_DEF:
            ember_ir_label_def_destroy((struct EmberIrLabelDef *)self);
            break;
    }
}
