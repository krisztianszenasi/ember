#ifndef EMBER_CORE_MODELS_EXECUTABLE_IO_INTERNAL_COMMON_H
#define EMBER_CORE_MODELS_EXECUTABLE_IO_INTERNAL_COMMON_H

#include <stdio.h>

#include <ember/core/models/executable/types.h>

int ember_executable_io_read_header(struct EmberExecutable *exe, FILE *file);
int ember_executable_io_write_header(
    const struct EmberExecutable *exe,
    FILE *file
);

int ember_executable_io_read_function(struct EmberExecutable *exe, FILE *file);
int ember_executable_io_write_function(
    const struct EmberFunction *function,
    FILE *file
);

#endif
