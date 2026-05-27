#ifndef EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_PASSES_H
#define EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_PASSES_H

#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/phases/lower_ast_to_ir/internal/models/context.h"

/**
 * @brief Generates the synthetic entry function.
 *
 * The entry function is the executable's starting point. It performs any
 * required initialization and then calls the user-defined main function.
 *
 * @param ctx Lowering context used to store generated IR and pass state.
 * @param root Root AST node of the semantically validated program.
 */
void do_generate_entry_function_pass(
    struct EmberLowerAstContext *ctx,
    struct EmberAstNode *root
);

/**
 * @brief Lowers the AST into assembler IR.
 *
 * This pass converts the semantically validated AST into the assembler's
 * intermediate representation.
 *
 * @param ctx Lowering context used to store generated IR and pass state.
 * @param root Root AST node of the semantically validated program.
 */
void do_lower_functions_pass(
    struct EmberLowerAstContext *ctx,
    struct EmberAstNode *root
);

#endif