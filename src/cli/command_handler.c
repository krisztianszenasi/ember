#include "cli/command_handler.h"

#include <stdio.h>
#include <stdlib.h>

#include <ember/vm.h>
#include <ember/version.h>
#include <ember/assembler.h>
#include <ember/core/models/logger.h>
#include <ember/core/models/executable/io.h>
#include <ember/assembly/instruction_info.h>

static void _show_version_number(struct EmberCliContext *ctx, struct EmberLogger *logger) {
    if (ctx->command != EMBER_CLI_COMMAND_NONE) {
        EMBER_DEBUG(logger, "cli", "ignoring --version flag becuase a command is already set");
        return;
    }
    printf("%s\n", EMBER_VERSION_DISPLAY);
}

static void _handle_flags(struct EmberCliContext *ctx, struct EmberLogger *logger) {
    EMBER_DEBUG(logger, "cli", "processing flags");

    if (ctx->flags & EMBER_CLI_FLAG_VERBOSE) {
        ember_logger_set_verbose(logger, true);
        EMBER_DEBUG(logger, "cli", "processing flag --verbose");
    }

    if (ctx->flags & EMBER_CLI_FLAG_QUIET) {
        EMBER_DEBUG(logger, "cli", "processing flag --quiet");
        ember_logger_set_suppress_warnings(logger, true);
    }

    if (ctx->flags & EMBER_CLI_FLAG_VERSION) {
        EMBER_DEBUG(logger, "cli", "processing flag --version");
        _show_version_number(ctx, logger);
    }
}

static int  _handle_command_assemble(struct EmberCliContext *ctx, struct EmberLogger *logger) {
    struct EmberExecutable *exe;
    int result;

    EMBER_INFO(logger, "cli", "starting assembler");
    
    if (!ctx->input_path) {
        EMBER_ERROR(logger, "cli", "input path was not set");
        return -1;
    }

    exe = ember_assembler_assemble(ctx->input_path, logger);
    if (!exe) {
        return -1;
    }

    if (!ctx->output_path) {
        EMBER_INFO(logger, "cli", "no output path provided, writing to 'a.emx'");
        ctx->output_path = "a.emx";
    }

    result = ember_executable_write(exe, ctx->output_path, logger);
    ember_executable_free(exe);
    return result;
}

static int _handle_command_disassemble(struct EmberCliContext *ctx, struct EmberLogger *logger) {
    FILE *out = stdout;
    struct EmberExecutable *exe;

    EMBER_INFO(logger, "cli", "starting disassembler");

    if (!ctx->input_path) {
        EMBER_ERROR(logger, "cli", "input path was not set");
        return -1;
    }

    exe = ember_executable_read(ctx->input_path, logger);
    if (!exe) { return -1; }

    if (ctx->output_path) {
        out = fopen(ctx->output_path, "w");
        if (!out) {
            EMBER_ERROR(logger, "cli", "could not open output file '%s'", ctx->output_path);
            return -1;
        }
    } else {
        EMBER_INFO(logger, "cli", "no output path provided, writing disassembly to stdout");
    }

    return ember_executable_disassemble_to(exe, stdout);
}

static int _handle_command_run(struct EmberCliContext *ctx, struct EmberLogger *logger) {
    struct EmberExecutable *exe;
    struct EmberVM *vm;
    enum EmberVMRunResult result;
    EMBER_INFO(logger, "cli", "starting to run exe");

    if (!ctx->input_path) {
        EMBER_ERROR(logger, "cli", "input path was not set");
        return -1;
    }

    exe = ember_executable_read(ctx->input_path, logger);
    if (!exe) { return -1; }

    vm = ember_vm_new();
    result = ember_vm_run(vm, exe, logger);
    ember_vm_destroy(vm);
    ember_executable_free(exe);

    return result;
}

static const char *_operand_kind_name(enum EmberAssemblyOperandKind kind)
{
    switch (kind) {
        case EMBER_OPERAND_NONE:
            return "none";
        case EMBER_OPERAND_VALUE:
            return "value";
        case EMBER_OPERAND_GLOBAL:
            return "global";
        case EMBER_OPERAND_FUNCTION:
            return "function";
        case EMBER_OPERAND_LABEL:
            return "label";
    }

    return "unknown";
}

static const char *_value_kind_name(enum EmberAssemblyValueKind kind)
{
    switch (kind) {
        case EMBER_VALUE_ANY:
            return "any";
        case EMBER_VALUE_INT:
            return "int";
        case EMBER_VALUE_FLOAT:
            return "float";
        case EMBER_VALUE_STRING:
            return "string";
        case EMBER_VALUE_BOOL:
            return "bool";
        case EMBER_VALUE_NONE:
            return "none";
    }

    return "unknown";
}

static void _print_instruction_signature(
    const struct EmberAssemblyInstructionInfo *info
) {
    printf("%s", info->name);

    for (size_t i = 0; i < info->operand_count; i++) {
        const struct EmberAssemblyOperandInfo *operand = &info->operands[i];

        printf(" <%s", _operand_kind_name(operand->operand_kind));

        if (operand->operand_kind == EMBER_OPERAND_VALUE) {
            printf(":%s", _value_kind_name(operand->value_kind));
        }

        printf(">");
    }
}

static void _print_instruction_list(void)
{
    size_t count = ember_assembly_instruction_info_count();

    printf("Available instructions:\n");

    for (size_t i = 0; i < count; i++) {
        const struct EmberAssemblyInstructionInfo *info =
            ember_assembly_instruction_info_at(i);

        if (info == NULL) {
            continue;
        }

        printf("  ");
        _print_instruction_signature(info);

        if (info->description != NULL) {
            printf(" - %s", info->description);
        }

        printf("\n");
    }
}

static void _print_single_instruction(
    const struct EmberAssemblyInstructionInfo *info
) {
    printf("Name: %s\n", info->name);

    printf("Usage: ");
    _print_instruction_signature(info);
    printf("\n");

    if (info->description != NULL) {
        printf("Description: %s\n", info->description);
    }

    for (size_t i = 0; i < info->operand_count; i++) {
        const struct EmberAssemblyOperandInfo *operand = &info->operands[i];
        const char *validator_description =
            ember_assembly_operand_validator_description(operand->validator);

        if (validator_description != NULL) {
            printf(
                "Operand %zu constraint: %s\n",
                i + 1,
                validator_description
            );
        }
    }
}

static int _handle_command_instruction(
    struct EmberCliContext *ctx,
    struct EmberLogger *logger
) {
    if (ctx->instruction_name == NULL) {
        _print_instruction_list();
        return 0;
    }

    const struct EmberAssemblyInstructionInfo *info =
        ember_assembly_instruction_info_by_name(ctx->instruction_name);

    if (info == NULL) {
        EMBER_ERROR(
            logger,
            "unknown instruction '%s'",
            ctx->instruction_name
        );
        return 1;
    }

    _print_single_instruction(info);
    return 0;
}

int ember_cli_handle_command(struct EmberCliContext *ctx) {
    struct EmberLogger *logger = ember_logger_new();
    ember_logger_set_log_file(logger, ctx->log_file_path);

    _handle_flags(ctx, logger);

    switch (ctx->command) {
        case EMBER_CLI_COMMAND_ASSEMBLE:
            return _handle_command_assemble(ctx, logger);
        case EMBER_CLI_COMMAND_DISASSEMBLE:
            return _handle_command_disassemble(ctx, logger);
        case EMBER_CLI_COMMAND_RUN: 
            return _handle_command_run(ctx, logger);
        case EMBER_CLI_COMMAND_INSTRUCTION:
            return _handle_command_instruction(ctx, logger);
        case EMBER_CLI_COMMAND_NONE: {
            EMBER_DEBUG(logger, "cli", "command was none");
            break;
        }
        default:
            EMBER_DEBUG(logger, "cli", "unknown cli command");
    }
    ember_logger_free(logger);
    return 0;
}
