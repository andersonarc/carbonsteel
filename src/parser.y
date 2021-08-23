%{
/**
 * @file parser.y
 * @author andersonarc (e.andersonarc@gmail.com)
 * @author jutta@pobox.com
 * @version 0.2
 * @date 2021-07-22
 * 
 * @copyright This code was taken from http://www.quut.com/c/ANSI-C-grammar-y.html
 *				and heavily modified by me, but it stays under original license
 *              (unspecified, but "free for usage with or without attribution")
 *
 *  Bison language grammar parser
 */
#include "stack.h" /* stack */
#include "lexer.h" /* lexer */

/**
 * Throws a parser error
 *
 * @param message The error message
*/
void yyerror(const char* message);

%}

%define parse.error detailed

%token	IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token	INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token	AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token	SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token	XOR_ASSIGN OR_ASSIGN
%token  TRUE FALSE

%token	INLINE CONST SIGNED UNSIGNED NEW
%token	IMPORT TYPE STRUCTURE_NAME ALIAS_NAME ST_VARIABLE_NAME PRIMITIVE_NAME
%token  ENUM ENUM_NAME FUNCTION FUNCTION_NAME FUNCTION_PARAMETER_NAME ELLIPSIS ALIAS

%token	IF ELSE WHILE DO FOR GOTO CONTINUE BREAK RETURN AS

%start translation_unit
%%

basic_expression
	: FUNCTION_NAME { add_ex_basic(EX_B_FUNCTION); }
	| ST_VARIABLE_NAME { add_ex_basic(EX_B_VARIABLE); }
	| FUNCTION_PARAMETER_NAME { add_ex_basic(EX_B_FUNCTION_PARAMETER); }
	| boolean { add_ex_basic(EX_B_BOOLEAN); }
	| number { add_ex_basic(EX_B_NUMBER); }
	| string { add_ex_basic(EX_B_STRING); }
	| constructor_expression { add_ex_basic(EX_B_CONSTRUCTOR); }
	| '(' expression ')' { add_ex_basic(EX_B_EXPRESSION); }
	;

boolean: boolean_ { stack_push_boolean(yytext); }
boolean_
	: TRUE
	| FALSE
	;

number
	: INT_CONSTANT { stack_push_number(yytext, EX_N_INT); }
	| FLOAT_CONSTANT { stack_push_number(yytext, EX_N_FLOAT); }
	;

string
	: STRING_LITERAL { stack_push_string(yytext); }
	;

constructor_basic
	: type function_invocation { add_ex_constructor(false); }
	| type '[' expression ']' function_invocation { add_ex_constructor(true); }
	;

constructor_expression
	: constructor_basic
	| NEW constructor_basic { add_ex_constructor_new(); }
	;

postfix_expression_recursive
	: basic_expression { start_ex_postfix(); }
	| postfix_expression_recursive '[' expression ']' { add_ex_postfix_level(EX_PL_INDEX); }
	| postfix_expression_recursive function_invocation { add_ex_postfix_level(EX_PL_INVOCATION); }
	| postfix_expression_recursive '.' IDENTIFIER /* structure or enum property, checked */ { add_ex_postfix_level(EX_PL_PROPERTY); }
	;

postfix_expression
	: postfix_expression_recursive { end_ex_postfix(EX_P_PLAIN); }
	| postfix_expression_recursive INC_OP { end_ex_postfix(EX_P_INCREMENT); }
	| postfix_expression_recursive DEC_OP { end_ex_postfix(EX_P_DECREMENT); }
	;

function_invocation
	: '(' argument_expression_list ')'
	| '(' ')'

argument_expression_list
	: expression
	| argument_expression_list ',' expression
	;

unary_expression_recursive
	: postfix_expression { start_ex_unary(); }
	| unary_operator_recursive unary_expression_recursive
	;

unary_operator_recursive
	: '&' { add_op_unary(OP_UN_REFERENCE); }
	| '*' { add_op_unary(OP_UN_DEREFERENCE); }
	| '~' { add_op_unary(OP_UN_BITWISE_NOT); }
	| '!' { add_op_unary(OP_UN_LOGIC_NOT); }
	;

unary_expression
	: unary_expression_recursive { end_ex_unary(EX_U_PLAIN); }
	| '+' unary_expression_recursive { end_ex_unary(EX_U_PLUS); }
	| '-' unary_expression_recursive { end_ex_unary(EX_U_MINUS); }
	| INC_OP unary_expression_recursive { end_ex_unary(EX_U_INCREMENT); }
	| DEC_OP unary_expression_recursive { end_ex_unary(EX_U_DECREMENT); }
	;

cast_expression
	: unary_expression { start_ex_cast(); }
	| cast_expression AS '(' type ')' { add_ex_cast_level(); }
	;

multiplicative_expression
	: cast_expression { end_ex_cast(); biex_start(multiply)(); }
	| multiplicative_expression '*' cast_expression { biex_add_op(multiply)(OP_M_MULTIPLY); }
	| multiplicative_expression '/' cast_expression { biex_add_op(multiply)(OP_M_DIVIDE); }
	| multiplicative_expression '%' cast_expression { biex_add_op(multiply)(OP_M_MODULO); }
	;

additive_expression
	: multiplicative_expression { biex_end(multiply)(); biex_start(addition)(); }
	| additive_expression '+' multiplicative_expression { biex_add_op(addition)(OP_A_PLUS); }
	| additive_expression '-' multiplicative_expression { biex_add_op(addition)(OP_A_MINUS); }
	;

shift_expression
	: additive_expression { biex_end(addition)(); biex_start(shift)(); }
	| shift_expression LEFT_OP additive_expression { biex_add_op(shift)(OP_S_LEFT); }
	| shift_expression RIGHT_OP additive_expression { biex_add_op(shift)(OP_S_RIGHT); }
	;

compare_expression
	: shift_expression { biex_end(shift)(); biex_start(compare)(); }
	| compare_expression '<' shift_expression { biex_add_op(compare)(OP_C_LT); }
	| compare_expression '>' shift_expression { biex_add_op(compare)(OP_C_GT); }
	| compare_expression LE_OP shift_expression { biex_add_op(compare)(OP_C_LE); }
	| compare_expression GE_OP shift_expression { biex_add_op(compare)(OP_C_GE); }
	;

equality_expression
	: compare_expression { biex_end(compare)(); biex_start(equal)(); }
	| equality_expression EQ_OP compare_expression { biex_add_op(equal)(OP_E_EQ); }
	| equality_expression NE_OP compare_expression { biex_add_op(equal)(OP_E_NEQ); }
	;

and_expression
	: equality_expression { biex_end(equal)(); biex_start(bit_and)(); }
	| and_expression '&' equality_expression { biex_add_op(bit_and)(OP_BIT_AND); }
	;

exclusive_or_expression
	: and_expression { biex_end(bit_and)(); biex_start(bit_xor)(); }
	| exclusive_or_expression '^' and_expression { biex_add_op(bit_xor)(OP_BIT_XOR); }
	;

inclusive_or_expression
	: exclusive_or_expression { biex_end(bit_xor)(); biex_start(bit_or)(); }
	| inclusive_or_expression '|' exclusive_or_expression { biex_add_op(bit_or)(OP_BIT_OR); }
	;

logical_and_expression
	: inclusive_or_expression { biex_end(bit_or)(); biex_start(and)(); }
	| logical_and_expression AND_OP inclusive_or_expression { biex_add_op(and)(OP_AND); }
	;

logical_or_expression
	: logical_and_expression { biex_end(and)(); biex_start(or)(); }
	| logical_or_expression OR_OP logical_and_expression { biex_add_op(or)(OP_OR); }
	;

conditional_expression
	: logical_or_expression { add_ex_condition(false); }
	| logical_or_expression '?' expression ':' conditional_expression { add_ex_condition(true); }
	;

expression
	: conditional_expression { add_expression(false); }
	| unary_expression assignment_operator expression { add_expression(true); }
	;

expression_block
	: expression { add_expression_block(); }
	;

assignment_operator
	: '=' { add_op_assign(OP_AS_PLAIN); }
	| MUL_ASSIGN { add_op_assign(OP_AS_MUL); }
	| DIV_ASSIGN { add_op_assign(OP_AS_DIV); }
	| MOD_ASSIGN { add_op_assign(OP_AS_MOD); }
	| ADD_ASSIGN { add_op_assign(OP_AS_ADD); }
	| SUB_ASSIGN { add_op_assign(OP_AS_SUB); }
	| LEFT_ASSIGN { add_op_assign(OP_AS_SHL); }
	| RIGHT_ASSIGN { add_op_assign(OP_AS_SHR); }
	| AND_ASSIGN { add_op_assign(OP_AS_BIT_AND); }
	| XOR_ASSIGN { add_op_assign(OP_AS_BIT_XOR); }
	| OR_ASSIGN { add_op_assign(OP_AS_BIT_OR); }
	;

property_declaration
    : type IDENTIFIER
	;

property_statement
	: property_declaration '=' expression_block ';' { add_st_variable(); }
	;

import_declaration
	: IMPORT { start_import(); } import_level_list ';' { end_import(); }
	;

import_level_list
	: IDENTIFIER
	| import_level_list '.' IDENTIFIER
	;

type
    : type_plain
    | type '*' { add_type_level(AST_TL_POINTER); }
	| type '[' ']' { add_type_level(AST_TL_ARRAY); }
	;

type_plain
	: PRIMITIVE_NAME
	| STRUCTURE_NAME
	| ENUM_NAME
	| ALIAS_NAME
	;

type_declaration
	: TYPE IDENTIFIER
	;

alias_declaration
	: ALIAS IDENTIFIER '=' type ';' { add_alias(); }
	;

structure_declaration
	: type_declaration { start_structure(); } '{' structure_member_list '}' ';' { end_structure(); }
	;

structure_member_list
	: structure_member
	| structure_member_list structure_member
	;

structure_member
	: property_declaration ';'
	;

enum_declaration
	: ENUM IDENTIFIER { start_enum(); } '{' enum_member_list '}' ';' { end_enum(); }
	;

enum_member_list
	: enum_member
	| enum_member_list ',' enum_member
	;

enum_member
	: IDENTIFIER '=' number
	| IDENTIFIER
	;

statement
	: compound_statement { add_statement(ST_COMPOUND); };
	| expression_statement { add_statement(ST_EXPRESSION); };
	| condition_statement { add_statement(ST_IF); };
	| iteration_statement { add_statement(ST_WHILE); };
	| jump_statement { add_statement(ST_JUMP); };
	;

compound_statement: { start_st_compound(); } compound_statement_ { end_st_compound(); };
compound_statement_
	: '{' '}'
	| '{' block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: property_statement
	| statement
	;

expression_statement: expression_statement_ { add_st_expression(); };
expression_statement_
	: ';'
	| expression_block ';'
	;

condition_if_statement
	: IF '(' expression_block ')' statement { add_st_if(); }
	;

condition_statement
	: condition_if_statement ELSE statement { add_st_if_else(); }
	| condition_if_statement
	;

iteration_statement
	: WHILE '(' expression_block ')' statement { add_st_while(); }
	;

jump_statement
	: CONTINUE ';' { add_st_jump(ST_J_CONTINUE); }
	| BREAK ';' { add_st_jump(ST_J_BREAK); }
	| RETURN ';' { add_st_jump(ST_J_RETURN_VOID); }
	| RETURN expression_block ';' { add_st_jump(ST_J_RETURN); }
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function
	| structure_declaration
    | enum_declaration
    | property_statement
	| alias_declaration
	| import_declaration
	;

function_parameter_list
	: property_declaration
	| function_parameter_list ',' property_declaration
	;

function_parameters
	: '(' function_parameter_list ')'
	| '(' ')'
	;

function
	: property_declaration { start_function(); } function_parameters { add_function_parameters(); } compound_statement { end_function(); }
	;

%%

/**
 * Throws a parser error
 *
 * @param message The error message
*/
void yyerror(const char* message) {
	error_syntax("%s", message);
}