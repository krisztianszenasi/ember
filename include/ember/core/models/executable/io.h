#ifndef EMBER_CORE_EXECUTABLE_IO_H
#define EMBER_CORE_EXECUTABLE_IO_H

#include <stdio.h>

#include <ember/core/models/logger.h>
#include <ember/core/models/executable/types.h>

/**
 * Reads an executable from the file at the given path.
 *
 * @param path The path to the executable file.
 * @param logger Optional logger for diagnostics.
 * @return A newly created executable instance on success, or NULL on failure.
 */
struct EmberExecutable *ember_executable_read(const char *path, struct EmberLogger *logger);

/**
 * Writes the executable to the file at the given path.
 *
 * @param self The executable to write.
 * @param path The output file path.
 * @param logger Optional logger for diagnostics.
 * @return 0 on success, non-zero on failure.
 */
int ember_executable_write(const struct EmberExecutable *self, const char *path, struct EmberLogger *logger);

/**
 * Writes a human-readable disassembly of the executable to the given stream.
 *
 * This function is primarily intended for debugging, inspection, and
 * disassembly of bytecode.
 *
 * @param self The executable to disassemble.
 * @param out The output stream to write to.
 * @return 0 on success, non-zero on failure.
 */
int ember_executable_disassemble_to(const struct EmberExecutable *self, FILE *out);

#endif