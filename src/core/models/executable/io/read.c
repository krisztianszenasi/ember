#include <ember/core/models/executable/io.h>

#include <stdint.h>
#include <string.h>

#include <ember/version.h>
#include <ember/core/error.h>
#include <ember/core/memory.h>
#include <ember/core/models/executable/builder.h>

#include "core/internal/binary_io.h"
#include "core/internal/models/executable/types.h"
#include "core/models/executable/io/internal/common.h"
#include "core/models/executable/io/internal/value_io.h"

static const uint8_t executable_magic[4] = { 'E', 'M', 'X', '\0' };

int ember_executable_io_read_header(struct EmberExecutable *exe, FILE *file) {
    uint8_t magic[sizeof(executable_magic)];
    uint16_t version;
    uint16_t function_count;
    uint16_t start_function_idx;

    if (exe == NULL || file == NULL) {
        return -1;
    }

    if (ember_binary_io_read_bytes(file, magic, sizeof(magic)) != 0) {
        return -1;
    }

    if (memcmp(magic, executable_magic, sizeof(magic)) != 0) {
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &version) != 0) {
        return -1;
    }

    if (version != EMBER_EXECUTABLE_FORMAT_VERSION) {
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &function_count) != 0) {
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &start_function_idx) != 0) {
        return -1;
    }

    exe->function_count = function_count;
    exe->start_function_idx = start_function_idx;
    return 0;
}

int ember_executable_io_read_function(struct EmberExecutable *exe, FILE *file) {
    char *name = NULL;
    uint16_t arity;
    uint16_t local_count;
    uint16_t const_count;
    uint32_t code_count;
    struct EmberFunction *func;

    if (ember_binary_io_read_string_alloc(file, &name) != 0) {
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &arity) != 0) {
        ember_free(name);
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &local_count) != 0) {
        ember_free(name);
        return -1;
    }

    if (ember_binary_io_read_u16_le(file, &const_count) != 0) {
        ember_free(name);
        return -1;
    }

    func = ember_executable_define_function(exe, name, arity, local_count);
    ember_free(name);
    if (func == NULL) {
        return -1;
    }

    for (size_t i = 0; i < const_count; i++) {
        struct EmberValue value;

        if (ember_executable_io_read_value(file, &value) != 0) {
            return -1;
        }

        ember_function_register_constant(func, value);
    }

    if (ember_binary_io_read_u32_le(file, &code_count) != 0) {
        return -1;
    }

    if (code_count > 0) {
        func->code_capacity = code_count;
        func->code = EMBER_GROW_ARRAY(
            uint8_t,
            func->code,
            0,
            func->code_capacity
        );
    }

    if (ember_binary_io_read_bytes(file, func->code, code_count) != 0) {
        return -1;
    }

    func->code_count = code_count;
    return 0;
}

struct EmberExecutable *ember_executable_read(
    const char *path,
    struct EmberLogger *logger
) {
    FILE *file;
    struct EmberExecutable *exe;
    size_t function_count;

    EMBER_INFO(logger, "io", "reading executable from '%s'", path);

    file = fopen(path, "rb");
    if (file == NULL) {
        EMBER_ERROR(logger, "io", "could not open '%s' for reading", path);
        return NULL;
    }

    exe = ember_executable_new();
    if (exe == NULL) {
        fclose(file);
        EMBER_ERROR(logger, "io", "could not allocate executable");
        return NULL;
    }

    if (ember_executable_io_read_header(exe, file) != 0) {
        fclose(file);
        ember_executable_free(exe);
        EMBER_ERROR(logger, "io", "failed to read executable header");
        return NULL;
    }

    function_count = exe->function_count;
    exe->function_count = 0;

    for (size_t i = 0; i < function_count; i++) {
        if (ember_executable_io_read_function(exe, file) != 0) {
            fclose(file);
            ember_executable_free(exe);
            EMBER_ERROR(logger, "io", "failed to read executable function %zu", i);
            return NULL;
        }
    }

    fclose(file);
    return exe;
}
