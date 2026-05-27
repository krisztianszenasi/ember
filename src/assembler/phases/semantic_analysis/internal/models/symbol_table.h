#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SYMBOL_TABLE_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SYMBOL_TABLE_H

#include <stddef.h>

#include "assembler/phases/semantic_analysis/internal/models/synbol.h"
#include "assembler/internal/models/ast/nodes/section_item/assignment.h"

/**
 * @brief Symbol table scope used during semantic analysis.
 *
 * Each symbol table stores the symbols defined in a single scope and may point
 * to a previous table representing the enclosing scope.
 */
struct EmberSymbolTable {
    /**
     * @brief Enclosing symbol table scope, or NULL if this is the root scope.
     */
    struct EmberSymbolTable *previous;

    /**
     * @brief Number of symbols stored in this scope.
     */
    size_t count;

    /**
     * @brief Allocated capacity of the symbols array.
     */
    size_t capacity;

    /**
     * @brief Symbols owned by this scope.
     */
    struct EmberSymbol **symbols;
};

/**
 * @brief Allocates and initializes a new root symbol table.
 *
 * The returned symbol table has no enclosing scope and starts empty.
 *
 * @return A newly allocated symbol table, or NULL on failure.
 */
struct EmberSymbolTable *ember_symbol_table_new(void);

/**
 * @brief Destroys a symbol table and all symbols it owns.
 *
 * This releases the symbols stored in the table and frees the table itself.
 * Enclosing scopes referenced through `previous` are not destroyed by this
 * function.
 *
 * @param self Symbol table to destroy. If NULL, no action is taken.
 */
void ember_symbol_table_destroy(struct EmberSymbolTable *self);

/**
 * @brief Registers a symbol in the current symbol table scope.
 *
 * If a symbol with the same name already exists in the current scope, the
 * registration fails and `clash` is set to the existing symbol.
 *
 * @param self Symbol table scope to register the symbol in.
 * @param kind Kind of symbol being registered.
 * @param name Name of the symbol to register.
 * @param node AST node associated with the symbol definition.
 * @param clash Output pointer receiving the conflicting symbol on duplicate
 *        registration, or NULL if the caller is not interested in it.
 * @return 0 on success, non-zero on failure.
 */
int ember_symbol_table_register(
    struct EmberSymbolTable *self,
    enum EmberSymbolKind kind,
    const char *name,
    const struct EmberAstNode *node,
    const struct EmberSymbol **clash
);

/**
 * @brief Registers a constant symbol in the current symbol table scope.
 *
 * Constant symbols are backed by assignment AST nodes, because a constant
 * definition stores both the symbol name and the value assigned to it. If a
 * symbol with the same name already exists in the current scope, registration
 * fails and `clash` is set to the existing symbol.
 *
 * @param self Symbol table scope to register the constant in.
 * @param name Name of the constant to register.
 * @param node Assignment AST node associated with the constant definition.
 * @param clash Output pointer receiving the conflicting symbol on duplicate
 *        registration, or NULL if the caller is not interested in it.
 * @return 0 on success, non-zero on failure.
 */
int ember_symbol_table_register_constant(
    struct EmberSymbolTable *self,
    const char *name,
    const struct EmberAstAssignment *node,
    const struct EmberSymbol **clash
);

/**
 * @brief Looks up a symbol by name.
 *
 * The lookup searches the current scope first and then continues through
 * enclosing scopes via `previous`.
 *
 * @param self Symbol table scope to search.
 * @param name Name of the symbol to look up.
 * @param out Output pointer receiving the resolved symbol on success.
 * @return 0 on success, non-zero if the symbol was not found.
 */
int ember_symbol_table_lookup(
    const struct EmberSymbolTable *self,
    const char *name,
    const struct EmberSymbol **out
);

/**
 * @brief Enters a new nested scope.
 *
 * This creates a new symbol table whose enclosing scope is `self`.
 *
 * @param self Current symbol table scope.
 * @return Newly created nested scope, or NULL on failure.
 */
struct EmberSymbolTable *ember_symbol_table_enter_scope(
    struct EmberSymbolTable *self
);

/**
 * @brief Exits the current scope.
 *
 * This destroys the current symbol table and returns its enclosing scope.
 *
 * @param self Current symbol table scope.
 * @return Enclosing scope, or NULL if there is none.
 */
struct EmberSymbolTable *ember_symbol_table_exit_scope(
    struct EmberSymbolTable *self
);

#endif