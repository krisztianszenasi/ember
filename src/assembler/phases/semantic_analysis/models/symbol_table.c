#include "assembler/phases/semantic_analysis/internal/models/symbol_table.h"

#include <string.h>
#include <ember/core/memory.h>

struct EmberSymbolTable *ember_symbol_table_new(void) {
    struct EmberSymbolTable *self = ember_alloc(sizeof(struct EmberSymbolTable));
    self->capacity = 0;
    self->count = 0;
    self->previous = NULL;
    self->symbols = NULL;
    return self;
}

void ember_symbol_table_destroy(struct EmberSymbolTable *self) {
    if (self == NULL) {
        return;
    }
    for (size_t i = 0; i < self->count; i++) {
        ember_symbol_destroy(self->symbols[i]);
    }
    EMBER_FREE_ARRAY(
        struct EmberSymbol *,
        self->symbols,
        self->capacity
    );
    ember_free(self);
}

static int _ember_symbol_table_lookup(
    const struct EmberSymbolTable *self,
    const char *name,
    struct EmberSymbol **out
) {
    if (self == NULL) {
        return -1;
    }
    for (size_t i = 0; i < self->count; i++) {
        if (strcmp(name, self->symbols[i]->name) == 0) {
            self->symbols[i]->reference_count++;
            *out = self->symbols[i]; // TODO: dont increase when looking up for duplicates
            return 0;
        }
    }
    return _ember_symbol_table_lookup(self->previous, name, out);
}

int ember_symbol_table_register(
    struct EmberSymbolTable *self,
    enum EmberSymbolKind kind,
    const char *name,
    const struct EmberAstNode *node,
    const struct EmberSymbol **clash
) {
    if (ember_symbol_table_lookup(self, name, clash) == 0) {
        return -1;
    }

    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->symbols = EMBER_GROW_ARRAY(
            struct EmberSymbol *,
            self->symbols,
            old_capacity,
            self->capacity
        );
    }

    self->symbols[self->count++] = ember_symbol_new(kind, name, node);
    return 0;
}

int ember_symbol_table_register_constant(
    struct EmberSymbolTable *self,
    const char *name,
    const struct EmberAstAssignment *node,
    const struct EmberSymbol **clash
) {
    if (ember_symbol_table_register(self, EMBER_SYMBOL_CONSTANT, name, &node->base.base, clash) != 0) {
        return -1;
    }

    struct EmberSymbol *symbol = NULL;
    if (_ember_symbol_table_lookup(self, name, &symbol) == 0) {
        symbol->value = node->lieral;
    }
    
    return 0;
}

struct EmberSymbolTable *ember_symbol_table_enter_scope(
    struct EmberSymbolTable *self
) {
    struct EmberSymbolTable *new = ember_symbol_table_new();
    new->previous = self;
    return new;
}

struct EmberSymbolTable *ember_symbol_table_exit_scope(
    struct EmberSymbolTable *self
) {
    struct EmberSymbolTable *previous = self->previous;
    ember_symbol_table_destroy(self);
    return previous;
}

int ember_symbol_table_lookup(
    const struct EmberSymbolTable *self,
    const char *name,
    const struct EmberSymbol **out
) {
    struct EmberSymbol *found;
    if (_ember_symbol_table_lookup(self, name, &found) == 0) {
        *out = found;
        return 0;
    };
    return -1;
}