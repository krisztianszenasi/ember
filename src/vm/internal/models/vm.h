#ifndef EMBER_VM_INTERNAL_MODELS_VM_H
#define EMBER_VM_INTERNAL_MODELS_VM_H

#include "vm/internal/models/process/base.h"

/**
 * @brief Internal VM state.
 *
 * The VM owns the currently running process. For now, the VM supports a single
 * process at a time. Later, this can be extended to store multiple processes
 * and a scheduler.
 */
struct EmberVM {
    /**
     * @brief Current process owned by the VM.
     *
     * This is `NULL` when no process has been created or after the process has
     * been destroyed.
     */
    struct EmberProcess *process;
};

#endif