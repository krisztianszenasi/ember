#ifndef EMBER_ASSEMBLER_INTERNAL_PHASES_H
#define EMBER_ASSEMBLER_INTERNAL_PHASES_H

#include <ember/core/models/logger.h>
#include <ember/core/models/executable/types.h>

#include "assembler/internal/models/ast/nodes/base.h"
#include "assembler/internal/models/ir/nodes/base.h"

/**
 * @brief Result of an assembler phase.
 *
 * Indicates whether a phase completed successfully or failed. Detailed
 * diagnostics for failures are reported through the logger passed to the phase.
 */
enum EmberAssemblerPhaseResult {
    /**
     * @brief The phase completed successfully.
     */
    ASSEMBLER_PHASE_OK,

    /**
     * @brief The phase failed.
     */
    ASSEMBLER_PHASE_ERROR,
};

/**
 * @brief Parses assembly source files into an AST.
 *
 * This phase reads the assembly source at the given path and builds the root
 * AST node used by later assembler phases.
 *
 * @param path Path to the assembly source file or input path to parse.
 * @param out_root Output pointer that receives the parsed AST root on success.
 * @param logger Logger used to report parsing diagnostics.
 * @return `ASSEMBLER_PHASE_OK` on success, or `ASSEMBLER_PHASE_ERROR` on
 *         failure.
 */
enum EmberAssemblerPhaseResult ember_do_parse_phase(
    const char *path,
    struct EmberAstNode **out_root,
    struct EmberLogger *logger
);

/**
 * @brief Performs semantic analysis on the parsed AST.
 *
 * This phase validates the AST, checks declarations and instruction operands,
 * and reports semantic diagnostics.
 *
 * @param ast_root Root node of the AST to analyze.
 * @param logger Logger used to report semantic diagnostics.
 * @return `ASSEMBLER_PHASE_OK` on success, or `ASSEMBLER_PHASE_ERROR` on
 *         failure.
 */
enum EmberAssemblerPhaseResult ember_do_semantic_analysis_phase(
    struct EmberAstNode *ast_root,
    struct EmberLogger *logger
);

/**
 * @brief Lowers the AST into the assembler IR.
 *
 * This phase converts the semantically valid AST into an intermediate
 * representation used for executable generation.
 *
 * @param ast_root Root node of the AST to lower.
 * @param out_ir Output pointer that receives the generated IR root on
 *        success.
 * @param logger Logger used to report lowering diagnostics.
 * @return `ASSEMBLER_PHASE_OK` on success, or `ASSEMBLER_PHASE_ERROR` on
 *         failure.
 */
enum EmberAssemblerPhaseResult ember_do_lower_ast_to_ir_phase(
    struct EmberAstNode *ast_root,
    struct EmberIrNode **out_ir,
    struct EmberLogger *logger
);

/**
 * @brief Generates an executable from the assembler IR.
 *
 * This phase converts the assembler IR into an executable representation that
 * can be loaded or executed by the runtime.
 *
 * @param ir_root Root node of the IR to convert.
 * @param out_exe Output pointer that receives the generated executable on
 *        success.
 * @param logger Logger used to report executable-generation diagnostics.
 * @return `ASSEMBLER_PHASE_OK` on success, or `ASSEMBLER_PHASE_ERROR` on
 *         failure.
 */
enum EmberAssemblerPhaseResult ember_do_generate_executable_phase(
    struct EmberIrNode *ir_root,
    struct EmberExecutable **out_exe,
    struct EmberLogger *logger
);

#endif