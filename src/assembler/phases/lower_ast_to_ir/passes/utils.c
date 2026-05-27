#include "utils.h"

#include "assembler/phases/lower_ast_to_ir/internal/special_names.h"

struct EmberIrValue convert_ast_literal_to_ir_value(
    struct EmberAstLiteral *literal
) {
    struct EmberIrValue value;
    switch (literal->kind) {
        case AST_LITERAL_BOOLEAN:
            value = ember_ir_boolean_value(literal->as.boolean_val);
            break;
        case AST_LITERAL_FLOAT:
            value = ember_ir_float_value(literal->as.float_val);
            break;
        case AST_LITERAL_INT:
            value = ember_ir_int_value(literal->as.int_val);
            break;
        case AST_LITERAL_STR:
            value = ember_ir_str_value(literal->as.str_val);
            break;
    }
    ember_ir_value_set_source_span(&value, literal->base.source_span);
    return value;
}

void make_call_instruction(
    struct EmberIrFunction *function,
    const char *function_name,
    struct EmberSourceSpan source_span
) {
    struct EmberIrInstruction *inst = ember_ir_function_add_instruction(
        function,
        EMBER_ASM_OPCODE_CALL,
        source_span
    );
    ember_ir_instruction_add_operand(
        inst,
        &ember_ir_func_ref_operand_new(function_name, source_span)->base
    );
}

void make_ldc_instruction(
    struct EmberIrFunction *function,
    struct EmberAstLiteral *literal
) {
    struct EmberIrInstruction *inst = ember_ir_function_add_instruction(
        function,
        EMBER_ASM_OPCODE_LDC,
        literal->base.source_span
    );
    ember_ir_instruction_add_operand(
        inst,
        &ember_ir_constant_operand_new(
            convert_ast_literal_to_ir_value(literal),
            literal->base.source_span
        )->base
    );
}

void make_setg_instruction(
    struct EmberIrFunction *function,
    struct EmberAstIdentifier *identifier
) {
    struct EmberIrInstruction *inst = ember_ir_function_add_instruction(
        function,
        EMBER_ASM_OPCODE_SETG,
        identifier->base.source_span
    );
    ember_ir_instruction_add_operand(
        inst,
        &ember_ir_global_ref_operand_new(
            identifier->name,
            identifier->base.source_span
        )->base
    );
}

void make_ret_instruction(
    struct EmberIrFunction *function,
    struct EmberSourceSpan source_span
) {
    ember_ir_function_add_instruction(
        function,
        EMBER_ASM_OPCODE_RET,
        source_span
    );
}

void make_halt_instruction(
    struct EmberIrFunction *function,
    struct EmberSourceSpan source_span
) {
    ember_ir_function_add_instruction(
        function,
        EMBER_ASM_OPCODE_HALT,
        source_span
    );
}

struct EmberIrFunction *create_entry_function(
    struct EmberIrProgram *program,
    struct EmberSourceSpan source_span
) {
    struct EmberIrFunction *function = ember_ir_program_add_function(
        program,
        EMBER_IR_FUNCTION_ENTRY,
        EMBER_ENTRY_FUNCTION_NAME,
        0,
        0,
        source_span
    );
    return function;
}

struct EmberIrFunction *create_init_function(
    struct EmberIrProgram *program,
    struct EmberSourceSpan source_span
) {
    struct EmberIrFunction *function = ember_ir_program_add_function(
        program,
        EMBER_IR_FUNCTION_INIT,
        EMBER_INIT_FUNCTION_NAME,
        0,
        0,
        source_span
    );
    return function;
}
