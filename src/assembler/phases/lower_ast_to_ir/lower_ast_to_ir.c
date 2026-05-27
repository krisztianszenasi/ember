#include "assembler/internal/phases.h"

#include "assembler/phases/lower_ast_to_ir/internal/passes.h"
#include "assembler/phases/lower_ast_to_ir/internal/models/context.h"

enum EmberAssemblerPhaseResult ember_do_lower_ast_to_ir_phase(
    struct EmberAstNode *ast_root,
    struct EmberIrNode **out_ir,
    struct EmberLogger *logger
) {
    struct EmberLowerAstContext context;
    ember_lower_ast_context_init(&context, logger, ast_root->source_span);

    do_generate_entry_function_pass(&context, ast_root);
    if (ast_lowering_should_stop(&context)) {
        goto failed;
    }

    do_lower_functions_pass(&context, ast_root);
    if (ast_lowering_should_stop(&context)) {
        goto failed;
    }

    if (context.status == LOWER_AST_PASS_OK) {
        *out_ir = &context.out_program->base;
        return ASSEMBLER_PHASE_OK;
    }

failed:
    ember_lower_ast_context_deinit(&context);
    return ASSEMBLER_PHASE_ERROR;
}
