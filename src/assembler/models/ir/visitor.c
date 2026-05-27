#include "assembler/internal/models/ir/visitor.h"

static void default_visit_ir_node(
    struct EmberIrVisitor *self,
    struct EmberIrNode *node
) {
    switch (node->kind) {
        case IR_NODE_PROGRAM:
            self->vtable.visit_ir_program(
                self,
                (struct EmberIrProgram *)node
            );
            break;

        case IR_NODE_FUNCTION:
            self->vtable.visit_ir_function(
                self,
                (struct EmberIrFunction *)node
            );
            break;

        case IR_NODE_CODE_ITEM:
            self->vtable.visit_ir_code_item(
                self,
                (struct EmberIrCodeItem *)node
            );
            break;

        case IR_NODE_VALUE:
            self->vtable.visit_ir_value(
                self,
                (struct EmberIrValue *)node
            );
            break;

        case IR_NODE_OPERAND:
            self->vtable.visit_ir_operand(
                self,
                (struct EmberIrOperand *)node
            );
            break;
    }
}

static void default_visit_ir_program(
    struct EmberIrVisitor *self,
    struct EmberIrProgram *node
) {
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ir_function(self, &node->functions[i]);
    }
}

static void default_visit_ir_function(
    struct EmberIrVisitor *self,
    struct EmberIrFunction *node
) {
    for (size_t i = 0; i < node->code_item_count; i++) {
        self->vtable.visit_ir_code_item(self, node->code_items[i]);
    }
}

static void default_visit_ir_code_item(
    struct EmberIrVisitor *self,
    struct EmberIrCodeItem *node
) {
    switch (node->kind) {
        case IR_CODE_ITEM_LABEL_DEF:
            self->vtable.visit_ir_label_def(
                self,
                (struct EmberIrLabelDef *)node
            );
            break;

        case IR_CODE_ITEM_INSTRUCTION:
            self->vtable.visit_ir_instruction(
                self,
                (struct EmberIrInstruction *)node
            );
            break;
    }
}

static void default_visit_ir_label_def(
    struct EmberIrVisitor *self,
    struct EmberIrLabelDef *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_instruction(
    struct EmberIrVisitor *self,
    struct EmberIrInstruction *node
) {
    for (size_t i = 0; i < node->operand_count; i++) {
        self->vtable.visit_ir_operand(self, node->operands[i]);
    }
}

static void default_visit_ir_operand(
    struct EmberIrVisitor *self,
    struct EmberIrOperand *node
) {
    switch (node->kind) {
        case IR_OPERAND_CONSTANT:
            self->vtable.visit_ir_constant_operand(
                self,
                (struct EmberIrConstantOperand *)node
            );
            break;

        case IR_OPERAND_IMMEDIATE:
            self->vtable.visit_ir_immediate_operand(
                self,
                (struct EmberIrImmediateOperand *)node
            );
            break;

        case IR_OPERAND_ARG_INDEX:
            self->vtable.visit_ir_arg_index_operand(
                self,
                (struct EmberIrIndexOperand *)node
            );
            break;

        case IR_OPERAND_LOCAL_INDEX:
            self->vtable.visit_ir_local_index_operand(
                self,
                (struct EmberIrIndexOperand *)node
            );
            break;

        case IR_OPERAND_FUNC_REF:
            self->vtable.visit_ir_func_ref_operand(
                self,
                (struct EmberIrRefOperand *)node
            );
            break;

        case IR_OPERAND_GLOBAL_REF:
            self->vtable.visit_ir_global_ref_operand(
                self,
                (struct EmberIrRefOperand *)node
            );
            break;

        case IR_OPERAND_LABEL_REF:
            self->vtable.visit_ir_label_ref_operand(
                self,
                (struct EmberIrRefOperand *)node
            );
            break;
    }
}

static void default_visit_ir_immediate_operand(
    struct EmberIrVisitor *self,
    struct EmberIrImmediateOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_constant_operand(
    struct EmberIrVisitor *self,
    struct EmberIrConstantOperand *node
) {
    self->vtable.visit_ir_value(self, &node->value);
}

static void default_visit_ir_arg_index_operand(
    struct EmberIrVisitor *self,
    struct EmberIrIndexOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_local_index_operand(
    struct EmberIrVisitor *self,
    struct EmberIrIndexOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_func_ref_operand(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_global_ref_operand(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_label_ref_operand(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    (void)self;
    (void)node;
}

static void default_visit_ir_value(
    struct EmberIrVisitor *self,
    struct EmberIrValue *value
) {
    (void)self;
    (void)value;
}

static const struct EmberIrVisitorVTable DEFAULT_VTABLE = {
    .visit_ir_node                = default_visit_ir_node,
    .visit_ir_program             = default_visit_ir_program,
    .visit_ir_function            = default_visit_ir_function,
    .visit_ir_code_item           = default_visit_ir_code_item,
    .visit_ir_label_def           = default_visit_ir_label_def,
    .visit_ir_instruction         = default_visit_ir_instruction,

    .visit_ir_operand             = default_visit_ir_operand,
    .visit_ir_immediate_operand   = default_visit_ir_immediate_operand,
    .visit_ir_constant_operand    = default_visit_ir_constant_operand,
    .visit_ir_arg_index_operand   = default_visit_ir_arg_index_operand,
    .visit_ir_local_index_operand = default_visit_ir_local_index_operand,
    .visit_ir_func_ref_operand    = default_visit_ir_func_ref_operand,
    .visit_ir_global_ref_operand  = default_visit_ir_global_ref_operand,
    .visit_ir_label_ref_operand   = default_visit_ir_label_ref_operand,

    .visit_ir_value               = default_visit_ir_value,
};

void ember_ir_visitor_init(
    struct EmberIrVisitor *self,
    struct EmberIrVisitorCtx context,
    const struct EmberIrVisitorVTable *vtable
) {
    self->context = context;
    ember_ir_visitor_set_vtable(self, vtable);
}

void ember_ir_visitor_set_vtable(
    struct EmberIrVisitor *self,
    const struct EmberIrVisitorVTable *vtable
) {
    self->vtable = DEFAULT_VTABLE;

    if (vtable == NULL) {
        return;
    }

    if (vtable->visit_ir_node != NULL) {
        self->vtable.visit_ir_node = vtable->visit_ir_node;
    }
    if (vtable->visit_ir_program != NULL) {
        self->vtable.visit_ir_program = vtable->visit_ir_program;
    }
    if (vtable->visit_ir_function != NULL) {
        self->vtable.visit_ir_function = vtable->visit_ir_function;
    }
    if (vtable->visit_ir_code_item != NULL) {
        self->vtable.visit_ir_code_item = vtable->visit_ir_code_item;
    }
    if (vtable->visit_ir_label_def != NULL) {
        self->vtable.visit_ir_label_def = vtable->visit_ir_label_def;
    }
    if (vtable->visit_ir_instruction != NULL) {
        self->vtable.visit_ir_instruction = vtable->visit_ir_instruction;
    }
    if (vtable->visit_ir_operand != NULL) {
        self->vtable.visit_ir_operand = vtable->visit_ir_operand;
    }
    if (vtable->visit_ir_immediate_operand != NULL) {
        self->vtable.visit_ir_immediate_operand =
            vtable->visit_ir_immediate_operand;
    }
    if (vtable->visit_ir_constant_operand != NULL) {
        self->vtable.visit_ir_constant_operand =
            vtable->visit_ir_constant_operand;
    }
    if (vtable->visit_ir_arg_index_operand != NULL) {
        self->vtable.visit_ir_arg_index_operand =
            vtable->visit_ir_arg_index_operand;
    }
    if (vtable->visit_ir_local_index_operand != NULL) {
        self->vtable.visit_ir_local_index_operand =
            vtable->visit_ir_local_index_operand;
    }
    if (vtable->visit_ir_func_ref_operand != NULL) {
        self->vtable.visit_ir_func_ref_operand =
            vtable->visit_ir_func_ref_operand;
    }
    if (vtable->visit_ir_global_ref_operand != NULL) {
        self->vtable.visit_ir_global_ref_operand =
            vtable->visit_ir_global_ref_operand;
    }
    if (vtable->visit_ir_label_ref_operand != NULL) {
        self->vtable.visit_ir_label_ref_operand =
            vtable->visit_ir_label_ref_operand;
    }
    if (vtable->visit_ir_value != NULL) {
        self->vtable.visit_ir_value = vtable->visit_ir_value;
    }
}