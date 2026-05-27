#include "core/models/executable/io/internal/value_io.h"

#include <inttypes.h>
#include <string.h>

#include <ember/core/models/object/str.h>

#include "core/internal/binary_io.h"
#include "core/internal/models/object/str.h"

int ember_executable_io_write_value(struct EmberValue value, FILE *file) {
    if (ember_binary_io_write_u8(file, (uint8_t)value.kind) != 0) {
        return -1;
    }

    switch (value.kind) {
        case EVK_BOOLEAN:
            return ember_binary_io_write_boolean(file, value.as.boolean_val);
        case EVK_FLOAT:
            return ember_binary_io_write_f64_le(file, value.as.float_val);
        case EVK_INT:
            return ember_binary_io_write_i64_le(file, value.as.int_val);
        case EVK_NONE:
            return 0;
        case EVK_OBJ:
            if (!IS_EO_STR(value)) {
                return -1;
            }
            return ember_binary_io_write_string(file, AS_EO_STR(value)->value);
    }

    return -1;
}

int ember_executable_io_read_value(FILE *file, struct EmberValue *out) {
    uint8_t kind;

    if (out == NULL) {
        return -1;
    }

    if (ember_binary_io_read_u8(file, &kind) != 0) {
        return -1;
    }

    switch ((enum EmberValueKind)kind) {
        case EVK_BOOLEAN: {
            bool value;

            if (ember_binary_io_read_boolean(file, &value) != 0) {
                return -1;
            }

            *out = EV_BOOLEAN(value);
            return 0;
        }
        case EVK_FLOAT: {
            double value;

            if (ember_binary_io_read_f64_le(file, &value) != 0) {
                return -1;
            }

            *out = EV_FLOAT(value);
            return 0;
        }
        case EVK_INT: {
            int64_t value;

            if (ember_binary_io_read_i64_le(file, &value) != 0) {
                return -1;
            }

            *out = EV_INT(value);
            return 0;
        }
        case EVK_NONE:
            *out = EV_NONE();
            return 0;
        case EVK_OBJ: {
            char *value = NULL;
            struct EmberStringObject *string_object;

            if (ember_binary_io_read_string_alloc(file, &value) != 0) {
                return -1;
            }

            string_object = ember_string_object_take(value, strlen(value));
            if (string_object == NULL) {
                return -1;
            }

            *out = EV_OBJ(string_object);
            return 0;
        }
    }

    return -1;
}

int ember_executable_io_disassemble_value(struct EmberValue value, FILE *out) {
    if (out == NULL) {
        return -1;
    }

    switch (value.kind) {
        case EVK_BOOLEAN:
            fprintf(out, "boolean %s", value.as.boolean_val ? "true" : "false");
            return 0;
        case EVK_FLOAT:
            fprintf(out, "float %g", value.as.float_val);
            return 0;
        case EVK_INT:
            fprintf(out, "int %" PRId64, value.as.int_val);
            return 0;
        case EVK_NONE:
            fprintf(out, "none");
            return 0;
        case EVK_OBJ:
            if (!IS_EO_STR(value)) {
                return -1;
            }

            fprintf(out, "str \"%s\"", AS_EO_STR(value)->value);
            return 0;
    }

    return -1;
}
