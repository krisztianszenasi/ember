#include <ember/core/models/executable/inspect.h>
#include "core/internal/models/executable/types.h"

const struct EmberFunction *ember_executable_get_function(const struct EmberExecutable *self, size_t idx) {
    return &self->functions[idx];
}

size_t ember_executable_get_start_function_idx(const struct EmberExecutable *self) {
    return self->start_function_idx;
}

const uint8_t *ember_function_get_code(const struct EmberFunction *self) {
    return self->code;
}

size_t ember_function_get_current_code_location(const struct EmberFunction *self) {
    return self->code_count;
}

size_t ember_function_get_local_count(const struct EmberFunction *self) {
    return self->local_count;
}

size_t ember_function_get_arity(const struct EmberFunction *self) {
    return self->arity;
}

int ember_function_read_const(
    const struct EmberFunction *self,
    size_t idx,
    struct EmberValue *out
) {
    *out = self->constants.values[idx];
    return 0;
}

size_t ember_function_get_const_count(const struct EmberFunction *self) {
    return self->constants.count;
}

size_t ember_executable_get_function_count(const struct EmberExecutable *self) {
    return self->function_count;
}

size_t ember_function_get_code_count(const struct EmberFunction *self) {
    return self->code_count;
}