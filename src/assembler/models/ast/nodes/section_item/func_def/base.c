#include "assembler/internal/models/ast/nodes/section_item/func_def/base.h"

#include <ember/core/memory.h>

struct EmberAstFuncDef *ember_ast_func_def_new(
    struct EmberAstIdentifier identifier,
    size_t arity,
    size_t local_count
) {
    struct EmberAstFuncDef *self = ember_alloc(sizeof(struct EmberAstFuncDef));
    ember_ast_section_item_init(&self->base, AST_SECTION_ITEM_FUNC_DEF, EMBER_SOURCE_SPAN_NONE);
    
    self->identifier = identifier;
    self->arity = arity;
    self->local_count = local_count;

    self->item_count = 0;
    self->item_capacity = 0;
    self->items = NULL;

    return self;
}

void ember_ast_func_def_destroy(
    struct EmberAstFuncDef *self
) {
    ember_ast_identifier_deinit(&self->identifier);
    for(size_t i = 0; i < self->item_count; i++) {
        ember_ast_code_item_destroy(self->items[i]);
    }
    EMBER_FREE_ARRAY(
        struct EmberAstCodeItem *,
        self->items,
        self->item_capacity
    );
    ember_free(self);
}

void ember_ast_func_def_add_code_item(
    struct EmberAstFuncDef *self,
    struct EmberAstCodeItem *code_item
) {
    if (self->item_count + 1 > self->item_capacity) {
        size_t old_capacity = self->item_capacity;
        self->item_capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->items = EMBER_GROW_ARRAY(
            struct EmberAstCodeItem *,
            self->items,
            old_capacity,
            self->item_capacity
        );
    }
    self->items[self->item_count++] = code_item;
}