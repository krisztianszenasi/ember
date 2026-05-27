#include "assembler/phases/semantic_analysis/internal/passes.h"

#include "utils.h"
#include "assembler/internal/models/ast/visitor.h"


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

static void process_only_constants_section_visit(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    if (node->resolved_kind != EMBER_SECTION_CONSTANTS) {
        return;
    }
    
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }
}

static void register_constant_visitt(
    struct EmberAstVisitor *self,
    struct EmberAstAssignment *node
) {
    const struct EmberSymbol *clash = NULL;
    if (ember_symbol_table_register_constant(
            main_context(self)->symbols,
            node->identifier.name,
            node,
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

static const struct EmberAstVisitorVTable register_constants_vtable = {
    .visit_ast_section      = process_only_constants_section_visit,
    .visit_ast_assignment   = register_constant_visitt,
};

void do_register_constants_pass(
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
        &register_constants_vtable
    );

    visitor.vtable.visit_ast_node(&visitor, root);
}