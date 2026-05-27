#include "assembler/phases/lower_ast_to_ir/internal/passes.h"

#include "utils.h"
#include "assembler/internal/models/ast/visitor.h"
#include "assembler/internal/models/ir/nodes/all.h"

/**
 * Visitor context and helpers
 * ============================================================================
 */

struct LowerFunctionsContext {
    struct EmberLowerAstContext *main_context;
    struct EmberIrFunction *current_function;
};

static inline struct LowerFunctionsContext *
subcontext(struct EmberAstVisitor *visitor) {
    return visitor->context.data;
}

static inline struct EmberLowerAstContext *
main_context(struct EmberAstVisitor *visitor) {
    return subcontext(visitor)->main_context;
}

static inline struct EmberSourceSpan
operand_source_span(const struct EmberAstOperand *operand) {
    switch (operand->kind) {
        case AST_OPERAND_IMMEDIATE:
            return operand->as.immediate.base.source_span;
        case AST_OPERAND_REFERENCE:
            return operand->as.reference.base.source_span;
    }
    return EMBER_SOURCE_SPAN_NONE;
}

/**
 * Visit functions
 * ============================================================================
 */


/**
 * Visit EmberAstFuncDef
 * --------------------------------------------------------
 */

static void lower_function(struct EmberAstVisitor *self, struct EmberAstFuncDef *node) {
    subcontext(self)->current_function = ember_ir_program_add_function(
        main_context(self)->out_program,
        EMBER_IR_FUNCTION_USER,
        node->identifier.name,
        node->arity,
        node->local_count,
        node->base.base.source_span
    );

    for (size_t i = 0; i < node->item_count; i++) {
        self->vtable.visit_ast_code_item(self, node->items[i]);
    }

    subcontext(self)->current_function = NULL;
}

/**
 * Visit EmberAstLabelDef
 * --------------------------------------------------------
 */

static void lower_label_def(struct EmberAstVisitor *self, struct EmberAstLabelDef *node) {
    ember_ir_function_add_label(
        subcontext(self)->current_function,
        node->identifier.name,
        node->base.base.source_span
    );
}

/**
 * Visit EmberAstInstruction
 * --------------------------------------------------------
 */

static void lower_instruction(struct EmberAstVisitor *self, struct EmberAstInstruction *node) {
    struct EmberIrInstruction *inst = ember_ir_function_add_instruction(
        subcontext(self)->current_function,
        node->opcode,
        node->base.base.source_span
    );
    
    struct EmberIrOperand *operands[3];
    switch (node->opcode) {
        /* Constants and stack operations. */
        case EMBER_ASM_OPCODE_LDC: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrConstantOperand *ir_operand = ember_ir_constant_operand_new(
                convert_ast_literal_to_ir_value(&ast_operand.resolved_value),
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }
        case EMBER_ASM_OPCODE_DUP:
        case EMBER_ASM_OPCODE_POP: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrImmediateOperand *ir_operand = ember_ir_immediate_operand_new(
                ast_operand.resolved_value.as.int_val,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }

        /* Argument, local, and global access. */
        case EMBER_ASM_OPCODE_GETA: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrIndexOperand *ir_operand = ember_ir_arg_index_operand_new(
                ast_operand.resolved_value.as.int_val,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }
        case EMBER_ASM_OPCODE_GETL:
        case EMBER_ASM_OPCODE_SETL: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrIndexOperand *ir_operand = ember_ir_local_index_operand_new(
                ast_operand.resolved_value.as.int_val,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }
        case EMBER_ASM_OPCODE_GETG:
        case EMBER_ASM_OPCODE_SETG: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrRefOperand *ir_operand = ember_ir_global_ref_operand_new(
                ast_operand.as.reference.name,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }

        /* Integer arithmetic. */
        case EMBER_ASM_OPCODE_IADD:
        case EMBER_ASM_OPCODE_ISUB:
        case EMBER_ASM_OPCODE_IMUL:
        case EMBER_ASM_OPCODE_IDIV:
        case EMBER_ASM_OPCODE_IREM:
            /* no args */
            break;

        /* Floating-point arithmetic. */
        case EMBER_ASM_OPCODE_FADD:
        case EMBER_ASM_OPCODE_FSUB:
        case EMBER_ASM_OPCODE_FMUL:
        case EMBER_ASM_OPCODE_FDIV:
            /* no args */
            break;

        /* String opretaions. */
        case EMEBR_ASM_OPCODE_CONCAT:
            /* no args */
            break;

        /* Logical operations. */
        case EMBER_ASM_OPCODE_AND:
        case EMBER_ASM_OPCODE_OR:
        case EMBER_ASM_OPCODE_NOT:
            /* no args */
            break;

        /* Comparisons. */
        case EMBER_ASM_OPCODE_EQ:
        case EMBER_ASM_OPCODE_NEQ:
        case EMBER_ASM_OPCODE_GT:
        case EMBER_ASM_OPCODE_GTE:
        case EMBER_ASM_OPCODE_LT:
        case EMBER_ASM_OPCODE_LTE:
            /* no args */
            break;

        /* Control flow. */
        case EMBER_ASM_OPCODE_JMP:
        case EMBER_ASM_OPCODE_JMP_IF_FALSE: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrRefOperand *ir_operand = ember_ir_label_ref_operand_new(
                ast_operand.as.reference.name,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }

        /* Function calls and returns. */
        case EMBER_ASM_OPCODE_CALL: {
            struct EmberAstOperand ast_operand = node->operands[0];
            struct EmberIrRefOperand *ir_operand = ember_ir_func_ref_operand_new(
                ast_operand.as.reference.name,
                operand_source_span(&ast_operand)
            );
            operands[0] = &ir_operand->base;
            break;
        }
        case EMBER_ASM_OPCODE_RET:
        case EMBER_ASM_OPCODE_HALT:
            /* no args */
            break;

        /* io */
        case EMBER_ASM_OPCODE_PRINT:
        case EMBER_ASM_OPCODE_INPUT:
            /* no args */
            break;

        case EMBER_ASM_OPCODE_STATUS_OK:
        case EMBER_ASM_OPCODE_STATUS_CODE:
        case EMBER_ASM_OPCODE_STATUS_CLEAR:
            /* no args */
            break;

        /* Cast operations. */
        case EMBER_ASM_OPCODE_I2F:            
        case EMBER_ASM_OPCODE_F2I:
        case EMBER_ASM_OPCODE_I2B:
        case EMBER_ASM_OPCODE_F2B:
        case EMBER_ASM_OPCODE_S2B:
        case EMBER_ASM_OPCODE_N2B:
        case EMBER_ASM_OPCODE_I2S:
        case EMBER_ASM_OPCODE_F2S:
        case EMBER_ASM_OPCODE_B2S:
        case EMBER_ASM_OPCODE_N2S:
            /* no args */
            break;

        /* parse instructions */
        case EMBER_ASM_OPCODE_PARSE_BOOL:
        case EMBER_ASM_OPCODE_PARSE_FLOAT:     
        case EMBER_ASM_OPCODE_PARSE_INT:
            /* no args */
            break;
    }

    for (size_t i = 0; i < node->operand_count; i++) {
        ember_ir_instruction_add_operand(inst, operands[i]);
    }
}


/**
 * Main pass function
 * ============================================================================
 */

static const struct EmberAstVisitorVTable lower_functions_vtable =  {
    .visit_ast_func_def = lower_function,
    .visit_ast_label_def = lower_label_def,
    .visit_ast_instruction = lower_instruction,
};

void do_lower_functions_pass(
    struct EmberLowerAstContext *ctx,
    struct EmberAstNode *root
) {
    struct LowerFunctionsContext subcontext = {
        .main_context = ctx,
        .current_function = NULL
    };
    struct EmberAstVisitor visitor;
    ember_ast_visitor_init(
        &visitor,
        (struct EmberAstVisitorCtx) { .data = &subcontext },
        &lower_functions_vtable
    );
    visitor.vtable.visit_ast_node(&visitor, root);
}
