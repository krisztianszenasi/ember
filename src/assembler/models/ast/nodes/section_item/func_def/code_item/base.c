#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/base.h"

#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/instruction.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/label_def.h"

void ember_ast_code_item_destroy(struct EmberAstCodeItem *self) {
    switch (self->kind) {
        case AST_CODE_ITEM_INSTRUCTION:
            ember_ast_instruction_destroy((struct EmberAstInstruction *)self);
            break;
        case AST_CODE_ITEM_LABEL_DEF:
            ember_ast_label_def_destroy((struct EmberAstLabelDef *)self);
            break;
        default:
            break;
    }
}