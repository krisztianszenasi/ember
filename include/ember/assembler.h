#ifndef EMBER_ASSEMBLER_H
#define EMBER_ASSEMBLER_H

#include <ember/core/models/logger.h>
#include <ember/core/models/executable/types.h>

/**
 * Assembles a source file into an executable.
 *
 * This function reads the assembly source located at `path`, parses and
 * validates it, and produces an executable representation on success.
 * Diagnostics encountered during assembly are reported through `logger`.
 *
 * @param path Path to the input assembly source file.
 * @param logger Logger used to emit diagnostics and other messages.
 * @return A newly assembled executable on success, or NULL on failure.
 */
struct EmberExecutable *ember_assembler_assemble(const char *path, struct EmberLogger *logger);

#endif