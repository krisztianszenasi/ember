#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_CONTEXT_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_CONTEXT_H

#include <stdbool.h>

#include <ember/core/models/logger.h>

#include "assembler/phases/semantic_analysis/internal/models/pass_result.h"
#include "assembler/phases/semantic_analysis/internal/models/symbol_table.h"

/**
 * @brief Shared state used during semantic analysis.
 *
 * The semantic context stores state that is shared across semantic-analysis
 * passes, including the logger, symbol table, main-function tracking, and the
 * current semantic-analysis status.
 */
struct EmberSemanticContext {
    /**
     * @brief Logger used to report semantic diagnostics.
     */
    struct EmberLogger *logger;

    /**
     * @brief Symbol table used to register and resolve semantic symbols.
     */
    struct EmberSymbolTable *symbols;

    /**
     * @brief Whether a valid main function has been found.
     */
    bool had_main;

    /**
     * @brief Most severe semantic-analysis status reported so far.
     */
    enum EmberSemanticPassResult status;
};

/**
 * @brief Initializes a semantic-analysis context.
 *
 * Sets up the context for a new semantic-analysis run, including diagnostic
 * state and symbol-table storage.
 *
 * @param self Semantic context to initialize.
 * @param logger Logger used to report semantic diagnostics.
 */
void ember_semantic_context_init(
    struct EmberSemanticContext *self,
    struct EmberLogger *logger
);

/**
 * @brief Releases resources owned by a semantic-analysis context.
 *
 * This function frees or clears any resources initialized by
 * `ember_semantic_context_init`.
 *
 * @param self Semantic context to deinitialize.
 */
void ember_semantic_context_deinit(struct EmberSemanticContext *self);

/**
 * @brief Returns whether semantic analysis should stop immediately.
 *
 * Semantic analysis should stop when a fatal semantic error has been reported,
 * because later passes may no longer be meaningful or safe to run.
 *
 * @param context Semantic context to inspect.
 * @return `true` if a fatal semantic error has occurred, otherwise `false`.
 */
static inline bool semantic_should_stop(
    const struct EmberSemanticContext *context
) {
    return context->status == SEMANTIC_PASS_FATAL;
}

static inline void ember_semantic_context_fatal(
    struct EmberSemanticContext *context
) {
    context->status = SEMANTIC_PASS_FATAL;
}

static inline void ember_semantic_context_error(
        struct EmberSemanticContext *context
) {
    context->status = SEMANTIC_PASS_ERROR;
}

#endif