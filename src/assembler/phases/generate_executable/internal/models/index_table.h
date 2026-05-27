#ifndef ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_INDEX_TABLE_H
#define ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_INDEX_TABLE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Kinds of elements that can be indexed during executable generation.
 *
 * Each kind has its own independent index counter. This allows functions,
 * globals, and labels to use separate index spaces.
 */
enum EmberIndexElementKind {
    INDEX_ELEMENT_FUNCTION = 0,
    INDEX_ELEMENT_GLOBAL,
    INDEX_ELEMENT_LABEL,
    INDEX_ELEMENT_COUNT,
};

/**
 * @brief Named element registered in an index table scope.
 *
 * An index element maps a name to the numeric index that will be written into
 * the generated executable.
 */
struct EmberIndexElement {
    /**
     * @brief Name of the indexed element.
     */
    const char *name;

    /**
     * @brief Numeric index assigned to the element.
     */
    size_t index;
};

/**
 * @brief Scoped table for resolving names to executable indexes.
 *
 * The index table is used during executable generation to map function names,
 * global names, and labels to their final executable indexes. Tables can be
 * chained through `previous` to model nested scopes, for example
 * function-local label scopes.
 */
struct EmberIndexTable {
    /**
     * @brief Previous index table scope, or NULL for the root scope.
     */
    struct EmberIndexTable *previous;

    /**
     * @brief Number of elements currently stored in this scope.
     */
    size_t count;

    /**
     * @brief Number of elements that can be stored before resizing is needed.
     */
    size_t capacity;

    /**
     * @brief Elements registered in this scope.
     */
    struct EmberIndexElement *elements;

    /**
     * @brief Next automatically assigned index for each element kind.
     */
    size_t next_indexes[INDEX_ELEMENT_COUNT];
};

/**
 * @brief Creates a new root index table.
 *
 * @return Newly allocated index table, or NULL on allocation failure.
 */
struct EmberIndexTable *ember_index_table_new(void);

/**
 * @brief Destroys an index table and its parent scopes.
 *
 * @param self Index table to destroy.
 */
void ember_index_table_destroy(struct EmberIndexTable *self);

/**
 * @brief Registers a named element in the current index table scope.
 *
 * If `custom_index` is used by the implementation, the element receives that
 * index instead of the next automatically assigned index. If a name already
 * exists in the current scope, registration fails.
 *
 * @param self Index table scope to register into.
 * @param name Name of the element to register.
 * @param kind Kind of indexed element.
 * @param custom_index Explicit index to assign, if supported by the caller.
 * @return true on successful registration, false on failure.
 */
bool ember_index_table_register(
    struct EmberIndexTable *self,
    const char *name,
    enum EmberIndexElementKind kind,
    size_t custom_index
);

/**
 * @brief Looks up a name in the current scope chain.
 *
 * The current scope is searched first. If the name is not found, parent scopes
 * are searched until the root scope is reached.
 *
 * @param self Index table scope where lookup should start.
 * @param name Name to look up.
 * @param out Output pointer receiving the resolved index.
 * @return true if the name was found, false otherwise.
 */
bool ember_index_table_lookup(
    struct EmberIndexTable *self,
    const char *name,
    size_t *out
);

/**
 * @brief Enters a new nested index table scope.
 *
 * The returned table uses `self` as its parent scope.
 *
 * @param self Current index table scope.
 * @return Newly created nested scope, or NULL on allocation failure.
 */
struct EmberIndexTable *ember_index_table_enter_scope(struct EmberIndexTable *self);

/**
 * @brief Exits the current index table scope.
 *
 * The current scope is destroyed and its parent scope is returned.
 *
 * @param self Current index table scope.
 * @return Parent scope, or NULL if there is no parent scope.
 */
struct EmberIndexTable *ember_index_table_exit_scope(struct EmberIndexTable *self);

#endif