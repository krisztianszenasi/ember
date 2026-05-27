#include <ember/core/models/value/array.h>

#include <ember/core/memory.h>

void ember_value_array_init(struct EmberValueArray *self) {
    self->count = 0;
    self->capacity = 0;
    self->values = NULL;
}

void ember_value_array_deinit(struct EmberValueArray *self) {
    for (size_t i = 0; i < self->count; i++) {
        ember_value_deinit(&self->values[i]);
    }
    EMBER_FREE_ARRAY(
        struct EmberValue,
        self->values,
        self->capacity
    );
    ember_value_array_init(self);
}

void ember_value_array_add(struct EmberValueArray *self, struct EmberValue value) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->values = EMBER_GROW_ARRAY(
            struct EmberValue,
            self->values,
            old_capacity,
            self->capacity
        );
    }
    self->values[self->count++] = value;
}

bool ember_value_array_find(
    const struct EmberValueArray *self,
    struct EmberValue value,
    size_t *out_idx
) {
    for (size_t i = 0; i < self->count; i++) {
        if (ember_value_eq(self->values[i], value)) {
            if (out_idx != NULL) {
                *out_idx = i;
            }
            return true;
        }
    }
    return false;
}