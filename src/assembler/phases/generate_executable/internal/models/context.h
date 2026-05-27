#ifndef ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_CONTEXT_H
#define ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_CONTEXT_H

#include <stdbool.h>

#include <ember/core/models/executable/types.h>
#include <ember/core/models/logger.h>

#include "assembler/phases/generate_executable/internal/models/index_table.h"
#include "assembler/phases/generate_executable/internal/models/pass_status.h"

/**
 * @brief Context shared by executable generation passes.
 *
 * The executable generation context stores the logger used for diagnostics,
 * the executable being built, the index table used for resolving generated
 * references, and the current pass status.
 */
struct EmberGenerateExeContext {
    /**
     * @brief Logger used for exe generation diagnostics and debug messages.
     */
    struct EmberLogger *logger;

    /**
     * @brief Executable being built by the generation phase.
     *
     * The context owns this value while executable generation is running. On a
     * successful phase result, ownership is transferred to the caller through
     * the phase output parameter.
     */
    struct EmberExecutable *out_exe;

    /**
     * @brief Index table used to map symbols to executable indexes.
     */
    struct EmberIndexTable *indexes;

    /**
     * @brief Current status of the executable generation phase.
     */
    enum EmberGenerateExePassStatus status;
};

/**
 * @brief Initializes an executable generation context.
 *
 * @param self Context to initialize.
 * @param logger Logger used for diagnostics and debug messages.
 */
void ember_generate_exe_context_init(
    struct EmberGenerateExeContext *self,
    struct EmberLogger *logger
);

/**
 * @brief Releases resources owned by an executable generation context.
 *
 * This function releases temporary context-owned resources such as the index
 * table. The generated executable is not destroyed here after ownership has
 * been transferred to the caller.
 *
 * @param self Context to deinitialize.
 */
void ember_generate_exe_context_deinit(struct EmberGenerateExeContext *self);

/**
 * @brief Returns whether executable generation should stop after a fatal error.
 *
 * @param context Executable generation context to inspect.
 * @return true if the pass status is fatal, false otherwise.
 */
static inline bool exe_gen_should_stop(struct EmberGenerateExeContext *context) {
    return context->status == EXE_GEN_PASS_FATAL;
}

static inline void exe_gen_fatal(struct EmberGenerateExeContext *context) {
    context->status = EXE_GEN_PASS_FATAL;
}

#endif