#include "assembler/internal/models/ast/nodes/section_item/base.h"

#include "assembler/internal/models/ast/nodes/section_item/assignment.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/base.h"

void ember_ast_section_item_destroy(struct EmberAstSectionItem *self) {
    switch (self->kind) {
        case AST_SECTION_ITEM_ASSIGNMENT:
            ember_ast_assignment_destroy((struct EmberAstAssignment *)self);
            break;
        case AST_SECTION_ITEM_FUNC_DEF:
            ember_ast_func_def_destroy((struct EmberAstFuncDef *)self);
            break;
    }
}