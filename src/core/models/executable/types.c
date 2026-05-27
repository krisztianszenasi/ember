#include <ember/core/models/executable/types.h>

#include <stdlib.h>
#include <ember/core/memory.h>

#include "core/internal/models/executable/types.h"


void ember_function_deinit(struct EmberFunction *self) {
    ember_value_array_deinit(&self->constants);
    EMBER_FREE_ARRAY(
        uint8_t,
        self->code,
        self->code_capacity
    );
}


struct EmberExecutable *ember_executable_new(void) {
    struct EmberExecutable *self = ember_alloc(sizeof(struct EmberExecutable));
    
    self->function_capacity = 0;
    self->function_count = 0;
    self->start_function_idx = 0;
    self->functions = NULL;
    
    return self;
}

void ember_executable_free(struct EmberExecutable *self) {
    if (self == NULL) {
        return;
    }

    for (size_t i = 0; i < self->function_count; i++) {
        ember_function_deinit(&self->functions[i]);
    }
    EMBER_FREE_ARRAY(
        struct EmberFunction,
        self->functions,
        self->function_capacity
    );
    ember_free(self);
}