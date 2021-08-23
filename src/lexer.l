O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%{
/**
 * @file lexer.l
 * @author andersonarc (e.andersonarc@gmail.com)
 * @author jutta@pobox.com
 * @version 0.2
 * @date 2021-07-22
 * 
 * @copyright This code was taken from http://www.quut.com/c/ANSI-C-grammar-l.html
 *				and heavily modified by me, but it stays under original license
 *              (unspecified, but "free for usage with or without attribution")
 *
 *  Flex language grammar lexer
 */

#include <string.h> /* string functions */

#include "ast.h" /* syntax tree */
#include "parser.h" /* parser */

void consume_multi_line_comment();

%}

%%
"/*"                    { consume_multi_line_comment(); }
"//".*                  { /* consume comment */ }

"alias"                 { return(ALIAS); }
"as"                    { return(AS); }
"break"					{ return(BREAK); }
"const"					{ return(CONST); }
"continue"				{ return(CONTINUE); }
"do"					{ return(DO); }
"else"					{ return(ELSE); }
"enum"					{ return(ENUM); }
"false"                 { return(FALSE); }
"for"					{ return(FOR); }
"if"					{ return(IF); }
"import"                { return(IMPORT); }
"inline"				{ return(INLINE); }
"new"                   { return(NEW); }
"return"				{ return(RETURN); }
"signed"				{ return(SIGNED); }
"true"                  { return(TRUE); }
"type"				    { return(TYPE); }
"unsigned"				{ return(UNSIGNED); }
"while"					{ return(WHILE); }

{L}{A}*					{ return ast_lex_token(yytext); }

{HP}{H}+{IS}?				{ return INT_CONSTANT; }
{NZ}{D}*{IS}?				{ return INT_CONSTANT; }
"0"{O}*{IS}?				{ return INT_CONSTANT; }
{CP}?"'"([^'\\\n]|{ES})+"'"	{ return INT_CONSTANT; }

{D}+{E}{FS}?				{ return FLOAT_CONSTANT; }
{D}*"."{D}+{E}?{FS}?		{ return FLOAT_CONSTANT; }
{D}+"."{E}?{FS}?			{ return FLOAT_CONSTANT; }
{HP}{H}+{P}{FS}?			{ return FLOAT_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?		{ return FLOAT_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ return FLOAT_CONSTANT; }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ return STRING_LITERAL; }

"..."				{ return ELLIPSIS; }
">>="				{ return RIGHT_ASSIGN; }
"<<="				{ return LEFT_ASSIGN; }
"+="				{ return ADD_ASSIGN; }
"-="				{ return SUB_ASSIGN; }
"*="				{ return MUL_ASSIGN; }
"/="				{ return DIV_ASSIGN; }
"%="				{ return MOD_ASSIGN; }
"&="				{ return AND_ASSIGN; }
"^="				{ return XOR_ASSIGN; }
"|="				{ return OR_ASSIGN; }
">>"				{ return RIGHT_OP; }
"<<"				{ return LEFT_OP; }
"++"				{ return INC_OP; }
"--"				{ return DEC_OP; }
"&&"				{ return AND_OP; }
"||"				{ return OR_OP; }
"<="				{ return LE_OP; }
">="				{ return GE_OP; }
"=="				{ return EQ_OP; }
"!="				{ return NE_OP; }
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

{WS}+			    { /* whitespace separates tokens */ }
.					{ /* discard bad characters */ }

%%

/**
 * Called at the end of input
 *
 * @return Always 1
 */
int yywrap() {
    return 1;
}

void consume_multi_line_comment() {
    int c;
    while ((c = input()) != 0) {
        if (c == '*') {
            while ((c = input()) == '*') {}

            if (c == '/') {
                return;
            }

            if (c == 0) {
                break;
            }
        }
    }
    error_syntax("unterminated comment");
}