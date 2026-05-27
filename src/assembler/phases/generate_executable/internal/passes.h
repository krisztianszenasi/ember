#ifndef ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_PASSES_H
#define ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_PASSES_H

#include "assembler/internal/models/ir/nodes/base.h"
#include "assembler/phases/generate_executable/internal/models/context.h"

/**
 * @brief Registers executable-level symbols before bytecode generation.
 *
 * Walks the IR program and assigns indexes to global variables and functions.
 * These indexes are stored in the executable generation context so later
 * passes can resolve references when emitting bytecode instructions.
 *
 * @param ctx Executable generation context.
 * @param root Root IR node to visit.
 */
void do_register_global_symbols_pass(
    struct EmberGenerateExeContext *ctx,
    struct EmberIrNode *root
);

/**
 * @brief Generates executable function bodies from IR.
 *
 * Walks IR function bodies and emits the corresponding bytecode instructions
 * into the executable. This pass uses the previously registered indexes to
 * resolve references to functions, globals.
 *
 * @param ctx Executable generation context.
 * @param root Root IR node to visit.
 */
void do_generate_functions_pass(
    struct EmberGenerateExeContext *ctx,
    struct EmberIrNode *root
);

#endif