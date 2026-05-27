#ifndef EMBER_ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_PASS_STATUS_H
#define EMBER_ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_PASS_STATUS_H

/**
 * @brief Status values used by executable generation passes.
 *
 * The status tracks whether executable generation can continue, whether
 * recoverable errors were reported, or whether a fatal error requires the
 * phase to stop immediately.
 */
enum EmberGenerateExePassStatus {
    /**
     * @brief Executable generation has completed successfully so far.
     */
    EXE_GEN_PASS_OK,

    /**
     * @brief A recoverable executable generation error was reported.
     */
    EXE_GEN_PASS_ERROR,

    /**
     * @brief A fatal executable generation error was reported.
     */
    EXE_GEN_PASS_FATAL,
};

#endif