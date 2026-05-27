#include "assembler/phases/semantic_analysis/internal/passes.h"

#include "utils.h"
#include "assembler/internal/common/logging.h"
#include "assembler/internal/models/ast/visitor.h"
#include "assembler/internal/models/semantic/special_names.h"

/**
 * Visitor context and helpers
 * ============================================================================
 */

static inline struct EmberSemanticContext *
main_context(struct EmberAstVisitor *visitor) {
    return visitor->context.data;
}

/**
 * Visit functions
 * ============================================================================
 */

static void skip_unknown_sections_visit(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    if (node->resolved_kind == EMBER_SECTION_UNKNOWN) {
        return;
    }
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }
}

static void register_function_visit(
    struct EmberAstVisitor *self,
    struct EmberAstFuncDef *node
) {
    if (ember_is_main_function(node->identifier.name)) {
        main_context(self)->had_main = true;
    }    

    const struct EmberSymbol *clash = NULL;
    if (ember_symbol_table_register(
        main_context(self)->symbols,
        EMBER_SYMBOL_FUNCTION,
        node->identifier.name,
        &node->base.base,
        &clash
    ) != 0) {
        report_identifier_redefinition_error(
            main_context(self),
            &node->identifier,
            clash
        );
        return;
    }
}

/**
 * Main pass function and visitor setup
 * ============================================================================
 */

static const struct EmberAstVisitorVTable register_functions_vtable = {
    .visit_ast_section  = skip_unknown_sections_visit,
    .visit_ast_func_def = register_function_visit,
};

void do_register_functions_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
) {
    if (ctx == NULL || root == NULL) {
        return;
    }

    struct EmberAstVisitor visitor;
    ember_ast_visitor_init(
        &visitor,
        (struct EmberAstVisitorCtx) {
            .data = ctx,
        },
        &register_functions_vtable
    );

    visitor.vtable.visit_ast_node(&visitor, root);

    if(ctx->had_main == false) {
        ASSEMBLER_ERROR(ctx->logger, "%s must have a main function", root->source_span.path);
        ctx->status = SEMANTIC_PASS_ERROR;
    }
}