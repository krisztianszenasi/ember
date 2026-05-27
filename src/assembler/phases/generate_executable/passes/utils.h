#ifndef EMBER_ASSEMBLER_PHASES_GENERATE_EXECTUABLE_PASSES_UTILS_H
#define EMBER_ASSEMBLER_PHASES_GENERATE_EXECTUABLE_PASSES_UTILS_H

#include <ember/core/opcode.h>

#include "assembler/internal/models/assembly/opcode.h"

/**
 * @brief Converts an assembler opcode to its executable bytecode opcode.
 *
 * The assembler and VM bytecode layers use separate opcode enums. This helper
 * maps an assembly-level opcode to the corresponding bytecode opcode emitted
 * into the generated executable.
 *
 * @param opcode Assembly opcode to convert.
 * @return Matching bytecode opcode.
 */
enum EmberOpcode convert_assembly_opcode_to_bytecode_opcode(
    enum EmberAssemblyOpcode opcode
);

#endif