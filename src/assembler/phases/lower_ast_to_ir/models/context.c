#include "assembler/phases/lower_ast_to_ir/internal/models/context.h"

#include "assembler/internal/models/ir/nodes/program.h"

void ember_lower_ast_context_init(
    struct EmberLowerAstContext *self,
    struct EmberLogger *logger,
    struct EmberSourceSpan source_span
) {
    self->logger = logger;
    self->out_program = ember_ir_program_new(source_span);
    self->status = LOWER_AST_PASS_OK;
}

void ember_lower_ast_context_deinit(struct EmberLowerAstContext *self) {
    ember_ir_program_destroy(self->out_program);
}
