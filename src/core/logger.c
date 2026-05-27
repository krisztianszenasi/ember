#include <ember/core/models/logger.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct EmberLogger {
    FILE *terminal_stream;
    FILE *log_file_stream;
    bool owns_log_file_stream;
    bool verbose;
    bool suppress_warnings;
};

static const char *_ember_logger_level_name(enum EmberLoggerLevel level) {
    switch (level) {
        case EMBER_LOG_DEBUG:
            return "debug";
        case EMBER_LOG_INFO:
            return "info";
        case EMBER_LOG_WARNING:
            return "warning";
        case EMBER_LOG_ERROR:
            return "error";
        default:
            return "unknown";
    }
}

static const char *_ember_logger_diag_level_name(enum EmberDiagnosticsLevel level) {
    switch (level) {
        case EMBER_DIAG_ERROR:
            return "error";
        case EMBER_DIAG_INFO:
            return "info";
        case EMBER_DIAG_WARNING:
            return "warning";
        default:
            return "unknown";
    }
}

static enum EmberLoggerLevel _ember_logger_verbose_level(void) {
#ifdef DEBUG
    return EMBER_LOG_DEBUG;
#else
    return EMBER_LOG_INFO;
#endif
}

static enum EmberLoggerLevel _ember_logger_terminal_min_level(
    const struct EmberLogger *logger
) {
    if (logger->suppress_warnings) {
        return EMBER_LOG_ERROR;
    }

    if (logger->verbose) {
        return _ember_logger_verbose_level();
    }

    return EMBER_LOG_WARNING;
}

static enum EmberLoggerLevel _ember_logger_file_min_level(
    const struct EmberLogger *logger
) {
    return _ember_logger_verbose_level();
}

static int _ember_logger_should_emit_terminal(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level
) {
    return level >= _ember_logger_terminal_min_level(logger);
}

static int _ember_logger_should_emit_file(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level
) {
    if (!logger->log_file_stream) {
        return 0;
    }

    return level >= _ember_logger_file_min_level(logger);
}

static void _ember_logger_write_message(
    FILE *stream,
    int show_tag,
    enum EmberLoggerLevel level,
    const char *tag,
    const char *format,
    va_list args
) {
    if (!stream) {
        return;
    }

    if (show_tag && tag && tag[0] != '\0') {
        fprintf(
            stream,
            "ember: %s: %s: ",
            tag,
            _ember_logger_level_name(level)
        );
    } else {
        fprintf(stream, "ember: %s: ", _ember_logger_level_name(level));
    }

    vfprintf(stream, format, args);
    fputc('\n', stream);
    fflush(stream);
}

struct EmberLogger *ember_logger_new(void) {
    struct EmberLogger *logger = malloc(sizeof(struct EmberLogger));
    if (!logger) {
        return NULL;
    }

    logger->terminal_stream = stderr;
    logger->log_file_stream = NULL;
    logger->owns_log_file_stream = false;
    logger->verbose = false;
    logger->suppress_warnings = false;

    return logger;
}

void ember_logger_free(struct EmberLogger *logger) {
    if (!logger) {
        return;
    }

    if (logger->owns_log_file_stream && logger->log_file_stream) {
        fclose(logger->log_file_stream);
    }

    free(logger);
}

void ember_logger_set_verbose(struct EmberLogger *logger, bool verbose) {
    if (!logger) {
        return;
    }
    logger->verbose = verbose;
}

void ember_logger_set_suppress_warnings(struct EmberLogger *logger, bool suppress) {
    if (!logger) {
        return;
    }
    logger->suppress_warnings = suppress;
}

int ember_logger_set_log_file(struct EmberLogger *logger, const char *path) {
    FILE *file;

    if (!logger) {
        return -1;
    }

    if (logger->owns_log_file_stream && logger->log_file_stream) {
        fclose(logger->log_file_stream);
        logger->log_file_stream = NULL;
        logger->owns_log_file_stream = false;
    }

    if (!path) {
        return 0;
    }

    file = fopen(path, "w");
    if (!file) {
        return -1;
    }

    logger->log_file_stream = file;
    logger->owns_log_file_stream = true;
    return 0;
}

void ember_logger_logv(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level,
    const char *tag,
    const char *format,
    va_list args
) {
    va_list terminal_args;
    va_list file_args;

    if (!logger || !format) {
        return;
    }

    if (_ember_logger_should_emit_terminal(logger, level)) {
        va_copy(terminal_args, args);
        _ember_logger_write_message(
            logger->terminal_stream,
            logger->verbose,
            level,
            tag,
            format,
            terminal_args
        );
        va_end(terminal_args);
    }

    if (_ember_logger_should_emit_file(logger, level)) {
        va_copy(file_args, args);
        _ember_logger_write_message(
            logger->log_file_stream,
            1,
            level,
            tag,
            format,
            file_args
        );
        va_end(file_args);
    }
}

void ember_logger_log(
    const struct EmberLogger *logger,
    enum EmberLoggerLevel level,
    const char *tag,
    const char *format,
    ...
) {
    va_list args;

    va_start(args, format);
    ember_logger_logv(logger, level, tag, format, args);
    va_end(args);
}

void ember_logger_diagnosticv(
    const struct EmberLogger *logger,
    enum EmberDiagnosticsLevel level,
    struct EmberSourceSpan source_span,
    const char *format,
    va_list args
) {
    va_list terminal_args;
    va_list file_args;
    const char *path;

    if (logger == NULL || format == NULL) {
        return;
    }

    if (level == EMBER_DIAG_INFO && logger->verbose == false) {
        return;
    }

    if (level == EMBER_DIAG_WARNING && logger->suppress_warnings == true) {
        return;
    }

    path = source_span.path != NULL ? source_span.path : "<unknown>";

    va_copy(terminal_args, args);
    fprintf(
        logger->terminal_stream,
        "%s:%d:%d: %s: ",
        path,
        source_span.start_line,
        source_span.start_column,
        _ember_logger_diag_level_name(level)
    );
    vfprintf(logger->terminal_stream, format, terminal_args);
    fputc('\n', logger->terminal_stream);
    va_end(terminal_args);

    if (logger->log_file_stream != NULL) {
        va_copy(file_args, args);
        fprintf(
            logger->log_file_stream,
            "%s:%d:%d: %s: ",
            path,
            source_span.start_line,
            source_span.start_column,
            _ember_logger_diag_level_name(level)
        );
        vfprintf(logger->log_file_stream, format, file_args);
        fputc('\n', logger->log_file_stream);
        va_end(file_args);
    }
}

void ember_logger_diagnostic(
    const struct EmberLogger *logger,
    enum EmberDiagnosticsLevel level,
    struct EmberSourceSpan source_span,
    const char *format,
    ...
) {
    va_list args;

    va_start(args, format);
    ember_logger_diagnosticv(logger, level,  source_span, format, args);
    va_end(args);
}