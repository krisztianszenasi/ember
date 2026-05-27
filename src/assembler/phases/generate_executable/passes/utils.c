#include "utils.h"

enum EmberOpcode convert_assembly_opcode_to_bytecode_opcode(enum EmberAssemblyOpcode opcode) {
    switch (opcode) {
        /* Constants and stack operations. */
        case EMBER_ASM_OPCODE_LDC:
            return OP_LDC;
        case EMBER_ASM_OPCODE_DUP:
            return OP_DUP;
        case EMBER_ASM_OPCODE_POP:
            return OP_POP;

        /* Argument, local, and global access. */
        case EMBER_ASM_OPCODE_GETA:
            return OP_GETA;
        case EMBER_ASM_OPCODE_GETL:
            return OP_GETL;
        case EMBER_ASM_OPCODE_SETL:
            return OP_SETL;
        case EMBER_ASM_OPCODE_GETG:
            return OP_GETG;
        case EMBER_ASM_OPCODE_SETG:
            return OP_SETG;

        /* Integer arithmetic. */
        case EMBER_ASM_OPCODE_IADD:
            return OP_IADD;
        case EMBER_ASM_OPCODE_ISUB:
            return OP_ISUB;
        case EMBER_ASM_OPCODE_IMUL:
            return OP_IMUL;
        case EMBER_ASM_OPCODE_IDIV:
            return OP_IDIV;
        case EMBER_ASM_OPCODE_IREM:
            return OP_IREM;

        /* Floating-point arithmetic. */
        case EMBER_ASM_OPCODE_FADD:
            return OP_FADD;
        case EMBER_ASM_OPCODE_FSUB:
            return OP_FSUB;
        case EMBER_ASM_OPCODE_FMUL:
            return OP_FMUL;
        case EMBER_ASM_OPCODE_FDIV:
            return OP_FDIV;

        /* String operations*/
        case EMEBR_ASM_OPCODE_CONCAT:
            return OP_CONCAT;

        /* Logical operations. */
        case EMBER_ASM_OPCODE_AND:
            return OP_AND;
        case EMBER_ASM_OPCODE_OR:
            return OP_OR;
        case EMBER_ASM_OPCODE_NOT:
            return OP_NOT;

        /* Comparisons. */
        case EMBER_ASM_OPCODE_EQ:
            return OP_EQ;
        case EMBER_ASM_OPCODE_NEQ:
            return OP_NEQ;
        case EMBER_ASM_OPCODE_GT:
            return OP_GT;
        case EMBER_ASM_OPCODE_GTE:
            return OP_GTE;
        case EMBER_ASM_OPCODE_LT:
            return OP_LT;
        case EMBER_ASM_OPCODE_LTE:
            return OP_LTE;

        /* Control flow. */
        case EMBER_ASM_OPCODE_JMP:
            return OP_JMP;
        case EMBER_ASM_OPCODE_JMP_IF_FALSE:
            return OP_JMP_IF_FALSE;

        /* Function calls and returns. */
        case EMBER_ASM_OPCODE_CALL:
            return OP_CALL;
        case EMBER_ASM_OPCODE_RET:
            return OP_RET;
        case EMBER_ASM_OPCODE_HALT:
            return OP_HALT;

        /* Input/output. */
        case EMBER_ASM_OPCODE_PRINT: 
            return OP_PRINT;
        case EMBER_ASM_OPCODE_INPUT:
            return OP_INPUT;

        /* Status code related. */
        case EMBER_ASM_OPCODE_STATUS_OK:
            return OP_STATUS_OK;
        case EMBER_ASM_OPCODE_STATUS_CODE:
            return OP_STATUS_CODE;
        case EMBER_ASM_OPCODE_STATUS_CLEAR:
            return OP_STATUS_CLEAR;

        /* Cast operations. */
        case EMBER_ASM_OPCODE_I2F:
            return OP_I2F;
        case EMBER_ASM_OPCODE_F2I:
            return OP_F2I;
        case EMBER_ASM_OPCODE_I2B:
            return OP_I2B;
        case EMBER_ASM_OPCODE_F2B:
            return OP_F2B;
        case EMBER_ASM_OPCODE_S2B:
            return OP_S2B;
        case EMBER_ASM_OPCODE_N2B:
            return OP_N2B;
        case EMBER_ASM_OPCODE_I2S:
            return OP_I2S;
        case EMBER_ASM_OPCODE_F2S:
            return OP_F2S;
        case EMBER_ASM_OPCODE_B2S:
            return OP_B2S;
        case EMBER_ASM_OPCODE_N2S:
            return OP_N2S;

        /* parse instructions */
        case EMBER_ASM_OPCODE_PARSE_BOOL:
            return OP_PARSE_BOOL;            
        case EMBER_ASM_OPCODE_PARSE_FLOAT:
            return OP_PARSE_FLOAT;            
        case EMBER_ASM_OPCODE_PARSE_INT:
            return OP_PARSE_INT;                    
    }
}
