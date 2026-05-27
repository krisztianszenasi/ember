#include "assembler/internal/models/ir/nodes/base.h"

#include "assembler/internal/models/ir/nodes/program.h"
#include "assembler/internal/models/ir/nodes/code_item.h"
#include "assembler/internal/models/ir/nodes/value.h"
#include "assembler/internal/models/ir/nodes/operand.h"

void ember_ir_node_init(
    struct EmberIrNode *self,
    enum EmberIrNodeKind kind,
    struct EmberSourceSpan source_span
) {
    self->kind = kind;
    self->source_span = source_span;
}

void ember_ir_node_destroy(struct EmberIrNode *self) {
    if (self == NULL) {
        return;
    }

    switch (self->kind) {
        case IR_NODE_PROGRAM:
            ember_ir_program_destroy((struct EmberIrProgram *)self);
            break;
        case IR_NODE_FUNCTION:
            /* not heap allocated */
            break;
        case IR_NODE_CODE_ITEM:
            ember_ir_code_item_destroy((struct EmberIrCodeItem *)self);
            break;
        case IR_NODE_VALUE:
            /* not heap allocated */
            break;
        case IR_NODE_OPERAND:
            ember_ir_operand_destroy((struct EmberIrOperand *)self);
            break;
    }
}
