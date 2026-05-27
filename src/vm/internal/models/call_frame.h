#ifndef EMBER_VM_INTERNAL_MODELS_CALL_FRAME_H
#define EMBER_VM_INTERNAL_MODELS_CALL_FRAME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ember/core/models/value/base.h>
#include <ember/core/models/executable/types.h>
#include <ember/core/models/executable/inspect.h>

/**
 * @brief Active function call state.
 *
 * A call frame represents one active function invocation. It stores the current
 * instruction pointer, the end of the function bytecode, the base slot for the
 * function's arguments and locals, and the function being executed.
 */
struct EmberCallFrame {
    /**
     * @brief Current instruction pointer.
     *
     * Points to the next bytecode byte to read.
     */
    const uint8_t *ip;

    /**
     * @brief Pointer one past the end of the function bytecode.
     *
     * Used to prevent instruction and operand reads from going past the end of
     * the function's code.
     */
    const uint8_t *code_end;

    /**
     * @brief Base slot for this function call.
     *
     * Arguments start at `slots[0]`. Locals start after the argument area at
     * `slots[arity]`.
     */
    struct EmberValue *slots;

    /**
     * @brief Function executed by this frame.
     *
     * The frame borrows this pointer from the executable.
     */
    const struct EmberFunction *function;
};

/**
 * @brief Initializes a call frame for a function invocation.
 *
 * The instruction pointer is initialized to the beginning of the function's
 * bytecode, and `code_end` is initialized to one past the final byte.
 *
 * @param self Frame to initialize.
 * @param function Function executed by this frame.
 * @param slots Base stack slot for this invocation's arguments and locals.
 */
void ember_call_frame_init(
    struct EmberCallFrame *self,
    const struct EmberFunction *function,
    struct EmberValue *slots
);

/**
 * @brief Reads an argument from the frame.
 *
 * @param self Frame to read from.
 * @param idx Argument index.
 * @param out Output pointer receiving the argument value.
 * @return `true` on success, `false` if the argument index is invalid.
 */
static inline bool ember_call_frame_geta(
    const struct EmberCallFrame *self,
    uint8_t idx,
    struct EmberValue *out
) {
    if (idx >= ember_function_get_arity(self->function)) {
        return false;
    }
    if (out != NULL) {
        *out = self->slots[idx];
    }
    return true;
}

/**
 * @brief Reads a local variable from the frame.
 *
 * Local index `0` is stored at `slots[arity]`.
 *
 * @param self Frame to read from.
 * @param idx Local variable index.
 * @param out Output pointer receiving the local value.
 * @return `true` on success, `false` if the local index is invalid.
 */
static inline bool ember_call_frame_getl(
    const struct EmberCallFrame *self,
    uint8_t idx,
    struct EmberValue *out
) {
    if (idx >= ember_function_get_local_count(self->function)) {
        return false;
    }
    if (out != NULL) {
        *out = self->slots[idx + ember_function_get_arity(self->function)];
    }
    return true;
}

/**
 * @brief Writes a local variable in the frame.
 *
 * Local index `0` is stored at `slots[arity]`.
 *
 * @param self Frame to modify.
 * @param idx Local variable index.
 * @param value Value to store.
 * @return `true` on success, `false` if the local index is invalid.
 */
static inline bool ember_call_frame_setl(
    struct EmberCallFrame *self,
    uint8_t idx,
    struct EmberValue value
) {
    if (idx >= ember_function_get_local_count(self->function)) {
        return false;
    }
    self->slots[idx + ember_function_get_arity(self->function)] = value;
    return true;
}

/**
 * @brief Checks whether a number of bytes can be read from the frame.
 *
 * @param self Frame to inspect.
 * @param byte_count Number of bytes that would be read.
 * @return `true` if the bytes are available, otherwise `false`.
 */
static inline bool ember_call_frame_can_read(
    const struct EmberCallFrame *self,
    size_t byte_count
) {
    return self->ip <= self->code_end && (size_t)(self->code_end - self->ip) >= byte_count;
}

/**
 * @brief Reads an unsigned 8-bit value from the bytecode stream.
 *
 * Advances the instruction pointer by one byte on success.
 *
 * @param self Frame to read from.
 * @param out Output pointer receiving the value.
 * @return `true` on success, `false` if there is not enough bytecode left.
 */
static inline bool ember_call_frame_read_u8(
    struct EmberCallFrame *self,
    uint8_t *out
) {
    if (!ember_call_frame_can_read(self, 1)) {
        return false;
    }
    if (out != NULL) {
        *out = *self->ip;
    }
    self->ip++;
    return true;
}

/**
 * @brief Reads an unsigned 16-bit little-endian value from the bytecode stream.
 *
 * The first byte is interpreted as the low byte and the second byte as the high
 * byte. Advances the instruction pointer by two bytes on success.
 *
 * @param self Frame to read from.
 * @param out Output pointer receiving the value.
 * @return `true` on success, `false` if there is not enough bytecode left.
 */
static inline bool ember_call_frame_read_u16(
    struct EmberCallFrame *self,
    uint16_t *out
) {
    if (!ember_call_frame_can_read(self, 2)) {
        return false;
    }
    uint16_t lo = (uint16_t)self->ip[0];
    uint16_t hi = (uint16_t)self->ip[1];
    if (out != NULL) {
        *out = (uint16_t)(lo | (hi << 8));
    }
    self->ip += 2;
    return true;
}

/**
 * @brief Reads a signed 16-bit little-endian value from the bytecode stream.
 *
 * The bytes are first read as an unsigned 16-bit little-endian value and then
 * converted to `int16_t`. Advances the instruction pointer by two bytes on
 * success.
 *
 * @param self Frame to read from.
 * @param out Output pointer receiving the value.
 * @return `true` on success, `false` if there is not enough bytecode left.
 */
static inline bool ember_call_frame_read_i16(
    struct EmberCallFrame *self,
    int16_t *out
) {
    uint16_t value;
    if (!ember_call_frame_read_u16(self, &value)) {
        return false;
    }
    if (out != NULL) {
        *out = (int16_t)value;
    }
    return true;
}

#endif