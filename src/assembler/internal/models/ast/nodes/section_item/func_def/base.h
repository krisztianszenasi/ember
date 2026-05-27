#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_BASE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_BASE_H

#include <stddef.h>

#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/ast/nodes/section_item/base.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/base.h"

/**
 * AST node representing a function definition section item.
 *
 * A function definition has a name, an arity, a local slot count, and a
 * dynamically managed list of code items forming the function body.
 *
 * The `items` field stores pointers because code items are polymorphic nodes
 * that may have different concrete types and sizes.
 */
struct EmberAstFuncDef {
    struct EmberAstSectionItem base;
    struct EmberAstIdentifier identifier;
    size_t arity;
    size_t local_count;
    size_t item_count;
    size_t item_capacity;
    struct EmberAstCodeItem **items;
};

/**
 * Allocates and initializes a function definition node.
 *
 * The returned node takes ownership of the given identifier value.
 *
 * @param identifier Identifier naming the function.
 * @param arity Number of function arguments.
 * @param local_count Number of local slots used by the function.
 * @return A newly allocated function definition node, or NULL on failure.
 */
struct EmberAstFuncDef *ember_ast_func_def_new(
    struct EmberAstIdentifier identifier,
    size_t arity,
    size_t local_count
);

/**
 * Destroys a function definition node and all resources it owns.
 *
 * This releases the embedded identifier, all owned code items, and the dynamic
 * item array, then frees the function definition node itself.
 *
 * @param self Pointer to the function definition node to destroy. If NULL, no
 *             action is taken.
 */
void ember_ast_func_def_destroy(
    struct EmberAstFuncDef *self
);

/**
 * Adds a code item to the function body.
 *
 * The given code item pointer is appended to the function's internal code item
 * array.
 *
 * @param self Pointer to the function definition node.
 * @param code_item Pointer to the code item to add.
 */
void ember_ast_func_def_add_code_item(
    struct EmberAstFuncDef *self,
    struct EmberAstCodeItem *code_item
);

#endif