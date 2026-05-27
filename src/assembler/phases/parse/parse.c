#include "assembler/internal/phases.h"

#include <stdio.h>

#include "assembler/internal/common/logging.h"
#include "assembler/phases/parse/internal/models/context.h"

extern FILE *yyin;

int yyparse(struct EmberParserContext *ctx);

enum EmberAssemblerPhaseResult ember_do_parse_phase(
    const char *path,
    struct EmberAstNode **out_root,
    struct EmberLogger *logger
) {
    FILE *file = NULL;
    struct EmberParserContext ctx = {0};

    if (out_root == NULL) {
        ASSEMBLER_ERROR(logger, "parser output pointer must not be null");
        return ASSEMBLER_PHASE_ERROR;
    }

    *out_root = NULL;

    ASSEMBLER_DEBUG(logger, "parsing '%s'", path);

    ember_parser_context_init(&ctx, path, logger);

    ASSEMBLER_INFO(logger, "opening source file '%s'", path);
    file = fopen(path, "r");
    if (file == NULL) {
        ASSEMBLER_ERROR(logger, "could not open '%s' for reading", path);
        goto error;
    }

    yyin = file;

    if (yyparse(&ctx) != 0) {
        ASSEMBLER_DEBUG(logger, "parsing '%s' failed", path);
        goto error;
    }

    if (ctx.out_program == NULL) {
        ASSEMBLER_ERROR(logger, "parser did not produce an AST");
        goto error;
    }

    *out_root = &ctx.out_program->base;

    fclose(file);
    yyin = NULL;

    ASSEMBLER_DEBUG(logger, "finished parsing '%s'", path);
    return ASSEMBLER_PHASE_OK;

error:
    if (file != NULL) {
        fclose(file);
    }

    yyin = NULL;
    ember_parser_context_deinit(&ctx);

    return ASSEMBLER_PHASE_ERROR;
}