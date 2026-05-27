#ifndef EMBER_CORE_INTERNAL_CORE_BINARY_IO_H
#define EMBER_CORE_INTERNAL_CORE_BINARY_IO_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Writes a raw sequence of bytes to the given file.
 *
 * @param file The file to write to.
 * @param data The byte buffer to write.
 * @param size The number of bytes to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_bytes(FILE *file, const void *data, size_t size);

/**
 * Writes an unsigned 8-bit value to the given file.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_u8(FILE *file, uint8_t value);

/**
 * Writes an unsigned 16-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_u16_le(FILE *file, uint16_t value);

/**
 * Writes a signed 16-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_i16_le(FILE *file, int16_t value);

/**
 * Writes an unsigned 32-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_u32_le(FILE *file, uint32_t value);

/**
 * Writes a signed 32-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_i32_le(FILE *file, int32_t value);

/**
 * Writes an unsigned 64-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_u64_le(FILE *file, uint64_t value);

/**
 * Writes a signed 64-bit value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_i64_le(FILE *file, int64_t value);

/**
 * Writes a 64-bit floating-point value in little-endian byte order.
 *
 * @param file The file to write to.
 * @param value The value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_f64_le(FILE *file, double value);

/**
 * Writes a length-prefixed string to the given file.
 *
 * The string is written as its length followed by its raw bytes, without the
 * trailing null terminator.
 *
 * @param file The file to write to.
 * @param s The string to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_string(FILE *file, const char *s);

/**
 * Writes a boolean value to the given file.
 *
 * The value is encoded as a single byte: 0 for false and 1 for true.
 *
 * @param file The file to write to.
 * @param value The boolean value to write.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_write_boolean(FILE *file, bool value);

/**
 * Reads a raw sequence of bytes from the given file.
 *
 * @param file The file to read from.
 * @param data The destination buffer.
 * @param size The number of bytes to read.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_bytes(FILE *file, void *data, size_t size);

/**
 * Reads an unsigned 8-bit value from the given file.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_u8(FILE *file, uint8_t *out);

/**
 * Reads an unsigned 16-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_u16_le(FILE *file, uint16_t *out);

/**
 * Reads a signed 16-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_i16_le(FILE *file, int16_t *out);

/**
 * Reads an unsigned 32-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_u32_le(FILE *file, uint32_t *out);

/**
 * Reads a signed 32-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_i32_le(FILE *file, int32_t *out);

/**
 * Reads an unsigned 64-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_u64_le(FILE *file, uint64_t *out);

/**
 * Reads a signed 64-bit value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_i64_le(FILE *file, int64_t *out);

/**
 * Reads a 64-bit floating-point value in little-endian byte order.
 *
 * @param file The file to read from.
 * @param out Receives the value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_f64_le(FILE *file, double *out);

/**
 * Reads a length-prefixed string from the given file into the provided buffer.
 *
 * The string is expected to be stored as its length followed by its raw bytes.
 * On success, the result is null-terminated in the destination buffer.
 *
 * @param file The file to read from.
 * @param out The destination buffer.
 * @param out_size The size of the destination buffer in bytes.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_string(FILE *file, char *out, size_t out_size);

/**
 * Reads a length-prefixed string into a newly allocated buffer.
 *
 * On success, `*out` receives a null-terminated heap-allocated string owned by
 * the caller.
 *
 * @param file The file to read from.
 * @param out Receives the allocated string on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_string_alloc(FILE *file, char **out);

/**
 * Reads a boolean value from the given file.
 *
 * The value is expected to be encoded as a single byte: 0 for false and 1 for
 * true.
 *
 * @param file The file to read from.
 * @param out Receives the boolean value on success.
 * @return 0 on success, non-zero on failure.
 */
int ember_binary_io_read_boolean(FILE *file, bool *out);

#endif
