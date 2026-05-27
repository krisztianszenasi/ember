#include "operand_validator.h"

static inline struct EmberSourceSpan
operand_source_span(struct EmberAstOperand *operand) {
    switch (operand->kind) {
        case AST_OPERAND_IMMEDIATE:
            return operand->as.immediate.base.source_span;
        case AST_OPERAND_REFERENCE:
            return operand->as.reference.base.source_span;
    }
    return (struct EmberSourceSpan){0};
}

static bool validate_positive_int(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    struct EmberAstOperand *operand
) {
    (void)func;
    if (operand->resolved_value.as.int_val <= 0) {
        EMBER_ERROR_AT(
            logger,
            operand_source_span(operand),
            "operand value must be greater than zero"
        );
        return false;
    }
    return true;
}

static bool validate_non_negative_int(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    struct EmberAstOperand *operand
) {
    (void)func;
    if (operand->resolved_value.as.int_val < 0) {
        EMBER_ERROR_AT(
            logger,
            operand_source_span(operand),
            "operand value must be zero or greater"
        );
        return false;
    }
    return true;
}

static bool validate_arg_index_in_range(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    struct EmberAstOperand *operand
) {
    int64_t index = operand->resolved_value.as.int_val;
    if (index < 0 || (size_t)index >= func->arity) {
        EMBER_ERROR_AT(
            logger,
            operand_source_span(operand),
            "argument index %lld is out of range; function has %zu argument%s",
            index,
            func->arity,
            func->arity == 1 ? "" : "s"
        );
        return false;
    }
    return true;
}

static bool validate_local_index_in_range(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    struct EmberAstOperand *operand
) {
    int64_t index = operand->resolved_value.as.int_val;
    if (index < 0 || (size_t)index >= func->local_count) {
        EMBER_ERROR_AT(
            logger,
            operand_source_span(operand),
            "local index %lld is out of range; function has %zu local%s",
            index,
            func->local_count,
            func->local_count == 1 ? "" : "s"
        );
        return false;
    }
    return true;
}

bool run_operand_validator(
    struct EmberLogger *logger,
    const struct EmberAstFuncDef *func,
    const struct EmberAssemblyOperandInfo *expected,
    struct EmberAstOperand *operand
) {
    if (operand->resolved.invalid) {
        return true;
    }

    switch (expected->validator) {
        case EMBER_OPERAND_VALIDATOR_NONE:
            return true;
        case EMBER_OPERAND_VALIDATOR_POSITIVE_INT:
            return validate_positive_int(logger, func, operand);
        case EMBER_OPERAND_VALIDATOR_NON_NEGATIVE_INT:
            return validate_non_negative_int(logger, func, operand);
        case EMBER_OPERAND_VALIDATOR_ARG_INDEX_IN_RANGE:
            return validate_arg_index_in_range(logger, func, operand);
        case EMBER_OPERAND_VALIDATOR_LOCAL_INDEX_IN_RANGE:
            return validate_local_index_in_range(logger, func, operand);
    }

    return false;
}