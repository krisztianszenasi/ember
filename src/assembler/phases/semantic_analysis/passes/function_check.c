#include "assembler/phases/semantic_analysis/internal/passes.h"

#include <stdbool.h>
#include <ember/assembly/instruction_info.h>

#include "utils.h"
#include "operand_validator.h"
#include "assembler/internal/models/ast/visitor.h"
#include "assembler/internal/models/semantic/type.h"
#include "assembler/internal/models/assembly/instruction_def.h"

/**
 * Visitor context and helpers
 * ============================================================================
 */

struct CheckFunctionContext {
    struct EmberSemanticContext *main_context;
    struct EmberAstFuncDef *current_func;
    bool is_label_phase;
};

static inline struct CheckFunctionContext *
subcontext(struct EmberAstVisitor *visitor) {
    return visitor->context.data;
}

static inline void
enter_scope(struct EmberAstVisitor *visitor, struct EmberAstFuncDef *func) {
    subcontext(visitor)->current_func = func;
    subcontext(visitor)->main_context->symbols = ember_symbol_table_enter_scope(
        subcontext(visitor)->main_context->symbols
    );
}

static inline void
exit_scope(struct EmberAstVisitor *visitor) {
    subcontext(visitor)->main_context->symbols = ember_symbol_table_exit_scope(
        subcontext(visitor)->main_context->symbols
    );
    subcontext(visitor)->current_func = NULL;
}

/**
 * Visitor functions
 * ============================================================================
 */


/**
 * Visit EmberAstSection
 * --------------------------------------------------------
 */

static void skip_unknown_sections_visit(struct EmberAstVisitor *self, struct EmberAstSection *node) {
    if (node->resolved_kind == EMBER_SECTION_UNKNOWN) {
        return;
    }
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }
}

/**
 * Visit EmberAstFuncDef
 * --------------------------------------------------------
 */

static void check_function_visit(struct EmberAstVisitor *self, struct EmberAstFuncDef *node) {
    enter_scope(self, node);

    subcontext(self)->is_label_phase = true;
    for (size_t i = 0; i < node->item_count; i++) {
        self->vtable.visit_ast_code_item(self, node->items[i]);
    }

    subcontext(self)->is_label_phase = false;
    for (size_t i = 0; i < node->item_count; i++) {
        self->vtable.visit_ast_code_item(self, node->items[i]);
    }
    report_unused_symbol_warnings(subcontext(self)->main_context);
    exit_scope(self);
}

/**
 * Visit EmberAstLabelDef
 * --------------------------------------------------------
 */

static void register_label_visit(struct EmberAstVisitor *self, struct EmberAstLabelDef *node) {
    if (subcontext(self)->is_label_phase == false) {
        return;
    }

    const struct EmberSymbol *clash = NULL;
    if (ember_symbol_table_register(
        subcontext(self)->main_context->symbols,
        EMBER_SYMBOL_LABEL,
        node->identifier.name,
        &node->base.base,
        &clash
    ) != 0) {
        report_identifier_redefinition_error(
            subcontext(self)->main_context,
            &node->identifier,
            clash
        );
        return;
    }
}

/**
 * Visit EmberAstInstruction
 * --------------------------------------------------------
 */

static void validate_operand(
    struct EmberAstInstruction *instruction,
    size_t operand_idx,
    const struct EmberAssemblyInstructionInfo *instruction_info,
    struct CheckFunctionContext *subcontext
) {
    struct EmberAstOperand *actual = &instruction->operands[operand_idx];
    const struct EmberAssemblyOperandInfo *expected = &instruction_info->operands[operand_idx];

    if (!ember_resolved_operand_matches_info(expected, &actual->resolved)) {
        EMBER_ERROR_AT(
            subcontext->main_context->logger,
            instruction->base.base.source_span,
            "invalid operand %zu for instruction '%s'",
            operand_idx + 1,
            instruction_info->name
        );
        ember_semantic_context_error(subcontext->main_context);
        return;
    }

    if (!run_operand_validator(
            subcontext->main_context->logger,
            subcontext->current_func,
            expected, actual
        )) {
        ember_semantic_context_error(subcontext->main_context);
        return;
    }
}

static bool validate_operand_count(
    struct EmberAstInstruction *instruction,
    const struct EmberAssemblyInstructionInfo *instruction_info,
    struct EmberSemanticContext *context
) {
    if (instruction->operand_count != instruction_info->operand_count) {
        EMBER_ERROR_AT(
            context->logger,
            instruction->base.base.source_span,
            "instruction '%s' expects %zu operand%s, but got %zu",
            instruction_info->name,
            instruction_info->operand_count,
            instruction_info->operand_count == 1 ? "" : "s",
            instruction->operand_count
        );
        ember_semantic_context_error(context);
        return false;
    }
    return true;
}

static bool handle_unknown_instruction(
    struct EmberAstInstruction *instruction,
    const struct EmberAssemblyInstructionInfo *instruction_info,
    struct EmberSemanticContext *context
) {
    if (instruction_info == NULL) {
        EMBER_ERROR_AT(
            context->logger,
            instruction->base.base.source_span,
            "unknown instruction opcode"
        );
        ember_semantic_context_error(context);
        return false;
    }
    return true;
}

static void check_instruction_visit(
    struct EmberAstVisitor *self,
    struct EmberAstInstruction *node
) {
    struct CheckFunctionContext *func_ctx = subcontext(self);
    struct EmberSemanticContext *semantic_ctx = func_ctx->main_context;

    if (func_ctx->is_label_phase) {
        return;
    }

    const struct EmberAssemblyInstructionInfo *inst_info =
        ember_assembly_instruction_info_by_opcode(node->opcode);

    if (!handle_unknown_instruction(node, inst_info, semantic_ctx)) {
        return;
    }

    if (!validate_operand_count(node, inst_info, semantic_ctx)) {
        return;
    }

    for (size_t i = 0; i < node->operand_count; i++) {
        self->vtable.visit_ast_operand(self, &node->operands[i]);
        validate_operand(node, i, inst_info, func_ctx);
    }
}

/**
 * Visit EmberAstOperand
 * --------------------------------------------------------
 */

static enum EmberAssemblyValueKind value_kind_from_literal(
    const struct EmberAstLiteral *literal
) {
    switch (literal->kind) {
        case AST_LITERAL_BOOLEAN:
            return EMBER_VALUE_BOOL;
        case AST_LITERAL_FLOAT:
            return EMBER_VALUE_FLOAT;
        case AST_LITERAL_INT:
            return EMBER_VALUE_INT;
        case AST_LITERAL_NONE:
            return EMBER_VALUE_NONE;
        case AST_LITERAL_STR:
            return EMBER_VALUE_STRING;
    }
    return EMBER_VALUE_ANY;
}

static void resolve_immediate_operand(struct EmberAstOperand *node) {
    node->resolved = (struct EmberResolvedOperand) {
        .operand_kind = EMBER_OPERAND_VALUE,
        .value_kind = value_kind_from_literal(&node->as.immediate),
        .symbol_kind = EMBER_SYMBOL_NONE,
        .invalid = false,
    };

    node->resolved_value = node->as.immediate;
}

static void resolve_reference_operand(
    struct EmberSemanticContext *ctx,
    struct EmberAstOperand *node
) {
    const struct EmberSymbol *symbol = NULL;

    if (ember_symbol_table_lookup(
            ctx->symbols,
            node->as.reference.name,
            &symbol
        ) != 0) {
        EMBER_ERROR_AT(
            ctx->logger,
            node->as.reference.base.source_span,
            "undefined reference '%s'",
            node->as.reference.name
        );

        ember_semantic_context_error(ctx);

        node->resolved = (struct EmberResolvedOperand) {
            .operand_kind = EMBER_OPERAND_NONE,
            .value_kind = EMBER_VALUE_ANY,
            .symbol_kind = EMBER_SYMBOL_NONE,
            .invalid = true,
        };

        return;
    }

    switch (symbol->kind) {
        case EMBER_SYMBOL_NONE:
        case EMBER_SYMBOL_SECTION:
            /* nothing to do */
            break;

        case EMBER_SYMBOL_CONSTANT:
            node->resolved = (struct EmberResolvedOperand) {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = value_kind_from_literal(&symbol->value),
                .symbol_kind = EMBER_SYMBOL_CONSTANT,
                .invalid = false,
            };
            node->resolved_value = symbol->value;
            return;

        case EMBER_SYMBOL_GLOBAL:
            node->resolved = (struct EmberResolvedOperand) {
                .operand_kind = EMBER_OPERAND_GLOBAL,
                .value_kind = EMBER_VALUE_ANY,
                .symbol_kind = EMBER_SYMBOL_GLOBAL,
                .invalid = false,
            };
            return;

        case EMBER_SYMBOL_FUNCTION:
            node->resolved = (struct EmberResolvedOperand) {
                .operand_kind = EMBER_OPERAND_FUNCTION,
                .value_kind = EMBER_VALUE_ANY,
                .symbol_kind = EMBER_SYMBOL_FUNCTION,
                .invalid = false,
            };
            return;

        case EMBER_SYMBOL_LABEL:
            node->resolved = (struct EmberResolvedOperand) {
                .operand_kind = EMBER_OPERAND_LABEL,
                .value_kind = EMBER_VALUE_ANY,
                .symbol_kind = EMBER_SYMBOL_LABEL,
                .invalid = false,
            };
            return;
    }

    node->resolved = (struct EmberResolvedOperand) {
        .operand_kind = EMBER_OPERAND_NONE,
        .value_kind = EMBER_VALUE_ANY,
        .symbol_kind = EMBER_SYMBOL_NONE,
        .invalid = true,
    };
}

static void check_operand_visit(
    struct EmberAstVisitor *self,
    struct EmberAstOperand *node
) {
    struct CheckFunctionContext *func_ctx = subcontext(self);
    struct EmberSemanticContext *semantic_ctx = func_ctx->main_context;

    node->resolved = (struct EmberResolvedOperand) {
        .operand_kind = EMBER_OPERAND_NONE,
        .value_kind = EMBER_VALUE_ANY,
        .symbol_kind = EMBER_SYMBOL_NONE,
        .invalid = true,
    };

    switch (node->kind) {
        case AST_OPERAND_IMMEDIATE:
            resolve_immediate_operand(node);
            return;

        case AST_OPERAND_REFERENCE:
            resolve_reference_operand(semantic_ctx, node);
            return;
    }
}


/**
 * Main pass function and visitor setup
 * ============================================================================
 */

static const struct EmberAstVisitorVTable check_function_vtable = {
    .visit_ast_section      = skip_unknown_sections_visit,
    .visit_ast_func_def     = check_function_visit,
    .visit_ast_label_def    = register_label_visit,
    .visit_ast_instruction  = check_instruction_visit,
    .visit_ast_operand      = check_operand_visit,
};

void do_function_check_pass(
    struct EmberSemanticContext *ctx,
    struct EmberAstNode *root
) {
    if (ctx == NULL || root == NULL) {
        return;
    }

    struct CheckFunctionContext subcontext = {
        .current_func = NULL,
        .is_label_phase = false,
        .main_context = ctx,
    };

    struct EmberAstVisitor visitor;
    ember_ast_visitor_init(
        &visitor,
        (struct EmberAstVisitorCtx) {
            .data = &subcontext,
        },
        &check_function_vtable
    );

    visitor.vtable.visit_ast_node(&visitor, root);
}
