#include "assembler/phases/parse/internal/models/context.h"

#include "assembler/internal/models/ast/nodes/program.h"

void ember_parser_context_init(
    struct EmberParserContext *self,
    const char *path,
    struct EmberLogger *logger
) {
    self->current_func_def = NULL;
    self->current_instruction = NULL;
    self->current_section = NULL;
    self->logger = logger;
    self->out_program = ember_ast_program_new();
    self->path = path;
}

void ember_parser_context_deinit(struct EmberParserContext *self) {
    ember_ast_program_destroy(self->out_program);
}