#ifndef EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_SPECIAL_NAMES_H
#define EMBER_ASSEMBLER_PHASES_LOWER_AST_TO_IR_INTERNAL_SPECIAL_NAMES_H

/**
 * @brief Name of the synthetic entry function.
 *
 * The entry function is generated during AST-to-IR lowering and is used as the
 * executable's actual starting point. It calls the initialization function when
 * global initialization is required, then calls the user-defined main function.
 */
#define EMBER_ENTRY_FUNCTION_NAME "$entry"

/**
 * @brief Name of the synthetic global initialization function.
 *
 * The initialization function is generated from the globals section during
 * AST-to-IR lowering. It assigns the initial values of global variables before
 * the user-defined main function is called.
 */
#define EMBER_INIT_FUNCTION_NAME "$init"

#endif