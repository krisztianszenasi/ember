#include "assembler/internal/models/ir/nodes/label_def.h"

#include <ember/core/memory.h>

struct EmberIrLabelDef *ember_ir_label_def_new(
    const char *name,
    struct EmberSourceSpan source_span
) {
    struct EmberIrLabelDef *self = ember_alloc(sizeof(struct EmberIrLabelDef));
    ember_ir_code_item_init(&self->base, IR_CODE_ITEM_LABEL_DEF, source_span);
    self->name = ember_strdup(name);
    return self;
}

void ember_ir_label_def_destroy(struct EmberIrLabelDef *self) {
    ember_free(self->name);
    ember_free(self);
}
