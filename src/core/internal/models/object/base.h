#ifndef EMBER_CORE_INTERNAL_MODELS_OBJECT_BASE_H
#define EMBER_CORE_INTERNAL_MODELS_OBJECT_BASE_H

#include <stdbool.h>
#include <stddef.h>

enum EmberObjectKind {
    EOK_STR,
};

struct EmberObject {
    enum EmberObjectKind kind;
    bool marked;
    size_t size;
    struct EmberObject *next;
};

static inline void ember_object_init(
    struct EmberObject *self,
    enum EmberObjectKind kind,
    size_t size
) {
    self->kind = kind;
    self->marked = false;
    self->size = size;
    self->next = NULL;
}

#endif