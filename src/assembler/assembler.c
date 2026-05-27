#include <ember/assembler.h>

#include "assembler/internal/phases.h"
#include "assembler/internal/common/logging.h"

struct EmberExecutable *ember_assembler_assemble(
    const char *path,
    struct EmberLogger *logger
) {
    ASSEMBLER_INFO(logger, "assembling '%s'", path);

    struct EmberAstNode *ast_root = NULL;
    struct EmberIrNode *ir_root = NULL;
    struct EmberExecutable *exe = NULL;

    if (ember_do_parse_phase(path, &ast_root, logger) != ASSEMBLER_PHASE_OK) {
        goto cleanup;
    }

    if (ember_do_semantic_analysis_phase(ast_root, logger) != ASSEMBLER_PHASE_OK) {
        goto cleanup;
    }

    if (ember_do_lower_ast_to_ir_phase(ast_root, &ir_root, logger) != ASSEMBLER_PHASE_OK) {
        goto cleanup;
    }

    if (ember_do_generate_executable_phase(ir_root, &exe, logger) != ASSEMBLER_PHASE_OK) {
        goto cleanup;
    }

cleanup:
    ember_ast_node_destroy(ast_root);
    ember_ir_node_destroy(ir_root);

    return exe;
}