#ifndef EMBER_CLI_INTERNAL_CLI_TYPES_H
#define EMBER_CLI_INTERNAL_CLI_TYPES_H

#include <stdint.h>
#include <stddef.h>

/**
 * Supported CLI commands.
 */
enum EmberCliCommand {
    EMBER_CLI_COMMAND_NONE,
    EMBER_CLI_COMMAND_ASSEMBLE,
    EMBER_CLI_COMMAND_DISASSEMBLE,
    EMBER_CLI_COMMAND_RUN,
    EMBER_CLI_COMMAND_INSTRUCTION,
};

/**
 * Bit flags controlling CLI behavior.
 */
enum EmberCliFlag {
    EMBER_CLI_FLAG_QUIET = 1,
    EMBER_CLI_FLAG_VERBOSE = 1 << 2,
    EMBER_CLI_FLAG_VERSION = 1 << 3,
};

/**
 * Stores parsed CLI state.
 *
 * The context contains the selected command, active flags, and any relevant
 * file paths derived from the command-line arguments.
 */
struct EmberCliContext {
    enum EmberCliCommand command;
    uint8_t flags;
    char *input_path;
    char *output_path;
    char *log_file_path;
    char *instruction_name;
};

/**
 * Initializes a CLI context.
 *
 * After this call, the context is in a valid default state with no command
 * selected, no flags enabled, and all optional paths cleared.
 *
 * @param self Pointer to the CLI context to initialize.
 */
void ember_cli_context_init(struct EmberCliContext *self);

#endif