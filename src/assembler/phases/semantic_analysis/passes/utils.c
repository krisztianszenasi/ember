#include "utils.h"

#include <string.h>
#include <stdbool.h>

#include "assembler/internal/models/semantic/special_names.h"

void report_identifier_redefinition_error(
    struct EmberSemanticContext *context,
    struct EmberAstIdentifier *identifier,
    const struct EmberSymbol *existing
) {
    if(!context || !identifier || !existing) {
        return;
    }
    EMBER_ERROR_AT(
        context->logger,
        identifier->base.source_span,
        "'%s' was already defined at %s:%d:%d",
        identifier->name,
        existing->node->source_span.path,
        existing->node->source_span.start_line,
        existing->node->source_span.start_column
    );
    context->status = SEMANTIC_PASS_ERROR;
}

static bool _should_be_ignored(const char *symbol_name) {
    return strcmp(symbol_name, EMBER_MAIN_FUNCTION_NAME) == 0 ||
           strcmp(symbol_name, EMBER_CONSTANTS_SECTION_NAME) == 0 ||
           strcmp(symbol_name, EMBER_GLOBALS_SECTION_NAME) == 0 ||
           strcmp(symbol_name, EMBER_CODE_SECTION_NAME) == 0;
}

void report_unused_symbol_warnings(struct EmberSemanticContext *context) {
    for (size_t i = 0; i < context->symbols->count; i++) {
        struct EmberSymbol *symbol = context->symbols->symbols[i];
        /* TODO: user made flag on symbols would be a cleaner solution */
        if (ember_source_span_is_none(&symbol->node->source_span)) {
            continue;
        }

        if (_should_be_ignored(symbol->name)) {
            continue;
        }

        if (symbol->reference_count == 0) {
            EMBER_WARNING_AT(
                context->logger,
                symbol->node->source_span,
                "symbol '%s' is unused",
                symbol->name
            );
        }
    }
}