#ifndef EMBER_ASSEMBLER_INTERNAL_LOGGING_H
#define EMBER_ASSEMBLER_INTERNAL_LOGGING_H

#include <ember/core/models/logger.h>

#define ASM_LOG_TAG "assembler"

#define ASSEMBLER_DEBUG(logger, ...) \
    EMBER_DEBUG((logger), ASM_LOG_TAG, __VA_ARGS__)

#define ASSEMBLER_INFO(logger, ...) \
    EMBER_INFO((logger), ASM_LOG_TAG, __VA_ARGS__)

#define ASSEMBLER_WARNING(logger, ...) \
    EMBER_WARNING((logger), ASM_LOG_TAG, __VA_ARGS__)

#define ASSEMBLER_ERROR(logger, ...) \
    EMBER_ERROR((logger), ASM_LOG_TAG, __VA_ARGS__)

#endif