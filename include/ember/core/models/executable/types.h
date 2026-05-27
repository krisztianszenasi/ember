#ifndef EMBER_CORE_EXECUTABLE_TYPE_H
#define EMBER_CORE_EXECUTABLE_TYPE_H

#include <stddef.h>

/**
 * Opaque executable object.
 *
 * An EmberExecutable represents a complete runnable program for the Ember VM.
 * It owns the functions and other data needed during execution.
 */
struct EmberExecutable;

/**
 * Creates a new executable object.
 *
 * @return A newly created executable, or NULL on failure.
 */
struct EmberExecutable *ember_executable_new(void);

/**
 * Destroys an executable object and releases all resources it owns.
 *
 * @param self The executable to destroy.
 */
void ember_executable_free(struct EmberExecutable *self);

/**
 * Opaque function object.
 *
 * An EmberFunction represents a single function defined within an
 * EmberExecutable. Function objects are used to build and inspect the
 * bytecode of a program.
 */
struct EmberFunction;

/**
 * Identifies a position in a function's bytecode.
 *
 * Labels are used as jump targets when emitting or patching control-flow
 * instructions.
 */
typedef size_t EmberLabel;

#endif