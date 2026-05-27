#ifndef EMBER_CORE_EXECUTABLE_INSPECT_H
#define EMBER_CORE_EXECUTABLE_INSPECT_H

#include <stddef.h>
#include <stdint.h>
#include <ember/core/models/value/base.h>
#include <ember/core/models/executable/types.h>

size_t ember_executable_get_function_count(const struct EmberExecutable *self);

/**
 * Returns the function at the given index in the executable.
 *
 * @param self The executable to read from.
 * @param idx The function index.
 * @return The function at the given index, or NULL if the index is invalid.
 */
const struct EmberFunction *ember_executable_get_function(
    const struct EmberExecutable *self,
    size_t idx
);

/**
 * Returns the index of the entry function of the executable.
 *
 * The entry function is the function that the VM starts executing first.
 *
 * @param self The executable to query.
 * @return The index of the entry function.
 */
size_t ember_executable_get_start_function_idx(const struct EmberExecutable *self);

/**
 * Returns a pointer to the beginning of the function's bytecode.
 *
 * The returned pointer refers to memory owned by the function.
 *
 * @param self The function to query.
 * @return A pointer to the start of the function's bytecode.
 */
const uint8_t *ember_function_get_code(const struct EmberFunction *self);

/**
 * @brief Returns the number of bytecode bytes emitted for a function.
 *
 * This count represents the current size of the function's code buffer and can
 * be used when calculating instruction addresses, label locations, or patch
 * positions.
 *
 * @param self Function to inspect.
 * @return Number of bytes currently stored in the function code buffer.
 */
size_t ember_function_get_code_count(const struct EmberFunction *self);

/**
 * Returns the number of local slots used by the function.
 *
 * @param self The function to query.
 * @return The number of local slots used by the function.
 */
size_t ember_function_get_local_count(const struct EmberFunction *self);

/**
 * Returns the arity of the function.
 *
 * The arity is the number of arguments the function accepts.
 *
 * @param self The function to query.
 * @return The number of arguments accepted by the function.
 */
size_t ember_function_get_arity(const struct EmberFunction *self);

/**
 * Reads a constant from the function's constant pool.
 *
 * @param self The function to read from.
 * @param idx The index of the constant.
 * @param out Receives the constant value on success.
 * @return 0 on success, non-zero if idx is out of bounds.
 */
int ember_function_read_const(
    const struct EmberFunction *self,
    size_t idx,
    struct EmberValue *out
);

/**
 * @brief Returns the number of constants stored in a function.
 *
 * This count represents the current size of the function's constant pool and
 * can be used when assigning constant indexes or validating constant 
 * references.
 *
 * @param self Function to inspect.
 * @return Number of constants currently stored in the function constant pool.
 */
size_t ember_function_get_const_count(const struct EmberFunction *self);

#endif