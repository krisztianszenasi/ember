#ifndef EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_PASSES_UTILS_H
#define EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_PASSES_UTILS_H

#include "assembler/internal/models/ast/nodes/literal.h"
#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/ir/nodes/function.h"
#include "assembler/internal/models/ir/nodes/program.h"

/**
 * @brief Converts an AST literal into an IR value.
 *
 * The returned IR value contains the literal value in the representation used
 * by the IR layer.
 *
 * @param literal AST literal to convert.
 * @return IR value representing the literal.
 */
struct EmberIrValue convert_ast_literal_to_ir_value(
    struct EmberAstLiteral *literal
);

/**
 * @brief Appends a function call instruction to an IR function.
 *
 * @param function Function to append the instruction to.
 * @param function_name Name of the function to call.
 */
void make_call_instruction(
    struct EmberIrFunction *function,
    const char *function_name,
    struct EmberSourceSpan source_span
);

/**
 * @brief Appends an LDC instruction to an IR function.
 *
 * @param function Function to append the instruction to.
 * @param literal Literal value to load.
 */
void make_ldc_instruction(
    struct EmberIrFunction *function,
    struct EmberAstLiteral *literal
);

/**
 * @brief Appends a SETG instruction to an IR function.
 *
 * @param function Function to append the instruction to.
 * @param identifier Identifier of the target global variable.
 */
void make_setg_instruction(
    struct EmberIrFunction *function,
    struct EmberAstIdentifier *identifier
);

/**
 * @brief Appends a return instruction to an IR function.
 *
 * @param function Function to append the instruction to.
 */
void make_ret_instruction(
    struct EmberIrFunction *function,
    struct EmberSourceSpan source_span
);

/**
 * @brief Appends a halt instruction to an IR function.
 *
 * @param function Function to append the instruction to.
 */
void make_halt_instruction(
    struct EmberIrFunction *function,
    struct EmberSourceSpan source_span
);

/**
 * @brief Creates and appends the generated entry function.
 *
 * The entry function is the runtime entry point emitted by the lowering phase.
 * It is responsible for calling the global initializer function when needed,
 * calling the user-defined main function, and halting execution.
 *
 * @param program IR program to append the function to.
 * @return Newly created entry function.
 */
struct EmberIrFunction *create_entry_function(
    struct EmberIrProgram *program,
    struct EmberSourceSpan source_span
);

/**
 * @brief Creates and appends the generated global initializer function.
 *
 * The initializer function contains instructions that initialize global
 * variables before the user-defined main function is called.
 *
 * @param program IR program to append the function to.
 * @return Newly created initializer function.
 */
struct EmberIrFunction *create_init_function(
    struct EmberIrProgram *program,
    struct EmberSourceSpan source_span
);

#endif
