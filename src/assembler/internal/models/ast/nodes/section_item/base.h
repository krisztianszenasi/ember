#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_BASE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_BASE_H

#include "assembler/internal/models/ast/nodes/base.h"

/**
 * Kinds of section item AST nodes.
 */
enum EmberAstSectionItemKind {
    AST_SECTION_ITEM_ASSIGNMENT,
    AST_SECTION_ITEM_FUNC_DEF,
};

/**
 * Base type for section item AST nodes.
 *
 * Section items are polymorphic nodes stored through base pointers inside
 * sections. The concrete section item kind is stored in `kind`.
 */
struct EmberAstSectionItem {
    struct EmberAstNode base;
    enum EmberAstSectionItemKind kind;
};

/**
 * Initializes a section item base.
 *
 * @param self Pointer to the section item base to initialize.
 * @param kind Concrete section item kind.
 * @param source_span Source span associated with the section item.
 */
static inline void ember_ast_section_item_init(
    struct EmberAstSectionItem *self,
    enum EmberAstSectionItemKind kind,
    struct EmberSourceSpan source_span
) {
    ember_ast_node_init(&self->base, AST_NODE_SECTION_ITEM, source_span);
    self->kind = kind;
}

/**
 * Destroys a section item node using section item-specific cleanup logic.
 *
 * This function dispatches based on `self->kind` and invokes the appropriate
 * destroy function for the concrete heap-allocated section item node.
 *
 * @param self Pointer to the section item node to destroy. If NULL, no action
 *             is taken.
 */
void ember_ast_section_item_destroy(struct EmberAstSectionItem *self);

#endif