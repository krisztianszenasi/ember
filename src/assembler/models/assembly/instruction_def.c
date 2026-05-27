#include <stddef.h>
#include <string.h>

#include <ember/assembly/instruction_info.h>

#include "assembler/internal/models/assembly/opcode.h"
#include "assembler/internal/models/assembly/instruction_def.h"

const struct EmberAssemblyInstructionInfo INSTRUCTIONS[] = {
    /* Constants and stack operations. */
    [EMBER_ASM_OPCODE_LDC] = {
        .name = "ldc",
        .description = "Loads a value onto the stack.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },
    [EMBER_ASM_OPCODE_DUP] = {
        .name = "dup",
        .description = "Duplicates stack values.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_INT,
                .validator = EMBER_OPERAND_VALIDATOR_POSITIVE_INT,
            },
        },
    },
    [EMBER_ASM_OPCODE_POP] = {
        .name = "pop",
        .description = "Removes values from the stack.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_INT,
                .validator = EMBER_OPERAND_VALIDATOR_POSITIVE_INT,
            },
        },
    },

    /* Argument, local, and global access. */
    [EMBER_ASM_OPCODE_GETA] = {
        .name = "geta",
        .description = "Loads a function argument onto the stack.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_INT,
                .validator = EMBER_OPERAND_VALIDATOR_ARG_INDEX_IN_RANGE,
            },
        },
    },
    [EMBER_ASM_OPCODE_GETL] = {
        .name = "getl",
        .description = "Loads a local variable onto the stack.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_INT,
                .validator = EMBER_OPERAND_VALIDATOR_LOCAL_INDEX_IN_RANGE,
            },
        },
    },
    [EMBER_ASM_OPCODE_SETL] = {
        .name = "setl",
        .description = "Stores the top stack value into a local variable.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_VALUE,
                .value_kind = EMBER_VALUE_INT,
                .validator = EMBER_OPERAND_VALIDATOR_LOCAL_INDEX_IN_RANGE,
            },
        },
    },
    [EMBER_ASM_OPCODE_GETG] = {
        .name = "getg",
        .description = "Loads a global variable onto the stack.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_GLOBAL,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },
    [EMBER_ASM_OPCODE_SETG] = {
        .name = "setg",
        .description = "Stores the top stack value into a global variable.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_GLOBAL,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },

    /* Integer arithmetic. */
    [EMBER_ASM_OPCODE_IADD] = {
        .name = "iadd",
        .description = "Adds two integer values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_ISUB] = {
        .name = "isub",
        .description = "Subtracts two integer values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_IMUL] = {
        .name = "imul",
        .description = "Multiplies two integer values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_IDIV] = {
        .name = "idiv",
        .description = "Divides two integer values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_IREM] = {
        .name = "irem",
        .description = "Computes the integer remainder.",
        .operand_count = 0,
    },

    /* Floating-point arithmetic. */
    [EMBER_ASM_OPCODE_FADD] = {
        .name = "fadd",
        .description = "Adds two floating-point values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_FSUB] = {
        .name = "fsub",
        .description = "Subtracts two floating-point values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_FMUL] = {
        .name = "fmul",
        .description = "Multiplies two floating-point values.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_FDIV] = {
        .name = "fdiv",
        .description = "Divides two floating-point values.",
        .operand_count = 0,
    },

    /* String operations. */
    [EMEBR_ASM_OPCODE_CONCAT] = {
        .name = "concat",
        .description = "Concats two string objects.",
        .operand_count = 0,
    },

    /* Logical operations. */
    [EMBER_ASM_OPCODE_AND] = {
        .name = "and",
        .description = "Computes logical AND.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_OR] = {
        .name = "or",
        .description = "Computes logical OR.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_NOT] = {
        .name = "not",
        .description = "Computes logical NOT.",
        .operand_count = 0,
    },

    /* Comparisons. */
    [EMBER_ASM_OPCODE_EQ] = {
        .name = "eq",
        .description = "Compares two values for equality.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_NEQ] = {
        .name = "neq",
        .description = "Compares two values for inequality.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_GT] = {
        .name = "gt",
        .description = "Checks whether one value is greater than another.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_GTE] = {
        .name = "gte",
        .description = "Checks whether one value is greater than or equal to another.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_LT] = {
        .name = "lt",
        .description = "Checks whether one value is less than another.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_LTE] = {
        .name = "lte",
        .description = "Checks whether one value is less than or equal to another.",
        .operand_count = 0,
    },

    /* Control flow. */
    [EMBER_ASM_OPCODE_JMP] = {
        .name = "jmp",
        .description = "Jumps to a label.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_LABEL,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },
    [EMBER_ASM_OPCODE_JMP_IF_FALSE] = {
        .name = "jmp_if_false",
        .description = "Jumps to a label if the top stack value is false.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_LABEL,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },

    /* Function calls and returns. */
    [EMBER_ASM_OPCODE_CALL] = {
        .name = "call",
        .description = "Calls a function.",
        .operand_count = 1,
        .operands = {
            [0] = {
                .operand_kind = EMBER_OPERAND_FUNCTION,
                .value_kind = EMBER_VALUE_ANY,
                .validator = EMBER_OPERAND_VALIDATOR_NONE,
            },
        },
    },
    [EMBER_ASM_OPCODE_RET] = {
        .name = "ret",
        .description = "Returns from the current function.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_HALT] = {
        .name = "halt",
        .description = "Stops the VM.",
        .operand_count = 0,
    },

    /* Input/output. */
    [EMBER_ASM_OPCODE_PRINT] = {
        .name = "print",
        .description = "Prints the top stack value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_INPUT] = {
        .name = "input",
        .description = "Reads an input value and pushes it onto the stack.",
        .operand_count = 0,
    },

    [EMBER_ASM_OPCODE_STATUS_OK] = {
        .name = "status_ok",
        .description = "Pushes boolean whether most recent instruction was successful or not",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_STATUS_CODE] = {
        .name = "status_code",
        .description = "Pushes the most recent status code.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_STATUS_CLEAR] = {
        .name = "status_clear",
        .description = "Clears the status code.",
        .operand_count = 0,
    },

    /* Cast operations. */
    [EMBER_ASM_OPCODE_I2F] = {
        .name = "i2f",
        .description = "Casts an int value to a float value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_F2I] = {
        .name = "f2i",
        .description = "Casts a float value to an int value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_I2B] = {
        .name = "i2b",
        .description = "Casts an int value to a boolean value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_F2B] = {
        .name = "f2b",
        .description = "Casts a float value to a boolean value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_S2B] = {
        .name = "s2b",
        .description = "Casts a string value to a boolean value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_N2B] = {
        .name = "n2b",
        .description = "Casts a none value to a boolean value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_I2S] = {
        .name = "i2s",
        .description = "Casts an int value to a string value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_F2S] = {
        .name = "f2s",
        .description = "Casts a float value to a string value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_B2S] = {
        .name = "b2s",
        .description = "Casts a boolean value to a string value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_N2S] = {
        .name = "n2s",
        .description = "Casts a none value to a string value.",
        .operand_count = 0,
    },

    /* Parse operations. */
    [EMBER_ASM_OPCODE_PARSE_BOOL] = {
        .name = "parse_bool",
        .description = "Parses a string value as a boolean value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_PARSE_FLOAT] = {
        .name = "parse_float",
        .description = "Parses a string value as a float value.",
        .operand_count = 0,
    },
    [EMBER_ASM_OPCODE_PARSE_INT] = {
        .name = "parse_int",
        .description = "Parses a string value as an int value.",
        .operand_count = 0,
    },
};

const char *ember_assembly_operand_validator_description(
    enum EmberAssemblyOperandValidatorKind validator
) {
    switch (validator) {
        case EMBER_OPERAND_VALIDATOR_NONE:
            return NULL;

        case EMBER_OPERAND_VALIDATOR_POSITIVE_INT:
            return "The value must be greater than zero.";

        case EMBER_OPERAND_VALIDATOR_NON_NEGATIVE_INT:
            return "The value must be zero or greater.";

        case EMBER_OPERAND_VALIDATOR_ARG_INDEX_IN_RANGE:
            return "The value must refer to an existing argument in the current function.";

        case EMBER_OPERAND_VALIDATOR_LOCAL_INDEX_IN_RANGE:
            return "The value must refer to an existing local in the current function.";
    }

    return NULL;
}

size_t ember_assembly_instruction_info_count(void) {
    return sizeof(INSTRUCTIONS) / sizeof(INSTRUCTIONS[0]);
}

const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_at(size_t index) {
    if (index >= ember_assembly_instruction_info_count()) {
        return NULL;
    }
    if (INSTRUCTIONS[index].name == NULL) {
        return NULL;
    }
    return &INSTRUCTIONS[index];
}

const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_by_name(
    const char *name
) {
    if (name == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < ember_assembly_instruction_info_count(); i++) {
        const struct EmberAssemblyInstructionInfo *info =
            ember_assembly_instruction_info_at(i);
        if (info != NULL && strcmp(info->name, name) == 0) {
            return info;
        }
    }
    return NULL;
}

const struct EmberAssemblyInstructionInfo *ember_assembly_instruction_info_by_opcode(
    enum EmberAssemblyOpcode opcode
) {
    return &INSTRUCTIONS[opcode];
}