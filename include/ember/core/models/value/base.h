#ifndef EMBER_CORE_VALUE_BASE_H
#define EMBER_CORE_VALUE_BASE_H

#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <ember/core/models/object/base.h>

/**
 * @brief Runtime value kind.
 *
 * This enum identifies the active variant stored in an `EmberValue`.
 */
enum EmberValueKind {
    /**
     * @brief Boolean value.
     */
    EVK_BOOLEAN,

    /**
     * @brief Double-precision floating-point value.
     */
    EVK_FLOAT,

    /**
     * @brief Signed 64-bit integer value.
     */
    EVK_INT,

    /**
     * @brief Absence of a value.
     */
    EVK_NONE,

    /**
     * @brief Heap-allocated object value.
     */
    EVK_OBJ,
};

/**
 * @brief Dynamically typed runtime value.
 *
 * `EmberValue` represents values manipulated by the VM at runtime. The `kind`
 * field determines which member of the `as` union is active.
 */
struct EmberValue {
    /**
     * @brief Active value variant.
     */
    enum EmberValueKind kind;

    /**
     * @brief Storage for the concrete value payload.
     */
    union {
        /**
         * @brief Boolean payload.
         */
        bool boolean_val;

        /**
         * @brief Floating-point payload.
         */
        double float_val;

        /**
         * @brief Signed integer payload.
         */
        int64_t int_val;

        /**
         * @brief Object pointer payload.
         */
        struct EmberObject *obj_val;
    } as;
};

/**
 * @brief Releases resources owned by a value.
 *
 * This is mainly relevant for values that reference heap-allocated objects.
 * Primitive values such as booleans, integers, floats, and none do not own
 * additional resources.
 *
 * @param self Value to deinitialize.
 */
void ember_value_deinit(struct EmberValue *self);

/**
 * @brief Creates a boolean Ember value.
 *
 * @param value Boolean payload.
 * @return Ember value containing the given boolean.
 */
#define EV_BOOLEAN(value) \
    ((struct EmberValue){ .kind = EVK_BOOLEAN, .as.boolean_val = (value) })

/**
 * @brief Creates a floating-point Ember value.
 *
 * @param value Floating-point payload.
 * @return Ember value containing the given double.
 */
#define EV_FLOAT(value) \
    ((struct EmberValue){ .kind = EVK_FLOAT, .as.float_val = (value) })

/**
 * @brief Creates an integer Ember value.
 *
 * @param value Signed integer payload.
 * @return Ember value containing the given integer.
 */
#define EV_INT(value) \
    ((struct EmberValue){ .kind = EVK_INT, .as.int_val = (value) })

/**
 * @brief Creates an object Ember value.
 *
 * @param value Object pointer payload.
 * @return Ember value containing the given object pointer.
 */
#define EV_OBJ(value) \
    ((struct EmberValue){ .kind = EVK_OBJ, .as.obj_val = (struct EmberObject *)(value) })

/**
 * @brief Creates a none Ember value.
 *
 * @return Ember value representing the absence of a value.
 */
#define EV_NONE() \
    ((struct EmberValue){ .kind = EVK_NONE })

/**
 * @brief Checks whether a value is a boolean.
 */
#define EV_IS_BOOLEAN(value) ((value).kind == EVK_BOOLEAN)

/**
 * @brief Checks whether a value is a float.
 */
#define EV_IS_FLOAT(value)   ((value).kind == EVK_FLOAT)

/**
 * @brief Checks whether a value is an integer.
 */
#define EV_IS_INT(value)     ((value).kind == EVK_INT)

/**
 * @brief Checks whether a value is none.
 */
#define EV_IS_NONE(value)    ((value).kind == EVK_NONE)

/**
 * @brief Checks whether a value is an object.
 */
#define EV_IS_OBJ(value)     ((value).kind == EVK_OBJ)

/**
 * @brief Extracts the boolean payload from a value.
 *
 * The caller must ensure that the value kind is `EVK_BOOLEAN`.
 */
#define EV_AS_BOOLEAN(value) ((value).as.boolean_val)

/**
 * @brief Extracts the floating-point payload from a value.
 *
 * The caller must ensure that the value kind is `EVK_FLOAT`.
 */
#define EV_AS_FLOAT(value)   ((value).as.float_val)

/**
 * @brief Extracts the integer payload from a value.
 *
 * The caller must ensure that the value kind is `EVK_INT`.
 */
#define EV_AS_INT(value)     ((value).as.int_val)

/**
 * @brief Extracts the object pointer payload from a value.
 *
 * The caller must ensure that the value kind is `EVK_OBJ`.
 */
#define EV_AS_OBJ(value)     ((value).as.obj_val)

/**
 * @brief Converts a value to its truthiness.
 *
 * Truthiness rules:
 * - booleans are truthy when true
 * - floats are truthy when non-zero
 * - integers are truthy when non-zero
 * - objects are truthy when non-NULL
 * - none is false
 *
 * @param value Value to test.
 * @return `true` if the value is truthy, otherwise `false`.
 */
static inline bool ember_value_is_truthy(struct EmberValue value) {
    switch (value.kind) {
        case EVK_BOOLEAN:
            return value.as.boolean_val;
        case EVK_FLOAT:
            return value.as.float_val != 0.0;
        case EVK_INT:
            return value.as.int_val != 0;
        case EVK_OBJ:
            return value.as.obj_val != NULL;
        case EVK_NONE:
            return false;
    }

    return false;
}

/**
 * @brief Checks two values for equality.
 *
 * Values of different kinds are considered unequal. Object values are compared
 * by pointer identity.
 *
 * @param a First value.
 * @param b Second value.
 * @return `true` if the values are equal, otherwise `false`.
 */
static inline bool ember_value_eq(struct EmberValue a, struct EmberValue b) {
    if (a.kind != b.kind) {
        return false;
    }

    switch (a.kind) {
        case EVK_BOOLEAN:
            return a.as.boolean_val == b.as.boolean_val;
        case EVK_FLOAT:
            return a.as.float_val == b.as.float_val;
        case EVK_INT:
            return a.as.int_val == b.as.int_val;
        case EVK_OBJ:
            return a.as.obj_val == b.as.obj_val;
        case EVK_NONE:
            return true;
    }

    return false;
}

/**
 * @brief Compares two orderable values.
 *
 * Only integer, float, and none values are currently orderable. Integers are
 * compared numerically with integers, and floats are compared numerically with
 * floats. Values of different kinds cannot be compared.
 *
 * @param a First value.
 * @param b Second value.
 * @return `-1` if `a < b`, `0` if `a == b`, and `1` if `a > b`.
 *
 * @warning On invalid comparisons, this function prints a runtime error and
 * returns `-1`. Callers should be aware that this makes invalid comparisons look
 * like less-than comparisons.
 */
static inline int ember_value_compare(struct EmberValue a, struct EmberValue b) {
    if (a.kind != b.kind) {
        fprintf(stderr, "runtime error: cannot compare values of different types\n");
        return -1;
    }

    switch (a.kind) {
        case EVK_INT:
            if (a.as.int_val < b.as.int_val) return -1;
            if (a.as.int_val > b.as.int_val) return 1;
            return 0;

        case EVK_FLOAT:
            if (a.as.float_val < b.as.float_val) return -1;
            if (a.as.float_val > b.as.float_val) return 1;
            return 0;

        case EVK_NONE:
            return 0;

        default:
            fprintf(stderr, "runtime error: value type is not orderable\n");
            return -1;
    }
}

/**
 * @brief Prints a value to standard output.
 *
 * Primitive values are printed directly. Object values are delegated to
 * `ember_print_object`.
 *
 * @param value Value to print.
 */
static inline void ember_value_print(struct EmberValue value) {
    switch (value.kind) {
        case EVK_BOOLEAN:
            printf("%s", value.as.boolean_val ? "true" : "false");
            break;

        case EVK_FLOAT:
            printf("%g", value.as.float_val);
            break;

        case EVK_INT:
            printf("%" PRId64, value.as.int_val);
            break;

        case EVK_NONE:
            printf("none");
            break;

        case EVK_OBJ:
            ember_print_object(value.as.obj_val);
            break;
    }
}

/**
 * @brief Checks whether the first value is greater than the second value.
 *
 * @param a First value.
 * @param b Second value.
 * @return `true` if `a > b`, otherwise `false`.
 */
static inline bool ember_value_gt(struct EmberValue a, struct EmberValue b) {
    return ember_value_compare(a, b) > 0;
}

/**
 * @brief Checks whether the first value is greater than or equal to the second value.
 *
 * @param a First value.
 * @param b Second value.
 * @return `true` if `a >= b`, otherwise `false`.
 */
static inline bool ember_value_gte(struct EmberValue a, struct EmberValue b) {
    return ember_value_compare(a, b) >= 0;
}

/**
 * @brief Checks whether the first value is less than the second value.
 *
 * @param a First value.
 * @param b Second value.
 * @return `true` if `a < b`, otherwise `false`.
 */
static inline bool ember_value_lt(struct EmberValue a, struct EmberValue b) {
    return ember_value_compare(a, b) < 0;
}

/**
 * @brief Checks whether the first value is less than or equal to the second value.
 *
 * @param a First value.
 * @param b Second value.
 * @return `true` if `a <= b`, otherwise `false`.
 */
static inline bool ember_value_lte(struct EmberValue a, struct EmberValue b) {
    return ember_value_compare(a, b) <= 0;
}

#endif