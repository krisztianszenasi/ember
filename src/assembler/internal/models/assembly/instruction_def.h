#ifndef EMBER_ASSEMBLER_INTERNAL_MODEL_ASSEMBLY_INSTRUCTION_DEF_H
#define EMBER_ASSEMBLER_INTERNAL_MODEL_ASSEMBLY_INSTRUCTION_DEF_H

#include <stdint.h>

#include <ember/assembly/instruction_info.h>
#include "assembler/internal/models/assembly/opcode.h"

/**
 * @brief Returns instruction metadata for an assembly opcode.
 *
 * @param opcode Assembly opcode whose metadata should be retrieved.
 * @return Instruction metadata for the opcode, or NULL if the opcode is invalid
 *         or has no metadata entry.
 */
const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_by_opcode(
    enum EmberAssemblyOpcode opcode
);

#endif