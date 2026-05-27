#ifndef EMBER_VM_INTERNAL_MODELS_PROCESS_RESULT_H
#define EMBER_VM_INTERNAL_MODELS_PROCESS_RESULT_H

/**
 * @brief Result returned after running a VM process.
 *
 * This describes why process execution stopped.
 */
enum EmberProcessRunResult {
    /**
     * @brief The process stopped normally.
     */
    EMBER_PROCESS_RUN_HALTED,

    /**
     * @brief The process stopped because of a fatal runtime error.
     */
    EMBER_PROCESS_RUN_ERROR,
};

#endif