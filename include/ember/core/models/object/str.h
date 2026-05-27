#ifndef EMBER_CORE_MODELS_OBJECT_STR_H
#define EMBER_CORE_MODELS_OBJECT_STR_H

#include <stdbool.h>
#include <stddef.h>

#include <ember/core/models/value/base.h>

struct EmberStringObject;

/**
 * @brief Creates a new string object by copying a null-terminated C string.
 *
 * The returned object owns its internal string buffer. The input string is not
 * modified and does not need to remain alive after this call.
 *
 * @param value Null-terminated string to copy.
 * @return Newly allocated string object, or NULL on allocation failure.
 */
struct EmberStringObject *ember_string_object_new(const char *value);

/**
 * @brief Creates a new string object by taking ownership of a string buffer.
 *
 * The buffer must be heap-allocated and null-terminated. After a successful
 * call, the string object owns the buffer and will free it when destroyed. If
 * object allocation fails, the buffer is freed by this function.
 *
 * @param value Heap-allocated string buffer to take ownership of.
 * @param length String length without the null terminator.
 * @return Newly allocated string object, or NULL on allocation failure.
 */
struct EmberStringObject *ember_string_object_take(
    char *value,
    size_t length
);

/**
 * @brief Concatenates two string objects.
 *
 * Creates a new string object containing the contents of `a` followed by the
 * contents of `b`.
 *
 * @param a First string object.
 * @param b Second string object.
 * @return Newly allocated concatenated string object, or NULL on failure.
 */
struct EmberStringObject *ember_string_object_concat(
    struct EmberStringObject *a,
    struct EmberStringObject *b
);

/**
 * @brief Returns the length of a string object.
 *
 * The returned length does not include the null terminator.
 *
 * @param self String object to inspect.
 * @return Number of characters stored in the string.
 */
size_t ember_string_object_get_length(struct EmberStringObject *self);

/**
 * @brief Destroys a string object and releases its owned memory.
 *
 * @param self String object to destroy.
 */
void ember_string_object_destroy(struct EmberStringObject *self);

/**
 * @brief Prints a string object to standard output.
 *
 * @param self String object to print.
 */
void ember_string_object_print(struct EmberStringObject *self);

/**
 * @brief Checks whether an object is a string object.
 *
 * @param self Object to inspect.
 * @return true if the object is a string object, false otherwise.
 */
bool ember_is_string_object(struct EmberObject *self);

/**
 * @brief Creates a string object from an integer value.
 *
 * @param value Integer value to convert.
 * @return Newly allocated string object, or NULL on allocation failure.
 */
struct EmberStringObject *ember_string_object_from_int(int64_t value);

/**
 * @brief Creates a string object from a floating-point value.
 *
 * @param value Floating-point value to convert.
 * @return Newly allocated string object, or NULL on allocation failure.
 */
struct EmberStringObject *ember_string_object_from_float(double value);

/**
 * @brief Creates a string object from a boolean value.
 *
 * The resulting string is either "true" or "false".
 *
 * @param value Boolean value to convert.
 * @return Newly allocated string object, or NULL on allocation failure.
 */
struct EmberStringObject *ember_string_object_from_bool(bool value);

/**
 * @brief Parses a string object as a floating-point value.
 *
 * On success, `out` receives an Ember float value. On failure, `out` is left
 * unchanged.
 *
 * @param self String object to parse.
 * @param out Output value receiving the parsed float.
 * @return true if parsing succeeded, false otherwise.
 */
bool ember_string_object_parse_to_float(
    struct EmberStringObject *self,
    struct EmberValue *out
);

/**
 * @brief Parses a string object as an integer value.
 *
 * On success, `out` receives an Ember integer value. On failure, `out` is left
 * unchanged.
 *
 * @param self String object to parse.
 * @param out Output value receiving the parsed integer.
 * @return true if parsing succeeded, false otherwise.
 */
bool ember_string_object_parse_to_int(
    struct EmberStringObject *self,
    struct EmberValue *out
);

/**
 * @brief Parses a string object as a boolean value.
 *
 * This parses semantic boolean strings, such as "true" and "false". It is
 * different from string truthiness conversion, where any non-empty string may
 * be considered true.
 *
 * @param self String object to parse.
 * @param out Output value receiving the parsed boolean.
 * @return true if parsing succeeded, false otherwise.
 */
bool ember_string_object_parse_to_bool(
    struct EmberStringObject *self,
    struct EmberValue *out
);

/**
 * @brief Creates an Ember object value containing a newly allocated string.
 */
#define EO_STR(value) \
    (EV_OBJ(ember_string_object_new((value))))

/**
 * @brief Checks whether an Ember value contains a string object.
 */
#define IS_EO_STR(value) \
    (EV_IS_OBJ((value)) && ember_is_string_object(EV_AS_OBJ((value))))

/**
 * @brief Extracts the string object pointer from an Ember object value.
 */
#define AS_EO_STR(value) \
    ((struct EmberStringObject *)EV_AS_OBJ((value)))

#endif