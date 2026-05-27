#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_TYPE_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_TYPE_H

/**
 * @brief Semantic kind of a validated assembly section.
 *
 * Sections are parsed by name first, then resolved during semantic analysis to
 * one of these section kinds. Later passes can use this resolved kind instead
 * of repeatedly comparing section name strings.
 */
enum EmberSectionKind {
    /**
     * @brief Section containing constant definitions.
     */
    EMBER_SECTION_CONSTANTS,

    /**
     * @brief Section containing global definitions.
     */
    EMBER_SECTION_GLOBALS,

    /**
     * @brief Section containing function definitions and executable code.
     */
    EMBER_SECTION_CODE,

    /**
     * @brief Section kind could not be resolved.
     */
    EMBER_SECTION_UNKNOWN,
};

/**
 * @brief Semantic kind of a resolved symbol.
 *
 * Symbol kinds describe what a named reference resolves to during semantic
 * analysis.
 */
enum EmberSymbolKind {
    /**
     * @brief Section name in general.
     */
    EMBER_SYMBOL_NONE,

    /**
     * @brief Section name in general.
     */
    EMBER_SYMBOL_SECTION,
    
    /**
     * @brief Constant symbol representing a compile-time value.
     */
    EMBER_SYMBOL_CONSTANT,

    /**
     * @brief Global symbol representing runtime storage.
     */
    EMBER_SYMBOL_GLOBAL,

    /**
     * @brief Function symbol representing callable code.
     */
    EMBER_SYMBOL_FUNCTION,

    /**
     * @brief Label symbol representing a function-local jump target.
     */
    EMBER_SYMBOL_LABEL,
};

#endif