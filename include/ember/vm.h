#ifndef EMBER_VM_H
#define EMBER_VM_H

#include <ember/core/models/logger.h>
#include <ember/core/models/executable/all.h>

/**
 * @brief Result of running the Ember virtual machine.
 *
 * Indicates whether VM execution completed successfully or stopped because of
 * a runtime error.
 */
enum EmberVMRunResult {
    /**
     * @brief VM execution completed successfully.
     */
    EMBER_VM_RUN_OK = 0,

    /**
     * @brief VM execution stopped because of a runtime error.
     */
    EMBER_VM_RUN_ERROR = 1,
};

/**
 * Opaque virtual machine object.
 *
 * An EmberVM represents a virtual machine instance capable of executing
 * Ember executables.
 */
struct EmberVM;

/**
 * Creates a new virtual machine instance.
 *
 * @return A newly created VM, or NULL on failure.
 */
struct EmberVM *ember_vm_new(void);


void ember_vm_destroy(struct EmberVM *self);

/**
 * Executes the given executable on the virtual machine.
 *
 * @param self The virtual machine instance.
 * @param executable The executable to run.
 * @param logger for diagnostics.
 */
enum EmberVMRunResult ember_vm_run(
    struct EmberVM *self,
    struct EmberExecutable *executable,
    struct EmberLogger *logger
);

/**
 * @brief Status code set by the last recoverable VM operation.
 *
 * Status codes are visible to bytecode through status-related instructions.
 * They are used for recoverable failures that the running program may inspect
 * and handle, without putting the process into a fatal error state.
 */
enum EmberProcessStatusCode {
    /**
     * @brief The last recoverable operation completed successfully.
     */
    EMBER_STATUS_OK = 0,

    /**
     * @brief A value could not be converted to the requested type.
     */
    EMBER_STATUS_INVALID_CAST,

    /**
     * @brief A division operation attempted to divide by zero.
     */
    EMBER_STATUS_DIVISION_BY_ZERO,

    /**
    * @brief An operation received a value of an unsupported type.
    *
    * This is used when an instruction expects operands of a specific type,
    * such as integers for integer arithmetic, but one or more operands have a
    * different value type.
    */
    EMBER_STATUS_INVALID_TYPE,

    /**
     * @brief IO related operations failed.
     */
    EMBER_STATUS_IO_ERROR,

    EMBER_STATUS_COUNT,
};

/**
 * @brief Returns the symbolic name of a process status code.
 *
 * The returned string is intended for diagnostics, debugging, and generated
 * built-in status constants. The returned pointer must not be freed by the
 * caller.
 *
 * @param status Process status code to convert.
 * @return String representation of the status code.
 */
const char *ember_process_status_to_string(enum EmberProcessStatusCode status);

#endif
