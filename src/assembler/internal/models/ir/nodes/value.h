#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_VALUE_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_VALUE_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ember/core/memory.h>

#include "assembler/internal/models/ir/nodes/base.h"

/**
 * Kinds of IR values.
 */
enum EmberIrValueKind {
    IR_VALUE_BOOLEAN,
    IR_VALUE_FLOAT,
    IR_VALUE_INT,
    IR_VALUE_STR,
};

/**
 * IR value used by constant operands.
 *
 * Values are small embeddable nodes stored by value rather than allocated as
 * standalone heap objects.
 */
struct EmberIrValue {
    struct EmberIrNode base;
    enum EmberIrValueKind kind;
    union {
        bool boolean_val;
        double float_val;
        int64_t int_val;
        char *str_val;
    } as;
};

/**
 * Creates and initializes a boolean IR value.
 *
 * @param value Boolean value to store.
 * @return An initialized boolean IR value.
 */
static inline struct EmberIrValue
ember_ir_boolean_value(bool value) {
    struct EmberIrValue self;
    ember_ir_node_init(&self.base, IR_NODE_VALUE, EMBER_SOURCE_SPAN_NONE);
    self.kind = IR_VALUE_BOOLEAN;
    self.as.boolean_val = value;
    return self;
}

/**
 * Creates and initializes a floating-point IR value.
 *
 * @param value Floating-point value to store.
 * @return An initialized floating-point IR value.
 */
static inline struct EmberIrValue
ember_ir_float_value(double value) {
    struct EmberIrValue self;
    ember_ir_node_init(&self.base, IR_NODE_VALUE, EMBER_SOURCE_SPAN_NONE);
    self.kind = IR_VALUE_FLOAT;
    self.as.float_val = value;
    return self;
}

/**
 * Creates and initializes an integer IR value.
 *
 * @param value Integer value to store.
 * @return An initialized integer IR value.
 */
static inline struct EmberIrValue
ember_ir_int_value(int64_t value) {
    struct EmberIrValue self;
    ember_ir_node_init(&self.base, IR_NODE_VALUE, EMBER_SOURCE_SPAN_NONE);
    self.kind = IR_VALUE_INT;
    self.as.int_val = value;
    return self;
}

/**
 * Creates and initializes a string IR value.
 *
 * The given string is copied and owned by the returned value.
 *
 * @param value String value to store.
 * @return An initialized string IR value.
 */
static inline struct EmberIrValue
ember_ir_str_value(const char *value) {
    struct EmberIrValue self;
    ember_ir_node_init(&self.base, IR_NODE_VALUE, EMBER_SOURCE_SPAN_NONE);
    self.kind = IR_VALUE_STR;
    self.as.str_val = ember_strdup(value);
    return self;
}

static inline void ember_ir_value_set_source_span(
    struct EmberIrValue *self,
    struct EmberSourceSpan source_span
) {
    self->base.source_span = source_span;
}

/**
 * Deinitializes an IR value in place.
 *
 * This releases resources owned by the active value variant. Since IR values
 * are intended to be embedded inside other IR nodes, this function does not
 * free the value object itself.
 *
 * @param self Pointer to the IR value to deinitialize.
 */
static inline void ember_ir_value_deinit(struct EmberIrValue *self) {
    if (self->kind == IR_VALUE_STR) {
        free(self->as.str_val);
    }
}

#endif
