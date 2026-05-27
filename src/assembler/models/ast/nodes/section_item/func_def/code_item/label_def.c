#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/label_def.h"

#include <ember/core/memory.h>

struct EmberAstLabelDef *ember_ast_label_def_new(
    struct EmberAstIdentifier identifier,
    struct EmberSourceSpan source_span
) {
    struct EmberAstLabelDef *self = ember_alloc(sizeof(struct EmberAstLabelDef));
    ember_ast_code_item_init(&self->base, AST_CODE_ITEM_LABEL_DEF, source_span);    
    self->identifier = identifier;
    return self;
}

void ember_ast_label_def_destroy(
    struct EmberAstLabelDef *self
) {
    ember_ast_identifier_deinit(&self->identifier);
    ember_free(self);
}