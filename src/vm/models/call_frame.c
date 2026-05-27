#include "vm/internal/models/call_frame.h"

#include "vm/internal/models/call_frame.h"

void ember_call_frame_init(
    struct EmberCallFrame *self,
    const struct EmberFunction *function,
    struct EmberValue *slots
) {
    const uint8_t *code = ember_function_get_code(function);
    size_t code_count = ember_function_get_code_count(function);

    self->function = function;
    self->slots = slots;
    self->ip = code;
    self->code_end = code + code_count;
}