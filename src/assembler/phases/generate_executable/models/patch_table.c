#include "assembler/phases/generate_executable/internal/models/patch_table.h"

#include <string.h>

#include <ember/core/memory.h>

static void ember_patch_element_deinit(struct EmberPatchElement *self) {
    if (self == NULL) {
        return;
    }

    EMBER_FREE_ARRAY(
        size_t,
        self->patch_locations,
        self->capacity
    );

    self->name = NULL;
    self->count = 0;
    self->capacity = 0;
    self->patch_locations = NULL;
}

struct EmberPatchTable *ember_patch_table_new(void) {
    struct EmberPatchTable *self = ember_alloc(sizeof(struct EmberPatchTable));

    self->count = 0;
    self->capacity = 0;
    self->elements = NULL;

    return self;
}

void ember_patch_table_destroy(struct EmberPatchTable *self) {
    if (self == NULL) {
        return;
    }

    for (size_t i = 0; i < self->count; i++) {
        ember_patch_element_deinit(&self->elements[i]);
    }

    EMBER_FREE_ARRAY(
        struct EmberPatchElement,
        self->elements,
        self->capacity
    );

    ember_free(self);
}

bool ember_patch_table_lookup(
    struct EmberPatchTable *self,
    const char *name,
    struct EmberPatchElement **out
) {
    if (self == NULL || name == NULL) {
        return false;
    }

    for (size_t i = 0; i < self->count; i++) {
        if (strcmp(name, self->elements[i].name) == 0) {
            if (out != NULL) {
                *out = &self->elements[i];
            }

            return true;
        }
    }

    return false;
}

static struct EmberPatchElement *ember_patch_table_add_element(
    struct EmberPatchTable *self,
    const char *name
) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->elements = EMBER_GROW_ARRAY(
            struct EmberPatchElement,
            self->elements,
            old_capacity,
            self->capacity
        );

        if (self->elements == NULL) {
            self->capacity = old_capacity;
            return NULL;
        }
    }

    struct EmberPatchElement *element = &self->elements[self->count];

    element->name = name;
    element->count = 0;
    element->capacity = 0;
    element->patch_locations = NULL;

    self->count++;

    return element;
}

static bool ember_patch_element_add_location(
    struct EmberPatchElement *self,
    size_t location
) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->patch_locations = EMBER_GROW_ARRAY(
            size_t,
            self->patch_locations,
            old_capacity,
            self->capacity
        );

        if (self->patch_locations == NULL) {
            self->capacity = old_capacity;
            return false;
        }
    }

    self->patch_locations[self->count] = location;
    self->count++;

    return true;
}

bool ember_patch_table_register(
    struct EmberPatchTable *self,
    const char *name,
    size_t location
) {
    if (self == NULL || name == NULL) {
        return false;
    }

    struct EmberPatchElement *element = NULL;

    if (!ember_patch_table_lookup(self, name, &element)) {
        element = ember_patch_table_add_element(self, name);

        if (element == NULL) {
            return false;
        }
    }

    return ember_patch_element_add_location(element, location);
}