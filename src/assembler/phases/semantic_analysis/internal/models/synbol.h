#ifndef EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SYMBOL_H
#define EMBER_ASSEMBLER_PHASES_SEMANTIC_ANALYSIS_INTERNAL_MODELS_SYMBOL_H

#include <stddef.h>

#include <ember/assembly/operand_kind.h>

#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/internal/models/ast/nodes/literal.h"
#include "assembler/internal/models/semantic/type.h"

/**
 * @brief Symbol recorded during semantic analysis.
 *
 * A symbol represents a named semantic entity such as a constant, global,
 * function, or label. Symbols are registered in the semantic symbol table and
 * later used to resolve identifier operands.
 */
struct EmberSymbol {
    /**
     * @brief Number of times this symbol has been referenced.
     */
    size_t reference_count;

    /**
     * @brief Kind of semantic entity represented by this symbol.
     */
    enum EmberSymbolKind kind;

    /**
     * @brief Name of the symbol.
     *
     * The symbol owns this string.
     */
    char *name;

    /**
     * @brief AST node that defines this symbol.
     *
     * This is a non-owning reference.
     */
    const struct EmberAstNode *node;

    /**
     * @brief Value type associated with the symbol, if applicable.
     *
     * For constants, this is the type of the compile-time value.
     * For globals, this may describe the initial value type.
     * For functions and labels, this is usually `EMBER_VALUE_ANY`.
     */
    enum EmberAssemblyValueKind value_kind;

    /**
     * @brief Compile-time value associated with the symbol, if applicable.
     *
     * This is mainly useful for constant symbols, because constants behave as
     * value aliases during operand resolution.
     */
    struct EmberAstLiteral value;
};

/**
 * @brief Allocates and initializes a new symbol.
 *
 * The symbol stores a copied version of `name` and keeps a non-owning
 * reference to the defining AST node.
 *
 * @param kind Kind of semantic entity represented by the symbol.
 * @param name Name of the symbol to copy.
 * @param node AST node defining the symbol.
 * @return A newly allocated symbol, or NULL on failure.
 */
struct EmberSymbol *ember_symbol_new(
    enum EmberSymbolKind kind,
    const char *name,
    const struct EmberAstNode *node
);

/**
 * @brief Destroys a symbol and releases its owned resources.
 *
 * This frees the copied symbol name and the symbol object itself. The defining
 * AST node referenced by `node` is not owned by the symbol and is not freed.
 *
 * @param self Symbol to destroy. If NULL, no action is taken.
 */
void ember_symbol_destroy(struct EmberSymbol *self);

#endif