#ifndef EMBER_VM_INTERNAL_MODELS_PROCESS_ERROR_H
#define EMBER_VM_INTERNAL_MODELS_PROCESS_ERROR_H

/**
 * @brief Kind of fatal runtime error raised by a VM process.
 *
 * Runtime errors represent unrecoverable execution failures. When one of these
 * errors is raised, the process enters the error state and bytecode execution
 * stops.
 */
enum EmberRuntimeErrorKind {
    /**
     * @brief No runtime error has occurred.
     */
    EMBER_RUNTIME_ERROR_NONE,

    /**
    * @brief The process tried to read past the end of a function's bytecode.
    *
    * This usually means an instruction was incomplete, for example an opcode that
    * expected an operand but reached the end of the bytecode stream first.
    */
    EMBER_RUNTIME_ERROR_UNEXPECTED_END_OF_BYTECODE,

    /**
     * @brief An instruction tried to read more values than were available on
     * the process stack.
     */
    EMBER_RUNTIME_ERROR_STACK_UNDERFLOW,

    /**
     * @brief An instruction tried to push more values than the process stack
     * can hold.
     */
    EMBER_RUNTIME_ERROR_STACK_OVERFLOW,

    /**
     * @brief A function return was attempted without a caller frame.
     */
    EMBER_RUNTIME_ERROR_CALL_FRAME_UNDERFLOW,

    /**
     * @brief A function call would exceed the maximum call frame depth.
     */
    EMBER_RUNTIME_ERROR_CALL_FRAME_OVERFLOW,

    /**
     * @brief The process encountered an unknown or unsupported opcode.
     */
    EMBER_RUNTIME_ERROR_INVALID_OPCODE,

    /**
     * @brief An instruction referenced a constant index outside the
     * executable's constant pool.
     */
    EMBER_RUNTIME_ERROR_INVALID_CONSTANT_INDEX,

    /**
     * @brief An instruction referenced an argument index outside the current
     * function's argument range.
     */
    EMBER_RUNTIME_ERROR_INVALID_ARG_INDEX,

    /**
     * @brief An instruction referenced a local index outside the current
     * function's local range.
     */
    EMBER_RUNTIME_ERROR_INVALID_LOCAL_INDEX,

    /**
     * @brief An instruction referenced a function index outside the executable's
     * function table.
     */
    EMBER_RUNTIME_ERROR_INVALID_FUNCTION_INDEX,

    /**
     * @brief An instruction referenced a global index outside the process's
     * global storage.
     */
    EMBER_RUNTIME_ERROR_INVALID_GLOBAL_INDEX,

    /**
     * Total number of runtime error codes.
     */
    EMBER_RUNTIME_ERROR_COUNT,
};

/**
 * @brief Fatal runtime error information for a process.
 */
struct EmberProcessError {
    /**
     * @brief Human-readable runtime error message.
     */
    const char *message;

    /**
     * @brief Machine-readable runtime error kind.
     */
    enum EmberRuntimeErrorKind kind;
};

/**
 * @brief Sets the runtime error information.
 *
 * @param self Error object to update.
 * @param message Human-readable error message.
 * @param kind Runtime error kind.
 */
void ember_process_error_set(
    struct EmberProcessError *self,
    const char *message,
    enum EmberRuntimeErrorKind kind
);

/**
 * @brief Returns the default message for a runtime error kind.
 *
 * @param kind Runtime error kind.
 * @return Static human-readable error message.
 */
const char *ember_runtime_error_message(enum EmberRuntimeErrorKind kind);

#endif