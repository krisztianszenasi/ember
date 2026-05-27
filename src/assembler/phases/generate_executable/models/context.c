#include "assembler/phases/generate_executable/internal/models/context.h"

#include <ember/core/models/executable/types.h>


void ember_generate_exe_context_init(struct EmberGenerateExeContext *self, struct EmberLogger *logger) {
    self->indexes = ember_index_table_new();
    self->logger = logger;
    self->out_exe = ember_executable_new();
    self->status = EXE_GEN_PASS_OK;
}

void ember_generate_exe_context_deinit(struct EmberGenerateExeContext *self) {
    ember_index_table_destroy(self->indexes);
}