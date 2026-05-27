#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_PROGRAM_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_PROGRAM_H

#include <stddef.h>
#include <stdbool.h>
#include "assembler/internal/models/ir/nodes/base.h"
#include "assembler/internal/models/ir/nodes/function.h"

/**
 * Root IR node representing an assembled program.
 *
 * A program owns a dynamically managed array of functions and optionally
 * records the name of the starting function selected during lowering.
 */
struct EmberIrProgram {
    struct EmberIrNode base;
    size_t count;
    size_t capacity;
    char *starting_function;
    struct EmberIrFunction *functions;
};

/**
 * Allocates and initializes a program node.
 *
 * The returned program node is heap allocated and is intended to be used as
 * the root of the assembler IR.
 *
 * @return A newly allocated program node, or NULL on failure.
 */
struct EmberIrProgram *ember_ir_program_new(struct EmberSourceSpan source_span);

/**
 * Destroys a program node and the functions it owns.
 *
 * This deinitializes all functions stored in the program, releases the
 * dynamic function array, and frees the program node itself.
 *
 * @param self Pointer to the program node to destroy. If NULL, no action is
 *             taken.
 */
void ember_ir_program_destroy(struct EmberIrProgram *self);

/**
 * Adds a function to the program.
 *
 * The new function is appended to the program's internal function array and
 * initialized with the provided metadata.
 *
 * @param self Pointer to the program node.
 * @param kind Kind of function to add.
 * @param name Name of the function to add.
 * @param arity Number of function arguments.
 * @param local_count Number of local slots used by the function.
 * @return Pointer to the newly added function, or NULL on failure.
 */
struct EmberIrFunction *ember_ir_program_add_function(
    struct EmberIrProgram *self,
    enum EmberIrFunctionKind kind,
    const char *name,
    size_t arity,
    size_t local_count,
    struct EmberSourceSpan source_span
);

/**
 * Returns the most recently added function in the program.
 *
 * @param self Pointer to the program node.
 * @return Pointer to the current function, or NULL if the program has no
 *         functions.
 */
struct EmberIrFunction *ember_ir_program_get_current_function(struct EmberIrProgram *self);

/**
 * Sets the program's starting function if one has not been recorded yet.
 *
 * @param self Pointer to the program node.
 * @param name Name of the starting function.
 * @return `true` if the starting function was set, or `false` if one was
 *         already present.
 */
bool ember_ir_program_set_starting_function(struct EmberIrProgram *self, const char *name);

/**
 * Replaces the program's starting function name unconditionally.
 *
 * @param self Pointer to the program node.
 * @param name Name of the starting function.
 */
void ember_ir_program_force_set_starting_function(struct EmberIrProgram *self, const char *name);

#endif
