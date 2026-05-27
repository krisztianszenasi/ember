#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_VISITOR_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_VISITOR_H

#include "assembler/internal/models/ast/nodes/all.h"

/**
 * Opaque visitor context payload.
 *
 * This wrapper allows callers to associate arbitrary visitor-specific state
 * with a visitor instance.
 */
struct EmberAstVisitorCtx {
    void *data;
};

struct EmberAstVisitor;

/**
 * Visitor callback table for AST traversal.
 *
 * Any callback may be set to NULL when constructing a visitor. In that case,
 * the visitor initialization logic is expected to substitute the default
 * implementation for that callback.
 *
 * The callbacks are ordered by AST structure rather than alphabetically:
 * generic node first, then root/container nodes, then section-item nodes and
 * other leaf or specialized nodes.
 */
struct EmberAstVisitorVTable {
    void (*visit_ast_node)              (struct EmberAstVisitor *self, struct EmberAstNode *node);
    void (*visit_ast_program)           (struct EmberAstVisitor *self, struct EmberAstProgram *node);
    void (*visit_ast_section)           (struct EmberAstVisitor *self, struct EmberAstSection *node);
    void (*visit_ast_section_item)      (struct EmberAstVisitor *self, struct EmberAstSectionItem *node);
    void (*visit_ast_assignment)        (struct EmberAstVisitor *self, struct EmberAstAssignment *node);
    void (*visit_ast_func_def)          (struct EmberAstVisitor *self, struct EmberAstFuncDef *node);
    void (*visit_ast_code_item)         (struct EmberAstVisitor *self, struct EmberAstCodeItem *node);
    void (*visit_ast_label_def)         (struct EmberAstVisitor *self, struct EmberAstLabelDef *node);
    void (*visit_ast_instruction)       (struct EmberAstVisitor *self, struct EmberAstInstruction *node);
    void (*visit_ast_operand)           (struct EmberAstVisitor *self, struct EmberAstOperand *node);
    void (*visit_ast_identifier)        (struct EmberAstVisitor *self, struct EmberAstIdentifier *node);
    void (*visit_ast_literal)           (struct EmberAstVisitor *self, struct EmberAstLiteral *node);
};

/**
 * AST visitor object.
 *
 * A visitor stores caller-provided context together with the effective visitor
 * callback table used during traversal.
 */
struct EmberAstVisitor {
    struct EmberAstVisitorCtx context;
    struct EmberAstVisitorVTable vtable;
};

/**
 * Initializes an AST visitor.
 *
 * The visitor stores the given context and initializes its effective vtable
 * from the provided callback table. Any NULL callback entries in `vtable`
 * may be replaced by default visitor implementations.
 *
 * @param self Pointer to the visitor to initialize.
 * @param context Caller-provided visitor context.
 * @param vtable Visitor callback table to use as overrides, or NULL to use
 *               only default visitor behavior.
 */
void ember_ast_visitor_init(
    struct EmberAstVisitor *self,
    struct EmberAstVisitorCtx context,
    const struct EmberAstVisitorVTable *vtable
);

/**
 * Updates the visitor's callback table.
 *
 * The given vtable is used to override the visitor's current callbacks. Any
 * NULL callback entries may be replaced with the corresponding default visitor
 * implementations.
 *
 * @param self Pointer to the visitor to update.
 * @param vtable Visitor callback table containing the new overrides.
 */
void ember_ast_visitor_set_vtable(
    struct EmberAstVisitor *self,
    const struct EmberAstVisitorVTable *vtable
);

#endif