#include "assembler/phases/generate_executable/internal/passes.h"

#include "assembler/internal/common/logging.h"
#include "assembler/internal/models/ir/visitor.h"
#include "assembler/phases/lower_ast_to_ir/internal/special_names.h"

/**
 * Context and helpers
 * ============================================================================
 */

static inline struct EmberGenerateExeContext *
subcontext(struct EmberIrVisitor *visitor) {
    return visitor->context.data;
}

/**
 * Visitor functions
 * ============================================================================
 */

static void register_function_visit(struct EmberIrVisitor *self, struct EmberIrFunction *node) {
    if (ember_index_table_register(
        subcontext(self)->indexes,
        node->name,
        INDEX_ELEMENT_FUNCTION,
        0
    ) == false) {
        EMBER_ERROR_AT(
            subcontext(self)->logger,
            node->base.source_span,
            "failed to register function '%s' during executable generation",
            node->name
        );
        ASSEMBLER_DEBUG(
            subcontext(self)->logger,
            "function '%s' could not be added to the executable index table; "
            "this indicates duplicate function that should have been rejected during semantic analysis",
            node->name
        );
        exe_gen_fatal(subcontext(self));
        return;
    }

    if (strcmp(node->name, EMBER_INIT_FUNCTION_NAME) != 0) {
        return;
    }

    for (size_t i = 0; i < node->code_item_count; i++) {
        if (exe_gen_should_stop(subcontext(self))) {
            return;
        }
        self->vtable.visit_ir_code_item(self, node->code_items[i]);
    }
}

static void register_global_visit(struct EmberIrVisitor *self, struct EmberIrRefOperand *node) {
    if (ember_index_table_register(
        subcontext(self)->indexes,
        node->name,
        INDEX_ELEMENT_GLOBAL,
        0
    ) == false) {
        EMBER_ERROR_AT(
            subcontext(self)->logger,
            node->base.base.source_span,
            "failed to register global '%s' during executable generation",
            node->name
        );
        ASSEMBLER_DEBUG(
            subcontext(self)->logger,
            "global '%s' could not be added to the executable index table; "
            "this indicates duplicate function that should have been rejected during semantic analysis",
            node->name
        );
        exe_gen_fatal(subcontext(self));
        return;
    }
}

/**
 * Main pass function
 * ============================================================================
 */

static const struct EmberIrVisitorVTable register_globals_vtable = {
    .visit_ir_function = register_function_visit,
    .visit_ir_global_ref_operand = register_global_visit,
};

void do_register_global_symbols_pass(
    struct EmberGenerateExeContext *ctx,
    struct EmberIrNode *root
) {
    struct EmberIrVisitor visitor;
    ember_ir_visitor_init(
        &visitor,
        (struct EmberIrVisitorCtx) { .data = ctx },
        &register_globals_vtable
    );
    visitor.vtable.visit_ir_node(&visitor, root);
}