#include <ember/core/models/object/str.h>

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ember/core/memory.h>

#include "core/internal/models/object/str.h"

static size_t ember_string_object_size(size_t length) {
    return sizeof(struct EmberStringObject) + length + 1;
}

struct EmberStringObject *ember_string_object_take(char *value, size_t length) {
    struct EmberStringObject *self = ember_alloc(sizeof(struct EmberStringObject));
    ember_object_init(&self->base, EOK_STR, ember_string_object_size(length));

    self->value = value;
    self->length = length;

    return self;
}

struct EmberStringObject *ember_string_object_new(const char *value) {
    if (value == NULL) {
        value = "";
    }

    size_t length = strlen(value);

    char *copy = ember_alloc(length + 1);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, value, length);
    copy[length] = '\0';

    return ember_string_object_take(copy, length);
}

void ember_string_object_destroy(struct EmberStringObject *self) {
    if (self == NULL) {
        return;
    }

    ember_free(self->value);
    ember_free(self);
}

bool ember_is_string_object(struct EmberObject *self) {
    return self != NULL && self->kind == EOK_STR;
}

size_t ember_string_object_get_length(struct EmberStringObject *self) {
    return self->length;
}

void ember_string_object_print(struct EmberStringObject *self) {
    if (self == NULL || self->value == NULL) {
        return;
    }

    printf("%s", self->value);
}

struct EmberStringObject *ember_string_object_from_int(int64_t value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%" PRId64, value);
    return ember_string_object_new(buffer);

}

struct EmberStringObject *ember_string_object_from_float(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);
    return ember_string_object_new(buffer);

}

struct EmberStringObject *ember_string_object_from_bool(bool value) {
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%s", value ? "true" : "false");
    return ember_string_object_new(buffer);
}


struct EmberStringObject *ember_string_object_concat(
    struct EmberStringObject *a,
    struct EmberStringObject *b
) {
    if (a == NULL || b == NULL || a->value == NULL || b->value == NULL) {
        return NULL;
    }

    size_t new_length = a->length + b->length;

    char *concatenated = ember_alloc(new_length + 1);
    if (concatenated == NULL) {
        return NULL;
    }

    memcpy(concatenated, a->value, a->length);
    memcpy(concatenated + a->length, b->value, b->length);
    concatenated[new_length] = '\0';

    return ember_string_object_take(concatenated, new_length);
}

static bool ember_string_is_empty(const char *value) {
    return value == NULL || value[0] == '\0';
}

bool ember_string_object_parse_to_float(
    struct EmberStringObject *self,
    struct EmberValue *out
) {
    if (self == NULL || out == NULL || ember_string_is_empty(self->value)) {
        return false;
    }

    errno = 0;
    char *end = NULL;
    double value = strtod(self->value, &end);

    if (end == self->value) {
        return false;
    }

    if (*end != '\0') {
        return false;
    }

    if (errno == ERANGE) {
        return false;
    }

    if (!isfinite(value)) {
        return false;
    }

    *out = EV_FLOAT(value);
    return true;
}

bool ember_string_object_parse_to_int(
    struct EmberStringObject *self,
    struct EmberValue *out
) {
    if (self == NULL || out == NULL || ember_string_is_empty(self->value)) {
        return false;
    }

    errno = 0;
    char *end = NULL;
    long long value = strtoll(self->value, &end, 10);

    if (end == self->value) {
        return false;
    }

    if (*end != '\0') {
        return false;
    }

    if (errno == ERANGE) {
        return false;
    }

    if (value < INT64_MIN || value > INT64_MAX) {
        return false;
    }

    *out = EV_INT((int64_t)value);
    return true;
}

bool ember_string_object_parse_to_bool(
    struct EmberStringObject *self,
    struct EmberValue *out
) {
    if (self == NULL || out == NULL || self->value == NULL) {
        return false;
    }

    if (strcmp(self->value, "true") == 0) {
        *out = EV_BOOLEAN(true);
        return true;
    }

    if (strcmp(self->value, "false") == 0) {
        *out = EV_BOOLEAN(false);
        return true;
    }

    return false;
}