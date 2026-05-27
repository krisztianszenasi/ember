#include "cli/types.h"

void ember_cli_context_init(struct EmberCliContext *self) {
    self->command = EMBER_CLI_COMMAND_NONE;
    self->flags = 0;
    self->input_path = NULL;
    self->output_path = NULL;
    self->instruction_name = NULL;
}