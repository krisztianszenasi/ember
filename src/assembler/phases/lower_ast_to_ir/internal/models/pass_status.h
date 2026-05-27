#ifndef EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_MODELS_PASS_STATUS_H
#define EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_MODELS_PASS_STATUS_H

/**
 * @brief Status returned by an AST-to-IR lowering pass.
 */
enum EmberLowerAstPassStatus {
    /**
     * @brief The pass completed successfully.
     */
    LOWER_AST_PASS_OK,

    /**
     * @brief The pass encountered a recoverable error.
     */
    LOWER_AST_PASS_ERROR,

    /**
     * @brief The pass encountered an unrecoverable error.
     */
    LOWER_AST_PASS_FATAL,
};

#endif