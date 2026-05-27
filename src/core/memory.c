#include <ember/core/memory.h>

#include <stdio.h>
#include <stdlib.h>

#include "ember/core/memory.h"

#include <string.h>

char *ember_strdup(const char *string)
{
    if (string == NULL) {
        return NULL;
    }

    size_t length = strlen(string);
    char *copy = EMBER_ALLOCATE(char, 0, length + 1);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, string, length + 1);

    return copy;
}

void *ember_alloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "EMBER ERROR: failed to allocate memory.\n");
        abort();
    }
    return ptr;
}

void *ember_realloc(void *ptr, size_t old_size, size_t new_size) {
    (void)old_size;

    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "EMBER ERROR: failed to reallocate memory.\n");
        abort();
    }
    return new_ptr;
}

void ember_free(void *ptr) {
    free(ptr);
}