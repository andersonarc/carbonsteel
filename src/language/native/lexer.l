D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P                       ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{
#include <stdio.h>
#include "y.tab.h"

void count(void);
%}

%%
"/*"			{ comment(); }
"//"[^\n]*              { /* consume //-comment */ }


"auto"			{ count(); return(AUTO); }
"_Bool"			{ count(); return(BOOL); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"_Complex"		{ count(); return(COMPLEX); }
"const"			{ count(); return(CONST); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"extern"		{ count(); return(EXTERN); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"_Imaginary"		{ count(); return(IMAGINARY); }
"inline"		{ count(); return(INLINE); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"register"		{ count(); return(REGISTER); }
"restrict"		{ count(); return(RESTRICT); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"static"		{ count(); return(STATIC); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"volatile"		{ count(); return(VOLATILE); }
"while"			{ count(); return(WHILE); }

{L}({L}|{D})*		{ count(); return(check_type()); }

0[xX]{H}+{IS}?		{ count(); return(CONSTANT); }
0[0-7]*{IS}?		{ count(); return(CONSTANT); }
[1-9]{D}*{IS}?		{ count(); return(CONSTANT); }
L?'(\\.|[^\\'\n])+'	{ count(); return(CONSTANT); }



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

    /* float and int suffix */
FSUFF  (f|F|l|L)
ISUFF  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))

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
    
    #define YYSTYPE CYYSTYPE
    #define YYLTYPE CYYLTYPE
    #define YY_DECL int myylex(CYYSTYPE* yylval_param, CYYLTYPE* yylloc_param, void* yyscanner, se_context* context)
%}

%option prefix="cyy"
%option reentrant bison-bridge bison-locations
%option warn nodefault
%option outfile="c_lexer.c" header-file="c_lexer.h"

%%

    /* comments */
"/*"                    { consume_multi_line_comment(yyscanner); }
"//".*                  { /* consume a single-line comment */    }


    /* keywords */
"import"                { return TOKEN_IMPORT;   }
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
{HPREF}{HEX}+{ISUFF}?		{ 
                                yylval_param->TOKEN_INT_CONSTANT = strtol(yytext, NULL, 16);   
                                return TOKEN_INT_CONSTANT; 
                            }

{DEC_NZ}{DEC}*{ISUFF}?	    { 
                                yylval_param->TOKEN_INT_CONSTANT = strtol(yytext, NULL, 10);   
                                return TOKEN_INT_CONSTANT; 
                            }

"0"{OCT}*{ISUFF}?			{ 
                                yylval_param->TOKEN_INT_CONSTANT = strtol(yytext, NULL, 8);
                                return TOKEN_INT_CONSTANT; 
                            }


    /* character constant */
{CPREF}?"'"([^'\\\n]|{CHR})+"'"	    { 
                                        yylval_param->TOKEN_CHAR_CONSTANT = parse_character_constant(yytext);
                                        return TOKEN_CHAR_CONSTANT; 
                                    }

    /* decimal float constants */
{DEC}+{EXP}{FSUFF}?			        { 
                                        yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                        return TOKEN_FLOAT_CONSTANT; 
                                    }

{DEC}*"."{DEC}+{EXP}?{FSUFF}?		{ 
                                        yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                        return TOKEN_FLOAT_CONSTANT; 
                                    }

{DEC}+"."{DEC}*{EXP}?{FSUFF}?	    { 
                                        yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                        return TOKEN_FLOAT_CONSTANT; 
                                    }


    /* hexadecimal float constants */
{HPREF}{HEX}+{HEXP}{FSUFF}?			    { 
                                            yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                            return TOKEN_FLOAT_CONSTANT; 
                                        }

{HPREF}{HEX}*"."{HEX}+{HEXP}{FSUFF}?    { 
                                            yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                            return TOKEN_FLOAT_CONSTANT;
                                        }

{HPREF}{HEX}+"."{HEX}*{HEXP}{FSUFF}?	{ 
                                            yylval_param->TOKEN_FLOAT_CONSTANT = strtof(yytext, NULL);
                                            return TOKEN_FLOAT_CONSTANT;
                                        }


    /* string literal */
SPREF?\"(\\.|[^\\"\n])*\"	            { 
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
">>"				{ return TOKEN_OP_SHIFT_RIGHT;      }
"<<"				{ return TOKEN_OP_SHIFT_LEFT;       }
"&&"				{ return TOKEN_OP_LOGIC_AND;        }
"||"				{ return TOKEN_OP_LOGIC_OR;         }
"<="				{ return TOKEN_OP_LESS_EQUAL;       }
">="				{ return TOKEN_OP_GREATER_EQUAL;    }
"=="				{ return TOKEN_OP_EQUAL;            }
"!="				{ return TOKEN_OP_NOT_EQUAL;        }

    /* other tokens */
";"					{ return ';'; }
("{"|"<%")			{ return '{'; }
("}"|"%>")			{ return '}'; }
","					{ return ','; }
":"					{ return ':'; }
"="					{ return '='; }
"("					{ return '('; }
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