#include "core/internal/binary_io.h"

#include <ember/core/memory.h>

#include <stdio.h>
#include <string.h>

int ember_binary_io_write_bytes(FILE *file, const void *data, size_t size) {
    if (!file || (!data && size != 0)) {
        return -1;
    }
    return fwrite(data, 1, size, file) == size ? 0 : -1;
}

int ember_binary_io_write_u8(FILE *file, uint8_t value) {
    return ember_binary_io_write_bytes(file, &value, 1);
}

int ember_binary_io_write_u16_le(FILE *file, uint16_t value) {
    uint8_t bytes[2];
    bytes[0] = (uint8_t)(value & 0xFF);
    bytes[1] = (uint8_t)((value >> 8) & 0xFF);
    return ember_binary_io_write_bytes(file, bytes, sizeof(bytes));
}

int ember_binary_io_write_i16_le(FILE *file, int16_t value) {
    return ember_binary_io_write_u16_le(file, (uint16_t)value);
}

int ember_binary_io_write_u32_le(FILE *file, uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = (uint8_t)(value & 0xFF);
    bytes[1] = (uint8_t)((value >> 8) & 0xFF);
    bytes[2] = (uint8_t)((value >> 16) & 0xFF);
    bytes[3] = (uint8_t)((value >> 24) & 0xFF);
    return ember_binary_io_write_bytes(file, bytes, sizeof(bytes));
}

int ember_binary_io_write_i32_le(FILE *file, int32_t value) {
    return ember_binary_io_write_u32_le(file, (uint32_t)value);
}

int ember_binary_io_write_u64_le(FILE *file, uint64_t value) {
    uint8_t bytes[8];
    bytes[0] = (uint8_t)(value & 0xFF);
    bytes[1] = (uint8_t)((value >> 8) & 0xFF);
    bytes[2] = (uint8_t)((value >> 16) & 0xFF);
    bytes[3] = (uint8_t)((value >> 24) & 0xFF);
    bytes[4] = (uint8_t)((value >> 32) & 0xFF);
    bytes[5] = (uint8_t)((value >> 40) & 0xFF);
    bytes[6] = (uint8_t)((value >> 48) & 0xFF);
    bytes[7] = (uint8_t)((value >> 56) & 0xFF);
    return ember_binary_io_write_bytes(file, bytes, sizeof(bytes));
}

int ember_binary_io_write_i64_le(FILE *file, int64_t value) {
    return ember_binary_io_write_u64_le(file, (uint64_t)value);
}

int ember_binary_io_write_f64_le(FILE *file, double value) {
    uint64_t bits;

    memcpy(&bits, &value, sizeof(bits));
    return ember_binary_io_write_u64_le(file, bits);
}

int ember_binary_io_write_string(FILE *file, const char *s) {
    size_t len;
    if (!file || !s) {
        return -1;
    }
    len = strlen(s);
    if (len > UINT32_MAX) {
        return -1;
    }
    if (ember_binary_io_write_u32_le(file, (uint32_t)len) != 0) {
        return -1;
    }
    return ember_binary_io_write_bytes(file, s, len);
}

int ember_binary_io_write_boolean(FILE *file, bool value) {
    return ember_binary_io_write_u8(file, value ? 1u : 0u);
}

int ember_binary_io_read_bytes(FILE *file, void *data, size_t size) {
    if (!file || (!data && size != 0)) {
        return -1;
    }
    return fread(data, 1, size, file) == size ? 0 : -1;
}

int ember_binary_io_read_u8(FILE *file, uint8_t *out) {
    return ember_binary_io_read_bytes(file, out, 1);
}

int ember_binary_io_read_u16_le(FILE *file, uint16_t *out) {
    uint8_t bytes[2];
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_bytes(file, bytes, sizeof(bytes)) != 0) {
        return -1;
    }
    *out = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
    return 0;
}

int ember_binary_io_read_i16_le(FILE *file, int16_t *out) {
    uint16_t value;
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_u16_le(file, &value) != 0) {
        return -1;
    }
    *out = (int16_t)value;
    return 0;
}

int ember_binary_io_read_u32_le(FILE *file, uint32_t *out) {
    uint8_t bytes[4];
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_bytes(file, bytes, sizeof(bytes)) != 0) {
        return -1;
    }
    *out = (uint32_t)bytes[0]
         | ((uint32_t)bytes[1] << 8)
         | ((uint32_t)bytes[2] << 16)
         | ((uint32_t)bytes[3] << 24);

    return 0;
}

int ember_binary_io_read_i32_le(FILE *file, int32_t *out) {
    uint32_t value;
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_u32_le(file, &value) != 0) {
        return -1;
    }
    *out = (int32_t)value;
    return 0;
}

int ember_binary_io_read_u64_le(FILE *file, uint64_t *out) {
    uint8_t bytes[8];
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_bytes(file, bytes, sizeof(bytes)) != 0) {
        return -1;
    }
    *out = (uint64_t)bytes[0]
         | ((uint64_t)bytes[1] << 8)
         | ((uint64_t)bytes[2] << 16)
         | ((uint64_t)bytes[3] << 24)
         | ((uint64_t)bytes[4] << 32)
         | ((uint64_t)bytes[5] << 40)
         | ((uint64_t)bytes[6] << 48)
         | ((uint64_t)bytes[7] << 56);
    return 0;
}

int ember_binary_io_read_i64_le(FILE *file, int64_t *out) {
    uint64_t value;
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_u64_le(file, &value) != 0) {
        return -1;
    }
    *out = (int64_t)value;
    return 0;
}

int ember_binary_io_read_f64_le(FILE *file, double *out) {
    uint64_t bits;

    if (out == NULL) {
        return -1;
    }

    if (ember_binary_io_read_u64_le(file, &bits) != 0) {
        return -1;
    }

    memcpy(out, &bits, sizeof(bits));
    return 0;
}

int ember_binary_io_read_string(FILE *file, char *out, size_t out_size) {
    uint32_t len;
    if (!file || !out || out_size == 0) {
        return -1;
    }
    if (ember_binary_io_read_u32_le(file, &len) != 0) {
        return -1;
    }
    if ((size_t)len + 1 > out_size) {
        return -1;
    }
    if (ember_binary_io_read_bytes(file, out, (size_t)len) != 0) {
        return -1;
    }
    out[len] = '\0';
    return 0;
}

int ember_binary_io_read_string_alloc(FILE *file, char **out) {
    uint32_t len;
    char *buffer;

    if (file == NULL || out == NULL) {
        return -1;
    }

    if (ember_binary_io_read_u32_le(file, &len) != 0) {
        return -1;
    }

    buffer = ember_alloc((size_t)len + 1);
    if (ember_binary_io_read_bytes(file, buffer, (size_t)len) != 0) {
        ember_free(buffer);
        return -1;
    }

    buffer[len] = '\0';
    *out = buffer;
    return 0;
}

int ember_binary_io_read_boolean(FILE *file, bool *out) {
    uint8_t value;
    if (!out) {
        return -1;
    }
    if (ember_binary_io_read_u8(file, &value) != 0) {
        return -1;
    }
    if (value > 1) {
        return -1;
    }
    *out = (value != 0);
    return 0;
}
