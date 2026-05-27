#include "assembler/phases/lower_ast_to_ir/internal/passes.h"

#include <stdbool.h>

#include "assembler/internal/models/ast/visitor.h"
#include "assembler/internal/models/semantic/special_names.h"
#include "assembler/phases/lower_ast_to_ir/internal/special_names.h"

#include "utils.h"

/**
 * Visitor context and helpers
 * ============================================================================
 */

struct GenerateEntryContext {
    bool generated_global_initializer;
    struct EmberLowerAstContext *main_context;
    struct EmberIrFunction *global_initializer;
};

static inline struct GenerateEntryContext *
subcontext(struct EmberAstVisitor *visitor) {
    return visitor->context.data;
}

static inline struct EmberLowerAstContext *
main_context(struct EmberAstVisitor *visitor) {
    return subcontext(visitor)->main_context;
}

/**
 * IR emission helpers
 * ============================================================================
 */

static void emit_global_initializer_return(struct EmberAstVisitor *self) {
    make_ret_instruction(
        subcontext(self)->global_initializer,
        EMBER_SOURCE_SPAN_NONE
    );
}

static void emit_global_initializer_assignment(
    struct EmberAstVisitor *self,
    struct EmberAstAssignment *node
) {
    struct EmberIrFunction *global_initializer = subcontext(self)->global_initializer;

    make_ldc_instruction(global_initializer, &node->lieral);
    make_setg_instruction(global_initializer, &node->identifier);
}

static void create_global_initializer_for_section(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    struct GenerateEntryContext *context = subcontext(self);

    context->generated_global_initializer = true;
    context->global_initializer = create_init_function(
        main_context(self)->out_program,
        node->base.source_span
    );

    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }

    emit_global_initializer_return(self);
}

/**
 * Visit functions
 * ============================================================================
 */

static void visit_section_for_global_initializer(
    struct EmberAstVisitor *self,
    struct EmberAstSection *node
) {
    if (node->resolved_kind != EMBER_SECTION_GLOBALS) {
        return;
    }

    create_global_initializer_for_section(self, node);
}

static void visit_global_assignment_for_initializer(
    struct EmberAstVisitor *self,
    struct EmberAstAssignment *node
) {
    emit_global_initializer_assignment(self, node);
}

/**
 * Custom processing with visitor and setup
 * ============================================================================
 */

static const struct EmberAstVisitorVTable generate_entry_function_vtable = {
    .visit_ast_section = visit_section_for_global_initializer,
    .visit_ast_assignment = visit_global_assignment_for_initializer,
};

static bool generate_global_initializer_function(
    struct EmberLowerAstContext *ctx,
    struct EmberAstNode *root
) {
    struct GenerateEntryContext subcontext = {
        .generated_global_initializer = false,
        .main_context = ctx,
        .global_initializer = NULL,
    };

    struct EmberAstVisitor visitor;
    ember_ast_visitor_init(
        &visitor,
        (struct EmberAstVisitorCtx) {
            .data = &subcontext,
        },
        &generate_entry_function_vtable
    );

    visitor.vtable.visit_ast_node(&visitor, root);
    return subcontext.generated_global_initializer;
}

/**
 * Main pass function
 * ============================================================================
 */

void do_generate_entry_function_pass(
    struct EmberLowerAstContext *ctx,
    struct EmberAstNode *root
) {
    struct EmberIrFunction *entry_function = create_entry_function(
        ctx->out_program,
        EMBER_SOURCE_SPAN_NONE
    );

    bool had_global_initializer = generate_global_initializer_function(ctx, root);
    if (had_global_initializer) {
        make_call_instruction(
            entry_function,
            EMBER_INIT_FUNCTION_NAME,
            EMBER_SOURCE_SPAN_NONE
        );
    }

    make_call_instruction(
        entry_function,
        EMBER_MAIN_FUNCTION_NAME,
        EMBER_SOURCE_SPAN_NONE
    );
    make_halt_instruction(entry_function, EMBER_SOURCE_SPAN_NONE);
}
