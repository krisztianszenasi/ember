#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_ASSEMBLY_OPCODE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_ASSEMBLY_OPCODE_H

/**
 * @brief Assembly-level opcode recognized by the assembler.
 *
 * These opcodes represent instructions as they appear in Ember assembly source.
 * They are used by the assembler's AST, semantic analysis, and lowering passes.
 *
 * Assembly opcodes are distinct from VM or bytecode opcodes, which may use a
 * different representation after lowering or executable generation.
 */
enum EmberAssemblyOpcode {
    /* Constants and stack operations. */
    EMBER_ASM_OPCODE_LDC,
    EMBER_ASM_OPCODE_DUP,
    EMBER_ASM_OPCODE_POP,

    /* Argument, local, and global access. */
    EMBER_ASM_OPCODE_GETA,
    EMBER_ASM_OPCODE_GETL,
    EMBER_ASM_OPCODE_SETL,
    EMBER_ASM_OPCODE_GETG,
    EMBER_ASM_OPCODE_SETG,

    /* Integer arithmetic. */
    EMBER_ASM_OPCODE_IADD,
    EMBER_ASM_OPCODE_ISUB,
    EMBER_ASM_OPCODE_IMUL,
    EMBER_ASM_OPCODE_IDIV,
    EMBER_ASM_OPCODE_IREM,

    /* Floating-point arithmetic. */
    EMBER_ASM_OPCODE_FADD,
    EMBER_ASM_OPCODE_FSUB,
    EMBER_ASM_OPCODE_FMUL,
    EMBER_ASM_OPCODE_FDIV,

    /* String operations */
    EMEBR_ASM_OPCODE_CONCAT,

    /* Logical operations. */
    EMBER_ASM_OPCODE_AND,
    EMBER_ASM_OPCODE_OR,
    EMBER_ASM_OPCODE_NOT,

    /* Comparisons. */
    EMBER_ASM_OPCODE_EQ,
    EMBER_ASM_OPCODE_NEQ,
    EMBER_ASM_OPCODE_GT,
    EMBER_ASM_OPCODE_GTE,
    EMBER_ASM_OPCODE_LT,
    EMBER_ASM_OPCODE_LTE,

    /* Control flow. */
    EMBER_ASM_OPCODE_JMP,
    EMBER_ASM_OPCODE_JMP_IF_FALSE,

    /* Function calls and returns. */
    EMBER_ASM_OPCODE_CALL,
    EMBER_ASM_OPCODE_RET,
    EMBER_ASM_OPCODE_HALT,

    /* Input/output. */
    EMBER_ASM_OPCODE_PRINT,
    EMBER_ASM_OPCODE_INPUT,

    /* Status code relataed. */
    EMBER_ASM_OPCODE_STATUS_OK,
    EMBER_ASM_OPCODE_STATUS_CODE,
    EMBER_ASM_OPCODE_STATUS_CLEAR,

    /* Cast operations. */
    EMBER_ASM_OPCODE_I2F,
    EMBER_ASM_OPCODE_F2I,
    EMBER_ASM_OPCODE_I2B,
    EMBER_ASM_OPCODE_F2B,
    EMBER_ASM_OPCODE_S2B,
    EMBER_ASM_OPCODE_N2B,
    EMBER_ASM_OPCODE_I2S,
    EMBER_ASM_OPCODE_F2S,
    EMBER_ASM_OPCODE_B2S,
    EMBER_ASM_OPCODE_N2S,

    /* parse instructions */
    EMBER_ASM_OPCODE_PARSE_BOOL,
    EMBER_ASM_OPCODE_PARSE_FLOAT,
    EMBER_ASM_OPCODE_PARSE_INT,
};

#endif