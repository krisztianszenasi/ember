#ifndef EMBER_CORE_MEMORY_H
#define EMBER_CORE_MEMORY_H

#include <stddef.h>

/**
 * @brief Allocates an array of values.
 *
 * This allocates enough memory for `count` values of `type`.
 *
 * @param type Element type.
 * @param size Unused legacy parameter.
 * @param count Number of elements to allocate.
 */
#define EMBER_ALLOCATE(type, size, count) \
    (type *)ember_realloc(NULL, 0, sizeof(type) * (count))

/**
 * @brief Frees a single allocated value.
 *
 * @param type Type of the allocated value.
 * @param pointer Pointer to free.
 */
#define EMBER_FREE(type, pointer) \
    ember_realloc((pointer), sizeof(type), 0)

/**
 * @brief Computes the next dynamic array capacity.
 *
 * New arrays start with capacity 8. Existing arrays grow by doubling their
 * current capacity.
 *
 * @param capacity Current capacity.
 * @return New grown capacity.
 */
#define EMBER_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

/**
 * @brief Resizes a dynamic array allocation.
 *
 * @param type Element type.
 * @param pointer Existing array pointer, or `NULL`.
 * @param old_capacity Previous element capacity.
 * @param new_capacity New element capacity.
 * @return Resized array pointer.
 */
#define EMBER_GROW_ARRAY(type, pointer, old_capacity, new_capacity) \
    (type *)ember_realloc(                                         \
        (pointer),                                                 \
        sizeof(type) * (old_capacity),                             \
        sizeof(type) * (new_capacity)                              \
    )

/**
 * @brief Frees a dynamic array allocation.
 *
 * @param type Element type.
 * @param pointer Array pointer to free.
 * @param old_capacity Current array capacity.
 */
#define EMBER_FREE_ARRAY(type, pointer, old_capacity) \
    ember_realloc((pointer), sizeof(type) * (old_capacity), 0)

/**
 * @brief Allocates a block of memory.
 *
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated memory, or `NULL` on allocation failure.
 */
void *ember_alloc(size_t size);

/**
 * @brief Allocates, resizes, or frees a memory block.
 *
 * If `ptr` is `NULL`, this behaves like an allocation. If `new_size` is 0, this
 * frees the block and returns `NULL`. Otherwise, this resizes the block.
 *
 * @param ptr Existing allocation, or `NULL`.
 * @param old_size Previous allocation size in bytes.
 * @param new_size Requested allocation size in bytes.
 * @return Pointer to the resized allocation, or `NULL` if freed or allocation
 * failed.
 */
void *ember_realloc(void *ptr, size_t old_size, size_t new_size);

/**
 * @brief Allocates and copies a null-terminated string.
 *
 * @param string String to copy.
 * @return Newly allocated copy of the string, or NULL on allocation failure.
 */
char *ember_strdup(const char *string);

/**
 * @brief Frees a block of memory.
 *
 * @param ptr Pointer to free. May be `NULL`.
 */
void ember_free(void *ptr);

#endif