#ifndef EMBER_CLI_INTERNAL_CLI_COMMAND_HANDLER_H
#define EMBER_CLI_INTERNAL_CLI_COMMAND_HANDLER_H

#include "types.h"

/**
 * Handles the command described by the CLI context.
 *
 * This function examines the parsed command-line state stored in `ctx` and
 * dispatches execution to the appropriate command handler.
 *
 * @param ctx Pointer to the CLI context containing the parsed command data.
 * @return 0 on success, non-zero on failure.
 */
int ember_cli_handle_command(struct EmberCliContext *ctx);

#endif