#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_H

#include <stddef.h>

#include "assembler/internal/models/semantic/type.h"
#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/ast/nodes/section_item/base.h"

/**
 * AST node representing a section.
 *
 * A section has an identifier and owns a dynamically managed array of section
 * items. The `items` field stores pointers because section items are
 * polymorphic nodes that may have different concrete types and sizes.
 */
struct EmberAstSection {
    struct EmberAstNode base;
    enum EmberSectionKind resolved_kind;
    struct EmberAstIdentifier identifier;
    size_t count;
    size_t capacity;
    struct EmberAstSectionItem **items;
};

/**
 * Initializes a section node.
 *
 * The section takes ownership of the given identifier value and starts with an
 * empty section item list.
 *
 * @param self Pointer to the section node to initialize.
 * @param identifier Identifier naming the section.
 * @param source_span Source span associated with the section.
 */
void ember_ast_section_init(
    struct EmberAstSection *self,
    struct EmberAstIdentifier identifier,
    struct EmberSourceSpan source_span
);

/**
 * Deinitializes a section node.
 *
 * This releases resources owned by the section, including its identifier and
 * all owned section items.
 *
 * @param self Pointer to the section node to deinitialize.
 */
void ember_ast_section_deinit(
    struct EmberAstSection *self
);

/**
 * Adds a section item to the section.
 *
 * The item pointer is appended to the section's internal item array.
 *
 * @param self Pointer to the section node.
 * @param item Pointer to the section item to add.
 */
void ember_ast_section_add_item(
    struct EmberAstSection *self,
    struct EmberAstSectionItem *item
);

#endif