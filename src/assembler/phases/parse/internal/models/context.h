#ifndef EMBER_ASSEMBLER_PHASES_PARSE_INTERNAL_MODELS_CONTEXT_H
#define EMBER_ASSEMBLER_PHASES_PARSE_INTERNAL_MODELS_CONTEXT_H

#include <ember/core/models/logger.h>

#include "assembler/internal/models/ast/nodes/program.h"
#include "assembler/internal/models/ast/nodes/section.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/base.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/instruction.h"

/**
 * @brief Parser state shared across lexer and parser actions.
 *
 * The parser context stores the source path, logger, produced AST root, and
 * the currently active AST nodes being built while parsing. Parser actions use
 * this structure to attach parsed nodes to the correct parent node.
 */
struct EmberParserContext {
    /**
     * @brief Path of the source file being parsed.
     */
    const char *path;

    /**
     * @brief Logger used for parser diagnostics and debug messages.
     */
    struct EmberLogger *logger;

    /**
     * @brief Root AST program produced by the parser.
     */
    struct EmberAstProgram *out_program;

    /**
     * @brief Section currently being parsed.
     */
    struct EmberAstSection *current_section;

    /**
     * @brief Function definition currently being parsed.
     */
    struct EmberAstFuncDef *current_func_def;

    /**
     * @brief Instruction currently being parsed.
     */
    struct EmberAstInstruction *current_instruction;
};

/**
 * @brief Initializes a parser context.
 *
 * @param self Context to initialize.
 * @param path Path of the source file being parsed.
 * @param logger Logger used for parser diagnostics.
 */
void ember_parser_context_init(
    struct EmberParserContext *self,
    const char *path,
    struct EmberLogger *logger
);

/**
 * @brief Releases resources owned by a parser context.
 *
 * This should be called when parsing fails or when the context still owns the
 * produced AST. If ownership of the AST has been transferred to the caller,
 * the context should no longer destroy it.
 *
 * @param self Context to deinitialize.
 */
void ember_parser_context_deinit(struct EmberParserContext *self);

#endif