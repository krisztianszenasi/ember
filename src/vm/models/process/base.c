#include "vm/internal/models/process/base.h"

#include <inttypes.h>
#include <string.h>

#include <ember/core/opcode.h>
#include <ember/core/memory.h>
#include <ember/core/models/value/base.h>
#include <ember/core/models/object/str.h>

#define EMBER_GC_INITIAL_THRESHOLD (1024 * 1024)
#define VM_LOG_TAG "vm"

static size_t ember_call_frame_current_offset(const struct EmberCallFrame *frame) {
    return (size_t)(frame->ip - ember_function_get_code(frame->function));
}

static void ember_process_set_status(
    struct EmberProcess *self,
    struct EmberLogger *logger,
    enum EmberProcessStatusCode status,
    size_t instruction_offset,
    const char *instruction_name,
    const char *detail
) {
    self->status = status;

    if (status == EMBER_STATUS_OK) {
        return;
    }

    EMBER_DEBUG(
        logger,
        VM_LOG_TAG,
        "pc=%zu %s failed with %s%s%s",
        instruction_offset,
        instruction_name,
        ember_process_status_to_string(status),
        detail != NULL ? ": " : "",
        detail != NULL ? detail : ""
    );
}

static enum EmberProcessRunResult ember_process_log_current_error(
    struct EmberProcess *self,
    struct EmberLogger *logger,
    const char *context
) {
    EMBER_DEBUG(
        logger,
        VM_LOG_TAG,
        "%s: %s",
        context,
        self->error.message != NULL ? self->error.message : "unknown runtime error"
    );
    return EMBER_PROCESS_RUN_ERROR;
}


struct EmberProcess *ember_process_new(struct EmberExecutable *executable) {
    struct EmberProcess *self = ember_alloc(sizeof(struct EmberProcess));

    self->state = EMBER_PROCESS_CREATED;
    self->status = EMBER_STATUS_OK;
    self->error.kind = EMBER_RUNTIME_ERROR_NONE;
    self->error.message = NULL;

    self->executable = executable;
    self->frame_count = 0;

    self->stack_top = self->stack;
    self->objects = NULL;
    self->bytes_allocated = 0;
    self->next_gc = EMBER_GC_INITIAL_THRESHOLD;

    self->global_capacity = UINT8_MAX + 1;
    self->globals = NULL;

    self->globals = EMBER_GROW_ARRAY(
        struct EmberValue,
        self->globals,
        0,
        self->global_capacity
    );

    return self;
}

void ember_process_destroy(struct EmberProcess *self) {
    EMBER_FREE_ARRAY(
        struct EmberValue,
        self->globals,
        self->global_capacity
    );

    ember_object_list_destroy(self->objects);
    ember_free(self);
}

void ember_process_runtime_error(
    struct EmberProcess *self,
    enum EmberRuntimeErrorKind kind
) {
    ember_process_error_set(
        &self->error,
        ember_runtime_error_message(kind),
        kind
    );
    self->state = EMBER_PROCESS_ERROR;
}

static void ember_process_collect_garbage(
    struct EmberProcess *self,
    struct EmberLogger *logger
) {
    size_t bytes_before = self->bytes_allocated;
    size_t object_count_before = 0;
    size_t object_count_after = 0;
    struct EmberValue *slot = self->stack;
    struct EmberObject *counted_object = self->objects;

    while (counted_object != NULL) {
        object_count_before++;
        counted_object = ember_object_list_get_next(counted_object);
    }

    EMBER_DEBUG(
        logger,
        VM_LOG_TAG,
        "starting garbage collection: bytes=%zu, objects=%zu, next_gc=%zu",
        bytes_before,
        object_count_before,
        self->next_gc
    );

    while (slot != self->stack_top) {
        if (EV_IS_OBJ(*slot)) {
            ember_object_mark(EV_AS_OBJ(*slot));
        }

        slot++;
    }

    struct EmberObject *previous = NULL;
    struct EmberObject *object = self->objects;

    while (object != NULL) {
        struct EmberObject *next = ember_object_list_get_next(object);

        if (ember_object_is_marked(object)) {
            ember_object_unmark(object);
            previous = object;
            object = next;
            continue;
        }

        if (previous == NULL) {
            self->objects = next;
        } else {
            ember_object_list_set_next(previous, next);
        }

        self->bytes_allocated -= ember_object_get_size(object);
        ember_object_destroy(object);

        object = next;
    }

    counted_object = self->objects;
    while (counted_object != NULL) {
        object_count_after++;
        counted_object = ember_object_list_get_next(counted_object);
    }

    self->next_gc = self->bytes_allocated * 2;

    if (self->next_gc < EMBER_GC_INITIAL_THRESHOLD) {
        self->next_gc = EMBER_GC_INITIAL_THRESHOLD;
    }

    EMBER_DEBUG(
        logger,
        VM_LOG_TAG,
        "finished garbage collection: bytes=%zu -> %zu, objects=%zu -> %zu, next_gc=%zu",
        bytes_before,
        self->bytes_allocated,
        object_count_before,
        object_count_after,
        self->next_gc
    );
}

void ember_process_take_object(
    struct EmberProcess *self,
    struct EmberObject *object,
    struct EmberLogger *logger
) {
    size_t object_size = ember_object_get_size(object);

    ember_object_list_set_next(object, self->objects);
    self->objects = object;
    self->bytes_allocated += object_size;

    EMBER_DEBUG(
        logger,
        VM_LOG_TAG,
        "tracking object allocation: size=%zu, bytes=%zu, next_gc=%zu",
        object_size,
        self->bytes_allocated,
        self->next_gc
    );

    if (self->bytes_allocated >= self->next_gc) {
        EMBER_DEBUG(
            logger,
            VM_LOG_TAG,
            "garbage collection threshold reached: bytes=%zu, threshold=%zu",
            self->bytes_allocated,
            self->next_gc
        );
        ember_process_collect_garbage(self, logger);
    }
}


enum EmberProcessRunResult ember_process_run(
    struct EmberProcess *self,
    struct EmberLogger *logger
) {
    EMBER_INFO(logger, VM_LOG_TAG, "starting process");

    self->state = EMBER_PROCESS_RUNNING;
    size_t start_function_idx = ember_executable_get_start_function_idx(self->executable);

    EMBER_DEBUG(logger, VM_LOG_TAG, "identified start function with idx %zu", start_function_idx);

    EMBER_DEBUG(logger, VM_LOG_TAG, "calling start function %zu", start_function_idx);
    struct EmberCallFrame *frame = NULL;
    if (ember_process_call_function(self, start_function_idx, &frame) == false) {
        return ember_process_log_current_error(
            self,
            logger,
            "failed to enter start function"
        );
    }

    #define FAIL(kind)                                                          \
        do {                                                                    \
            ember_process_runtime_error(self, (kind));                          \
            EMBER_DEBUG(                                                        \
                logger,                                                         \
                VM_LOG_TAG,                                                     \
                "pc=%zu %s",                                                    \
                instruction_offset,                                             \
                self->error.message != NULL                                     \
                    ? self->error.message                                       \
                    : "unknown runtime error"                                   \
            );                                                                  \
            return EMBER_PROCESS_RUN_ERROR;                                     \
        } while (0)

    #define READ_U8(out)                                                        \
        do {                                                                    \
            if (!ember_call_frame_read_u8(frame, &(out))) {                     \
                FAIL(EMBER_RUNTIME_ERROR_UNEXPECTED_END_OF_BYTECODE);           \
            }                                                                   \
        } while (0)

    #define READ_U16(out)                                                       \
        do {                                                                    \
            if (!ember_call_frame_read_u16(frame, &(out))) {                    \
                FAIL(EMBER_RUNTIME_ERROR_UNEXPECTED_END_OF_BYTECODE);           \
            }                                                                   \
        } while (0)

    #define READ_I16(out)                                                       \
        do {                                                                    \
            if (!ember_call_frame_read_i16(frame, &(out))) {                    \
                FAIL(EMBER_RUNTIME_ERROR_UNEXPECTED_END_OF_BYTECODE);           \
            }                                                                   \
        } while (0)

    #define PUSH(value)                                                         \
        do {                                                                    \
            if (!ember_process_push(self, (value))) {                           \
                return ember_process_log_current_error(                          \
                    self,                                                       \
                    logger,                                                     \
                    "stack push failed"                                         \
                );                                                              \
            }                                                                   \
        } while (0)

    #define POP(out)                                                            \
        do {                                                                    \
            if (!ember_process_pop(self, &(out))) {                             \
                return ember_process_log_current_error(                          \
                    self,                                                       \
                    logger,                                                     \
                    "stack pop failed"                                          \
                );                                                              \
            }                                                                   \
        } while (0)

    #define PEEK(distance, out)                                                 \
        do {                                                                    \
            if (!ember_process_peek(self, (distance), &(out))) {                \
                return ember_process_log_current_error(                          \
                    self,                                                       \
                    logger,                                                     \
                    "stack peek failed"                                         \
                );                                                              \
            }                                                                   \
        } while (0)

    #define GET_ARG(idx, out)                                                   \
        do {                                                                    \
            if (!ember_call_frame_geta(frame, (idx), &(out))) {                 \
                FAIL(EMBER_RUNTIME_ERROR_INVALID_ARG_INDEX);                    \
            }                                                                   \
        } while (0)

    #define GET_LOCAL(idx, out)                                                 \
        do {                                                                    \
            if (!ember_call_frame_getl(frame, (idx), &(out))) {                 \
                FAIL(EMBER_RUNTIME_ERROR_INVALID_LOCAL_INDEX);                  \
            }                                                                   \
        } while (0)

    #define SET_LOCAL(idx, value)                                               \
        do {                                                                    \
            if (!ember_call_frame_setl(frame, (idx), (value))) {                \
                FAIL(EMBER_RUNTIME_ERROR_INVALID_LOCAL_INDEX);                  \
            }                                                                   \
        } while (0)

    #define GET_GLOBAL(idx, out)                                                \
        do {                                                                    \
            if (!ember_process_getg(self, (idx), &(out))) {                     \
                return ember_process_log_current_error(                          \
                    self,                                                       \
                    logger,                                                     \
                    "global read failed"                                        \
                );                                                              \
            }                                                                   \
        } while (0)

    #define SET_GLOBAL(idx, value)                                              \
        do {                                                                    \
            if (!ember_process_setg(self, (idx), (value))) {                    \
                return ember_process_log_current_error(                          \
                    self,                                                       \
                    logger,                                                     \
                    "global write failed"                                       \
                );                                                              \
            }                                                                   \
        } while (0)
    #define READ_CONST(idx, out)                                                \
        do {                                                                    \
            if (ember_function_read_const(frame->function, (idx), &(out)) != 0){\
                FAIL(EMBER_RUNTIME_ERROR_INVALID_CONSTANT_INDEX);               \
            }                                                                   \
        } while (0)
    #define BINARY_OP(expected_kind, a_field, op, b_field, make_result)         \
        do {                                                                    \
            struct EmberValue b;                                                \
            struct EmberValue a;                                                \
            POP(b);                                                             \
            POP(a);                                                             \
                                                                                \
            if (a.kind != (expected_kind) || b.kind != (expected_kind)) {       \
                self->status = EMBER_STATUS_INVALID_TYPE;                       \
                PUSH(EV_NONE());                                                \
                break;                                                          \
            }                                                                   \
                                                                                \
            self->status = EMBER_STATUS_OK;                                     \
            struct EmberValue result =                                          \
                make_result(a.as.a_field op b.as.b_field);                      \
            PUSH(result);                                                       \
        } while (0)

    for (;;) {
        uint8_t instruction;
        size_t instruction_offset = ember_call_frame_current_offset(frame);
        struct EmberInstructionInfo info;
        READ_U8(instruction);
        info = ember_opcode_get_instruction_info((enum EmberOpcode)instruction);

        EMBER_DEBUG(
            logger,
            VM_LOG_TAG,
            "pc=%zu executing %s",
            instruction_offset,
            info.name != NULL ? info.name : "<invalid>"
        );

        switch (instruction) {
            /* constants and stack */
            case OP_LDC: {
                uint8_t idx;
                struct EmberValue value;
                READ_U8(idx);
                READ_CONST(idx, value);
                PUSH(value);
                break;
            }
            case OP_LDC_W: {
                uint16_t idx;
                struct EmberValue value;
                READ_U16(idx);
                READ_CONST(idx, value);
                PUSH(value);
                break;
            }
            case OP_DUP: {
                uint8_t count;
                READ_U8(count);
                if (ember_process_dup(self, count) == false) {
                    return EMBER_PROCESS_RUN_ERROR;
                }
                break;
            }
            case OP_POP: {
                uint8_t count;
                READ_U8(count);
                if (ember_process_pop_n(self, count) == false) {
                    return EMBER_PROCESS_RUN_ERROR;
                }
                break;
            }

            /* argument, local, and global access */
            case OP_GETA: {
                uint8_t idx;
                struct EmberValue value;
                READ_U8(idx);
                GET_ARG(idx, value);
                PUSH(value);
                break;
            }
            case OP_GETL: {
                uint8_t idx;
                struct EmberValue value;
                READ_U8(idx);
                GET_LOCAL(idx, value);
                PUSH(value);
                break;
            }
            case OP_SETL: {
                uint8_t idx;
                struct EmberValue value;
                READ_U8(idx);
                POP(value);
                SET_LOCAL(idx, value);
                break;
            }
            case OP_GETG: {
                uint16_t idx;
                struct EmberValue value;
                READ_U16(idx);
                GET_GLOBAL(idx, value);
                PUSH(value);
                break;
            }
            case OP_SETG: {
                uint16_t idx;
                struct EmberValue value;
                READ_U16(idx);
                POP(value);
                SET_GLOBAL(idx, value);
                break;
            }

            /* integer arithmetic */
            case OP_IADD: {
                BINARY_OP(EVK_INT, int_val, +, int_val, EV_INT);
                break;
            }
            case OP_ISUB: {
                BINARY_OP(EVK_INT, int_val, -, int_val, EV_INT);
                break;
            }
            case OP_IMUL: {
                BINARY_OP(EVK_INT, int_val, *, int_val, EV_INT);
                break;
            }
            case OP_IDIV: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                if (a.kind != EVK_INT || b.kind != EVK_INT) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected integer operands"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                if (b.as.int_val == 0) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_DIVISION_BY_ZERO,
                        instruction_offset,
                        info.name,
                        "attempted division by zero"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                self->status = EMBER_STATUS_OK;
                PUSH(EV_INT(a.as.int_val / b.as.int_val));
                break;
            }
            case OP_IREM: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                if (a.kind != EVK_INT || b.kind != EVK_INT) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected integer operands"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                if (b.as.int_val == 0) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_DIVISION_BY_ZERO,
                        instruction_offset,
                        info.name,
                        "attempted remainder by zero"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                self->status = EMBER_STATUS_OK;
                PUSH(EV_INT(a.as.int_val % b.as.int_val));
                break;
            }

            /* floating-point arithmetic */
             case OP_FADD: {
                BINARY_OP(EVK_FLOAT, float_val, +, float_val, EV_FLOAT);
                break;
            }
            case OP_FSUB: {
                BINARY_OP(EVK_FLOAT, float_val, -, float_val, EV_FLOAT);
                break;
            }
            case OP_FMUL: {
                BINARY_OP(EVK_FLOAT, float_val, *, float_val, EV_FLOAT);
                break;
            }
            case OP_FDIV: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                if (a.kind != EVK_FLOAT|| b.kind != EVK_FLOAT) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected floating-point operands"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                if (b.as.float_val == 0) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_DIVISION_BY_ZERO,
                        instruction_offset,
                        info.name,
                        "attempted division by zero"
                    );
                    PUSH(EV_NONE());
                    break;
                }
                self->status = EMBER_STATUS_OK;
                PUSH(EV_FLOAT(a.as.float_val / b.as.float_val));
                break;
            }

            /* String operations. */
            case OP_CONCAT: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);

                if (IS_EO_STR(a) == false || IS_EO_STR(b) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected string operands"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                self->status = EMBER_STATUS_OK;
                struct EmberStringObject *concat = ember_string_object_concat(AS_EO_STR(a), AS_EO_STR(b));
                PUSH(EV_OBJ(concat));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)concat,
                    logger
                );
                break;
            }

            /* logical operations */
            case OP_AND: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_is_truthy(a) && ember_value_is_truthy(b)));
                break;
            }
            case OP_OR: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_is_truthy(a) || ember_value_is_truthy(b)));
                break;
            }
            case OP_NOT: {
                struct EmberValue a;
                POP(a);
                PUSH(EV_BOOLEAN(!ember_value_is_truthy(a)));
                break;
            }

            /* comparisons */
            case OP_EQ: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_eq(a, b)));
                break;
            }
            case OP_NEQ: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_eq(a, b) == false));
                break;
            }
            case OP_GT: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_gt(a, b)));
                break;
            }
            case OP_GTE: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_gte(a, b)));
                break;
            }
            case OP_LT: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_lt(a, b)));
                break;
            }
            case OP_LTE: {
                struct EmberValue a;
                struct EmberValue b;
                POP(b);
                POP(a);
                PUSH(EV_BOOLEAN(ember_value_lte(a, b)));
                break;
            }

            /* control flow */
            case OP_JMP: {
                int16_t offset;
                READ_I16(offset);
                frame->ip += offset;
                break;
            }
            case OP_JMP_IF_FALSE: {
                int16_t offset;
                struct EmberValue condition;
                READ_I16(offset);
                POP(condition);
                if (ember_value_is_truthy(condition) == false) {
                    frame->ip += offset;
                }
                break;
            }

            /* calls */
            case OP_CALL: {
                uint16_t func_idx;
                READ_U16(func_idx);
                EMBER_DEBUG(
                    logger,
                    VM_LOG_TAG,
                    "pc=%zu calling function %u",
                    instruction_offset,
                    func_idx
                );
                if (ember_process_call_function(self, func_idx, &frame) == false) {
                    return ember_process_log_current_error(
                        self,
                        logger,
                        "function call failed"
                    );
                }
                break;
            }
            case OP_RET: {
                EMBER_DEBUG(logger, VM_LOG_TAG, "pc=%zu returning from function", instruction_offset);
                if (ember_process_return_function(self, &frame) == false) {
                    return ember_process_log_current_error(
                        self,
                        logger,
                        "function return failed"
                    );
                }
                break;
            }
            case OP_HALT: {
                self->state = EMBER_PROCESS_HALTED;
                EMBER_INFO(logger, VM_LOG_TAG, "process halted normally");
                return EMBER_PROCESS_RUN_HALTED;
            }

            /* io */
            case OP_PRINT: {
                struct EmberValue value;
                POP(value);
                ember_value_print(value);
                break;
            }
            case OP_INPUT: {
                char buffer[1024];

                EMBER_DEBUG(logger, VM_LOG_TAG, "pc=%zu reading input", instruction_offset);
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_IO_ERROR,
                        instruction_offset,
                        info.name,
                        "failed to read from stdin"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                buffer[strcspn(buffer, "\n")] = '\0';

                struct EmberStringObject *string = ember_string_object_new(buffer);
                self->status = EMBER_STATUS_OK;
                PUSH(EV_OBJ(string));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)string,
                    logger
                );
                break;
            }

            /* status related instructions */
            case OP_STATUS_OK: {
                PUSH(EV_BOOLEAN(self->status == EMBER_STATUS_OK));
                break;
            }
            case OP_STATUS_CODE: {
                PUSH(EV_INT(self->status));
                break;
            }
            case OP_STATUS_CLEAR: {
                EMBER_DEBUG(logger, VM_LOG_TAG, "pc=%zu clearing status", instruction_offset);
                self->status = EMBER_STATUS_OK;
                break;
            }
            /* cast instructions */
            case OP_I2F: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_INT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected integer operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                double float_val = (double)EV_AS_INT(value);
                self->status = EMBER_STATUS_OK;
                PUSH(EV_FLOAT(float_val));
                break;
            }

            case OP_F2I: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_FLOAT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected floating-point operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                int64_t int_val = (int64_t)EV_AS_FLOAT(value);
                self->status = EMBER_STATUS_OK;
                PUSH(EV_INT(int_val));
                break;
            }

            case OP_I2B: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_INT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected integer operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                bool bool_val = EV_AS_INT(value) != 0;
                self->status = EMBER_STATUS_OK;
                PUSH(EV_BOOLEAN(bool_val));
                break;
            }

            case OP_F2B: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_FLOAT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected floating-point operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                bool bool_val = EV_AS_FLOAT(value) != 0.0;
                self->status = EMBER_STATUS_OK;
                PUSH(EV_BOOLEAN(bool_val));
                break;
            }

            case OP_S2B: {
                struct EmberValue value;
                POP(value);

                if (IS_EO_STR(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected string operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                bool bool_val = ember_string_object_get_length(AS_EO_STR(value)) != 0;
                self->status = EMBER_STATUS_OK;
                PUSH(EV_BOOLEAN(bool_val));
                break;
            }

            case OP_N2B: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_NONE(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected none operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                self->status = EMBER_STATUS_OK;
                PUSH(EV_BOOLEAN(false));
                break;
            }

            case OP_I2S: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_INT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected integer operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberStringObject *string = ember_string_object_from_int(EV_AS_INT(value));
                self->status = EMBER_STATUS_OK;
                PUSH(EV_OBJ(string));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)string,
                    logger
                );
                break;
            }

            case OP_F2S: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_FLOAT(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected floating-point operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberStringObject *string = ember_string_object_from_float(EV_AS_FLOAT(value));

                self->status = EMBER_STATUS_OK;
                PUSH(EV_OBJ(string));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)string,
                    logger
                );
                break;
            }

            case OP_B2S: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_BOOLEAN(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected boolean operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberStringObject *string = ember_string_object_from_bool(EV_AS_BOOLEAN(value));

                self->status = EMBER_STATUS_OK;
                PUSH(EV_OBJ(string));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)string,
                    logger
                );
                break;
            }

            case OP_N2S: {
                struct EmberValue value;
                POP(value);

                if (EV_IS_NONE(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected none operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberStringObject *string = ember_string_object_new("none");
                self->status = EMBER_STATUS_OK;
                PUSH(EV_OBJ(string));
                ember_process_take_object(
                    self,
                    (struct EmberObject *)string,
                    logger
                );
                break;
            }

            /* parse instructions */
            case OP_PARSE_INT: {
                struct EmberValue value;
                POP(value);

                if (IS_EO_STR(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected string operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberValue int_val;
                if (ember_string_object_parse_to_int(AS_EO_STR(value), &int_val) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_CAST,
                        instruction_offset,
                        info.name,
                        "string could not be parsed as int"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                self->status = EMBER_STATUS_OK;
                PUSH(int_val);
                break;
            }

            case OP_PARSE_FLOAT: {
                struct EmberValue value;
                POP(value);

                if (IS_EO_STR(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected string operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberValue float_val;
                if (ember_string_object_parse_to_float(AS_EO_STR(value), &float_val) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_CAST,
                        instruction_offset,
                        info.name,
                        "string could not be parsed as float"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                self->status = EMBER_STATUS_OK;
                PUSH(float_val);
                break;
            }

            case OP_PARSE_BOOL: {
                struct EmberValue value;
                POP(value);

                if (IS_EO_STR(value) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_TYPE,
                        instruction_offset,
                        info.name,
                        "expected string operand"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                struct EmberValue bool_val;
                if (ember_string_object_parse_to_bool(AS_EO_STR(value), &bool_val) == false) {
                    ember_process_set_status(
                        self,
                        logger,
                        EMBER_STATUS_INVALID_CAST,
                        instruction_offset,
                        info.name,
                        "string could not be parsed as bool"
                    );
                    PUSH(EV_NONE());
                    break;
                }

                self->status = EMBER_STATUS_OK;
                PUSH(bool_val);
                break;
            }
            default:
                FAIL(EMBER_RUNTIME_ERROR_INVALID_OPCODE);
        }
    }
}
