#include <ember/core/models/executable/io.h>

#include <inttypes.h>

#include <ember/version.h>
#include <ember/core/opcode.h>

#include "core/internal/models/executable/types.h"
#include "core/models/executable/io/internal/common.h"
#include "core/models/executable/io/internal/value_io.h"

static inline uint8_t read_u8(const uint8_t *code, size_t *pc) {
    return code[(*pc)++];
}

static inline uint16_t read_u16(const uint8_t *code, size_t *pc) {
    uint16_t lo = (uint16_t)code[(*pc)++];
    uint16_t hi = (uint16_t)code[(*pc)++];
    return lo | (hi << 8);
}

static inline int16_t read_i16(const uint8_t *code, size_t *pc) {
    return (int16_t)read_u16(code, pc);
}

static int ember_function_disassemble_to(
    const struct EmberFunction *self,
    size_t idx,
    FILE *out
) {
    if (self == NULL || out == NULL) {
        return -1;
    }

    fprintf(out, "function[%zu] %s:\n", idx, self->name);
    fprintf(out, "\tarity: %zu\n", self->arity);
    fprintf(out, "\tlocal_count: %zu\n", self->local_count);
    fprintf(out, "\tconstant_count: %zu\n", self->constants.count);
    fprintf(out, "\tcode_count: %zu\n", self->code_count);

    if (self->constants.count > 0) {
        fprintf(out, "\n\tconstants:\n");
        for (size_t i = 0; i < self->constants.count; i++) {
            fprintf(out, "\t\t[%zu]: ", i);
            ember_executable_io_disassemble_value(self->constants.values[i], out);
            fprintf(out, "\n");
        }
    }

    fprintf(out, "\n\tcode:\n");

    for (size_t counter = 0; counter < self->code_count;) {
        size_t inst_offset = counter;
        struct EmberInstructionInfo info;

        fprintf(out, "\t\t%04zu ", inst_offset);

        info = ember_opcode_get_instruction_info(self->code[counter++]);
        fprintf(out, "%s", info.name);

        for (size_t i = 0; i < info.operand_count; i++) {
            switch (info.operands[i]) {
                case OPERAND_NONE:
                    break;
                case OPERAND_U8:
                    fprintf(out, " %" PRIu8, read_u8(self->code, &counter));
                    break;
                case OPERAND_U16:
                    fprintf(out, " %" PRIu16, read_u16(self->code, &counter));
                    break;
                case OPERAND_I16:
                    fprintf(out, " %" PRId16, read_i16(self->code, &counter));
                    break;
            }
        }

        fprintf(out, "\n");
    }

    return 0;
}

int ember_executable_disassemble_to(const struct EmberExecutable *self, FILE *out) {
    if (self == NULL || out == NULL) {
        return -1;
    }

    fprintf(out, "Ember Executable\n");
    fprintf(out, "header:\n");
    fprintf(out, "\tmagic: EMX\n");
    fprintf(out, "\tversion: %d\n", EMBER_EXECUTABLE_FORMAT_VERSION);
    fprintf(out, "\tfunction_count: %zu\n", self->function_count);
    fprintf(out, "\tstart_function_idx: %zu\n", self->start_function_idx);

    for (size_t i = 0; i < self->function_count; i++) {
        fprintf(out, "\n");
        ember_function_disassemble_to(&self->functions[i], i, out);
    }

    return 0;
}
