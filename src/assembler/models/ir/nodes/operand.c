#include "assembler/internal/models/ir/nodes/operand.h"

#include <ember/core/memory.h>

void ember_ir_operand_init(
    struct EmberIrOperand *self,
    enum EmberIrOperandKind kind,
    struct EmberSourceSpan source_span
) {
    ember_ir_node_init(&self->base, IR_NODE_OPERAND, source_span);
    self->kind = kind;
}

void ember_ir_operand_destroy(struct EmberIrOperand *self) {
    if (self == NULL) {
        return;
    }

    switch (self->kind) {
        case IR_OPERAND_CONSTANT:
            ember_ir_constant_operand_destroy(
                (struct EmberIrConstantOperand *)self
            );
            break;

        case IR_OPERAND_ARG_INDEX:
        case IR_OPERAND_LOCAL_INDEX:
            ember_ir_index_operand_destroy(
                (struct EmberIrIndexOperand *)self
            );
            break;

        case IR_OPERAND_GLOBAL_REF:
        case IR_OPERAND_FUNC_REF:
        case IR_OPERAND_LABEL_REF:
            ember_ir_ref_operand_destroy(
                (struct EmberIrRefOperand *)self
            );
            break;

        case IR_OPERAND_IMMEDIATE:
            ember_ir_immediate_operand_destroy(
                (struct EmberIrImmediateOperand *)self
            );
            break;
    }
}

struct EmberIrConstantOperand *ember_ir_constant_operand_new(
    struct EmberIrValue value,
    struct EmberSourceSpan source_span
) {
    struct EmberIrConstantOperand *self =
        ember_alloc(sizeof(struct EmberIrConstantOperand));

    if (self == NULL) {
        return NULL;
    }

    ember_ir_operand_init(&self->base, IR_OPERAND_CONSTANT, source_span);
    self->value = value;

    return self;
}

void ember_ir_constant_operand_destroy(struct EmberIrConstantOperand *self) {
    if (self == NULL) {
        return;
    }

    ember_ir_value_deinit(&self->value);
    ember_free(self);
}

static struct EmberIrIndexOperand *ember_ir_index_operand_new(
    enum EmberIrOperandKind kind,
    uint32_t index,
    struct EmberSourceSpan source_span
) {
    struct EmberIrIndexOperand *self =
        ember_alloc(sizeof(struct EmberIrIndexOperand));

    if (self == NULL) {
        return NULL;
    }

    ember_ir_operand_init(&self->base, kind, source_span);
    self->index = index;

    return self;
}

struct EmberIrIndexOperand *ember_ir_arg_index_operand_new(
    uint32_t index,
    struct EmberSourceSpan source_span
) {
    return ember_ir_index_operand_new(IR_OPERAND_ARG_INDEX, index, source_span);
}

struct EmberIrIndexOperand *ember_ir_local_index_operand_new(
    uint32_t index,
    struct EmberSourceSpan source_span
) {
    return ember_ir_index_operand_new(IR_OPERAND_LOCAL_INDEX, index, source_span);
}

void ember_ir_index_operand_destroy(struct EmberIrIndexOperand *self) {
    if (self == NULL) {
        return;
    }

    ember_free(self);
}

struct EmberIrImmediateOperand *ember_ir_immediate_operand_new(
    int64_t value,
    struct EmberSourceSpan source_span
) {
    struct EmberIrImmediateOperand *self =
        ember_alloc(sizeof(struct EmberIrImmediateOperand));

    if (self == NULL) {
        return NULL;
    }

    ember_ir_operand_init(&self->base, IR_OPERAND_IMMEDIATE, source_span);
    self->value = value;

    return self;
}

void ember_ir_immediate_operand_destroy(struct EmberIrImmediateOperand *self) {
    if (self == NULL) {
        return;
    }

    ember_free(self);
}

static int ember_ir_ref_operand_kind_is_valid(enum EmberIrOperandKind kind) {
    return kind == IR_OPERAND_GLOBAL_REF ||
           kind == IR_OPERAND_FUNC_REF ||
           kind == IR_OPERAND_LABEL_REF;
}

struct EmberIrRefOperand *ember_ir_ref_operand_new(
    enum EmberIrOperandKind kind,
    const char *name,
    struct EmberSourceSpan source_span
) {
    if (!ember_ir_ref_operand_kind_is_valid(kind)) {
        return NULL;
    }

    struct EmberIrRefOperand *self =
        ember_alloc(sizeof(struct EmberIrRefOperand));

    if (self == NULL) {
        return NULL;
    }

    ember_ir_operand_init(&self->base, kind, source_span);
    self->name = ember_strdup(name);

    if (self->name == NULL) {
        ember_free(self);
        return NULL;
    }

    return self;
}

struct EmberIrRefOperand *ember_ir_global_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
) {
    return ember_ir_ref_operand_new(IR_OPERAND_GLOBAL_REF, name, source_span);
}

struct EmberIrRefOperand *ember_ir_func_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
) {
    return ember_ir_ref_operand_new(IR_OPERAND_FUNC_REF, name, source_span);
}

struct EmberIrRefOperand *ember_ir_label_ref_operand_new(
    const char *name,
    struct EmberSourceSpan source_span
) {
    return ember_ir_ref_operand_new(IR_OPERAND_LABEL_REF, name, source_span);
}

void ember_ir_ref_operand_destroy(struct EmberIrRefOperand *self) {
    if (self == NULL) {
        return;
    }

    ember_free(self->name);
    ember_free(self);
}
