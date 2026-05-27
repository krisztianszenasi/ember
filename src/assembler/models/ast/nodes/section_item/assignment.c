#include "assembler/internal/models/ast/nodes/section_item/assignment.h"

#include <ember/core/memory.h>

struct EmberAstAssignment *ember_ast_assignment_new(
    struct EmberAstIdentifier identifier,
    struct EmberAstLiteral literal,
    struct EmberSourceSpan source_span
) {
    struct EmberAstAssignment *self = ember_alloc(sizeof(struct EmberAstAssignment));
    ember_ast_section_item_init(&self->base, AST_SECTION_ITEM_ASSIGNMENT, source_span);
    self->identifier = identifier;
    self->lieral = literal;
    return self;
}

void ember_ast_assignment_destroy(struct EmberAstAssignment *self) {
    ember_ast_identifier_deinit(&self->identifier);
    ember_free(self);
}