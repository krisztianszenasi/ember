#ifndef EMBER_ASSEMBLY_OPERAND_KIND_H
#define EMBER_ASSEMBLY_OPERAND_KIND_H

/**
 * @brief Describes the semantic role expected from an assembly operand.
 *
 * Operand kinds describe what the operand must represent after resolution.
 * Value operands may be written directly as immediate literals or indirectly
 * as references to constant symbols.
 */
enum EmberAssemblyOperandKind {
    /**
     * @brief Indicates that no operand is expected.
     */
    EMBER_OPERAND_NONE,

    /**
     * @brief Compile-time value operand.
     *
     * A value operand may be an immediate literal, such as an integer or string,
     * or a reference to a constant symbol whose value has the required type.
     */
    EMBER_OPERAND_VALUE,

    /**
     * @brief Reference to a global runtime storage symbol.
     */
    EMBER_OPERAND_GLOBAL,

    /**
     * @brief Reference to a function symbol.
     */
    EMBER_OPERAND_FUNCTION,

    /**
     * @brief Reference to a label used as a jump target.
     */
    EMBER_OPERAND_LABEL,
};

/**
 * @brief Describes the value type expected from a value operand.
 *
 * Value kinds are used to constrain immediate literals and constant symbols.
 * Use `EMBER_VALUE_ANY` when no specific value type is required.
 */
enum EmberAssemblyValueKind {
    /**
     * @brief Any value type is accepted.
     */
    EMBER_VALUE_ANY,

    /**
     * @brief Integer value.
     */
    EMBER_VALUE_INT,

    /**
     * @brief Floating-point value.
     */
    EMBER_VALUE_FLOAT,

    /**
     * @brief String value.
     */
    EMBER_VALUE_STRING,

    /**
     * @brief Boolean value.
     */
    EMBER_VALUE_BOOL,
};

#endif