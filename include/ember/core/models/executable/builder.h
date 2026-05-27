#ifndef EMBER_CORE_EXECUTABLE_BUILDER_H
#define EMBER_CORE_EXECUTABLE_BUILDER_H

#include <stddef.h>
#include <stdint.h>

#include <ember/core/opcode.h>
#include <ember/core/models/value/base.h>
#include <ember/core/models/executable/types.h>

/**
 * Defines a new function in the executable.
 *
 * The returned function can then be populated with bytecode and constants.
 * Ownership of the function remains with the executable. Callers must not
 * free the returned function directly.
 *
 * @param self The executable that will own the new function.
 * @param name The name of the function.
 * @param arity The number of arguments accepted by the function.
 * @param local_count The number of local slots used by the function.
 * @return The newly created function, or NULL on failure.
 */
struct EmberFunction *ember_executable_define_function(
    struct EmberExecutable *self,
    const char *name,
    size_t arity,
    size_t local_count
);

void ember_executable_set_starter_function_idx(
    struct EmberExecutable *self,
    size_t idx
);

/**
 * Appends a single byte to the function's bytecode.
 *
 * This is typically used to emit an opcode or an 8-bit operand.
 *
 * @param self The function to write to.
 * @param byte The byte to append.
 */
void ember_function_write_u8(struct EmberFunction *self, uint8_t byte);

/**
 * Appends a 16-bit unsigned word to the function's bytecode.
 *
 * This is typically used to emit a 16-bit operand.
 *
 * @param self The function to write to.
 * @param word The 16-bit value to append.
 */
void ember_function_write_u16(struct EmberFunction *self, uint16_t word);

/**
 * Appends a 16-bit signed word to the function's bytecode.
 *
 * This is typically used to emit signed offsets, such as jump operands.
 *
 * @param self The function to write to.
 * @param word The signed 16-bit value to append.
 */
void ember_function_write_i16(struct EmberFunction *self, int16_t word);

/**
 * Returns a label for the current bytecode position.
 *
 * The returned label identifies the location at which the next emitted
 * bytecode instruction or operand will be written. It can later be used as
 * the target of jump instructions or jump patching operations.
 *
 * @param self The function to query.
 * @return A label representing the current bytecode position.
 */
EmberLabel ember_function_make_label(struct EmberFunction *self);

/**
 * Emits a backward jump to a previously created label.
 *
 * The jump target is encoded relative to the current instruction position.
 *
 * @param self The function to write to.
 * @param label The target label to jump to.
 */
void ember_function_emit_backward_label(
    struct EmberFunction *self,
    size_t label
);

/**
 * Emits a forward jump.
 *
 * The returned location can later be passed to
 * ember_function_patch_forward_jump() once the target label is known.
 *
 * @param self The function to write to.
 * @return The bytecode offset of the jump operand to patch later.
 */
size_t ember_function_emit_forward_label(
    struct EmberFunction *self
);

/**
 * Patches a previously emitted forward jump so that it targets the given label.
 *
 * @param self The function to patch.
 * @param patch_location The bytecode offset previously returned by
 *        ember_function_emit_forward_jump().
 * @param label The target label for the jump.
 */
void ember_function_patch_forward_label(
    struct EmberFunction *self,
    size_t patch_location,
    size_t label
);

/**
 * Adds a constant to the function's constant pool.
 *
 * @param self The function that owns the constant pool.
 * @param constant The constant value to store.
 * @return The index of the stored constant.
 */
size_t ember_function_register_constant(
    struct EmberFunction *self,
    struct EmberValue constant
);

/**
 * Writes a byte into the function's code buffer at a position relative to the
 * current end of the buffer.
 *
 * The `backward_idx` parameter specifies how many bytes back from the current
 * end of the code buffer should be patched. A value of `0` patches the most
 * recently written byte, effectively behaving like an in-place overwrite of
 * the last emitted byte.
 *
 * This is typically used during code generation to patch previously emitted
 * placeholder bytes once their final value becomes known.
 *
 * @param self Pointer to the function whose code buffer should be patched.
 * @param backward_idx Number of bytes to step backward from the current end of
 *                     the code buffer before writing.
 * @param byte Byte value to write.
 */
void ember_function_patch_u8(
    struct EmberFunction *self,
    size_t backward_idx,
    uint8_t byte
);

#endif