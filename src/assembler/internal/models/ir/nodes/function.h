#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_FUNCTION_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_IR_NODES_FUNCTION_H

#include <stddef.h>
#include "assembler/internal/models/ir/nodes/base.h"
#include "assembler/internal/models/assembly/opcode.h"
#include "assembler/internal/models/ir/nodes/code_item.h"
#include "assembler/internal/models/ir/nodes/label_def.h"
#include "assembler/internal/models/ir/nodes/instruction.h"

/**
 * Kinds of IR functions.
 */
enum EmberIrFunctionKind {
    EMBER_IR_FUNCTION_ENTRY,
    EMBER_IR_FUNCTION_INIT,
    EMBER_IR_FUNCTION_USER,
};

/**
 * IR node representing a function body.
 *
 * A function stores its kind, name, signature metadata, and a dynamically
 * managed list of code items forming its body.
 *
 * The `code_items` field stores pointers because code items are polymorphic
 * nodes that may have different concrete types and sizes.
 */
struct EmberIrFunction {
    struct EmberIrNode base;
    enum EmberIrFunctionKind kind;
    char *name;
    size_t arity;
    size_t local_count;
    size_t code_item_count;
    size_t code_item_capacity;
    struct EmberIrCodeItem **code_items;
};

/**
 * Initializes a function node in place.
 *
 * The function starts with no name and an empty code item list.
 *
 * @param self Pointer to the function node to initialize.
 */
void ember_ir_function_init(
    struct EmberIrFunction *self,
    struct EmberSourceSpan source_span
);

/**
 * Deinitializes a function node.
 *
 * This destroys all owned code items, releases the dynamic code item array,
 * and resets the function to an empty initialized state.
 *
 * @param self Pointer to the function node to deinitialize.
 */
void ember_ir_function_deinit(struct EmberIrFunction *self);

/**
 * Adds a code item to the function body.
 *
 * The given code item pointer is appended to the function's internal code
 * item array.
 *
 * @param self Pointer to the function node.
 * @param code_item Pointer to the code item to add.
 */
void ember_ir_function_add_code_item(
    struct EmberIrFunction *self,
    struct EmberIrCodeItem *code_item
);

/**
 * Allocates, appends, and returns a label definition code item.
 *
 * @param self Pointer to the function node.
 * @param name Name of the label to create.
 * @return Pointer to the newly created label definition, or NULL on failure.
 */
struct EmberIrLabelDef *ember_ir_function_add_label(
    struct EmberIrFunction *self,
    const char *name,
    struct EmberSourceSpan source_span
);

/**
 * Allocates, appends, and returns an instruction code item.
 *
 * @param self Pointer to the function node.
 * @param opcode Opcode of the instruction to create.
 * @return Pointer to the newly created instruction, or NULL on failure.
 */
struct EmberIrInstruction *ember_ir_function_add_instruction(
    struct EmberIrFunction *self,
    enum EmberAssemblyOpcode opcode,
    struct EmberSourceSpan source_span
);

#endif
