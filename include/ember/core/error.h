#ifndef EMBER_CORE_ERROR_H
#define EMBER_CORE_ERROR_H

/**
 * Reports a fatal error and terminates the program.
 *
 * The message is formatted using printf-style formatting rules.
 *
 * @param format The format string describing the error message.
 * @param ... Additional arguments referenced by the format string.
 */
void ember_error(const char *format, ...);

#endif