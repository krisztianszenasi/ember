#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_PASES_OPERAND_VALIDATOR_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_PASES_OPERAND_VALIDATOR_H

#include <stdbool.h>
#include <ember/core/models/logger.h>

#include "assembler/internal/models/assembly/instruction_def.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/base.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/instruction.h"

/**
 * @brief Runs the additional validator for an instruction operand.
 *
 * This function dispatches the validator specified by `expected->validator`.
 * The operand is expected to have already been resolved and checked against
 * the basic operand kind and value kind requirements.
 *
 * Validators use the current function definition when they need function-local
 * information, such as argument count or local count.
 *
 * @param logger Logger used to report validation diagnostics.
 * @param func Current function definition being checked.
 * @param expected Expected operand metadata for this operand position.
 * @param operand Operand to validate.
 * @return `true` if the operand passes validation, otherwise `false`.
 */
bool run_operand_validator(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    const struct EmberAssemblyOperandInfo *expected,
    struct EmberAstOperand *operand
);

#endif