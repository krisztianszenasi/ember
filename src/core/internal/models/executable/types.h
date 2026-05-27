#ifndef EMBER_CORE_INTERNAL_CORE_EXECUTABLE_TYPE_H
#define EMBER_CORE_INTERNAL_CORE_EXECUTABLE_TYPE_H

#include <stddef.h>
#include <stdint.h>

#include <ember/core/models/value/array.h>

/**
 * @brief Runtime representation of an executable function.
 *
 * A function stores its name, emitted bytecode, arity, local variable count,
 * and constant pool. The VM executes the bytecode stored in `code` and uses
 * the constant pool to load literal values during execution.
 */
struct EmberFunction {
    /**
     * @brief Function name.
     */
    char *name;

    /**
     * @brief Bytecode buffer for the function body.
     */
    uint8_t *code;

    /**
     * @brief Number of bytecode bytes currently stored in the code buffer.
     */
    size_t code_count;

    /**
     * @brief Maximum number of bytecode bytes that can be stored in the code buffer.
     */
    size_t code_capacity;

    /**
     * @brief Number of arguments expected by the function.
     */
    size_t arity;

    /**
     * @brief Number of local variable slots used by the function.
     */
    size_t local_count;

    /**
     * @brief Constant pool used by the function.
     */
    struct EmberValueArray constants;
};

void ember_function_init(struct EmberFunction *self);

void ember_function_deinit(struct EmberFunction *self);

/**
 * @brief Runtime representation of an Ember executable.
 *
 * An executable contains all functions emitted by the assembler and identifies
 * the function where VM execution should start.
 */
struct EmberExecutable {
    /**
     * @brief Function table stored in the executable.
     */
    struct EmberFunction *functions;

    /**
     * @brief Number of functions currently stored in the executable.
     */
    size_t function_count;

    /**
     * @brief Maximum number of functions that can be stored in the executable.
     */
    size_t function_capacity;

    /**
     * @brief Index of the function where execution starts.
     */
    size_t start_function_idx;
};

#endif