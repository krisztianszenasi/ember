#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_BASE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_BASE_H

#include "assembler/internal/models/ast/nodes/base.h"

/**
 * Kinds of function body code item AST nodes.
 */
enum EmberAstCodeItemKind {
    AST_CODE_ITEM_INSTRUCTION,
    AST_CODE_ITEM_LABEL_DEF,
};

/**
 * Base type for function body code item AST nodes.
 *
 * Code items are polymorphic nodes stored through base pointers inside
 * function definitions. The concrete code item kind is stored in `kind`.
 */
struct EmberAstCodeItem {
    struct EmberAstNode base;
    enum EmberAstCodeItemKind kind;
};

/**
 * Initializes a code item base.
 *
 * @param self Pointer to the code item base to initialize.
 * @param kind Concrete code item kind.
 * @param source_span Source span associated with the code item.
 */
static inline void ember_ast_code_item_init(
    struct EmberAstCodeItem *self,
    enum EmberAstCodeItemKind kind,
    struct EmberSourceSpan source_span
) {
    ember_ast_node_init(&self->base, AST_NODE_CODE_ITEM, source_span);
    self->kind = kind;
}

/**
 * Destroys a code item node using code item-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes the appropriate
 * destroy function for the concrete heap-allocated code item node.
 *
 * @param self Pointer to the code item node to destroy. If NULL, no action is
 *             taken.
 */
void ember_ast_code_item_destroy(struct EmberAstCodeItem *self);

#endif