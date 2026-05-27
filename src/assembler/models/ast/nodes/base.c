#include "assembler/internal/models/ast/nodes/base.h"

#include "assembler/internal/models/ast/nodes/program.h"
#include "assembler/internal/models/ast/nodes/section_item/base.h"


void ember_ast_node_init(
    struct EmberAstNode *self,
    enum EmberAstNodeKind kind,
    struct EmberSourceSpan source_span
) {
    self->kind = kind;
    self->source_span = source_span;
}

void ember_ast_node_destroy(struct EmberAstNode *self) {
    if (self == NULL) {
        return;
    }

    switch (self->kind) {
        case AST_NODE_PROGRAM:
            ember_ast_program_destroy((struct EmberAstProgram *)self);
            break;
        case AST_NODE_SECTION_ITEM:
            ember_ast_section_item_destroy((struct EmberAstSectionItem *)self);
            break;
        default:
            break;
    }
}