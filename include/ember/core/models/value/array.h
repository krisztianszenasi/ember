#ifndef EMBER_CORE_VALUE_ARRAY_H
#define EMBER_CORE_VALUE_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#include <ember/core/models/value/base.h>

/**
 * @brief Dynamic array of Ember values.
 *
 * This structure stores a growable sequence of `EmberValue` instances. It is
 * typically used for collections such as constant pools, temporary value lists,
 * or other VM/runtime-owned value storage.
 *
 * The array owns the memory pointed to by `values`, but it does not necessarily
 * own heap-allocated objects referenced by the values themselves.
 */
struct EmberValueArray {
    /**
     * @brief Number of values currently stored in the array.
     */
    size_t count;

    /**
     * @brief Number of values that can be stored without reallocating.
     */
    size_t capacity;

    /**
     * @brief Pointer to the allocated value storage.
     *
     * This pointer is `NULL` when the array is empty and has no allocated
     * storage.
     */
    struct EmberValue *values;
};

/**
 * @brief Initializes an empty value array.
 *
 * After initialization, the array contains no values and owns no allocated
 * storage. The array must later be released with `ember_value_array_deinit`.
 *
 * @param self Value array to initialize.
 */
void ember_value_array_init(struct EmberValueArray *self);

/**
 * @brief Releases memory owned by a value array.
 *
 * This frees the array storage and resets the array to an empty state. It does
 * not necessarily free heap-allocated objects referenced by individual values.
 *
 * @param self Value array to deinitialize.
 */
void ember_value_array_deinit(struct EmberValueArray *self);

/**
 * @brief Appends a value to the end of the array.
 *
 * The array grows automatically when its current capacity is not large enough.
 *
 * @param self Value array to append to.
 * @param value Value to append.
 */
void ember_value_array_add(
    struct EmberValueArray *self,
    struct EmberValue value
);

/**
 * @brief Finds the first occurrence of a value in the array.
 *
 * Values are compared using the Ember value equality rules implemented by the
 * value model.
 *
 * @param self Value array to search.
 * @param value Value to find.
 * @param out_idx Optional output parameter. If non-NULL and the value is found,
 *        receives the index of the first matching value. If the value is not
 *        found, this parameter is left unchanged.
 * @return `true` if the value was found, otherwise `false`.
 */
bool ember_value_array_find(
    const struct EmberValueArray *self,
    struct EmberValue value,
    size_t *out_idx
);

#endif