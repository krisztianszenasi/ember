#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_PROGRAM_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_PROGRAM_H

#include <stddef.h>

#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/internal/models/ast/nodes/section.h"

/**
 * Root AST node representing an assembled source file.
 *
 * A program owns a dynamically managed array of sections and serves as the
 * root of the assembler AST.
 */
struct EmberAstProgram {
    struct EmberAstNode base;
    size_t count;
    size_t capacity;
    struct EmberAstSection *sections;
};

/**
 * Allocates and initializes a program node.
 *
 * The returned program node is heap allocated and is intended to be used as
 * the root of the AST.
 *
 * @return A newly allocated program node, or NULL on failure.
 */
struct EmberAstProgram *ember_ast_program_new(void);

/**
 * Destroys a program node and all resources it owns.
 *
 * This releases the program's owned sections and any resources recursively
 * owned by those sections, then frees the program node itself.
 *
 * @param self Pointer to the program node to destroy. If NULL, no action is
 *             taken.
 */
void ember_ast_program_destroy(
    struct EmberAstProgram *self
);

/**
 * Adds a section to the program.
 *
 * The given identifier is used as the new section's name. The section is
 * appended to the program's internal section array.
 *
 * @param self Pointer to the program node.
 * @param identifier Identifier naming the section to add.
 * @return Pointer to the newly added section, or NULL on failure.
 */
struct EmberAstSection *ember_ast_program_add_section(
    struct EmberAstProgram *self,
    struct EmberAstIdentifier identifier
);

#endif