#include "assembler/internal/models/ir/nodes/program.h"

#include <ember/core/memory.h>

struct EmberIrProgram *ember_ir_program_new(struct EmberSourceSpan source_span) {
    struct EmberIrProgram *self = ember_alloc(sizeof(struct EmberIrProgram));
    ember_ir_node_init(&self->base, IR_NODE_PROGRAM, source_span);
    self->capacity = 0;
    self->count = 0;
    self->functions = NULL;
    self->starting_function = NULL;
    return self;
}

void ember_ir_program_destroy(struct EmberIrProgram *self) {
    for (size_t i = 0; i < self->count; i++) {
        ember_ir_function_deinit(&self->functions[i]);
    }
    EMBER_FREE_ARRAY(struct EmberIrFunction *, self->functions, self->capacity);
    ember_free(self);
}

struct EmberIrFunction *ember_ir_program_add_function(
    struct EmberIrProgram *self,
    enum EmberIrFunctionKind kind,
    const char *name,
    size_t arity,
    size_t local_count,
    struct EmberSourceSpan source_span
) {
    if (self->count + 1 > self->capacity) {
        size_t old_capacity = self->capacity;
        self->capacity = EMBER_GROW_CAPACITY(old_capacity);
        self->functions = EMBER_GROW_ARRAY(
            struct EmberIrFunction,
            self->functions,
            old_capacity,
            self->capacity
        );
    }
    struct EmberIrFunction *func = &self->functions[self->count++];
    ember_ir_function_init(func, source_span);
    func->kind = kind;
    func->name = ember_strdup(name);
    func->arity = arity;
    func->local_count = local_count;
    return func;
}

struct EmberIrFunction *ember_ir_program_get_current_function(struct EmberIrProgram *self) {
    if (self->count == 0) {
        return NULL;
    }
    return &self->functions[self->count - 1];
}

bool ember_ir_program_set_starting_function(struct EmberIrProgram *self, const char *name) {
    if (self->starting_function != NULL) {
        return false;
    }
    self->starting_function = ember_strdup(name);
    return true;
}

void ember_ir_program_force_set_starting_function(struct EmberIrProgram *self, const char *name) {
    ember_free(self->starting_function);
    self->starting_function = ember_strdup(name);
}
