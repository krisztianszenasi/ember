#ifndef EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_INSTRUCTION_H
#define EMBER_ASSEMBLER_INTERNAL_MODELS_AST_NODES_SECTION_ITEM_FUNC_DEF_CODE_ITEM_INSTRUCTION_H

#include <stddef.h>

#include "assembler/internal/models/assembly/opcode.h"
#include "assembler/internal/models/ast/nodes/literal.h"
#include "assembler/internal/models/ast/nodes/identifier.h"
#include "assembler/internal/models/semantic/resolved_operand.h"
#include "assembler/internal/models/ast/nodes/section_item/func_def/code_item/base.h"

/**
 * Kinds of instruction operands.
 */
enum EmberAstOperandKind {
    AST_OPERAND_IMMEDIATE,
    AST_OPERAND_REFERENCE,
};

/**
 * @brief AST value representing an instruction operand.
 *
 * An operand is either an immediate literal or an identifier reference.
 * Operands are stored directly inside instruction nodes instead of being
 * allocated as separate AST nodes.
 *
 * During semantic analysis, `resolved` is filled with the operand's resolved
 * role and value type. If the operand resolves to a concrete compile-time
 * value, such as an immediate literal or constant reference, `resolved_value`
 * stores that value for later phases.
 */
struct EmberAstOperand {
    enum EmberAstOperandKind kind;
    struct EmberResolvedOperand resolved;
    struct EmberAstLiteral resolved_value;
    union {
        struct EmberAstLiteral immediate;
        struct EmberAstIdentifier reference;
    } as;
};

/**
 * Creates and initializes an immediate operand.
 *
 * The returned operand takes ownership of the given literal value.
 *
 * @param literal Literal value stored in the operand.
 * @return An initialized immediate operand.
 */
static inline struct EmberAstOperand ember_ast_immediate_operand(
    struct EmberAstLiteral literal
) {
    return (struct EmberAstOperand){
        .kind = AST_OPERAND_IMMEDIATE,
        .as = {
            .immediate = literal,
        },
    };
}

/**
 * Creates and initializes a reference operand.
 *
 * The returned operand takes ownership of the given identifier value.
 *
 * @param identifier Identifier stored in the operand.
 * @return An initialized reference operand.
 */
static inline struct EmberAstOperand ember_ast_reference_operand(
    struct EmberAstIdentifier identifier
) {
    return (struct EmberAstOperand){
        .kind = AST_OPERAND_REFERENCE,
        .as = {
            .reference = identifier,
        },
    };
}

/**
 * Deinitializes an operand.
 *
 * This releases resources owned by the active operand variant in place. Since
 * operands are intended to be embedded inside instruction nodes, this function
 * does not free the operand object itself.
 *
 * @param self Pointer to the operand to deinitialize.
 */
static inline void ember_ast_operand_deinit(struct EmberAstOperand *self) {
    switch (self->kind) {
        case AST_OPERAND_IMMEDIATE:
            ember_ast_literal_deinit(&self->as.immediate);
            break;
        case AST_OPERAND_REFERENCE:
            ember_ast_identifier_deinit(&self->as.reference);
            break;
    }
}

/**
 * AST node representing an instruction code item.
 *
 * An instruction stores its opcode together with a dynamically managed array
 * of operands.
 */
struct EmberAstInstruction {
    struct EmberAstCodeItem base;
    enum EmberAssemblyOpcode opcode;
    size_t operand_count;
    size_t operand_capacity;
    struct EmberAstOperand *operands;
};

/**
 * Allocates and initializes an instruction node.
 *
 * @param opcode Opcode of the instruction.
 * @return A newly allocated instruction node, or NULL on failure.
 */
struct EmberAstInstruction *ember_ast_instruction_new(enum EmberAssemblyOpcode opcode);

/**
 * Destroys an instruction node and all resources it owns.
 *
 * This releases all owned operands, the dynamic operand array, and the
 * instruction node itself.
 *
 * @param self Pointer to the instruction node to destroy. If NULL, no action
 *             is taken.
 */
void ember_ast_instruction_destroy(struct EmberAstInstruction *self);

/**
 * Adds an operand to an instruction.
 *
 * The given operand value is appended to the instruction's internal operand
 * array.
 *
 * @param self Pointer to the instruction node.
 * @param operand Operand to add.
 */
void ember_ast_instruction_add_operand(
    struct EmberAstInstruction *self,
    struct EmberAstOperand operand
);

#endif