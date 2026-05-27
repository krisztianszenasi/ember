#ifndef EMBER_CORE_INTERNAL_MODELS_OBJECT_STR_H
#define EMBER_CORE_INTERNAL_MODELS_OBJECT_STR_H

#include <stddef.h>

#include "core/internal/models/object/base.h"

struct EmberStringObject {
    struct EmberObject base;
    char *value;
    size_t length;
};

#endif