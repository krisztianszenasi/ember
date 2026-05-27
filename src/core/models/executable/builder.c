#include <ember/core/models/executable/builder.h>
#include "core/internal/models/executable/types.h"

#include <string.h>

#include <ember/core/memory.h>
#include <ember/core/opcode.h>

struct EmberFunction *ember_executable_define_function(
    struct EmberExecutable *self,
    const char *name,
    size_t arity,
    size_t local_count
) {
    if (self->function_count + 1 > self->function_capacity) {
        size_t old_capacity = self->function_capacity;
        self->function_capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->functions = EMBER_GROW_ARRAY(
            struct EmberFunction,
            self->functions,
            old_capacity,
            self->function_capacity
        );
    }
    struct EmberFunction *func = &self->functions[self->function_count++];

    func->name = ember_strdup(name);
    func->arity = arity;
    func->local_count = local_count;
    func->code_count = 0;
    func->code_capacity = 0;
    func->code = NULL;
    ember_value_array_init(&func->constants);
    
    return func;
}

void ember_executable_set_starter_function_idx(
    struct EmberExecutable *self,
    size_t idx
) {
    self->start_function_idx = idx;
}

void ember_function_write_u8(struct EmberFunction *self, uint8_t byte) {
    if (self->code_count + 1 > self->code_capacity) {
        size_t old_capacity = self->code_capacity;
        self->code_capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->code = EMBER_GROW_ARRAY(
            uint8_t,
            self->code,
            old_capacity,
            self->code_capacity
        );
    }
    self->code[self->code_count++] = byte;
}

void ember_function_write_u16(struct EmberFunction *self, uint16_t word) {
    ember_function_write_u8(self, (uint8_t)(word & 0xff));
    ember_function_write_u8(self, (uint8_t)((word >> 8) & 0xff));
}

void ember_function_write_i16(struct EmberFunction *self, int16_t word) {
    ember_function_write_u16(self, (uint16_t)word);
}

size_t ember_function_register_constant(struct EmberFunction *self, struct EmberValue constant) {
    size_t existing_idx;
    if (ember_value_array_find(&self->constants, constant, &existing_idx)) {
        return existing_idx;
    }
    ember_value_array_add(&self->constants, constant);
    return self->constants.count - 1;
}

EmberLabel ember_function_make_label(struct EmberFunction *self) {
    return self->code_count;
}

void ember_function_emit_backward_label(
    struct EmberFunction *self,
    size_t label
) {
    size_t jump_from = self->code_count + 2;
    size_t offset = jump_from - label;
    int16_t asd = (int16_t)offset;
    ember_function_write_i16(self, -asd);
}

EmberLabel ember_function_emit_forward_label(
    struct EmberFunction *self
) {
    size_t patch_location = self->code_count;
    ember_function_write_i16(self, 0);
    return patch_location;
}

void ember_function_patch_forward_label(
    struct EmberFunction *self,
    size_t patch_location,
    size_t label
) {
    ptrdiff_t offset = (ptrdiff_t)label - (ptrdiff_t)(patch_location + 2);
    int16_t jump = (int16_t)offset;

    self->code[patch_location]     = (uint8_t)(jump & 0xff);
    self->code[patch_location + 1] = (uint8_t)(((uint16_t)jump >> 8) & 0xff);
}

void ember_function_patch_u8(
    struct EmberFunction *self,
    size_t backward_idx,
    uint8_t byte
) {
    self->code[self->code_count - backward_idx - 1] = byte;
}
