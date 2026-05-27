#ifndef EMBER_ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_PATCH_TABLE_H
#define EMBER_ASSEMBLER_PHASES_GENERATE_EXECUTABLE_INTERNAL_MODELS_PATCH_TABLE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Patch locations registered for a single unresolved name.
 *
 * A patch element stores all executable locations that need to be updated once
 * the referenced name is resolved. This is mainly used for forward label
 * references, where a jump may refer to a label that is defined later in the
 * same function.
 */
struct EmberPatchElement {
    /**
     * @brief Name of the unresolved target.
     */
    const char *name;

    /**
     * @brief Number of patch locations stored for this target.
     */
    size_t count;

    /**
     * @brief Number of patch locations that can be stored before resizing.
     */
    size_t capacity;

    /**
     * @brief Exe locations that must be patched once the target is known.
     */
    size_t *patch_locations;
};

/**
 * @brief Table of unresolved references that must be patched later.
 *
 * The patch table is used during executable generation when an instruction
 * references a target that has not been assigned an index yet. For example, a
 * jump instruction may reference a label that appears later in the function.
 * The generator records the instruction operand location here and patches it
 * once the label is registered.
 */
struct EmberPatchTable {
    /**
     * @brief Number of unresolved target entries stored in the table.
     */
    size_t count;

    /**
     * @brief Number of target entries that can be stored before resizing.
     */
    size_t capacity;

    /**
     * @brief Unresolved target entries and their patch locations.
     */
    struct EmberPatchElement *elements;
};

/**
 * @brief Creates a new patch table.
 *
 * @return Newly allocated patch table, or NULL on allocation failure.
 */
struct EmberPatchTable *ember_patch_table_new(void);

/**
 * @brief Destroys a patch table.
 *
 * Releases all patch elements and their stored patch-location arrays.
 *
 * @param self Patch table to destroy.
 */
void ember_patch_table_destroy(struct EmberPatchTable *self);

/**
 * @brief Registers a location that must be patched later.
 *
 * If an entry for `name` already exists, `location` is appended to that entry.
 * Otherwise, a new patch entry is created. The location refers to the offset
 * in the bytecode where the dummy value been written and needs patching when
 * actual address is knonw.
 *
 * @param self Patch table to update.
 * @param name Name of the unresolved target.
 * @param location Executable location that must be patched.
 * @return true on successful registration, false on allocation failure.
 */
bool ember_patch_table_register(
    struct EmberPatchTable *self,
    const char *name,
    size_t location
);

/**
 * @brief Looks up patch locations for an unresolved target.
 *
 * @param self Patch table to search.
 * @param name Name of the unresolved target.
 * @param out Output pointer receiving the matching patch element.
 * @return true if an entry exists for the name, false otherwise.
 */
bool ember_patch_table_lookup(
    struct EmberPatchTable *self,
    const char *name,
    struct EmberPatchElement **out
);

#endif