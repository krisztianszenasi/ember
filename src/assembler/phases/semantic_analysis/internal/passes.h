#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_PASSES_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_PASSES_H

#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/phases/semantic_analysis/internal/models/context.h"

/**
 * @brief Checks that all sections are valid and well-formed.
 *
 * This pass verifies that each section has a supported name and that every
 * item inside a section is valid for that section.
 *
 * @param ctx Semantic-analysis context updated by the pass.
 * @param root Root node of the AST to check.
 */
void do_section_check_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
);

/**
 * @brief Registers constants declared in the constants section.
 *
 * This pass visits the constants section and registers each constant symbol in
 * the semantic context's symbol table. If a constant name is already defined,
 * the pass reports a redefinition error and updates the semantic context.
 *
 * @param ctx Semantic-analysis context updated by the pass.
 * @param root Root node of the AST to process.
 */
void do_register_constants_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
);

/**
 * @brief Registers globals declared in the globals section.
 *
 * This pass visits the globals section and registers each global symbol in the
 * semantic context's symbol table. If a global name is already defined, the
 * pass reports a redefinition error and updates the semantic context.
 *
 * @param ctx Semantic-analysis context updated by the pass.
 * @param root Root node of the AST to process.
 */
void do_register_globals_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
);

/**
 * @brief Registers function declarations in the symbol table.
 *
 * This pass visits each function definition and registers its identifier as a
 * function symbol so call instructions can resolve function references. If a
 * function name is already defined, the pass reports a redefinition error and
 * updates the semantic context.
 *
 * @param ctx Semantic-analysis context updated by the pass.
 * @param root Root node of the AST to process.
 */
void do_register_functions_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
);

/**
 * @brief Checks the semantic correctness of function bodies.
 *
 * This pass validates function-local labels and instructions. Labels are
 * registered before instruction validation so jumps may reference labels
 * declared later in the same function.
 *
 * @param ctx Semantic-analysis context updated by the pass.
 * @param root Root node of the AST to process.
 */
void do_function_check_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
);

#endif