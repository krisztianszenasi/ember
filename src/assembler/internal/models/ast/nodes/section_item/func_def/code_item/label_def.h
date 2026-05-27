#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_LABEL_DEF_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_LABEL_DEF_H

#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/base.h"

/**
 * AST node representing a label definition code item.
 *
 * A label definition introduces a function-local label identified by name.
 * This node is heap allocated because code items are polymorphic and are
 * stored through base pointers inside function definitions.
 */
struct EmberAstLabelDef {
    struct EmberAstCodeItem base;
    struct EmberAstIdentifier identifier;
};

/**
 * Allocates and initializes a label definition node.
 *
 * The returned node takes ownership of the given identifier value.
 *
 * @param identifier Identifier naming the label.
 * @param source_span Source span associated with the label definition.
 * @return A newly allocated label definition node, or NULL on failure.
 */
struct EmberAstLabelDef *ember_ast_label_def_new(
    struct EmberAstIdentifier identifier,
    struct EmberSourceSpan source_span
);

/**
 * Destroys a label definition node and all resources it owns.
 *
 * This releases the embedded identifier and frees the label definition node
 * itself.
 *
 * @param self Pointer to the label definition node to destroy. If NULL, no
 *             action is taken.
 */
void ember_ast_label_def_destroy(
    struct EmberAstLabelDef *self
);

#endif