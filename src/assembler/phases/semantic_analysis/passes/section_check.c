#include "assembler/phases/semantic_analysis/internal/passes.h"

#include <string.h>

#include "assembler/internal/models/ast/visitor.h"
#include "assembler/internal/models/semantic/type.h"
#include "assembler/internal/models/semantic/special_names.h"

/**
 * Visitor context and helpers
 * ============================================================================
 */

struct SectionCheckContext {
    struct EmberSemanticContext *main_context;
    enum EmberSectionKind current_section;
};

static inline struct SectionCheckContext *
subcontext(struct EmberAstVisitor *visitor) {
    return visitor->context.data;
}

static inline struct EmberSemanticContext *
main_context(struct EmberAstVisitor *visitor) {
    return subcontext(visitor)->main_context;
}

static enum EmberSectionKind get_section_kind_for(const char *name) {
    if (strcmp(name, EMBER_CONSTANTS_SECTION_NAME) == 0) {
        return EMBER_SECTION_CONSTANTS;
    }
    if (strcmp(name, EMBER_GLOBALS_SECTION_NAME) == 0) {
        return EMBER_SECTION_GLOBALS;
    }
    if (strcmp(name, EMBER_CODE_SECTION_NAME) == 0) {
        return EMBER_SECTION_CODE;
    }
    return EMBER_SECTION_UNKNOWN;
}

static bool validate_known_section(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    if (node->resolved_kind != EMBER_SECTION_UNKNOWN) {
        return true;
    }

    EMBER_WARNING_AT(
        main_context(self)->logger,
        node->base.source_span,
        "section '%s' is not recognized and will be ignored",
        node->identifier.name
    );
    return false;
}

static bool register_section_name(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    const struct EmberSymbol *clash = NULL;

    if (ember_symbol_table_register(
            main_context(self)->symbols,
            EMBER_SYMBOL_SECTION,
            node->identifier.name,
            &node->base,
            &clash
        ) == 0) {
        return true;
    }

    EMBER_WARNING_AT(
        main_context(self)->logger,
        node->base.source_span,
        "duplicate section '%s' will be ignored",
        node->identifier.name
    );
    node->resolved_kind = EMBER_SECTION_UNKNOWN;
    return false;
}

/**
 * Visit functions
 * ============================================================================
 */

static void check_section_name_visit(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    node->resolved_kind = get_section_kind_for(node->identifier.name);

    if (!validate_known_section(self, node)) {
        return;
    }

    if (!register_section_name(self, node)) {
        return;
    }

    subcontext(self)->current_section = node->resolved_kind;

    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }

    subcontext(self)->current_section = EMBER_SECTION_UNKNOWN;
}

static void check_item_kind_visit(
    struct EmberAstVisitor *self,
    struct EmberAstSectionItem *node
) {
    enum EmberSectionKind current_section = subcontext(self)->current_section;

    if (current_section != EMBER_SECTION_CODE && node->kind == AST_SECTION_ITEM_FUNC_DEF) {
        EMBER_ERROR_AT(
            main_context(self)->logger,
            node->base.source_span,
            "functions can only appear in the code section"
        );
        ember_semantic_context_fatal(main_context(self));
        return;
    }

    if (current_section == EMBER_SECTION_CODE && node->kind != AST_SECTION_ITEM_FUNC_DEF) {
        EMBER_ERROR_AT(
            main_context(self)->logger,
            node->base.source_span,
            "assignments can only appear in the constants or globals section"
        );
        ember_semantic_context_fatal(main_context(self));
        return;
    }
}

/**
 * Main pass function and visitor setup
 * ============================================================================
 */

static const struct EmberAstVisitorVTable section_check_vtable = {
    .visit_ast_section      = check_section_name_visit,
    .visit_ast_section_item = check_item_kind_visit,
};

void do_section_check_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
) {
    if (ctx == NULL || root == NULL) {
        return;
    }

    struct SectionCheckContext subcontext = {
        .main_context = ctx,
        .current_section = EMBER_SECTION_UNKNOWN,
    };

    struct EmberAstVisitor visitor;
    ember_ast_visitor_init(
        &visitor,
        (struct EmberAstVisitorCtx) {
            .data = &subcontext,
        },
        &section_check_vtable
    );

    visitor.vtable.visit_ast_node(&visitor, root);
}