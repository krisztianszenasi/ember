#include <ember/core/opcode.h>
#include <stdint.h>

static struct EmberInstructionInfo INSTRUCTION_INFO[] = {
    /* constants and stack */
    [OP_LDC] = {
        .name = "ldc",
        .operand_count = 1,
        .operands = { OPERAND_U8 },
    },
    [OP_LDC_W] = {
        .name = "ldc_w",
        .operand_count = 1,
        .operands = { OPERAND_U16 },
    },
    [OP_DUP] = {
        .name = "dup",
        .operand_count = 1,
        .operands = { OPERAND_U8 }
    },
    [OP_POP] = {
        .name = "pop",
        .operand_count = 1,
        .operands = { OPERAND_U8 },
    },

    /* argument, local, and global access */
    [OP_GETA] = {
        .name = "geta",
        .operand_count = 1,
        .operands = { OPERAND_U8 },
    },
    [OP_GETL] = {
        .name = "getl",
        .operand_count = 1,
        .operands = { OPERAND_U8 },
    },
    [OP_SETL] = {
        .name = "setl",
        .operand_count = 1,
        .operands = { OPERAND_U8 }
    },
    [OP_GETG] = {
        .name = "getg",
        .operand_count = 1,
        .operands = { OPERAND_U16 }
    },
    [OP_SETG] = {
        .name = "setg",
        .operand_count = 1,
        .operands = { OPERAND_U16 }
    },

    /* integer arithmetic */
    [OP_IADD] = {
        .name = "iadd",
        .operand_count = 0,
    },
    [OP_ISUB] = {
        .name = "isub",
        .operand_count = 0,
    },
    [OP_IMUL] = {
        .name = "imul",
        .operand_count = 0,
    },
    [OP_IDIV] = {
        .name = "idiv",
        .operand_count = 0,
    },
    [OP_IREM] = {
        .name = "irem",
        .operand_count = 0,
    },

    /* floating-point arithmetic */
    [OP_FADD] = {
        .name = "fadd",
        .operand_count = 0,
    },
    [OP_FSUB] = {
        .name = "fsub",
        .operand_count = 0,
    },
    [OP_FMUL] = {
        .name = "fmul",
        .operand_count = 0,
    },
    [OP_FDIV] = {
        .name = "fdiv",
        .operand_count = 0,
    },

    /* string operations */
    [OP_CONCAT] = {
        .name = "concat",
        .operand_count = 0,
    },

    /* logical operations */
    [OP_AND] = {
        .name = "and",
        .operand_count = 0,
    },
    [OP_OR] = {
        .name = "or",
        .operand_count = 0,
    },
    [OP_NOT] = {
        .name = "not",
        .operand_count = 0,
    },

    /* comparisons */
    [OP_EQ] = {
        .name = "eq",
        .operand_count = 0,
    },
    [OP_NEQ] = {
        .name = "neq",
        .operand_count = 0,
    },
    [OP_GT] = {
        .name = "gt",
        .operand_count = 0,
    },
    [OP_GTE] = {
        .name = "gte",
        .operand_count = 0,
    },
    [OP_LT] = {
        .name = "lt",
        .operand_count = 0,
    },
    [OP_LTE] = {
        .name = "lte",
        .operand_count = 0,
    },

    /* control flow */
    [OP_JMP] = {
        .name = "jmp",
        .operand_count = 1,
        .operands = { OPERAND_I16 }
    },
    [OP_JMP_IF_FALSE] = {
        .name = "jmp_if_false",
        .operand_count = 1,
        .operands = { OPERAND_I16 }
    },

    /* calls */
    [OP_CALL] = {
        .name = "call",
        .operand_count = 1,
        .operands = { OPERAND_I16 },
    },
    [OP_RET] = {
        .name = "ret",
        .operand_count = 0,
    },
    [OP_HALT] = {
        .name = "halt",
        .operand_count = 0,
    },

    /* io */
    [OP_PRINT] = {
        .name = "print",
        .operand_count = 0,
    },
    [OP_INPUT] = {
        .name = "input",
        .operand_count = 0,
    },

    /* status code related */
    [OP_STATUS_OK] = {
        .name = "status_ok",
        .operand_count = 0,
    },
    [OP_STATUS_CODE] = {
        .name = "status_code",
        .operand_count = 0,
    },
    [OP_STATUS_CLEAR] = {
        .name = "status_clear",
        .operand_count = 0,
    },

    /* cast instructions */
    [OP_I2F] = {
        .name = "i2f",
        .operand_count = 0,
    },
    [OP_F2I] = {
        .name = "f2i",
        .operand_count = 0,
    },
    [OP_I2B] = {
        .name = "i2b",
        .operand_count = 0,
    },
    [OP_F2B] = {
        .name = "f2b",
        .operand_count = 0,
    },
    [OP_S2B] = {
        .name = "s2b",
        .operand_count = 0,
    },
    [OP_N2B] = {
        .name = "n2b",
        .operand_count = 0,
    },
    [OP_I2S] = {
        .name = "i2s",
        .operand_count = 0,
    },
    [OP_F2S] = {
        .name = "f2s",
        .operand_count = 0,
    },
    [OP_B2S] = {
        .name = "b2s",
        .operand_count = 0,
    },
    [OP_N2S] = {
        .name = "n2s",
        .operand_count = 0,
    },

    /* parse instructions */
    [OP_PARSE_BOOL] = {
        .name = "parse_bool",
        .operand_count = 0,
    },
    [OP_PARSE_FLOAT] = {
        .name = "parse_float",
        .operand_count = 0,
    },
    [OP_PARSE_INT] = {
        .name = "parse_int",
        .operand_count = 0,
    },
};

size_t ember_opcode_operand_kind_get_size(enum EmberOperandKind operand_kind) {
    switch (operand_kind) {
        case OPERAND_NONE:
            return 0;
        case OPERAND_U8:
            return sizeof(uint8_t);
        case OPERAND_U16:
            return sizeof(uint16_t);
        case OPERAND_I16:
            return sizeof(int16_t);
        default:
            return 0;
    }
}

struct EmberInstructionInfo ember_opcode_get_instruction_info(enum EmberOpcode opcode) {
    return INSTRUCTION_INFO[opcode];
}
