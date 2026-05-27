#ifndef EMBER_CORE_OPCODE_H
#define EMBER_CORE_OPCODE_H

#include <stddef.h>

#include <ember/assembly/instruction_info.h>

/**
 * @brief Bytecode operation code.
 *
 * Opcodes describe the executable instructions understood by the VM. Each
 * opcode may have zero or more encoded operands, described by
 * `EmberInstructionInfo`.
 */
enum EmberOpcode {
    /**
     * @brief Load a constant using a short constant-pool index.
     */
    OP_LDC,

    /**
     * @brief Load a constant using a wide constant-pool index.
     */
    OP_LDC_W,

    /**
     * @brief Duplicate one or more values from the top of the stack.
     */
    OP_DUP,

    /**
     * @brief Pop and discard one or more values from the top of the stack.
     */
    OP_POP,

    /**
     * @brief Push a function argument value onto the stack.
     */
    OP_GETA,

    /**
     * @brief Push a local variable value onto the stack.
     */
    OP_GETL,

    /**
     * @brief Pop a value from the stack and store it in a local variable.
     */
    OP_SETL,

    /**
     * @brief Push a global variable value onto the stack.
     */
    OP_GETG,

    /**
     * @brief Pop a value from the stack and store it in a global variable.
     */
    OP_SETG,

    /**
     * @brief Add two integer values.
     */
    OP_IADD,

    /**
     * @brief Subtract two integer values.
     */
    OP_ISUB,

    /**
     * @brief Multiply two integer values.
     */
    OP_IMUL,

    /**
     * @brief Divide two integer values.
     */
    OP_IDIV,

    /**
     * @brief Compute the integer remainder of two integer values.
     */
    OP_IREM,

    /**
     * @brief Add two floating-point values.
     */
    OP_FADD,

    /**
     * @brief Subtract two floating-point values.
     */
    OP_FSUB,

    /**
     * @brief Concats two string objects.
     */
    OP_CONCAT,

    /**
     * @brief Multiply two floating-point values.
     */
    OP_FMUL,

    /**
     * @brief Divide two floating-point values.
     */
    OP_FDIV,

    /**
     * @brief Compute logical AND.
     */
    OP_AND,

    /**
     * @brief Compute logical OR.
     */
    OP_OR,

    /**
     * @brief Compute logical NOT.
     */
    OP_NOT,

    /**
     * @brief Compare two values for equality.
     */
    OP_EQ,

    /**
     * @brief Compare two values for inequality.
     */
    OP_NEQ,

    /**
     * @brief Compare whether the left value is greater than the right value.
     */
    OP_GT,

    /**
     * @brief Compare whether the left value is greater than or equal to the
     * right value.
     */
    OP_GTE,

    /**
     * @brief Compare whether the left value is less than the right value.
     */
    OP_LT,

    /**
     * @brief Compare whether the left value is less than or equal to the right
     * value.
     */
    OP_LTE,

    /**
     * @brief Jump by a relative bytecode offset.
     */
    OP_JMP,

    /**
     * @brief Jump by a relative bytecode offset if the popped condition is
     * false.
     */
    OP_JMP_IF_FALSE,

    /**
     * @brief Call a function by function table index.
     */
    OP_CALL,

    /**
     * @brief Return from the current function.
     */
    OP_RET,

    /**
     * @brief Stop the current process normally.
     */
    OP_HALT,

    /**
     * @brief Print a value.
     */
    OP_PRINT,

    /**
     * @brief Read an input value.
     */
    OP_INPUT,

    /**
     * @brief Pushes true if the most recent status-producing instruction succeeded.
     */
    OP_STATUS_OK,

    /**
    * @brief Pushes the status code produced by the most recent status-producing instruction.
    */
    OP_STATUS_CODE,

    /**
    * @brief Clears the most recent status information.
    */
    OP_STATUS_CLEAR,

    /**
     * @brief Converts the integer value on top of the stack to a floating-point value.
     */
    OP_I2F,

    /**
     * @brief Converts the floating-point value on top of the stack to an integer value.
     */
    OP_F2I,

    /**
     * @brief Converts the integer value on top of the stack to a boolean value.
     */
    OP_I2B,

    /**
     * @brief Converts the floating-point value on top of the stack to a boolean value.
     */
    OP_F2B,

    /**
     * @brief Converts the string object on top of the stack to a boolean value.
     */
    OP_S2B,

    /**
     * @brief Converts the none value on top of the stack to a boolean value.
     */
    OP_N2B,

    /**
     * @brief Converts the integer value on top of the stack to a string object.
     */
    OP_I2S,

    /**
     * @brief Converts the floating-point value on top of the stack to a string object.
     */
    OP_F2S,

    /**
     * @brief Converts the boolean value on top of the stack to a string object.
     */
    OP_B2S,

    /**
     * @brief Converts the none value on top of the stack to a string object.
     */
    OP_N2S,

    /**
     * @brief Parses a string to bool.
     */
    OP_PARSE_BOOL,
    
    /**
     * @brief Parses a string to float.
     */
    OP_PARSE_FLOAT,
    
    /**
     * @brief Parses a string to int.
     */
    OP_PARSE_INT,
};

/**
 * @brief Encoded operand kind used by a bytecode instruction.
 */
enum EmberOperandKind {
    /**
     * @brief No operand.
     */
    OPERAND_NONE = 0,

    /**
     * @brief Unsigned 8-bit operand.
     */
    OPERAND_U8,

    /**
     * @brief Unsigned 16-bit operand.
     */
    OPERAND_U16,

    /**
     * @brief Signed 16-bit operand.
     */
    OPERAND_I16,
};

/**
 * @brief Returns the encoded byte size of an operand kind.
 *
 * @param operand_kind Operand kind to inspect.
 * @return Operand size in bytes. Returns 0 for `OPERAND_NONE`.
 */
size_t ember_opcode_operand_kind_get_size(enum EmberOperandKind operand_kind);

/**
 * @brief Metadata describing one opcode's encoded instruction format.
 */
struct EmberInstructionInfo {
    /**
     * @brief Human-readable opcode name.
     */
    const char *name;

    /**
     * @brief Number of encoded operands used by the instruction.
     */
    size_t operand_count;

    /**
     * @brief Encoded operand kinds.
     *
     * Only the first `operand_count` entries are meaningful.
     */
    enum EmberOperandKind operands[EMBER_MAX_INSTRUCTION_OPERANDS];
};

/**
 * @brief Returns metadata for an opcode.
 *
 * The returned metadata describes the opcode name and encoded operand layout.
 *
 * @param opcode Opcode to inspect.
 * @return Instruction metadata for the opcode.
 */
struct EmberInstructionInfo ember_opcode_get_instruction_info(
    enum EmberOpcode opcode
);

#endif