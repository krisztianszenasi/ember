#include "assembler/internal/models/ast/visitor.h"

static void default_visit_ast_node(struct EmberAstVisitor *self, struct EmberAstNode *node) {
    switch (node->kind) {
        case AST_NODE_IDENTIFIER:
            self->vtable.visit_ast_identifier(self, (struct EmberAstIdentifier *)node);
            break;
        case AST_NODE_LITERAL:
            self->vtable.visit_ast_literal(self, (struct EmberAstLiteral *)node);
            break;
        case AST_NODE_PROGRAM:
            self->vtable.visit_ast_program(self, (struct EmberAstProgram *)node);
            break;
        case AST_NODE_SECTION:
            self->vtable.visit_ast_section(self, (struct EmberAstSection *)node);
            break;
        case AST_NODE_SECTION_ITEM:
            self->vtable.visit_ast_section_item(self, (struct EmberAstSectionItem *)node);
            break;
        default:
            break;
    }
}

static void default_visit_ast_program(struct EmberAstVisitor *self, struct EmberAstProgram *node) {
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section(self, &node->sections[i]);
    }
}

static void default_visit_ast_section(struct EmberAstVisitor *self, struct EmberAstSection *node) {
    self->vtable.visit_ast_identifier(self, &node->identifier);
    for (size_t i = 0; i < node->count; i++) {
        self->vtable.visit_ast_section_item(self, node->items[i]);
    }
}

void default_visit_ast_section_item(struct EmberAstVisitor *self, struct EmberAstSectionItem *node) {
    switch (node->kind) {
        case AST_SECTION_ITEM_ASSIGNMENT:
            self->vtable.visit_ast_assignment(self, (struct EmberAstAssignment *)node);
            break;
        case AST_SECTION_ITEM_FUNC_DEF:
            self->vtable.visit_ast_func_def(self, (struct EmberAstFuncDef *)node);
            break;
        default:
            break;
    }
}

static void default_visit_ast_assignment(struct EmberAstVisitor *self, struct  EmberAstAssignment *node) {
    self->vtable.visit_ast_identifier(self, &node->identifier);
}

static void default_visit_ast_func_def(struct EmberAstVisitor *self, struct EmberAstFuncDef *node) {
    self->vtable.visit_ast_identifier(self, &node->identifier);
}

static void default_visit_ast_code_item(struct EmberAstVisitor *self, struct EmberAstCodeItem *node) {
    switch (node->kind) {
        case AST_CODE_ITEM_INSTRUCTION:
            self->vtable.visit_ast_instruction(self, (struct EmberAstInstruction *)node);
            break;
        case AST_CODE_ITEM_LABEL_DEF:
            self->vtable.visit_ast_label_def(self, (struct EmberAstLabelDef *)node);
            break;
    }
}

static void default_visit_ast_label_def(struct EmberAstVisitor *self, struct EmberAstLabelDef *node) {
    self->vtable.visit_ast_identifier(self, &node->identifier);
}

static void default_visit_ast_instruction(struct EmberAstVisitor *self, struct EmberAstInstruction *node) {
    for (size_t i = 0; i < node->operand_count; i++) {
        self->vtable.visit_ast_operand(self, &node->operands[i]);
    }   
}

static void default_visit_ast_operand(struct EmberAstVisitor *self, struct EmberAstOperand *node) {
    (void)self;
    (void)node;
}

static void default_visit_ast_identifier(struct EmberAstVisitor *self, struct EmberAstIdentifier *node) {
    (void)self;
    (void)node;
}

static void default_visit_ast_literal(struct EmberAstVisitor *self, struct EmberAstLiteral *node) {
    (void)self;
    (void)node;
}

static const struct EmberAstVisitorVTable DEFAULT_VTABLE = {
    .visit_ast_node              = default_visit_ast_node,
    .visit_ast_program           = default_visit_ast_program,
    .visit_ast_section           = default_visit_ast_section,
    .visit_ast_section_item      = default_visit_ast_section_item,
    .visit_ast_assignment        = default_visit_ast_assignment,
    .visit_ast_func_def          = default_visit_ast_func_def,
    .visit_ast_code_item         = default_visit_ast_code_item,
    .visit_ast_label_def         = default_visit_ast_label_def,
    .visit_ast_instruction       = default_visit_ast_instruction,
    .visit_ast_operand           = default_visit_ast_operand,
    .visit_ast_identifier        = default_visit_ast_identifier,
    .visit_ast_literal           = default_visit_ast_literal,
};

void ember_ast_visitor_init(
    struct EmberAstVisitor *self,
    struct EmberAstVisitorCtx context,
    const struct EmberAstVisitorVTable *vtable
) {
    self->context = context;
    ember_ast_visitor_set_vtable(self, vtable);
}

void ember_ast_visitor_set_vtable(
    struct EmberAstVisitor *self,
    const struct EmberAstVisitorVTable *vtable
) {
    self->vtable = DEFAULT_VTABLE;
    
    if (vtable == NULL) {
        return;
    }

    if (vtable->visit_ast_node != NULL) {
        self->vtable.visit_ast_node = vtable->visit_ast_node;
    }
    if (vtable->visit_ast_program != NULL) {
        self->vtable.visit_ast_program = vtable->visit_ast_program;
    }
    if (vtable->visit_ast_section != NULL) {
        self->vtable.visit_ast_section = vtable->visit_ast_section;
    }
    if (vtable->visit_ast_section_item != NULL) {
        self->vtable.visit_ast_section_item = vtable->visit_ast_section_item;
    }
    if (vtable->visit_ast_assignment != NULL) {
        self->vtable.visit_ast_assignment = vtable->visit_ast_assignment;
    }
    if (vtable->visit_ast_func_def != NULL) {
        self->vtable.visit_ast_func_def = vtable->visit_ast_func_def;
    }
    if (vtable->visit_ast_code_item != NULL) {
        self->vtable.visit_ast_code_item = vtable->visit_ast_code_item;
    }
    if (vtable->visit_ast_label_def != NULL) {
        self->vtable.visit_ast_label_def = vtable->visit_ast_label_def;
    }
    if (vtable->visit_ast_instruction != NULL) {
        self->vtable.visit_ast_instruction = vtable->visit_ast_instruction;
    }
    if (vtable->visit_ast_operand != NULL) {
        self->vtable.visit_ast_operand = vtable->visit_ast_operand;
    }
    if (vtable->visit_ast_identifier != NULL) {
        self->vtable.visit_ast_identifier = vtable->visit_ast_identifier;
    }
    if (vtable->visit_ast_literal != NULL) {
        self->vtable.visit_ast_literal = vtable->visit_ast_literal;
    }
}