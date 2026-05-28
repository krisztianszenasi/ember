#include "assembler/phases/generate_executable/internal/passes.h"

#include <stdint.h>

#include <ember/core/models/executable/builder.h>
#include <ember/core/models/object/str.h>
#include <ember/core/opcode.h>

#include "assembler/internal/common/logging.h"
#include "assembler/internal/models/ir/visitor.h"
#include "assembler/phases/generate_executable/internal/models/patch_table.h"

#include "utils.h"

/**
 * Context and helpers
 * ============================================================================
 */

struct GenerateFunctionsContext {
    struct EmberGenerateExeContext *main_context;
    struct EmberPatchTable *patch_table;
    struct EmberFunction *current_function;
};

static inline struct GenerateFunctionsContext *
subcontext(struct EmberIrVisitor *visitor) {
    return visitor->context.data;
}

static inline struct EmberGenerateExeContext *
main_context(struct EmberIrVisitor *visitor) {
    return subcontext(visitor)->main_context;
}

static inline struct EmberFunction *
current_function(struct EmberIrVisitor *visitor) {
    return subcontext(visitor)->current_function;
}

static void report_exe_gen_error(
    struct EmberIrVisitor *self,
    struct EmberSourceSpan span,
    const char *message
) {
    EMBER_ERROR_AT(main_context(self)->logger, span, "%s", message);
    exe_gen_fatal(main_context(self));
}

static void enter_function_scope(struct GenerateFunctionsContext *context) {
    context->main_context->indexes = ember_index_table_enter_scope(context->main_context->indexes);
}

static void exit_function_scope(struct GenerateFunctionsContext *context) {
    context->main_context->indexes = ember_index_table_exit_scope(context->main_context->indexes);
}

static bool ensure_u8_index(
    struct EmberIrVisitor *self,
    struct EmberSourceSpan span,
    size_t index,
    const char *what
) {
    if (index <= UINT8_MAX) {
        return true;
    }

    EMBER_ERROR_AT(
        main_context(self)->logger,
        span,
        "%s index %zu does not fit into one byte",
        what,
        index
    );
    exe_gen_fatal(main_context(self));
    return false;
}

static bool ensure_u16_index(
    struct EmberIrVisitor *self,
    struct EmberSourceSpan span,
    size_t index,
    const char *what
) {
    if (index <= UINT16_MAX) {
        return true;
    }

    EMBER_ERROR_AT(
        main_context(self)->logger,
        span,
        "%s index %zu does not fit into two bytes",
        what,
        index
    );
    exe_gen_fatal(main_context(self));
    return false;
}

static bool lookup_index_or_fatal(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node,
    const char *kind,
    size_t *out_index
) {
    if (ember_index_table_lookup(main_context(self)->indexes, node->name, out_index)) {
        return true;
    }

    EMBER_ERROR_AT(
        main_context(self)->logger,
        node->base.base.source_span,
        "unknown %s reference '%s' during executable generation",
        kind,
        node->name
    );
    ASSEMBLER_DEBUG(
        main_context(self)->logger,
        "failed to resolve %s reference '%s' in the executable index table; "
        "semantic analysis should have rejected this earlier",
        kind,
        node->name
    );
    exe_gen_fatal(main_context(self));
    return false;
}

static struct EmberValue lower_value(struct EmberIrValue *value) {
    switch (value->kind) {
        case IR_VALUE_BOOLEAN:
            return EV_BOOLEAN(value->as.boolean_val);
        case IR_VALUE_FLOAT:
            return EV_FLOAT(value->as.float_val);
        case IR_VALUE_INT:
            return EV_INT(value->as.int_val);
        case IR_VALUE_NONE:
            return EV_NONE();
        case IR_VALUE_STR:
            return EO_STR(value->as.str_val);
    }

    return EV_NONE();
}

/**
 * Function generation
 * ============================================================================
 */

static void generate_function_visit(
    struct EmberIrVisitor *self,
    struct EmberIrFunction *node
) {
    struct GenerateFunctionsContext *context = subcontext(self);

    context->current_function = ember_executable_define_function(
        context->main_context->out_exe,
        node->name,
        node->arity,
        node->local_count
    );

    if (context->current_function == NULL) {
        EMBER_ERROR_AT(
            main_context(self)->logger,
            node->base.source_span,
            "failed to create executable function '%s'",
            node->name
        );
        exe_gen_fatal(main_context(self));
        return;
    }

    enter_function_scope(context);

    for (size_t i = 0; i < node->code_item_count; i++) {
        if (exe_gen_should_stop(main_context(self))) {
            break;
        }

        self->vtable.visit_ir_code_item(self, node->code_items[i]);
    }

    exit_function_scope(context);
    context->current_function = NULL;
}

/**
 * Label generation
 * ============================================================================
 */

static void patch_forward_label_references(
    struct EmberIrVisitor *self,
    const char *name,
    size_t address
) {
    struct EmberPatchElement *patch_element = NULL;

    if (!ember_patch_table_lookup(subcontext(self)->patch_table, name, &patch_element)) {
        return;
    }

    for (size_t i = 0; i < patch_element->count; i++) {
        ember_function_patch_forward_label(
            current_function(self),
            patch_element->patch_locations[i],
            address
        );
    }
}

static void generate_label_def_visit(
    struct EmberIrVisitor *self,
    struct EmberIrLabelDef *node
) {
    size_t address = ember_function_make_label(current_function(self));

    if (!ember_index_table_register(
            main_context(self)->indexes,
            node->name,
            INDEX_ELEMENT_LABEL,
            address
        )) {
        EMBER_ERROR_AT(
            main_context(self)->logger,
            node->base.base.source_span,
            "failed to register label '%s' during executable generation",
            node->name
        );
        ASSEMBLER_DEBUG(
            main_context(self)->logger,
            "label '%s' could not be added to the executable index table; "
            "this usually indicates a duplicate label that should have been rejected during semantic analysis",
            node->name
        );
        exe_gen_fatal(main_context(self));
        return;
    }

    patch_forward_label_references(self, node->name, address);
}

/**
 * Instruction generation
 * ============================================================================
 */

static void generate_instruction_visit(
    struct EmberIrVisitor *self,
    struct EmberIrInstruction *node
) {
    ember_function_write_u8(
        current_function(self),
        convert_assembly_opcode_to_bytecode_opcode(node->opcode)
    );

    for (size_t i = 0; i < node->operand_count; i++) {
        if (exe_gen_should_stop(main_context(self))) {
            return;
        }

        self->vtable.visit_ir_operand(self, node->operands[i]);
    }
}

/**
 * Operand generation
 * ============================================================================
 */

static void generate_immediate_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrImmediateOperand *node
) {
    if (!ensure_u8_index(
            self,
            node->base.base.source_span,
            node->value,
            "immediate operand"
        )) {
        return;
    }

    ember_function_write_u8(current_function(self), (uint8_t)node->value);
}

static void generate_constant_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrConstantOperand *node
) {
    struct EmberFunction *function = current_function(self);
    size_t index = ember_function_register_constant(function, lower_value(&node->value));

    if (index <= UINT8_MAX) {
        ember_function_write_u8(function, (uint8_t)index);
        return;
    }

    if (index <= UINT16_MAX) {
        /*
         * The instruction opcode has already been emitted. If the constant does
         * not fit into one byte, rewrite that opcode to the wide LDC variant
         * and emit the two-byte constant index.
         */
        ember_function_patch_u8(function, 0, OP_LDC_W);
        ember_function_write_u16(function, (uint16_t)index);
        return;
    }

    EMBER_ERROR_AT(
        main_context(self)->logger,
        node->base.base.source_span,
        "constant pool index %zu does not fit into two bytes",
        index
    );
    exe_gen_fatal(main_context(self));
}

static void generate_u8_index_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrIndexOperand *node
) {
    if (!ensure_u8_index(
            self,
            node->base.base.source_span,
            node->index,
            "operand"
        )) {
        return;
    }

    ember_function_write_u8(current_function(self), (uint8_t)node->index);
}

static void generate_function_ref_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    size_t function_index = 0;

    if (!lookup_index_or_fatal(self, node, "function", &function_index)) {
        return;
    }

    if (!ensure_u16_index(
            self,
            node->base.base.source_span,
            function_index,
            "function"
        )) {
        return;
    }

    ember_function_write_u16(current_function(self), (uint16_t)function_index);
}

static void generate_global_ref_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    size_t global_index = 0;

    if (!lookup_index_or_fatal(self, node, "global", &global_index)) {
        return;
    }

    if (!ensure_u16_index(
            self,
            node->base.base.source_span,
            global_index,
            "global"
        )) {
        return;
    }

    ember_function_write_u16(current_function(self), (uint16_t)global_index);
}

static void generate_label_ref_operand_visit(
    struct EmberIrVisitor *self,
    struct EmberIrRefOperand *node
) {
    size_t label_address = 0;

    if (ember_index_table_lookup(main_context(self)->indexes, node->name, &label_address)) {
        ember_function_emit_backward_label(current_function(self), label_address);
        return;
    }

    size_t patch_location = ember_function_emit_forward_label(current_function(self));

    if (!ember_patch_table_register(
            subcontext(self)->patch_table,
            node->name,
            patch_location
        )) {
        EMBER_ERROR_AT(
            main_context(self)->logger,
            node->base.base.source_span,
            "failed to register forward label reference '%s'",
            node->name
        );
        exe_gen_fatal(main_context(self));
    }
}

/**
 * Visitor setup and main function
 * ============================================================================
 */

static const struct EmberIrVisitorVTable generate_functions_vtable = {
    .visit_ir_function = generate_function_visit,
    .visit_ir_label_def = generate_label_def_visit,
    .visit_ir_instruction = generate_instruction_visit,
    .visit_ir_immediate_operand = generate_immediate_operand_visit,
    .visit_ir_constant_operand = generate_constant_operand_visit,
    .visit_ir_arg_index_operand = generate_u8_index_operand_visit,
    .visit_ir_local_index_operand = generate_u8_index_operand_visit,
    .visit_ir_func_ref_operand = generate_function_ref_operand_visit,
    .visit_ir_global_ref_operand = generate_global_ref_operand_visit,
    .visit_ir_label_ref_operand = generate_label_ref_operand_visit,
};

void do_generate_functions_pass(
    struct EmberGenerateExeContext *ctx,
    struct EmberIrNode *root
) {
    if (ctx == NULL || root == NULL) {
        return;
    }

    struct GenerateFunctionsContext subcontext = {
        .main_context = ctx,
        .patch_table = ember_patch_table_new(),
        .current_function = NULL,
    };

    if (subcontext.patch_table == NULL) {
        EMBER_ERROR_AT(
            ctx->logger,
            EMBER_SOURCE_SPAN_NONE,
            "failed to allocate patch table during executable generation"
        );
        exe_gen_fatal(ctx);
        return;
    }

    struct EmberIrVisitor visitor;
    ember_ir_visitor_init(
        &visitor,
        (struct EmberIrVisitorCtx) {
            .data = &subcontext,
        },
        &generate_functions_vtable
    );

    visitor.vtable.visit_ir_node(&visitor, root);

    ember_patch_table_destroy(subcontext.patch_table);
}
