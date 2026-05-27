#ifndef EMBER_CORE_MODELS_EXECUTABLE_IO_INTERNAL_VALUE_IO_H
#define EMBER_CORE_MODELS_EXECUTABLE_IO_INTERNAL_VALUE_IO_H

#include <stdio.h>

#include <ember/core/models/value/base.h>

int ember_executable_io_read_value(FILE *file, struct EmberValue *out);
int ember_executable_io_write_value(struct EmberValue value, FILE *file);
int ember_executable_io_disassemble_value(struct EmberValue value, FILE *out);

#endif
