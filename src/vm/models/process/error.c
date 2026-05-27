#include "vm/internal/models/process/error.h"

#include <stddef.h>

void ember_process_error_set(
    struct EmberProcessError *self,
    const char *message,
    enum EmberRuntimeErrorKind kind
) {
    if (self == NULL) {
        return;
    }

    self->kind = kind;
    self->message = message;
}

static const char *RUNTIME_ERROR_MESSAGES[EMBER_RUNTIME_ERROR_COUNT] = {
    [EMBER_RUNTIME_ERROR_NONE]                          = "none",
    [EMBER_RUNTIME_ERROR_UNEXPECTED_END_OF_BYTECODE]    = "unexpected end of bytecode",
    [EMBER_RUNTIME_ERROR_STACK_UNDERFLOW]               = "stack underflow",
    [EMBER_RUNTIME_ERROR_STACK_OVERFLOW]                = "stack overflow",
    [EMBER_RUNTIME_ERROR_CALL_FRAME_UNDERFLOW]          = "call frame underflow",
    [EMBER_RUNTIME_ERROR_CALL_FRAME_OVERFLOW]           = "call frame overflow",
    [EMBER_RUNTIME_ERROR_INVALID_OPCODE]                = "invalid opcode",
    [EMBER_RUNTIME_ERROR_INVALID_CONSTANT_INDEX]        = "invalid constant index",
    [EMBER_RUNTIME_ERROR_INVALID_ARG_INDEX]             = "invalid argument index",
    [EMBER_RUNTIME_ERROR_INVALID_LOCAL_INDEX]           = "invalid local index",
    [EMBER_RUNTIME_ERROR_INVALID_FUNCTION_INDEX]        = "invalid function index",
    [EMBER_RUNTIME_ERROR_INVALID_GLOBAL_INDEX]          = "invalid global index",
};

const char *ember_runtime_error_message(enum EmberRuntimeErrorKind kind) {
    if ((size_t)kind >= EMBER_RUNTIME_ERROR_COUNT || RUNTIME_ERROR_MESSAGES[kind] == NULL) {
        return "unknown runtime error";
    }

    return RUNTIME_ERROR_MESSAGES[kind];
}

void ember_runtime_error(
    struct EmberProcessError *self,
    enum EmberRuntimeErrorKind kind
) {
    ember_process_error_set(
        self,
        ember_runtime_error_message(kind),
        kind
    );
}