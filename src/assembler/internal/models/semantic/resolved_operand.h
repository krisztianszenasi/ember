#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_RESOLVED_OPERAND_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_RESOLVED_OPERAND_H

#include <stdbool.h>

#include <ember/assembly/instruction_info.h>

#include "assembler/internal/models/semantic/type.h"

/**
 * @brief Semantic information resolved for an instruction operand.
 *
 * This structure describes what an operand represents after semantic
 * resolution. It is used to compare parsed operands against public instruction
 * metadata such as `EmberAssemblyOperandInfo`.
 */
struct EmberResolvedOperand {
    /**
     * @brief Resolved operand role.
     *
     * For constants, this should usually be `EMBER_OPERAND_VALUE`, because
     * constants behave as compile-time value aliases.
     */
    enum EmberAssemblyOperandKind operand_kind;

    /**
     * @brief Resolved value type.
     *
     * Meaningful for `EMBER_OPERAND_VALUE`. Use `EMBER_VALUE_ANY` when no value
     * type is known or required.
     */
    enum EmberAssemblyValueKind value_kind;

    /**
     * @brief Symbol kind for operands that resolve to named symbols.
     *
     * This is useful internally for diagnostics and lowering. For immediate
     * values, this may be left unset or ignored.
     */
    enum EmberSymbolKind symbol_kind;

    /**
     * @brief Whether operand resolution failed.
     *
     * This allows later validation to avoid producing duplicate diagnostics
     * after an earlier undefined-reference or invalid-value error.
     */
    bool invalid;
};

/**
 * @brief Checks whether an actual value kind satisfies an expected value kind.
 *
 * `EMBER_VALUE_ANY` accepts every value kind.
 *
 * @param expected Expected value kind.
 * @param actual Actual resolved value kind.
 * @return `true` if the actual value kind is accepted, otherwise `false`.
 */
static inline bool ember_value_kind_matches(
    enum EmberAssemblyValueKind expected,
    enum EmberAssemblyValueKind actual
) {
    return expected == EMBER_VALUE_ANY || expected == actual;
}

/**
 * @brief Checks whether a resolved operand satisfies expected operand metadata.
 *
 * Invalid resolved operands are treated as matching because the original
 * resolution error has already been reported, and this avoids duplicate
 * follow-up diagnostics.
 *
 * @param expected Expected operand metadata from instruction info.
 * @param actual Resolved operand information.
 * @return `true` if the resolved operand is accepted, otherwise `false`.
 */
static inline bool ember_resolved_operand_matches_info(
    const struct EmberAssemblyOperandInfo *expected,
    const struct EmberResolvedOperand *actual
) {
    if (actual->invalid) {
        return true;
    }

    if (expected->operand_kind != actual->operand_kind) {
        return false;
    }

    if (expected->operand_kind == EMBER_OPERAND_VALUE) {
        return ember_value_kind_matches(expected->value_kind, actual->value_kind);
    }

    return true;
}

#endif