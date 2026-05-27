#ifndef EMBER_CORE_LOGGER_H
#define EMBER_CORE_LOGGER_H

#include <stdbool.h>
#include <stdarg.h>
#include <ember/core/models/source_span.h>

/**
 * Opaque logger object.
 *
 * An EmberLogger writes diagnostics to stderr by default and can optionally
 * mirror diagnostics to a log file.
 */
struct EmberLogger;

/**
 * Log severity levels.
 */
enum EmberLoggerLevel {
    EMBER_LOG_DEBUG = 0,
    EMBER_LOG_INFO,
    EMBER_LOG_WARNING,
    EMBER_LOG_ERROR,
};

enum EmberDiagnosticsLevel {
    EMBER_DIAG_INFO = 0,
    EMBER_DIAG_WARNING,
    EMBER_DIAG_ERROR,
};

/**
 * Creates a new logger.
 *
 * Default behavior:
 * - terminal output goes to stderr
 * - only warnings and errors are shown on the terminal
 * - tags are hidden on the terminal
 * - no log file is configured
 *
 * @return A newly created logger, or NULL on failure.
 */
struct EmberLogger *ember_logger_new(void);

/**
 * Destroys a logger.
 *
 * If a log file is configured through ember_logger_set_log_file(), it is
 * closed automatically.
 *
 * @param logger The logger to destroy.
 */
void ember_logger_free(struct EmberLogger *logger);

/**
 * Enables or disables verbose terminal logging.
 *
 * When verbose logging is enabled:
 * - release builds show INFO, WARNING, and ERROR on stderr
 * - debug builds show DEBUG, INFO, WARNING, and ERROR on stderr
 * - tags are shown on stderr
 *
 * When verbose logging is disabled:
 * - only WARNING and ERROR are shown on stderr
 * - tags are hidden on stderr
 *
 * @param logger The logger to update.
 * @param verbose Whether verbose logging is enabled.
 */
void ember_logger_set_verbose(struct EmberLogger *logger, bool verbose);

/**
 * Enables or disables warning suppression.
 *
 * When warning suppression is enabled, only errors are emitted.
 *
 * @param logger The logger to update.
 * @param suppress Whether warnings should be suppressed.
 */
void ember_logger_set_suppress_warnings(struct EmberLogger *logger, bool suppress);

/**
 * Opens or replaces the log file used by the logger.
 *
 * When a log file is configured:
 * - messages are also written to the log file
 * - tags are always shown in the log file
 * - the log file automatically uses verbose logging:
 *   INFO, WARNING, and ERROR in release builds, and
 *   DEBUG, INFO, WARNING, and ERROR in debug builds
 *
 * Passing NULL disables file logging.
 *
 * @param logger The logger to update.
 * @param path The log file path, or NULL to disable file logging.
 * @return 0 on success, non-zero on failure.
 */
int ember_logger_set_log_file(struct EmberLogger *logger, const char *path);

/**
 * Writes a formatted log message.
 *
 * @param logger The logger to use.
 * @param level The severity level of the message.
 * @param tag A short tag identifying the source or category of the message.
 * @param format A printf-style format string.
 * @param ... Additional arguments referenced by the format string.
 */
void ember_logger_log(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level,
    const char *tag,
    const char *format,
    ...
);

/**
 * Writes a formatted log message using a va_list.
 *
 * @param logger The logger to use.
 * @param level The severity level of the message.
 * @param tag A short tag identifying the source or category of the message.
 * @param format A printf-style format string.
 * @param args Additional arguments referenced by the format string.
 */
void ember_logger_logv(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level,
    const char *tag,
    const char *format,
    va_list args
);

void ember_logger_diagnostic(
    const struct EmberLogger *logger,
    enum EmberDiagnosticsLevel level,
    struct EmberSourceSpan source_span,
    const char *format,
    ...
);

void ember_logger_diagnosticv(
    const struct EmberLogger *logger,
    enum EmberDiagnosticsLevel level,
    struct EmberSourceSpan source_span,
    const char *format,
    va_list args
);

#ifdef DEBUG
#define EMBER_DEBUG(logger, tag, ...) \
    ember_logger_log((logger), EMBER_LOG_DEBUG, (tag), __VA_ARGS__)
#else
#define EMBER_DEBUG(logger, tag, ...) ((void)0)
#endif

#define EMBER_INFO(logger, tag, ...) \
    ember_logger_log((logger), EMBER_LOG_INFO, (tag), __VA_ARGS__)

#define EMBER_WARNING(logger, tag, ...) \
    ember_logger_log((logger), EMBER_LOG_WARNING, (tag), __VA_ARGS__)

#define EMBER_ERROR(logger, tag, ...) \
    ember_logger_log((logger), EMBER_LOG_ERROR, (tag), __VA_ARGS__)

#define EMBER_INFO_AT(logger, source_span, ...) \
    ember_logger_diagnostic((logger), EMBER_DIAG_INFO, (source_span), __VA_ARGS__)

#define EMBER_WARNING_AT(logger, source_span, ...) \
    ember_logger_diagnostic((logger), EMBER_DIAG_WARNING, (source_span), __VA_ARGS__)

#define EMBER_ERROR_AT(logger, source_span, ...) \
    ember_logger_diagnostic((logger), EMBER_DIAG_ERROR, (source_span), __VA_ARGS__)

#endif