#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_PASS_RESULT_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_PASS_RESULT_H

/**
 * @brief Status of semantic analysis.
 *
 * Describes whether semantic analysis is still valid, has reported
 * recoverable errors, or has encountered a fatal error that prevents further
 * meaningful analysis.
 */
enum EmberSemanticPassResult {
    /**
     * @brief No semantic errors have been reported.
     */
    SEMANTIC_PASS_OK,

    /**
     * @brief One or more non-fatal semantic errors have been reported.
     *
     * The input is invalid, but semantic analysis may still continue to report
     * additional diagnostics.
     */
    SEMANTIC_PASS_ERROR,

    /**
     * @brief A fatal semantic error has been reported.
     *
     * Further semantic analysis should stop because the AST or semantic state
     * may be unreliable.
     */
    SEMANTIC_PASS_FATAL,
};

#endif