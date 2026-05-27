#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_PASSES_UTILS_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_PASSES_UTILS_H

#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/phases/semantic_analysis/internal/models/synbol.h"
#include "assembler/phases/semantic_analysis/internal/models/context.h"

/**
 * @brief Reports an identifier redefinition error.
 *
 * Emits a diagnostic for an identifier whose name conflicts with an already
 * registered symbol in the same scope. The diagnostic can use the existing
 * symbol to point out what the new identifier clashes with.
 *
 * @param context Semantic analysis context used for reporting.
 * @param identifier Identifier that attempted to redefine an existing symbol.
 * @param existing Existing symbol that already uses the same name.
 */
void report_identifier_redefinition_error(
    struct EmberSemanticContext *context,
    struct EmberAstIdentifier *identifier,
    const struct EmberSymbol *existing
);

/**
 * @brief Reports warnings for unused symbols in the current scope.
 *
 * Walks the currently active symbol table scope and emits warnings for symbols
 * that were registered but never used. Symbols from parent scopes are not
 * reported by this function.
 *
 * Some compiler-defined symbols are intentionally ignored, because they are not
 * user-declared symbols. This includes section names, the main function, and
 * built-in status code constants.
 * 
 * @param context Semantic analysis context used for reporting.
 */
void report_unused_symbol_warnings(struct EmberSemanticContext *context);

#endif