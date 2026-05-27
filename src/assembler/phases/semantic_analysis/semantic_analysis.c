#include "assembler/internal/phases.h"

#include "passes/utils.h"
#include "assembler/phases/semantic_analysis/internal/passes.h"
#include "assembler/phases/semantic_analysis/internal/models/context.h"

enum EmberAssemblerPhaseResult ember_do_semantic_analysis_phase(
    struct EmberAstNode *ast_root,
    struct EmberLogger *logger
) {
    struct EmberSemanticContext context;
    enum EmberAssemblerPhaseResult result = ASSEMBLER_PHASE_OK;
    ember_semantic_context_init(&context, logger);
    
    do_section_check_pass(&context, ast_root);
    if (semantic_should_stop(&context)) {
        result = ASSEMBLER_PHASE_ERROR;
        goto cleanup;
    }

    do_register_constants_pass(&context, ast_root);
    if (semantic_should_stop(&context)) {
        result = ASSEMBLER_PHASE_ERROR;
        goto cleanup;
    }

    do_register_globals_pass(&context, ast_root);
    if (semantic_should_stop(&context)) {
        result = ASSEMBLER_PHASE_ERROR;
        goto cleanup;
    }

    do_register_functions_pass(&context, ast_root);
    if (semantic_should_stop(&context)) {
        result = ASSEMBLER_PHASE_ERROR;
        goto cleanup;
    }

    do_function_check_pass(&context, ast_root);
    if (semantic_should_stop(&context)) {
        result = ASSEMBLER_PHASE_ERROR;
        goto cleanup;
    }

    report_unused_symbol_warnings(&context);

    if (context.status != SEMANTIC_PASS_OK) {
        result = ASSEMBLER_PHASE_ERROR;
    }

cleanup:
    ember_semantic_context_deinit(&context);
    return result;
}