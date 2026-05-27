#include <ember/core/models/value/base.h>

#include "core/internal/models/object/base.h"

void ember_value_deinit(struct EmberValue *self) {
    if (self->kind == EVK_OBJ) {
        ember_object_destroy(EV_AS_OBJ(*self));
    }
}