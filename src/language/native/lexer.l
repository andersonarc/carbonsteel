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
 *  C-Native lexical analyzer rules for Flex
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
    #include "language/native/parser.h" /* parser */
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
     * Handles body skips for different file passes
     */
    #define start_skip_action if (context->expect_skip_from == true) {
    #define end_skip_action context->expect_skip_from = false; }
    #define consider_skip() start_skip_action BEGIN(SKIP); end_skip_action
    
    #define YYSTYPE CYYSTYPE
    #define YYLTYPE CYYLTYPE
    #define YY_DECL int cyylex(CYYSTYPE* yylval_param, CYYLTYPE* yylloc_param, void* yyscanner, se_context* context)
%}

%option prefix="cyy"
%option reentrant bison-bridge bison-locations
%option warn nodefault
%option outfile="native_lexer.c" header-file="native_lexer.h"
%s SKIP
%s SKIP_BODY
%s SKIP_PARAMETERS
%s SKIP_PARAMETERS_FULL
%s SKIP_STATEMENT

%%

    /* comments */
"/*"                    { consume_multi_line_comment(yyscanner); }
"//".*                  { /* consume a single-line comment */    }
"#".*                   { /* consume a single-line comment */    }


    /* skip start */
<SKIP>"{"               {
                            logd("skip: body");
                            ++context->skip_pair_count;
                            context->skip_until = '}';
                            BEGIN(0);
                            BEGIN(SKIP_BODY);
                        }

<SKIP>"("               {
                            logd("skip: parameters");
                            ++context->skip_pair_count;
                            context->skip_until = ')';
                            BEGIN(0);
                            BEGIN(SKIP_PARAMETERS);
                        }

<SKIP>"="               {
                            logd("skip: statement");
                            context->skip_until = ';';
                            BEGIN(0);
                            BEGIN(SKIP_STATEMENT);
                        }

    /* skip handler */
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_PARAMETERS_FULL>"\n"                            { yylloc_param->first_line++; yylloc_param->last_line++; }
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_PARAMETERS_FULL>{CPREF}?"'"([^'\\\n]|{CHR})+"'" { logd("skip: consuming a string"); }
<SKIP_BODY,SKIP_PARAMETERS,SKIP_STATEMENT,SKIP_PARAMETERS_FULL>{SPREF}?\"([^"\\\n]|{CHR})*\"   { logd("skip: consuming a character"); }

    /* skip body */
<SKIP_BODY>"{"          {
                           //  ! todo macros for these ones. 
                            logd("body skip: counting {");
                            ++context->skip_pair_count;
                        }

<SKIP_BODY>"}"          {
                            logd("body skip: counting }");
                            --context->skip_pair_count;
                            if (context->skip_pair_count == 0) {
                                BEGIN(0);
                                context->skip_until = 0;
                                return CTOKEN_SKIPPED_BODY;
                            }
                        }

<SKIP_BODY>[^{}\n]+ {
                        logd("discard %s", yytext);
                    }

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
                                context->skip_until = 0;
                                /* todo make this optional */
                                logd("parameter skip: forwarding to body skip");
                                // todo DO NOT do it in case there is no body!
                                context->expect_skip_from = true;
                                consider_skip();
                                return CTOKEN_SKIPPED_PARAMETERS;
                            }
                        }


    /* skip parameters full */
<SKIP_PARAMETERS_FULL>"("    {
                                logd("parameter skip: counting (");
                                ++context->skip_pair_count;
                            }

<SKIP_PARAMETERS_FULL>")"    {
                                logd("parameter skip: counting )");
                                --context->skip_pair_count;
                                if (context->skip_pair_count == 0) {
                                    BEGIN(0);
                                    context->skip_until = 0;
                                }
                            }

<SKIP_PARAMETERS_FULL>[^()\n]+   {
                                    logd("discard %s", yytext);
                                }

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
                                return CTOKEN_SKIPPED_STATEMENT;
                            } else {
                                logd("statement skip: met non-terminal ;")
                            }                            
                        }

<SKIP_STATEMENT>[^{};"'\n]+ {
                                logd("discard %s", yytext);
                            }


    /* keywords */
"auto"					{ return CTOKEN_AUTO; }
"break"					{ return CTOKEN_BREAK; }
"case"					{ return CTOKEN_CASE; }
"const"					{ return CTOKEN_CONST; }
"continue"				{ return CTOKEN_CONTINUE; }
"default"				{ return CTOKEN_DEFAULT; }
"do"					{ return CTOKEN_DO; }
"else"					{ return CTOKEN_ELSE; }
"enum"					{ consider_skip(); return CTOKEN_ENUM; }
"extern"				{ return CTOKEN_EXTERN; }
"for"					{ return CTOKEN_FOR; }
"goto"					{ return CTOKEN_GOTO; }
"if"					{ return CTOKEN_IF; }
"inline"				{ return CTOKEN_INLINE; }
"register"				{ return CTOKEN_REGISTER; }
"restrict"				{ return CTOKEN_RESTRICT; }
"return"				{ return CTOKEN_RETURN; }
"signed"				{ return CTOKEN_SIGNED; }
"sizeof"				{ return CTOKEN_SIZEOF; }
"static"				{ return CTOKEN_STATIC; }
"struct"				{ consider_skip(); return CTOKEN_STRUCT; }
"switch"				{ return CTOKEN_SWITCH; }
"typedef"				{ return CTOKEN_TYPEDEF; }
"union"					{ return CTOKEN_UNION; }
"unsigned"				{ return CTOKEN_UNSIGNED; }
"volatile"				{ return CTOKEN_VOLATILE; }
"while"					{ return CTOKEN_WHILE; }
"_Alignas"                              { return CTOKEN_ALIGNAS; }
"_Alignof"                              { return CTOKEN_ALIGNOF; }
"_Atomic"                               { return CTOKEN_ATOMIC; }
"_Bool"                                 { return CTOKEN_BOOL; }
"_Complex"                              { return CTOKEN_COMPLEX; }
"_Generic"                              { return CTOKEN_GENERIC; }
"_Imaginary"                            { return CTOKEN_IMAGINARY; }
"_Noreturn"                             { return CTOKEN_NORETURN; }
"_Static_assert"                        { return CTOKEN_STATIC_ASSERT; }
"_Thread_local"                         { return CTOKEN_THREAD_LOCAL; }
"__builtin_va_list"                     { logw("valist is not supported"); return ast_lex_token_native(context, yylval_param, "int"); /* todo temporary workaround */ }
"__func__"                              { return CTOKEN_FUNC_NAME; }
"__inline"                              {}
"__restrict"                            {}
"__extension__"                         {}
"__attribute__"                         { BEGIN(SKIP_PARAMETERS_FULL); }
"__asm__"                               { BEGIN(SKIP_PARAMETERS_FULL); }


    /* identifier */
{AZ}{AN}*		            { consider_skip(); return ast_lex_token_native(context, yylval_param, yytext); }


    /* integer constants */
{HPREF}{HEX}+		        { 
                                yylval_param->CTOKEN_LONG_CONSTANT = strtol(yytext, NULL, 16);   
                                return CTOKEN_LONG_CONSTANT; 
                            }

{DEC_NZ}{DEC}*	            { 
                                yylval_param->CTOKEN_LONG_CONSTANT = strtol(yytext, NULL, 10);   
                                return CTOKEN_LONG_CONSTANT; 
                            }

"0"{OCT}*			        { 
                                yylval_param->CTOKEN_LONG_CONSTANT = strtol(yytext, NULL, 8);
                                return CTOKEN_LONG_CONSTANT; 
                            }


    /* character constant */
{CPREF}?"'"([^'\\\n]|{CHR})+"'"	    { 
                                        yylval_param->CTOKEN_CHAR_CONSTANT = parse_character_constant(yytext);
                                        return CTOKEN_CHAR_CONSTANT; 
                                    }


    /* decimal float constants */
{DEC}+{EXP} 		                { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT; 
                                    }

{DEC}*"."{DEC}+{EXP}?		        { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT; 
                                    }

{DEC}+"."{DEC}*{EXP}?		        { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT; 
                                    }


    /* hexadecimal float constants */
{HPREF}{HEX}+{HEXP}			        { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT; 
                                    }

{HPREF}{HEX}*"."{HEX}+{HEXP}        { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT;
                                    }

{HPREF}{HEX}+"."{HEX}*{HEXP}	    { 
                                        yylval_param->CTOKEN_DOUBLE_CONSTANT = strtod(yytext, NULL);
                                        return CTOKEN_DOUBLE_CONSTANT;
                                    }


    /* string literal */
{SPREF}?\"([^"\\\n]|{CHR})*\"	    { 
                                        yylval_param->CTOKEN_STRING_LITERAL = copy_string(yytext);
                                        return CTOKEN_STRING_LITERAL;
                                    }


    /* assignment operators */
"*="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_MULTIPLY;      return CTOKEN_OP_ASSIGN; }
"/="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_DIVIDE;        return CTOKEN_OP_ASSIGN; }
"%="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_MODULO;        return CTOKEN_OP_ASSIGN; }

"+="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_ADD;           return CTOKEN_OP_ASSIGN; }
"-="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_SUBTRACT;      return CTOKEN_OP_ASSIGN; }

">>="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_SHIFT_RIGHT;   return CTOKEN_OP_ASSIGN; }
"<<="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_SHIFT_LEFT;    return CTOKEN_OP_ASSIGN; }

"&="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_BINARY_AND;    return CTOKEN_OP_ASSIGN; }
"^="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_BINARY_XOR;    return CTOKEN_OP_ASSIGN; }
"|="				{ yylval_param->CTOKEN_OP_ASSIGN = OP_A_BINARY_OR;     return CTOKEN_OP_ASSIGN; }

    /* unary operators */
"->"                { return CTOKEN_OP_POINTER_PROPERTY; }
"++"				{ return CTOKEN_OP_INCREMENT;        }
"--"				{ return CTOKEN_OP_DECREMENT;        }

    /* binary operators */
">>"				{ return CTOKEN_OP_SHIFT_RIGHT;      }
"<<"				{ return CTOKEN_OP_SHIFT_LEFT;       }
"&&"				{ return CTOKEN_OP_LOGIC_AND;        }
"||"				{ return CTOKEN_OP_LOGIC_OR;         }
"<="				{ return CTOKEN_OP_LESS_EQUAL;       }
">="				{ return CTOKEN_OP_GREATER_EQUAL;    }
"=="				{ return CTOKEN_OP_EQUAL;            }
"!="				{ return CTOKEN_OP_NOT_EQUAL;        }

    /* other tokens */
"..."               { return CTOKEN_C_VARARG;            }
";"					{ return ';'; }
"{"                 {
                        start_skip_action
                            logd("skip: body");
                            ++context->skip_pair_count;
                            context->skip_until = '}';
                            BEGIN(0);
                            BEGIN(SKIP_BODY);
                        end_skip_action
                        else return '{';
                    }
"}"			        { return '}'; }
"("                 {
                        start_skip_action
                            logd("skip: parameters");
                            ++context->skip_pair_count;
                            context->skip_until = ')';
                            BEGIN(0);
                            BEGIN(SKIP_PARAMETERS);
                        end_skip_action
                        else return '(';
                    }
"="                 {
                        start_skip_action
                            logd("skip: statement");
                            context->skip_until = ';';
                            BEGIN(0);
                            BEGIN(SKIP_STATEMENT);
                        end_skip_action
                        else return '=';
                    }
","					{ return ','; }
":"					{ return ':'; }
")"					{ return ')'; }
("["|"<:")			{ return '['; }
("]"|":>")			{ return ']'; }
"."					{ return '.'; }
"&"					{ return '&'; }
"!"					{ return '!'; }
"~"					{ return '~'; }
"-"					{ return '-'; }
"+"					{ return '+'; }
"*"					{ return '*'; }
"/"					{ return '/'; }
"%"					{ return '%'; }
"<"					{ return '<'; }
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
    return 1;
}