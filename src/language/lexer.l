/**
 * @file lexer.l
 * @author andersonarc (e.andersonarc@gmail.com)
 * @author jutta@pobox.com
 * @version 0.3
 * @date 2021-07-22
 * 
 * @copyright This code was taken from http://www.quut.com/c/ANSI-C-grammar-l.html
 *				and heavily modified by me, but it stays under original license
 *              (unspecified, but "free for usage with or without attribution")
 *
 *  Main lexical analyzer rules for Flex
 */

/* FS  (f|F|l|L) */
/* IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?)) */
/* CP  (u|U|L) */
/* SP  (u8|u|U|L) */

    /* numbers and characters */
OCT    [0-7]
DEC    [0-9]
DEC_NZ [1-9]
CHR    (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
HEX    [a-fA-F0-9]

    /* identifier characters */
AZ     [a-zA-Z_]
AN     [a-zA-Z_0-9]

    /* decimal and hexadecimal exponent */
EXP    ([Ee][+-]?{DEC}+)
HEXP   ([Pp][+-]?{DEC}+)

    /* character, string and hex number prefix */
CPREF  (u|U|L)
SPREF  (u8|u|U|L)
HPREF  (0[xX])

    /* whitespaces */
WS     [ \t\v\f]

%{
        /* includes */
    #include "language/parser.h" /* parser */
    #include "ast/root.h"    /* syntax tree */
    #include "ast/lookup.h"  /* ast lookup */
    #include "misc/memory.h" /* memory copying */

        /* functions */
    /**
    * Consumes a multi-line comment
    *
    * @param[in] scanner The lexical scanner
    */
    void consume_multi_line_comment(yyscan_t scanner);

    /**
    * Resolves a character constant from a string
    *
    * @param[in] value The string
    *
    * @todo Wide characters are not supported
    *
    * @return The character constant
    */
    char parse_character_constant(char* value);

    /**
     * Handles the skip action for current token
     */
    #define lexer_skip_action(name)                         \
        BEGIN(0);                                           \
        switch (context_should_skip(context, yytext[0])) {  \
            case SCTX_SA_NONE:                              \
            case SCTX_SA_EXIT:                              \
                return yytext[0];                           \
                                                            \
            case SCTX_SA_START:                             \
                logd("skip: " #name);                       \
                BEGIN(SKIP_##name);                         \
                break;                                      \
                                                            \
            otherwise_error                                 \
        }

    #define YYSTYPE MYYSTYPE
    #define YYLTYPE MYYLTYPE
    #define YY_DECL int myylex(MYYSTYPE* yylval_param, MYYLTYPE* yylloc_param, void* yyscanner, se_context* context)
%}

%option prefix="myy"
%option reentrant bison-bridge bison-locations
%option warn nodefault
%option outfile="main_lexer.c" header-file="main_lexer.h"
%s SKIP
%s SKIP_BODY
%s SKIP_PARAMETERS
%s SKIP_STATEMENT
%s SKIP_GENERICS

%%

    /* comments */
"/*"                    { consume_multi_line_comment(yyscanner); }
"//".*                  { /* consume a single-line comment */    }

    /* skip: consume strings and newlines */
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_GENERICS>"\n"                            { yylloc_param->first_line++; yylloc_param->last_line++; }
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_GENERICS>{CPREF}?"'"([^'\\\n]|{CHR})+"'" { logd("skip: consuming a string"); }
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_GENERICS>{SPREF}?\"([^"\\\n]|{CHR})*\"   { logd("skip: consuming a character"); }

    /* skip body */
<SKIP_BODY>"{"          {
                            logd("body skip: counting {");
                            ++context->skip_pair_count;
                        }

<SKIP_BODY>"}"          {
                            logd("body skip: counting }");
                            --context->skip_pair_count;
                            if (context->skip_pair_count == 0) {
                                BEGIN(0);
                                context_finish_skip(context);
                                return TOKEN_SKIPPED_BODY;
                            }
                        }

<SKIP_BODY>[^{}\n]+     { logd("discard %s", yytext); }

    /* skip parameters */
<SKIP_PARAMETERS>"("    {
                            logd("parameter skip: counting (");
                            ++context->skip_pair_count;
                        }

<SKIP_PARAMETERS>")"    {
                            logd("parameter skip: counting )");
                            --context->skip_pair_count;
                            if (context->skip_pair_count == 0) {
                                BEGIN(0);
                                context_finish_skip(context);

                                logd("parameter skip: forwarding to body skip");
                                context_skip_specific_unless(context, context->pass, '{', ';');
                                return TOKEN_SKIPPED_PARAMETERS;
                            }
                        }

<SKIP_PARAMETERS>[^()\n]+   { logd("discard %s", yytext); }

    /* skip statement */
<SKIP_STATEMENT>"{"     {
                            logd("statement skip: counting {");
                            ++context->skip_pair_count;
                        }

<SKIP_STATEMENT>"}"     {
                            logd("statement skip: counting }");
                            --context->skip_pair_count;
                        }

<SKIP_STATEMENT>";"     {
                            if (context->skip_pair_count == 0) {
                                logd("statement skip: met terminal ;")
                                BEGIN(0);
                                context_finish_skip(context);
                                return TOKEN_SKIPPED_STATEMENT;
                            } else {
                                logd("statement skip: met non-terminal ;")
                            }                            
                        }

<SKIP_STATEMENT>[^{};"'\n]+ {
                                logd("discard %s", yytext);
                            }

    /* skip generics */
<SKIP_GENERICS>"<"      {
                            logd("generic skip: counting <");
                            ++context->skip_pair_count;
                        }

<SKIP_GENERICS>">"          {
                                //todo make this a function/macro
                                logd("generic skip: counting >");
                                --context->skip_pair_count;
                                if (context->skip_pair_count == 0) {
                                    BEGIN(0);
                                    context_finish_skip(context);

                                    logd("generic skip: forwarding to parameter skip");
                                    context_skip_specific_unless(context, context->pass, '(', SCTX_SKIP_ANY);
                                }
                            }

<SKIP_GENERICS>[^<>\n]+     {
                                logd("discard %s", yytext);
                            }


    /* keywords */
"import"                { return TOKEN_IMPORT;   }
"native"                { return TOKEN_NATIVE;   }
"extern"                { return TOKEN_EXTERN;   }
"alias"                 { return TOKEN_ALIAS;    }
"enum"					{ return TOKEN_ENUM;     }
"type"					{ return TOKEN_TYPE;     }
"new"                   { return TOKEN_NEW;      }
"as"                    { return TOKEN_AS;       }

"if"					{ return TOKEN_IF;       }
"else"					{ return TOKEN_ELSE;     }

"do"					{ return TOKEN_DO;       }
"while"					{ return TOKEN_WHILE;    }

"continue"				{ return TOKEN_CONTINUE; }
"return"				{ return TOKEN_RETURN;   }
"break"					{ return TOKEN_BREAK;    }

"unsigned"              { return TOKEN_UNSIGNED; }

    /* boolean constants */
"false"                     { 
                                yylval_param->TOKEN_FALSE = false;
                                return TOKEN_FALSE; 
                            }

"true"				        { 
                                yylval_param->TOKEN_TRUE  = true;
                                return TOKEN_TRUE;  
                            }

    /* identifier */
{AZ}{AN}*		            { return ast_lex_token(context, yylval_param, yytext); }


    /* integer constants */
{HPREF}{HEX}+		        { 
                                yylval_param->TOKEN_LONG_CONSTANT = strtol(yytext, NULL, 16);   
                                return TOKEN_LONG_CONSTANT; 
                            }

{DEC_NZ}{DEC}*	            { 
                                yylval_param->TOKEN_LONG_CONSTANT = strtol(yytext, NULL, 10);   
                                return TOKEN_LONG_CONSTANT; 
                            }

"0"{OCT}*			        { 
                                yylval_param->TOKEN_LONG_CONSTANT = strtol(yytext, NULL, 8);
                                return TOKEN_LONG_CONSTANT; 
                            }


    /* character constant */
{CPREF}?"'"([^'\\\n]|{CHR})+"'"	    { 
                                        yylval_param->TOKEN_CHAR_CONSTANT = parse_character_constant(yytext);
                                        return TOKEN_CHAR_CONSTANT; 
                                    }


    /* decimal float constants */
{DEC}+{EXP} 		                { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT; 
                                    }

{DEC}*"."{DEC}+{EXP}?		        { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT; 
                                    }

{DEC}+"."{DEC}*{EXP}?		        { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT; 
                                    }


    /* hexadecimal float constants */
{HPREF}{HEX}+{HEXP}			        { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT; 
                                    }

{HPREF}{HEX}*"."{HEX}+{HEXP}        { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT;
                                    }

{HPREF}{HEX}+"."{HEX}*{HEXP}	    { 
                                        yylval_param->TOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return TOKEN_DOUBLE_CONSTANT;
                                    }


    /* string literal */
{SPREF}?\"([^"\\\n]|{CHR})*\"	    { 
                                        yylval_param->TOKEN_STRING_LITERAL = copy_string(yytext);
                                        return TOKEN_STRING_LITERAL;
                                    }


    /* assignment operators */
"*="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_MULTIPLY;      return TOKEN_OP_ASSIGN; }
"/="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_DIVIDE;        return TOKEN_OP_ASSIGN; }
"%="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_MODULO;        return TOKEN_OP_ASSIGN; }

"+="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_ADD;           return TOKEN_OP_ASSIGN; }
"-="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_SUBTRACT;      return TOKEN_OP_ASSIGN; }

">>="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_SHIFT_RIGHT;   return TOKEN_OP_ASSIGN; }
"<<="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_SHIFT_LEFT;    return TOKEN_OP_ASSIGN; }

"&="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_BINARY_AND;    return TOKEN_OP_ASSIGN; }
"^="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_BINARY_XOR;    return TOKEN_OP_ASSIGN; }
"|="				{ yylval_param->TOKEN_OP_ASSIGN = OP_A_BINARY_OR;     return TOKEN_OP_ASSIGN; }

    /* unary operators */
"->"                { return TOKEN_OP_POINTER_PROPERTY; }
"++"				{ return TOKEN_OP_INCREMENT;        }
"--"				{ return TOKEN_OP_DECREMENT;        }

    /* binary operators */
    /* ">>"				{ return TOKEN_OP_SHIFT_RIGHT;      } */
    /* "<<"				{ return TOKEN_OP_SHIFT_LEFT;       } */
"&&"				{ return TOKEN_OP_LOGIC_AND;        }
"||"				{ return TOKEN_OP_LOGIC_OR;         }
"<="				{ return TOKEN_OP_LESS_EQUAL;       }
">="				{ return TOKEN_OP_GREATER_EQUAL;    }
"=="				{ return TOKEN_OP_EQUAL;            }
"!="				{ return TOKEN_OP_NOT_EQUAL;        }

    /* other tokens */
"..."               { return TOKEN_C_VARARG;            }
";"					{ return ';'; }
"{"                 { lexer_skip_action(BODY); }
"}"			        { return '}'; }
"("                 { lexer_skip_action(PARAMETERS); }
"="                 { lexer_skip_action(STATEMENT); }
","					{ return ','; }
":"					{ return ':'; }
")"					{ return ')'; }
"["			        { return '['; }
"]"     			{ return ']'; }
"."					{ return '.'; }
"&"					{ return '&'; }
"!"					{ return '!'; }
"~"					{ return '~'; }
"-"					{ return '-'; }
"+"					{ return '+'; }
"*"					{ return '*'; }
"/"					{ return '/'; }
"%"					{ return '%'; }
"<"                 { lexer_skip_action(GENERICS); }
">"					{ return '>'; }
"^"					{ return '^'; }
"|"					{ return '|'; }
"?"					{ return '?'; }

    /* misc tokens */
{WS}+			    { /* whitespaces separate tokens */ }
"\n"                { yylloc_param->first_line++; yylloc_param->last_line++; }
.					{ /* discard bad characters */      }

%%

/**
 * Called at the end of input
 *
 * @return Always 1
 */
int yywrap(yyscan_t yyscanner) {
    //todo use it in imports?
    return 1;
}

/**
 * Consumes a multi-line comment
 *
 * @param[in] scanner The lexical scanner
 */
void consume_multi_line_comment(yyscan_t scanner) {
    int c;

    while ((c = input(scanner)) != 0) {
        if (c == '*') {
            /* skip other asterisk tokens */
            while ((c = input(scanner)) == '*') {}

            /* comment end */
            if (c == '/') {
                return;
            }

            /* end of file */
            if (c == 0) {
                break;
            }
        }
    }

    error_syntax("unterminated comment");
}

/**
 * Resolves a character constant from a string
 *
 * @param[in] value The string
 *
 * @return The character constant
 */
char parse_character_constant(char* value) {
    int index = 1;

    /* skip prefixes */
    if (value[0] != '\'') index++;

    /* skip escape characters */
    while (value[index] == '\\') index++;

    /* special case for '\\' or '\'' */
    if (value[index] == '\'' && value[index + 1] != '\'') index--;

    /* safety check */
    if (index >= strlen(value)) {
        error_internal("parsing a character constant failed: value \"%s\" got index %d", value, index);
    }

    return value[index];
}