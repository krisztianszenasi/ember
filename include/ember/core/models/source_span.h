#ifndef EMBER_CORE_SOURCE_SPAN_H
#define EMBER_CORE_SOURCE_SPAN_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Sentinel source span used when no source location is available.
 */
#define EMBER_SOURCE_SPAN_NONE                                      \
    ((struct EmberSourceSpan){                                      \
        .path = NULL,                                               \
        .start_line = -1,                                           \
        .start_column = -1,                                         \
        .end_line = -1,                                             \
        .end_column = -1                                            \
    })

/**
 * @brief Describes a location range in a source file.
 *
 * Source spans are used by diagnostics to point to the part of the input file
 * where an AST node, IR node, or error originated. A span with a NULL path is
 * treated as an unavailable or internal source location.
 */
struct EmberSourceSpan {
    /**
     * @brief Path of the source file, or NULL if no source location is available.
     */
    const char *path;

    /**
     * @brief First line covered by the span.
     */
    int start_line;

    /**
     * @brief First column covered by the span.
     */
    int start_column;

    /**
     * @brief Last line covered by the span.
     */
    int end_line;

    /**
     * @brief Last column covered by the span.
     */
    int end_column;
};

/**
 * @brief Initializes a source span as unavailable.
 *
 * After initialization, the span is equivalent to EMBER_SOURCE_SPAN_NONE.
 *
 * @param self Source span to initialize.
 */
static inline void ember_core_source_span_init(struct EmberSourceSpan *self) {
    self->path = NULL;
    self->end_column = -1;
    self->end_line = -1;
    self->start_column = -1;
    self->start_line = -1;
}

/**
 * @brief Checks whether a source span represents an unavailable location.
 *
 * @param self Source span to inspect.
 * @return true if the span has no source location, false otherwise.
 */
static inline bool ember_source_span_is_none(const struct EmberSourceSpan *self) {
    return self->path == NULL;
}

#endif