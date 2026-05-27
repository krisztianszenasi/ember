#include <cli.h>

#include <ember/version.h>

#include "cli/types.h"
#include "cli/command_handler.h"

int main(int argc, char **argv) {
    struct EmberCliContext ctx = {0};
    ember_cli_context_init(&ctx);

    clioptions(EMBER_VERSION_DISPLAY, argc, argv) {
        cliopt("-h, --help\tShows usage") {
            cliusage(CLIEXIT);
        }
        cliopt("-v, --verbose\tSets verbosity") {
            ctx.flags |= EMBER_CLI_FLAG_VERBOSE;
        }
        cliopt("--log-file filename\tSets the log file") {
            ctx.log_file_path = cliarg;
        }
        cliopt("--quiet\tSuppresses warnings") {
            ctx.flags |= EMBER_CLI_FLAG_QUIET;
        }
        cliopt("-V, --version\tShows version number") {
            ctx.flags |= EMBER_CLI_FLAG_VERSION;
        }
        cliopt("<disassemble> filename\tPrints the executable in a human-readable format") {
            ctx.input_path = cliarg;
            ctx.command = EMBER_CLI_COMMAND_DISASSEMBLE;
        }
        cliopt("<assemble> filename\tAssembles an executable") {
            ctx.input_path = cliarg;
            ctx.command = EMBER_CLI_COMMAND_ASSEMBLE;
        }
        cliopt("<run> filename\tRuns the given executable") {
            ctx.input_path = cliarg;
            ctx.command = EMBER_CLI_COMMAND_RUN;
        }
        cliopt("<instruction> [name]\tPrints information for all instructions or one specific instruction") {
            ctx.command = EMBER_CLI_COMMAND_INSTRUCTION;
            if (cliarg[0] != '\0') {
                ctx.instruction_name = cliarg;
            } else {
                ctx.instruction_name = NULL;
            }
        }
        cliopt("-o, --output filename\tSpecifies the output file") {
            ctx.output_path = cliarg;
        }
        cliopt() {
            if (cliarg[0] == '-') {
                clierror("Unknown option", cliarg);
            }
            clierror("Unexpected argument", cliarg);
        }
    }
    if (argc == 1) {
        cliusage(CLIEXIT);
    }
    return ember_cli_handle_command(&ctx);
}