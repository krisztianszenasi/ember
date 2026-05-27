%{
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <ember/core/models/logger.h>
#include "assembler/phases/parse/internal/models/context.h"
#include "assembler/internal/models/ast/nodes/all.h"

int yylex(void);
void yyerror(struct EmberParserContext *ctx, const char *s);

#define YYLTYPE_TO_SOURCE_SPAN(yyloc)                               \
    ((struct EmberSourceSpan) {                                     \
        .path = ctx->path,                                          \
        .start_line = (yyloc).first_line,                           \
        .start_column = (yyloc).first_column,                       \
        .end_line = (yyloc).last_line,                              \
        .end_column = (yyloc).last_column                           \
    })

%}

%locations
%define parse.error verbose
%parse-param { struct EmberParserContext *ctx }

%union {
    struct EmberAstIdentifier identifier;
    struct EmberAstSectionItem *section_item;
    
    struct EmberAstOperand operand;
    struct EmberAstCodeItem *code_item;
    struct EmberAstLabelDef *label_def;
    struct EmberAstInstruction *instruction;
    struct EmberAstLiteral literal;

    enum EmberAssemblyOpcode opcode;
    bool boolean_val;
    double float_val;
    int64_t int_val;
    char *str_val;
}

%token FUNC ARITY LOCALS

%token <opcode> OPCODE
%token <str_val> IDENTIFIER
%token <boolean_val> BOOLEAN
%token <float_val> FLOAT
%token <int_val> INT
%token <str_val> STRING

%type <operand> operand
%type <section_item> section_item assignment func_def
%type <code_item> code_item
%type <label_def> label_def
%type <instruction> instruction
%type <identifier> identifier
%type <literal> literal

%%

program
    :   section_list
        {
            ember_ast_node_set_source_span(
                &ctx->out_program->base,
                YYLTYPE_TO_SOURCE_SPAN(@1)
            );
        }
    ;

section_list
    :   section_list section
    |   /* empty */
    ;

section
    :   section_header section_item_list
        {
            ember_ast_node_set_source_span(
                &ctx->current_section->base,
                YYLTYPE_TO_SOURCE_SPAN(@$)
            );
            ctx->current_section = NULL;
        }
    ;

section_header
    :   '.' identifier ':'
        {
            ctx->current_section = ember_ast_program_add_section(ctx->out_program, $2);
        }
    ;

section_item_list
    :   section_item_list section_item
        {
            ember_ast_section_add_item(ctx->current_section, $2);
        }
    |   /* empty */
    ;

section_item
    :   assignment
    |   func_def
    ;

assignment
    :   identifier '=' literal
        {
            $$ = &ember_ast_assignment_new($1, $3, YYLTYPE_TO_SOURCE_SPAN(@$))->base;
        }
    ;

func_def
    :   func_header func_body
        {
            ctx->current_func_def->base.base.source_span = YYLTYPE_TO_SOURCE_SPAN(@$);
            $$ = &ctx->current_func_def->base;
            ctx->current_func_def = NULL;
        }
    ;

literal
    :   BOOLEAN { $$ = ember_ast_boolean_literal($1, YYLTYPE_TO_SOURCE_SPAN(@1)); }
    |   FLOAT   { $$ = ember_ast_float_literal($1, YYLTYPE_TO_SOURCE_SPAN(@1)); }
    |   INT     { $$ = ember_ast_int_literal($1, YYLTYPE_TO_SOURCE_SPAN(@1)); }
    |   STRING  { $$ = ember_ast_str_literal($1, YYLTYPE_TO_SOURCE_SPAN(@1)); }
    ;

func_header
    :   FUNC identifier '(' ARITY '=' INT ',' LOCALS '=' INT ')' ':'
        {
            ctx->current_func_def = ember_ast_func_def_new($2, $6, $10);
        }
    ;

func_body
    :   func_body code_item
        {
            ember_ast_func_def_add_code_item(ctx->current_func_def, $2);
        }
    |   /* emprty */
    ;

code_item
    :   instruction
        {
            $$ = &$1->base;
            ember_ast_node_set_source_span(&$1->base.base, YYLTYPE_TO_SOURCE_SPAN(@1));
            ctx->current_instruction = NULL;
        }
    |   label_def
        {
            $$ = &$1->base;
        }
    ;

instruction
    :   opcode operands
        {
            $$ = ctx->current_instruction;
        }
    ;

opcode
    :   OPCODE
        {
            ctx->current_instruction = ember_ast_instruction_new($1);
        }
    ;

label_def
    :   '@' identifier ':'
        {
            $$ = ember_ast_label_def_new($2, YYLTYPE_TO_SOURCE_SPAN(@$));
        }
    ;

operands
    :   operands operand
        {
            ember_ast_instruction_add_operand(ctx->current_instruction, $2);
        }
    |   /* empty */
    ;

operand
    :   literal     { $$ = ember_ast_immediate_operand($1); }
    |   identifier  { $$ = ember_ast_reference_operand($1); }
    ;

identifier
    :   IDENTIFIER
        {
            $$ = ember_ast_identifier($1, YYLTYPE_TO_SOURCE_SPAN(@1));
        }
    ;


%%

void yyerror(struct EmberParserContext *ctx, const char *s) {
    EMBER_ERROR_AT(
        ctx->logger,
        YYLTYPE_TO_SOURCE_SPAN(yylloc),
        "%s",
        s
    );
}