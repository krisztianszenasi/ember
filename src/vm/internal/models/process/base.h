#ifndef EMBER_VM_INTERNAL_MODELS_PROCESS_BASE_H
#define EMBER_VM_INTERNAL_MODELS_PROCESS_BASE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ember/vm.h>
#include <ember/core/models/logger.h>
#include <ember/core/models/value/base.h>
#include <ember/core/models/object/base.h>

#include "vm/internal/limits.h"
#include "vm/internal/models/call_frame.h"
#include "vm/internal/models/process/error.h"
#include "vm/internal/models/process/result.h"

/**
 * @brief Execution state of a VM process.
 *
 * A process represents one running instance of an executable. Its state
 * controls whether it can be executed, is currently executing, has completed
 * normally, or has stopped because of a fatal runtime error.
 */
enum EmberProcessState {
    /**
     * @brief The process has been allocated but is not ready to run yet.
     */
    EMBER_PROCESS_CREATED,

    /**
     * @brief The process has been initialized and can be executed.
     */
    EMBER_PROCESS_READY,

    /**
     * @brief The process is currently executing bytecode.
     */
    EMBER_PROCESS_RUNNING,

    /**
     * @brief The process stopped normally.
     */
    EMBER_PROCESS_HALTED,

    /**
     * @brief The process stopped because of a fatal runtime error.
     */
    EMBER_PROCESS_ERROR,
};

/**
 * @brief Runtime state for one executable instance.
 *
 * The process owns all execution-specific state: call frames, operand stack,
 * globals, recoverable operation status, and fatal runtime error information.
 */
struct EmberProcess {
    /**
     * @brief Current execution state of the process.
     */
    enum EmberProcessState state;

    /**
     * @brief Status code of the last recoverable VM operation.
     *
     * This is visible to bytecode through status-related instructions.
     */
    enum EmberProcessStatusCode status;

    /**
     * @brief Fatal runtime error information.
     *
     * This is set when the process enters `EMBER_PROCESS_ERROR`.
     */
    struct EmberProcessError error;

    /**
     * @brief Executable currently being run by this process.
     *
     * The process borrows this pointer. It does not own the executable.
     */
    const struct EmberExecutable *executable;

    /**
     * @brief Number of active call frames.
     */
    size_t frame_count;

    /**
     * @brief Fixed-size call frame stack.
     */
    struct EmberCallFrame frames[EMBER_VM_CALL_FRAME_MAX];

    /**
     * @brief Fixed-size operand/value stack.
     */
    struct EmberValue stack[EMBER_VM_STACK_MAX];

    /**
     * @brief Pointer to the next free stack slot.
     *
     * The top value is stored at `stack_top[-1]` when the stack is not empty.
     */
    struct EmberValue *stack_top;

    /**
     * @brief Number of allocated global slots.
     */
    size_t global_capacity;

    /**
     * @brief Runtime global variable storage.
     *
     * Globals are indexed slots owned by the process.
     */
    struct EmberValue *globals;

    /**
     * @brief Linked list of all objects allocated by the process.
     */
    struct EmberObject *objects;

    /**
     * @brief Total number of heap bytes allocated for managed objects.
     */
    size_t bytes_allocated;

    /**
     * @brief Allocation threshold for the next garbage collection.
     */
    size_t next_gc;
};

/**
 * @brief Creates a new process for an executable.
 *
 * The returned process owns its runtime state, including stack, frames, and
 * global storage. The executable pointer is borrowed and must outlive the
 * process.
 *
 * @param executable Executable to run.
 * @return Newly allocated process, or `NULL` on allocation failure.
 */
struct EmberProcess *ember_process_new(struct EmberExecutable *executable);

/**
 * @brief Destroys a process and releases its owned runtime storage.
 *
 * @param self Process to destroy. May be `NULL`.
 */
void ember_process_destroy(struct EmberProcess *self);

/**
 * @brief Runs a VM process until it stops.
 *
 * The process executes bytecode starting from its current call frame. Execution
 * continues until the process halts normally or encounters a fatal runtime
 * error.
 *
 * @param self Process to run.
 * @return `EMBER_PROCESS_RUN_HALTED` if the process stopped normally,
 * otherwise `EMBER_PROCESS_RUN_ERROR`.
 */
enum EmberProcessRunResult ember_process_run(
    struct EmberProcess *self,
    struct EmberLogger *logger
);

/**
 * @brief Takes ownership of an object and adds it to the managed heap.
 *
 * The object is linked into the process-managed object list and becomes
 * eligible for garbage collection. This function also updates heap accounting
 * and may trigger a garbage collection if the configured allocation threshold
 * is reached.
 *
 * After this call succeeds, the caller must not free the object manually.
 *
 * @param self Process that will manage the object.
 * @param object Object to add to the managed heap.
 */
void ember_process_take_object(
    struct EmberProcess *self,
    struct EmberObject *object,
    struct EmberLogger *logger
);

/**
 * @brief Puts the process into a fatal runtime error state.
 *
 * This records the runtime error kind and marks the process as failed. Fatal
 * runtime errors stop execution and are not recoverable from bytecode.
 *
 * @param self Process that failed.
 * @param kind Runtime error kind.
 */
void ember_process_runtime_error(
    struct EmberProcess *self,
    enum EmberRuntimeErrorKind kind
);

/**
 * @brief Calls a function by executable function index.
 *
 * This validates the function index, checks call frame capacity, verifies that
 * enough arguments are available on the stack, reserves local slots, and
 * creates a new call frame.
 *
 * Arguments are expected to already be on the stack. The new frame's slots
 * pointer is set to the first argument.
 *
 * @param self Process performing the call.
 * @param function_idx Function index in the executable.
 * @param out Optional output pointer receiving the new call frame.
 * @return `true` on success, `false` if a fatal runtime error was raised.
 */
static inline bool ember_process_call_function(
    struct EmberProcess *self,
    size_t function_idx,
    struct EmberCallFrame **out
) {
    size_t function_count = ember_executable_get_function_count(self->executable);

    if (function_idx >= function_count) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_INVALID_FUNCTION_INDEX);
        return false;
    }

    if (self->frame_count >= EMBER_VM_CALL_FRAME_MAX) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_CALL_FRAME_OVERFLOW);
        return false;
    }

    const struct EmberFunction *function = ember_executable_get_function(self->executable, function_idx);
    size_t arity = ember_function_get_arity(function);
    size_t local_count = ember_function_get_local_count(function);
    size_t stack_count = (size_t)(self->stack_top - self->stack);

    if (stack_count < arity) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_UNDERFLOW);
        return false;
    }

    if (stack_count + local_count > EMBER_VM_STACK_MAX)  {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_OVERFLOW);
        return false;
    }

    struct EmberValue *slots = self->stack_top - arity;
    // TODO: initialize locals to nil/default value.
    self->stack_top += local_count;
    struct EmberCallFrame *new_frame = &self->frames[self->frame_count++];
    ember_call_frame_init(new_frame, function, slots);
    if (out != NULL) {
        *out = new_frame;
    }
    return true;
}

/**
 * @brief Returns from the current function.
 *
 * This pops the current call frame and returns the new current frame through
 * `out`. Returning from the last remaining frame is treated as call frame
 * underflow; the entry function should normally halt the process instead.
 *
 * @param self Process returning from a function.
 * @param out Optional output pointer receiving the new current frame.
 * @return `true` on success, `false` if a fatal runtime error was raised.
 */
static inline bool ember_process_return_function(
    struct EmberProcess *self,
    struct EmberCallFrame **out
) {
    if (self->frame_count <= 1) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_CALL_FRAME_UNDERFLOW);
        return false;
    }

    struct EmberCallFrame *current = &self->frames[self->frame_count - 1];
    size_t arity = ember_function_get_arity(current->function);
    size_t local_count = ember_function_get_local_count(current->function);
    struct EmberValue *frame_base = current->slots;
    struct EmberValue *frame_limit = frame_base + arity + local_count;
    bool has_return_value = self->stack_top > frame_limit;
    struct EmberValue return_value = EV_NONE();

    if (has_return_value) {
        return_value = self->stack_top[-1];
    }

    self->stack_top = frame_base;
    if (has_return_value) {
        *self->stack_top = return_value;
        self->stack_top++;
    }

    self->frame_count--;
    if (out != NULL) {
        *out = &self->frames[self->frame_count - 1];
    }
    return true;
}

/**
 * @brief Pushes a value onto the process stack.
 *
 * @param self Process whose stack is modified.
 * @param value Value to push.
 * @return `true` on success, `false` on stack overflow.
 */
static inline bool ember_process_push(
    struct EmberProcess *self,
    struct EmberValue value
) {
    size_t count = (size_t)(self->stack_top - self->stack);
    if (count >= EMBER_VM_STACK_MAX) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_OVERFLOW);
        return false;
    }
    *self->stack_top = value;
    self->stack_top++;
    return true;
}

/**
 * @brief Pops a value from the process stack.
 *
 * @param self Process whose stack is modified.
 * @param out Output pointer receiving the popped value.
 * @return `true` on success, `false` on stack underflow.
 */
static inline bool ember_process_pop(
    struct EmberProcess *self,
    struct EmberValue *out
) {
    size_t count = (size_t)(self->stack_top - self->stack);
    if (count == 0) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_UNDERFLOW);
        return false;
    }
    self->stack_top--;
    if (out != NULL) {
        *out = *self->stack_top;
    }
    return true;
}

/**
 * @brief Reads a value from the process stack without popping it.
 *
 * `distance == 0` reads the top value, `distance == 1` reads the value below
 * the top, and so on.
 *
 * @param self Process whose stack is inspected.
 * @param distance Distance from the top of the stack.
 * @param out Optional output pointer receiving the value.
 * @return `true` on success, `false` on stack underflow.
 */
static inline bool ember_process_peek(
    struct EmberProcess *self,
    size_t distance,
    struct EmberValue *out
) {
    size_t count = (size_t)(self->stack_top - self->stack);
    if (distance >= count) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_UNDERFLOW);
        return false;
    }
    if (out != NULL) {
        *out = self->stack_top[-1 - (ptrdiff_t)distance];
    }
    return true;
}

/**
 * @brief Reads a global value by index.
 *
 * @param self Process whose globals are inspected.
 * @param idx Global slot index.
 * @param out Optional output pointer receiving the global value.
 * @return `true` on success, `false` if the global index is invalid.
 */
static inline bool ember_process_getg(
    struct EmberProcess *self,
    uint16_t idx,
    struct EmberValue *out
) {
    if (idx >= self->global_capacity) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_INVALID_GLOBAL_INDEX);
        return false;
    }
    if (out != NULL) {
        *out = self->globals[idx];
    }
    return true;
}

/**
 * @brief Writes a global value by index.
 *
 * @param self Process whose globals are modified.
 * @param idx Global slot index.
 * @param value Value to store.
 * @return `true` on success, `false` if the global index is invalid.
 */
static inline bool ember_process_setg(
    struct EmberProcess *self,
    uint16_t idx,
    struct EmberValue value
) {
    if (idx >= self->global_capacity) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_INVALID_GLOBAL_INDEX);
        return false;
    }
    self->globals[idx] = value;
    return true;
}

/**
 * @brief Duplicates values from the top of the process stack.
 *
 * Duplicates the top `count` stack values while preserving their original
 * order. For example, duplicating two values changes the stack from
 * `[a, b, c]` to `[a, b, c, b, c]`.
 *
 * If `count` is greater than the number of values currently on the stack, this
 * raises a stack underflow runtime error. If duplicating the values would exceed
 * the maximum stack size, this raises a stack overflow runtime error.
 *
 * @param self Process whose stack is modified.
 * @param count Number of top stack values to duplicate.
 * @return `true` on success, `false` if a fatal runtime error was raised.
 */
static inline bool ember_process_dup(
    struct EmberProcess *self,
    size_t count
) {
    size_t stack_count = (size_t)(self->stack_top - self->stack);
    if (count > stack_count) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_UNDERFLOW);
        return false;
    }

    if (stack_count + count > EMBER_VM_STACK_MAX) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_OVERFLOW);
        return false;
    }

    struct EmberValue *src = self->stack_top - count;
    for (size_t i = 0; i < count; i++) {
        self->stack_top[i] = src[i];
    }
    self->stack_top += count;
    return true;
}

/**
 * @brief Removes values from the top of the process stack.
 *
 * Pops and discards the top `count` stack values. If `count` is greater than
 * the number of values currently on the stack, this raises a stack underflow
 * runtime error.
 *
 * @param self Process whose stack is modified.
 * @param count Number of top stack values to remove.
 * @return `true` on success, `false` if a fatal runtime error was raised.
 */
static inline bool ember_process_pop_n(
    struct EmberProcess *self,
    size_t count
) {
    size_t stack_count = (size_t)(self->stack_top - self->stack);
    if (count > stack_count) {
        ember_process_runtime_error(self, EMBER_RUNTIME_ERROR_STACK_UNDERFLOW);
        return false;
    }
    self->stack_top -= count;
    return true;
}

#endif
