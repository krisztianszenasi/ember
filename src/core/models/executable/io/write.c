#include <ember/core/models/executable/io.h>

#include <stdint.h>
#include <stdio.h>

#include <ember/version.h>
#include <ember/core/error.h>

#include "core/internal/binary_io.h"
#include "core/internal/models/executable/types.h"
#include "core/models/executable/io/internal/common.h"
#include "core/models/executable/io/internal/value_io.h"

static const uint8_t executable_magic[4] = { 'E', 'M', 'X', '\0' };

int ember_executable_io_write_header(
    const struct EmberExecutable *exe,
    FILE *file
) {
    if (exe == NULL || file == NULL) {
        return -1;
    }

    if (exe->function_count > UINT16_MAX) {
        return -1;
    }

    if (exe->start_function_idx > UINT16_MAX) {
        return -1;
    }

    if (ember_binary_io_write_bytes(file, executable_magic, sizeof(executable_magic)) != 0) {
        return -1;
    }

    if (ember_binary_io_write_u16_le(file, EMBER_EXECUTABLE_FORMAT_VERSION) != 0) {
        return -1;
    }

    if (ember_binary_io_write_u16_le(file, (uint16_t)exe->function_count) != 0) {
        return -1;
    }

    return ember_binary_io_write_u16_le(file, (uint16_t)exe->start_function_idx);
}

int ember_executable_io_write_function(
    const struct EmberFunction *function,
    FILE *file
) {
    if (function == NULL || file == NULL) {
        return -1;
    }

    if (ember_binary_io_write_string(file, function->name) != 0) {
        return -1;
    }

    if (ember_binary_io_write_u16_le(file, function->arity) != 0) {
        return -1;
    }

    if (ember_binary_io_write_u16_le(file, function->local_count) != 0) {
        return -1;
    }

    if (function->constants.count > UINT16_MAX) {
        return -1;
    }

    if (ember_binary_io_write_u16_le(file, (uint16_t)function->constants.count) != 0) {
        return -1;
    }

    for (size_t i = 0; i < function->constants.count; i++) {
        if (ember_executable_io_write_value(function->constants.values[i], file) != 0) {
            return -1;
        }
    }

    if (function->code_count > UINT32_MAX) {
        return -1;
    }

    if (ember_binary_io_write_u32_le(file, (uint32_t)function->code_count) != 0) {
        return -1;
    }

    return ember_binary_io_write_bytes(file, function->code, function->code_count);
}

int ember_executable_write(
    const struct EmberExecutable *self,
    const char *path,
    struct EmberLogger *logger
) {
    FILE *out = fopen(path, "wb");

    if (out == NULL) {
        EMBER_ERROR(logger, "io", "could not open '%s' for writing", path);
        return -1;
    }

    if (ember_executable_io_write_header(self, out) != 0) {
        fclose(out);
        EMBER_ERROR(logger, "io", "failed to write executable header");
        return -1;
    }

    for (size_t i = 0; i < self->function_count; i++) {
        if (ember_executable_io_write_function(&self->functions[i], out) != 0) {
            fclose(out);
            EMBER_ERROR(logger, "io", "failed to write function '%s'", self->functions[i].name);
            return -1;
        }
    }

    fclose(out);
    return 0;
}
