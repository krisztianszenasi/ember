#ifndef ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_MODELS_CONTEXT_H
#define ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_MODELS_CONTEXT_H

#include <stdbool.h>

#include <ember/core/models/logger.h>

#include "assembler/internal/models/ir/nodes/program.h"
#include "assembler/phases/lower_ast_to_ir/internal/models/pass_status.h"

/**
 * @brief Context shared by AST-to-IR lowering passes.
 *
 * The lowering context stores the logger used for diagnostics, the IR program
 * produced by the lowering phase, and the current pass status. Visitor
 * functions use this context to emit diagnostics, append lowered IR nodes, and
 * stop lowering after fatal errors.
 */
struct EmberLowerAstContext {
    /**
     * @brief Logger used for lowering diagnostics and debug messages.
     */
    struct EmberLogger *logger;

    /**
     * @brief IR program produced by the lowering phase.
     */
    struct EmberIrProgram *out_program;

    /**
     * @brief Current status of the lowering phase.
     */
    enum EmberLowerAstPassStatus status;
};

/**
 * @brief Initializes an AST-to-IR lowering context.
 *
 * @param self Context to initialize.
 * @param logger Logger used for diagnostics and debug messages.
 */
void ember_lower_ast_context_init(
    struct EmberLowerAstContext *self,
    struct EmberLogger *logger,
    struct EmberSourceSpan source_span
);

/**
 * @brief Releases resources owned by an AST-to-IR lowering context.
 *
 * If the produced IR program is still owned by the context, this function
 * should release it. If ownership has been transferred to the caller, the
 * context should no longer destroy it.
 *
 * @param self Context to deinitialize.
 */
void ember_lower_ast_context_deinit(struct EmberLowerAstContext *self);

/**
 * @brief Returns whether AST lowering should stop after a fatal error.
 *
 * @param context Lowering context to inspect.
 * @return true if the lowering status is fatal, false otherwise.
 */
static inline bool ast_lowering_should_stop(
    const struct EmberLowerAstContext *context
) {
    return context->status == LOWER_AST_PASS_FATAL;
}

#endif
