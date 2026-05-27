#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <ember/core/error.h>

void ember_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    fputs("ember: error: ", stderr);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);

    va_end(args);
    exit(EXIT_FAILURE);
}