#include <ember/core/models/object/base.h>

#include <ember/core/models/object/str.h>

#include "core/internal/models/object/base.h"

void ember_print_object(struct EmberObject *object) {
    if (object == NULL) {
        return;
    }

    switch (object->kind) {
        case EOK_STR:
            ember_string_object_print((struct EmberStringObject *)object);
            break;
    }
}

void ember_object_destroy(struct EmberObject *object) {
    if (object == NULL) {
        return;
    }

    switch (object->kind) {
        case EOK_STR:
            ember_string_object_destroy((struct EmberStringObject *)object);
            break;
    }
}

void ember_object_list_destroy(struct EmberObject *head) {
    while (head != NULL) {
        struct EmberObject *next = head->next;
        ember_object_destroy(head);
        head = next;
    }
}

struct EmberObject *ember_object_list_get_next(struct EmberObject *self) {
    if (self == NULL) {
        return NULL;
    }

    return self->next;
}

void ember_object_list_set_next(
    struct EmberObject *self,
    struct EmberObject *next
) {
    if (self == NULL) {
        return;
    }

    self->next = next;
}

size_t ember_object_get_size(struct EmberObject *self) {
    if (self == NULL) {
        return 0;
    }

    return self->size;
}

void ember_object_mark(struct EmberObject *self) {
    if (self == NULL) {
        return;
    }

    self->marked = true;
}

void ember_object_unmark(struct EmberObject *self) {
    if (self == NULL) {
        return;
    }

    self->marked = false;
}

bool ember_object_is_marked(struct EmberObject *self) {
    if (self == NULL) {
        return false;
    }

    return self->marked;
}