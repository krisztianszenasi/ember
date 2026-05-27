#include "assembler/phases/semantic_analysis/internal/models/synbol.h"

#include <ember/core/memory.h>

struct EmberSymbol *ember_symbol_new(
    enum EmberSymbolKind kind,
    const char *name,
    const struct EmberAstNode *node
) {
    struct EmberSymbol *self = ember_alloc(sizeof(struct EmberSymbol));
    self->kind = kind;
    self->name = ember_strdup(name);
    self->node = node;
    self->reference_count = 0;
    return self;
}

void ember_symbol_destroy(struct EmberSymbol *self) {
    if (self == NULL) {
        return;
    }
    ember_free(self->name);
    ember_free(self);
}