#include "assembler/internal/models/ast/nodes/section.h"

#include <ember/core/memory.h>

void ember_ast_section_init(
    struct EmberAstSection *self,
    struct EmberAstIdentifier identifier,
    struct EmberSourceSpan source_span
) {
    ember_ast_node_init(&self->base, AST_NODE_SECTION, source_span);
    self->identifier = identifier;
    self->capacity = 0;
    self->count = 0;
    self->items = NULL;
}

void ember_ast_section_deinit(struct EmberAstSection *self) {
    if (!self) { return; }
    ember_ast_identifier_deinit(&self->identifier);
    *self = (struct EmberAstSection) {0};
}

void ember_ast_section_add_item(struct EmberAstSection *self, struct EmberAstSectionItem *item) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->items = EMBER_GROW_ARRAY(
            struct EmberAstSectionItem *,
            self->items,
            old_capacity,
            self->capacity
        );
    }
    self->items[self->count++] = item;
}