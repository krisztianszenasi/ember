#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SEMANTIC_SPECIAL_NAMES_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SEMANTIC_SPECIAL_NAMES_H

#include <stdbool.h>
#include <string.h>

/**
 * @brief Name of the program entry function.
 */
#define EMBER_MAIN_FUNCTION_NAME "main"

/**
 * @brief Name of the constants section.
 */
#define EMBER_CONSTANTS_SECTION_NAME "constants"

/**
 * @brief Name of the globals section.
 */
#define EMBER_GLOBALS_SECTION_NAME "globals"

/**
 * @brief Name of the code section.
 */
#define EMBER_CODE_SECTION_NAME "code"

/**
 * @brief Checks whether a name refers to the program entry function.
 *
 * @param name Name to check.
 * @return true if the name is equal to the main function name, false otherwise.
 */
static inline bool ember_is_main_function(const char *name) {
    return name != NULL && strcmp(name, EMBER_MAIN_FUNCTION_NAME) == 0;
}

#endif