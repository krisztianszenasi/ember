%option noyywrap
%option never-interactive

%{
    #include <stdlib.h>
    #include <ember/core/memory.h>
    #include "assembler/internal/models/ast/nodes/all.h"
    #include "assembler/phases/parse/internal/models/context.h"
    #include "parser.tab.h"

    static int yyline = 1;
    static int yycolumn = 1;

    #define SET_LOC()                                      \
        do {                                               \
            yylloc.first_line = yyline;                    \
            yylloc.first_column = yycolumn;                \
            yylloc.last_line = yyline;                     \
            yylloc.last_column = yycolumn + yyleng - 1;    \
        } while (0)

    #define RETURN_TOKEN(tok)   \
        do {                    \
            SET_LOC();          \
            yycolumn += yyleng; \
            return (tok);       \
        } while (0)
%}
%%

"func"      { RETURN_TOKEN(FUNC);   }
"arity"     { RETURN_TOKEN(ARITY);  }
"locals"    { RETURN_TOKEN(LOCALS); }

"ldc" {
    yylval.opcode = EMBER_ASM_OPCODE_LDC;
    RETURN_TOKEN(OPCODE);
}

"dup" {
    yylval.opcode = EMBER_ASM_OPCODE_DUP;
    RETURN_TOKEN(OPCODE);
}

"pop" {
    yylval.opcode = EMBER_ASM_OPCODE_POP;
    RETURN_TOKEN(OPCODE);
}

"geta" {
    yylval.opcode = EMBER_ASM_OPCODE_GETA;
    RETURN_TOKEN(OPCODE);
}

"getl" {
    yylval.opcode = EMBER_ASM_OPCODE_GETL;
    RETURN_TOKEN(OPCODE);
}

"setl" {
    yylval.opcode = EMBER_ASM_OPCODE_SETL;
    RETURN_TOKEN(OPCODE);
}

"getg" {
    yylval.opcode = EMBER_ASM_OPCODE_GETG;
    RETURN_TOKEN(OPCODE);
}

"setg" {
    yylval.opcode = EMBER_ASM_OPCODE_SETG;
    RETURN_TOKEN(OPCODE);
}

"iadd" {
    yylval.opcode = EMBER_ASM_OPCODE_IADD;
    RETURN_TOKEN(OPCODE);
}

"isub" {
    yylval.opcode = EMBER_ASM_OPCODE_ISUB;
    RETURN_TOKEN(OPCODE);
}

"imul" {
    yylval.opcode = EMBER_ASM_OPCODE_IMUL;
    RETURN_TOKEN(OPCODE);
}

"idiv" {
    yylval.opcode = EMBER_ASM_OPCODE_IDIV;
    RETURN_TOKEN(OPCODE);
}

"irem" {
    yylval.opcode = EMBER_ASM_OPCODE_IREM;
    RETURN_TOKEN(OPCODE);
}

"fadd" {
    yylval.opcode = EMBER_ASM_OPCODE_FADD;
    RETURN_TOKEN(OPCODE);
}

"fsub" {
    yylval.opcode = EMBER_ASM_OPCODE_FSUB;
    RETURN_TOKEN(OPCODE);
}

"fmul" {
    yylval.opcode = EMBER_ASM_OPCODE_FMUL;
    RETURN_TOKEN(OPCODE);
}

"fdiv" {
    yylval.opcode = EMBER_ASM_OPCODE_FDIV;
    RETURN_TOKEN(OPCODE);
}

"concat" {
    yylval.opcode = EMEBR_ASM_OPCODE_CONCAT;
    RETURN_TOKEN(OPCODE);
}

"and" {
    yylval.opcode = EMBER_ASM_OPCODE_AND;
    RETURN_TOKEN(OPCODE);
}

"or" {
    yylval.opcode = EMBER_ASM_OPCODE_OR;
    RETURN_TOKEN(OPCODE);
}

"not" {
    yylval.opcode = EMBER_ASM_OPCODE_NOT;
    RETURN_TOKEN(OPCODE);
}

"eq" {
    yylval.opcode = EMBER_ASM_OPCODE_EQ;
    RETURN_TOKEN(OPCODE);
}

"neq" {
    yylval.opcode = EMBER_ASM_OPCODE_NEQ;
    RETURN_TOKEN(OPCODE);
}

"gt" {
    yylval.opcode = EMBER_ASM_OPCODE_GT;
    RETURN_TOKEN(OPCODE);
}

"gte" {
    yylval.opcode = EMBER_ASM_OPCODE_GTE;
    RETURN_TOKEN(OPCODE);
}

"lt" {
    yylval.opcode = EMBER_ASM_OPCODE_LT;
    RETURN_TOKEN(OPCODE);
}

"lte" {
    yylval.opcode = EMBER_ASM_OPCODE_LTE;
    RETURN_TOKEN(OPCODE);
}

"jmp" {
    yylval.opcode = EMBER_ASM_OPCODE_JMP;
    RETURN_TOKEN(OPCODE);
}

"jmp_if_false" {
    yylval.opcode = EMBER_ASM_OPCODE_JMP_IF_FALSE;
    RETURN_TOKEN(OPCODE);
}

"call" {
    yylval.opcode = EMBER_ASM_OPCODE_CALL;
    RETURN_TOKEN(OPCODE);
}

"ret" {
    yylval.opcode = EMBER_ASM_OPCODE_RET;
    RETURN_TOKEN(OPCODE);
}

"halt" {
    yylval.opcode = EMBER_ASM_OPCODE_HALT;
    RETURN_TOKEN(OPCODE);
}

"print" {
    yylval.opcode = EMBER_ASM_OPCODE_PRINT;
    RETURN_TOKEN(OPCODE);
}

"input" {
    yylval.opcode = EMBER_ASM_OPCODE_INPUT;
    RETURN_TOKEN(OPCODE);
}

"status_ok" {
    yylval.opcode = EMBER_ASM_OPCODE_STATUS_OK;
    RETURN_TOKEN(OPCODE);
}

"status_code" {
    yylval.opcode = EMBER_ASM_OPCODE_STATUS_CODE;
    RETURN_TOKEN(OPCODE);
}

"status_clear" {
    yylval.opcode = EMBER_ASM_OPCODE_STATUS_CLEAR;
    RETURN_TOKEN(OPCODE);
}

"i2f" {
    yylval.opcode = EMBER_ASM_OPCODE_I2F;
    RETURN_TOKEN(OPCODE);
}

"f2i" {
    yylval.opcode = EMBER_ASM_OPCODE_F2I;
    RETURN_TOKEN(OPCODE);
}

"i2b" {
    yylval.opcode = EMBER_ASM_OPCODE_I2B;
    RETURN_TOKEN(OPCODE);
}

"f2b" {
    yylval.opcode = EMBER_ASM_OPCODE_F2B;
    RETURN_TOKEN(OPCODE);
}

"s2b" {
    yylval.opcode = EMBER_ASM_OPCODE_S2B;
    RETURN_TOKEN(OPCODE);
}

"n2b" {
    yylval.opcode = EMBER_ASM_OPCODE_N2B;
    RETURN_TOKEN(OPCODE);
}

"i2s" {
    yylval.opcode = EMBER_ASM_OPCODE_I2S;
    RETURN_TOKEN(OPCODE);
}

"f2s" {
    yylval.opcode = EMBER_ASM_OPCODE_F2S;
    RETURN_TOKEN(OPCODE);
}

"b2s" {
    yylval.opcode = EMBER_ASM_OPCODE_B2S;
    RETURN_TOKEN(OPCODE);
}

"n2s" {
    yylval.opcode = EMBER_ASM_OPCODE_N2S;
    RETURN_TOKEN(OPCODE);
}

"parse_bool" {
    yylval.opcode = EMBER_ASM_OPCODE_PARSE_BOOL;
    RETURN_TOKEN(OPCODE);
}

"parse_float" {
    yylval.opcode = EMBER_ASM_OPCODE_PARSE_FLOAT;
    RETURN_TOKEN(OPCODE);
}

"parse_int" {
    yylval.opcode = EMBER_ASM_OPCODE_PARSE_INT;
    RETURN_TOKEN(OPCODE);
}

[ \t\r]+ {
    yycolumn += yyleng;
}

\n {
    yyline += yyleng;
    yycolumn = 1;
}

[.:()=,@] {
    SET_LOC();
    yycolumn += yyleng;
    return yytext[0];
}

-?[0-9]+ {
    yylval.int_val = atoi(yytext);
    RETURN_TOKEN(INT);
}

-?[0-9]+\.[0-9]+ {
    yylval.float_val = atof(yytext);
    RETURN_TOKEN(FLOAT);
}

"true" {
    yylval.boolean_val = true;
    RETURN_TOKEN(BOOLEAN);
}

"false" {
    yylval.boolean_val = false;
    RETURN_TOKEN(BOOLEAN);
}

"none" {
    RETURN_TOKEN(NONE);
}

\"([^\"\\\n]|\\.)*\" {
    SET_LOC();
    yycolumn += yyleng;

    size_t src_len = (size_t)yyleng;
    char *copy = malloc(src_len); /* enough, since unescaped string is never longer */
    if (copy == NULL) {
        return 0;
    }

    size_t j = 0;
    for (size_t i = 1; i < src_len - 1; i++) {
        if (yytext[i] == '\\') {
            i++;
            switch (yytext[i]) {
                case 'n':
                    copy[j++] = '\n';
                    break;
                case 't':
                    copy[j++] = '\t';
                    break;
                case '\\':
                    copy[j++] = '\\';
                    break;
                case '"':
                    copy[j++] = '"';
                    break;
                default:
                    copy[j++] = yytext[i];
                    break;
            }
        } else {
            copy[j++] = yytext[i];
        }
    }

    copy[j] = '\0';
    yylval.str_val = copy;
    return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str_val = ember_strdup(yytext);
    RETURN_TOKEN(IDENTIFIER);
}

";"[^\n]* {
    yycolumn += yyleng;
}

%%