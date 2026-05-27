#include "assembler/internal/phases.h"

#include "assembler/phases/generate_executable/internal/models/context.h"
#include "assembler/phases/generate_executable/internal/passes.h"

enum EmberAssemblerPhaseResult ember_do_generate_executable_phase(
    struct EmberIrNode *ir_root,
    struct EmberExecutable **out_exe,
    struct EmberLogger *logger
) {
    if (ir_root == NULL || out_exe == NULL) {
        return ASSEMBLER_PHASE_ERROR;
    }

    *out_exe = NULL;
    enum EmberAssemblerPhaseResult result = ASSEMBLER_PHASE_ERROR;

    struct EmberGenerateExeContext context;
    ember_generate_exe_context_init(&context, logger);

    do_register_global_symbols_pass(&context, ir_root);
    if (exe_gen_should_stop(&context)) {
        goto cleanup;
    }

    do_generate_functions_pass(&context, ir_root);
    if (exe_gen_should_stop(&context)) {
        goto cleanup;
    }

    if (context.status != EXE_GEN_PASS_OK || context.out_exe == NULL) {
        goto cleanup;
    }

    *out_exe = context.out_exe;
    context.out_exe = NULL;
    result = ASSEMBLER_PHASE_OK;

cleanup:
    if (result != ASSEMBLER_PHASE_OK) {
        ember_executable_free(context.out_exe);
        context.out_exe = NULL;
    }

    ember_generate_exe_context_deinit(&context);
    return result;
}