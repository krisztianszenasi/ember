#include "assembler/internal/models/ast/nodes/program.h"

#include <ember/core/memory.h>

struct EmberAstProgram  *ember_ast_program_new(void) {
    struct EmberAstProgram *self = ember_alloc(sizeof(struct EmberAstProgram));
    ember_ast_node_init(
        &self->base,
        AST_NODE_PROGRAM,
        EMBER_SOURCE_SPAN_NONE
    );
    self->capacity = 0;
    self->count = 0;
    self->sections = NULL;
    return self;
}

void ember_ast_program_destroy(struct EmberAstProgram *self) {
    for (size_t i = 0; i < self->count; i++) {
        ember_ast_section_deinit(&self->sections[i]);
    }
    EMBER_FREE_ARRAY(struct EmberAstSection, self->sections, self->capacity);
    ember_free(self);
}

struct EmberAstSection *ember_ast_program_add_section(
    struct EmberAstProgram *self,
    struct EmberAstIdentifier identifier
) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->sections = EMBER_GROW_ARRAY(
            struct EmberAstSection,
            self->sections,
            old_capacity,
            self->capacity
        );
    }
    struct EmberAstSection *section = &self->sections[self->count++];
    ember_ast_section_init(section, identifier, EMBER_SOURCE_SPAN_NONE);
    return section;
}
