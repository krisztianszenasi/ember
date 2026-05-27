#ifndef EMBER_ASSEMBLY_INSTRUCTION_INFO_H
#define EMBER_ASSEMBLY_INSTRUCTION_INFO_H

#include <stddef.h>

#include <ember/assembly/operand_kind.h>

#define EMBER_MAX_INSTRUCTION_OPERANDS 3

/**
 * @brief Describes an additional validation rule for an assembly operand.
 *
 * Validator kinds describe reusable operand constraints, such as requiring an
 * integer value to be positive or to refer to a valid local slot. The semantic
 * analyzer implements the actual validation logic for each kind.
 */
enum EmberAssemblyOperandValidatorKind {
    /**
     * @brief No additional validation is required.
     */
    EMBER_OPERAND_VALIDATOR_NONE,

    /**
     * @brief The resolved integer value must be greater than zero.
     */
    EMBER_OPERAND_VALIDATOR_POSITIVE_INT,

    /**
     * @brief The resolved integer value must be zero or greater.
     */
    EMBER_OPERAND_VALIDATOR_NON_NEGATIVE_INT,

    /**
     * @brief The resolved integer value must refer to an existing argument.
     */
    EMBER_OPERAND_VALIDATOR_ARG_INDEX_IN_RANGE,

    /**
     * @brief The resolved integer value must refer to an existing local.
     */
    EMBER_OPERAND_VALIDATOR_LOCAL_INDEX_IN_RANGE,
};

/**
 * @brief Describes the expected operand for one instruction operand position.
 *
 * Operand info describes what the operand must represent after semantic
 * resolution. For `EMBER_OPERAND_VALUE`, the operand may be written either as
 * an immediate literal or as a reference to a constant symbol with the required
 * value type.
 */
struct EmberAssemblyOperandInfo {
    /**
     * @brief Semantic kind expected for the operand.
     */
    enum EmberAssemblyOperandKind operand_kind;

    /**
     * @brief Value type expected for value operands.
     *
     * This field is meaningful when `operand_kind` is `EMBER_OPERAND_VALUE`.
     * Use `EMBER_VALUE_ANY` when no specific value type is required.
     */
    enum EmberAssemblyValueKind value_kind;

    /**
     * @brief Additional validation rule for this operand.
     *
     * Use `EMBER_OPERAND_VALIDATOR_NONE` when no extra validation is required.
     */
    enum EmberAssemblyOperandValidatorKind validator;
};

/**
 * @brief Describes an assembly-level instruction.
 *
 * Contains the public metadata needed to inspect, validate, or document an
 * instruction supported by the assembler.
 */
struct EmberAssemblyInstructionInfo {
    /**
     * @brief Instruction mnemonic, such as `ldc`, `call`, or `jmp`.
     */
    const char *name;

    /**
     * @brief Human-readable description of the instruction.
     */
    const char *description;

    /**
     * @brief Number of operands expected by this instruction.
     */
    size_t operand_count;

    /**
     * @brief Metadata for each expected operand.
     */
    struct EmberAssemblyOperandInfo operands[EMBER_MAX_INSTRUCTION_OPERANDS];
};

/**
 * @brief Returns a human-readable description of an operand validator.
 *
 * @param validator Validator kind to describe.
 * @return Description of the validator, or NULL if no additional validation is
 *         required.
 */
const char *ember_assembly_operand_validator_description(
    enum EmberAssemblyOperandValidatorKind validator
);

/**
 * @brief Returns the number of supported assembly instructions.
 *
 * @return Number of entries available through
 *         ember_assembly_instruction_info_at().
 */
size_t ember_assembly_instruction_info_count(void);

/**
 * @brief Returns instruction metadata by index.
 *
 * @param index Index of the instruction metadata to retrieve.
 * @return Instruction metadata for the given index, or NULL if the index is out
 *         of range.
 */
const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_at(
    size_t index
);

/**
 * @brief Looks up instruction metadata by instruction name.
 *
 * @param name Name of the assembly instruction to look up.
 * @return Instruction metadata for the given name, or NULL if no such
 *         instruction exists.
 */
const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_by_name(
    const char *name
);

#endif