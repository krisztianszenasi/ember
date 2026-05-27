#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_ASSIGNMENT_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_ASSIGNMENT_H

#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/ast/nodes/section_item/base.h"
#include "assembler/internal/models/ast/nodes/literal.h"

/**
 * AST node representing an assignment section item.
 *
 * An assignment binds an identifier to a literal value within a section.
 * This node is heap allocated because section items are polymorphic and are
 * stored through base pointers.
 */
struct EmberAstAssignment {
    struct EmberAstSectionItem base;
    struct EmberAstIdentifier identifier;
    struct EmberAstLiteral lieral;
};

/**
 * Allocates and initializes an assignment node.
 *
 * The returned node takes ownership of the given identifier and literal
 * values.
 *
 * @param identifier Identifier being assigned to.
 * @param literal Literal value assigned to the identifier.
 * @param source_span Source span associated with the assignment.
 * @return A newly allocated assignment node, or NULL on failure.
 */
struct EmberAstAssignment *ember_ast_assignment_new(
    struct EmberAstIdentifier identifier,
    struct EmberAstLiteral literal,
    struct EmberSourceSpan source_span
);

/**
 * Destroys an assignment node and all resources it owns.
 *
 * This releases the embedded identifier and literal values, then frees the
 * assignment node itself.
 *
 * @param self Pointer to the assignment node to destroy. If NULL, no action is
 *             taken.
 */
void ember_ast_assignment_destroy(struct EmberAstAssignment *self);

#endif