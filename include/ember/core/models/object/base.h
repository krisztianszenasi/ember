#ifndef EMBER_CORE_OBJECT_BASE_H
#define EMBER_CORE_OBJECT_BASE_H

#include <stdbool.h>
#include <stddef.h>

struct EmberObject;

/**
 * @brief Prints an object to standard output.
 *
 * Dispatches to the appropriate object-specific print function based on the
 * object's runtime kind.
 *
 * @param object Object to print.
 */
void ember_print_object(struct EmberObject *object);

/**
 * @brief Destroys an object and releases its owned memory.
 *
 * Dispatches to the appropriate object-specific destroy function based on the
 * object's runtime kind.
 *
 * @param object Object to destroy.
 */
void ember_object_destroy(struct EmberObject *object);

/**
 * @brief Destroys a linked list of managed objects.
 *
 * Walks the object list starting at `head` and destroys every object in the
 * list.
 *
 * @param head First object in the list.
 */
void ember_object_list_destroy(struct EmberObject *head);

/**
 * @brief Returns the next object in the managed object list.
 *
 * @param self Object whose next pointer should be returned.
 * @return Next object in the list, or NULL if there is no next object.
 */
struct EmberObject *ember_object_list_get_next(struct EmberObject *self);

/**
 * @brief Sets the next object in the managed object list.
 *
 * @param self Object whose next pointer should be updated.
 * @param head Object that should become the next list element.
 */
void ember_object_list_set_next(
    struct EmberObject *self,
    struct EmberObject *head
);

/**
 * @brief Returns the number of heap bytes owned by an object.
 *
 * The returned size is used by the VM for heap accounting and garbage
 * collection thresholds.
 *
 * @param self Object to inspect.
 * @return Number of heap bytes owned by the object.
 */
size_t ember_object_get_size(struct EmberObject *self);

/**
 * @brief Marks an object as reachable for garbage collection.
 *
 * @param self Object to mark.
 */
void ember_object_mark(struct EmberObject *self);

/**
 * @brief Clears the garbage collection mark from an object.
 *
 * This is usually done during the sweep phase for objects that survived the
 * current collection cycle.
 *
 * @param self Object to unmark.
 */
void ember_object_unmark(struct EmberObject *self);

/**
 * @brief Checks whether an object is marked as reachable.
 *
 * @param self Object to inspect.
 * @return true if the object is marked, false otherwise.
 */
bool ember_object_is_marked(struct EmberObject *self);

#endif