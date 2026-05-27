#include "assembler/phases/semantic_analysis/internal/models/context.h"

#include <ember/vm.h>


static void register_builtins(struct EmberSymbolTable *table) {
    for (size_t i = 0; i < EMBER_STATUS_COUNT; i++) {
        /* TODO: this causes memory leak, store these on context and free them in deinit as well */
        ember_symbol_table_register_constant(
            table,
            ember_process_status_to_string(i),
            ember_ast_assignment_new(
                ember_ast_identifier(ember_process_status_to_string(i), EMBER_SOURCE_SPAN_NONE),
                ember_ast_int_literal(i, EMBER_SOURCE_SPAN_NONE),
                EMBER_SOURCE_SPAN_NONE
            ),
            NULL
        );
    }
}

void ember_semantic_context_init(struct EmberSemanticContext *self, struct EmberLogger *logger) {
    self->had_main = false;
    self->logger = logger;
    self->status = SEMANTIC_PASS_OK;
    self->symbols = ember_symbol_table_new();
    register_builtins(self->symbols);
}

void ember_semantic_context_deinit(struct EmberSemanticContext *self) {
    ember_symbol_table_destroy(self->symbols);
}