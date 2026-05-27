#include "assembler/phases/generate_executable/internal/models/index_table.h"

#include <string.h>
#include <ember/core/memory.h>

struct EmberIndexTable *ember_index_table_new(void) {
    struct EmberIndexTable *self = ember_alloc(sizeof(struct EmberIndexTable));
    self->capacity = 0;
    self->count = 0;
    self->elements = NULL;
    self->next_indexes[0] = 0;
    self->next_indexes[1] = 0;
    self->next_indexes[2] = 0;
    self->previous = NULL;
    return self;
}

void ember_index_table_destroy(struct EmberIndexTable *self) {
    if (self == NULL) {
        return;
    }
    EMBER_FREE_ARRAY(
        struct EmberIndexElement,
        self->elements,
        self->capacity
    );
    ember_free(self);
}

bool ember_index_table_register(
    struct EmberIndexTable *self,
    const char *name,
    enum EmberIndexElementKind kind,
    size_t custom_index
) {
    size_t dummy;
    if (ember_index_table_lookup(self, name, &dummy)) {
        return false;
    }

    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->elements = EMBER_GROW_ARRAY(
            struct EmberIndexElement,
            self->elements,
            old_capacity,
            self->capacity
        );
    }

    size_t index_to_use;
    if (kind == INDEX_ELEMENT_LABEL) {
        index_to_use = custom_index;
    } else {
        index_to_use = self->next_indexes[kind]++;
    }
    self->elements[self->count].index = index_to_use;
    self->elements[self->count].name = name;
    self->count++;
    return true;
}

bool ember_index_table_lookup(
    struct EmberIndexTable *self,
    const char *name,
    size_t *out
) {
    if (self == NULL) {
        return false;
    }
    for (size_t i = 0; i < self->count; i++) {
        if (strcmp(name, self->elements[i].name) == 0) {
            *out = self->elements[i].index;
            return true;
        }
    }
    return ember_index_table_lookup(self->previous, name, out);
}

struct EmberIndexTable *ember_index_table_enter_scope(struct EmberIndexTable *self) {
    struct EmberIndexTable *new = ember_index_table_new();
    new->previous = self;
    return new;
}

struct EmberIndexTable *ember_index_table_exit_scope(struct EmberIndexTable *self) {
    struct EmberIndexTable *previous = self->previous;
    ember_index_table_destroy(self);
    return previous;
}