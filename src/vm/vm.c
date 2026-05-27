#include <ember/vm.h>

#include <stdio.h>

#include <ember/core/memory.h>
#include "vm/internal/models/vm.h"

const char *ember_process_status_to_string(enum EmberProcessStatusCode status) {
    switch (status) {
        case EMBER_STATUS_OK:
            return "STATUS_OK";
        case EMBER_STATUS_INVALID_CAST:
            return "STATUS_INVALID_CAST";
        case EMBER_STATUS_DIVISION_BY_ZERO:
            return "STATUS_DIVISION_BY_ZERO";
        case EMBER_STATUS_INVALID_TYPE:
            return "STATUS_INVALID_TYPE";
        case EMBER_STATUS_IO_ERROR:
            return "STATUS_IO_ERROR";
        default:
            return "<unknown>";
    }
}

struct EmberVM *ember_vm_new(void) {
    struct EmberVM *self = ember_alloc(sizeof(struct EmberVM));
    self->process = NULL;
    return self;
}

void ember_vm_destroy(struct EmberVM *self) {
    if (self == NULL) {
        return;
    }
    ember_process_destroy(self->process);
    ember_free(self);
}

enum EmberVMRunResult ember_vm_run(
    struct EmberVM *self,
    struct EmberExecutable *executable,
    struct EmberLogger *logger
) {
    if (self == NULL || executable == NULL) {
        EMBER_ERROR(logger, "vm", "vm run requested with null vm or executable");
        return EMBER_VM_RUN_ERROR;
    }

    EMBER_INFO(logger, "vm", "starting vm");

    if (self->process != NULL) {
        EMBER_DEBUG(logger, "vm", "destroying previous process before spawning a new one");
        ember_process_destroy(self->process);
        self->process = NULL;
    }

    EMBER_INFO(logger, "vm", "spawning process");
    self->process = ember_process_new(executable);

    if (self->process == NULL) {
        EMBER_ERROR(logger, "vm", "failed to allocate process");
        return EMBER_VM_RUN_ERROR;
    }

    enum EmberProcessRunResult result = ember_process_run(self->process, logger);
    if (result == EMBER_PROCESS_RUN_ERROR) {
        EMBER_ERROR(
            logger,
            "vm",
            "process failed: %s",
            self->process->error.message != NULL
                ? self->process->error.message
                : "unknown runtime error"
        );
        return EMBER_VM_RUN_ERROR;
    }

    EMBER_INFO(logger, "vm", "vm run completed");
    return EMBER_VM_RUN_OK;
}
