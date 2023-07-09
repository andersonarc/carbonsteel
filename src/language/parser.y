/**
 * @file parser.y
 * @author andersonarc (e.andersonarc@gmail.com)
 * @author jutta@pobox.com
 * @version 0.3
 * @date 2021-07-22
 * 
 * @copyright This code was taken from http://www.quut.com/c/ANSI-C-grammar-y.html
 *				and heavily modified by me, but it stays under original license
 *              (unspecified, but "free for usage with or without attribution")
 *
 *  Main parser grammar rules for Bison
 */


%code requires {
	/* header code */

			/* includes */
	#include "syntax/expression/complex.h" /* expressions */
	#include "syntax/statement/statement.h" /* statements */
	#include "syntax/declaration/declaration.h" /* declarations */

	typedef struct type_and_name {
		bool is_full;
		ast_type type;
		char* name;
	} type_and_name;
}

%code provides {	
	/* header code */

		/* functions */
	/**
	* Throws a parser error
	*
	* @param[in] location Location of the error
	* @param[in] scanner  The token scanner
	* @param[in] context  Current compiler context
	* @param[in] message  The error message
	*/
	void myyerror(MYYLTYPE* location, void* scanner, se_context* context, const char* message);
}

%{	
	/* source code */

		/* includes */
	#include <ctype.h> /* number checking */

	#include "language/context.h" /* parser context */
	#include "language/parser.h"  /* parser header */
	#include "misc/union.h" 	  /* union initialization */
	#include "ast/type/check.h"	  /* type checking */
	#include "ast/type/resolve.h" /* default type values */

	int myylex(MYYSTYPE* yylval_param, MYYLTYPE* yylloc_param, void* yyscanner, se_context* context);
%}


	/* bison options */
%param {void* yyscanner}  /* pass the yyscanner parameter */
%param {se_context* context} /* track the context as a parameter */

%define api.token.prefix {TOKEN_} /* add a token prefix to avoid collisions */
%define api.prefix {myy}	      /* add an api prefix to support multiple parsers */
%define api.value.type union 	  /* use a union type */

%define parse.error custom  /* use custom error handling */
%define api.pure 	full 	/* use pure parser interface */
%define parse.lac 	full	/* accurate list of expected tokens */
%locations 				   	/* track token locations */
//%define parse.trace




		/* tokens */

	/* identifier */
%token <char*>  IDENTIFIER 			"identifier"

	/* constants */
%token <long long>   LONG_CONSTANT  "long constant"
%token <char>   CHAR_CONSTANT   	"char constant"
%token <double> DOUBLE_CONSTANT 	"double constant"
%token <char*>  STRING_LITERAL 		"string literal"
%token <bool>   TRUE				"true"
%token <bool>   FALSE				"false"

	/* unary operators */
%token 	OP_POINTER_PROPERTY "->"
		OP_INCREMENT   		"++"
		OP_DECREMENT     	"--"

	/* binary operators */
%token	OP_SHIFT_LEFT  		"<<" 				
		OP_SHIFT_RIGHT   	">>"

		OP_LESS_EQUAL  		"<=" 				
		OP_GREATER_EQUAL 	">="

		OP_EQUAL       		"==" 				
		OP_NOT_EQUAL     	"!="

		OP_LOGIC_AND 		"&&"				
		OP_LOGIC_OR    		"||"


	/* assignment operator */
%token 	<op_assign>	 OP_ASSIGN 	"assignment operator"	


	/* keywords */
%token	IMPORT 		"import"
		NATIVE		"native"
		EXTERN		"extern"
		ALIAS		"alias"
		ENUM		"enum"
		TYPE		"type"
		NEW			"new"
		AS			"as"

		IF			"if"
		ELSE		"else"

		DO			"do"
		WHILE   	"while"

		CONTINUE 	"continue"
		RETURN		"return"
		BREAK		"break"

		UNSIGNED    "unsigned"

	/* type references */
%token <dc_structure*> 			STRUCTURE_NAME 		"structure name"
%token <dc_alias*> 				ALIAS_NAME 			"alias name"
%token <ast_type_primitive*>	PRIMITIVE_NAME		"primitive type name"
%token <dc_enum*>				ENUM_NAME			"enum name"

	/* value references */
%token <dc_st_variable*>		VARIABLE_NAME  			"variable name"
%token <dc_function*> 			FUNCTION_NAME 			"function name"
%token <dc_function_parameter*> FUNCTION_PARAMETER_NAME	"function parameter name"

	/* special tokens */
%token <void*> ANY_NAME /* should only be used by the ast_lex_token function */
%token SKIPPED_PARAMETERS
%token SKIPPED_BODY
%token SKIPPED_STATEMENT

	/* other tokens */
%token C_VARARG "..."




		/* rule types */
%nterm  <type_and_name> type_and_name

	/* function */
%nterm  <char*> 		function_or_variable_name
%nterm 	<dc_function*>  function
%nterm 	<dc_function*>  function_prefix
%nterm 	<dc_function*>  function_declaration
%nterm 	<dc_function*>  function_definition
%nterm 	<dc_function_parameters> 			function_parameters
%nterm 	<dc_function_parameters> 			extern_function_parameters
%nterm 	<arraylist(dc_function_parameter)>  function_parameter_list
%nterm 	<dc_function_parameter>			  	function_parameter

	/* structure */
%nterm  <char*> 		 structure_name
%nterm 	<dc_structure*>  structure_declaration
%nterm 	<list(dc_structure_member)>		  structure_body
%nterm 	<arraylist(dc_structure_member)>  structure_member_list
%nterm	<dc_structure_member>			  structure_member

	/* enum */
%nterm  <char*>  	enum_name
%nterm 	<dc_enum*>  enum_declaration
%nterm 	<list(dc_enum_member)>		 enum_body
%nterm 	<arraylist(dc_enum_member)>  enum_member_list
%nterm 	<dc_enum_member>			 enum_member

	/* variable */
%nterm 	<dc_st_variable*>  global_variable_declaration_statement
%nterm 	<dc_st_variable*>  variable_declaration_statement

	/* alias */
%nterm	<char*>		 alias_name
%nterm 	<dc_alias*>  alias_declaration

	/* import */
%nterm 	<dc_import*>  import_declaration
%nterm 	<dc_import*>  import_declaration_
%nterm 	<arraylist(dc_import_node)>  import_node_list
%nterm 	<dc_import_node>		     import_node

	/* primitive expressions */
%nterm  <bool>  boolean_expression
%nterm  <ex_number>	 number_expression

	/* constructor expressions */
%nterm 	<ex_constructor*>  constructor_expression
%nterm 	<ex_constructor*>  constructor_expression_
%nterm	<ex_constructor*>  constructor_expression_basic

	/* enum member expression */
%nterm 	<ex_enum_member*>  enum_member_expression

	/* basic expression */
%nterm 	<ex_basic>  basic_expression

	/* postfix expressions */
%nterm 	<ex_postfix>  postfix_expression
%nterm 	<ex_postfix>  postfix_expression_recursive

	/* invocation expressions */
%nterm 	<list(expression_ptr)>  invocation_expression
%nterm 	<arraylist(expression_ptr)>  invocation_argument_list

	/* unary expressions */
%nterm 	<ex_unary>	unary_expression
%nterm  <ex_unary>  unary_expression_recursive

	/* cast expression */
%nterm  <ex_cast>  cast_expression

	/* binary expression */
%nterm  <ex_binary>  binary_expression
	/* binary operator priorities */
%left OP_LOGIC_OR
%left OP_LOGIC_AND
%left '|'
%left '^'
%left '&'
%left OP_EQUAL OP_NOT_EQUAL
%left OP_LESS_EQUAL OP_GREATER_EQUAL '<' '>' 
%left OP_SHIFT_LEFT OP_SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'

	/* conditional expression */
%nterm  <ex_condition>  conditional_expression

	/* expression */
%nterm 	<expression*>  expression
%nterm 	<op_assign>    assignment_operator

	/* expression block */
%nterm  <ex_block>  expression_block

	/* type */
%nterm  <ast_type>  type
%nterm  <ast_type>  type_recursive
%nterm  <ast_type>  type_terminal

	/* statement */
%nterm 	<statement*>  statement

	/* compound statement */
%nterm 	<list(st_compound_item)>  compound_statement
%nterm 	<arraylist(st_compound_item)>  compound_statement_item_list
%nterm	<st_compound_item>             compound_statement_item

	/* expression statement */
%nterm 	<st_expression>  expression_statement

	/* if statement */
%nterm	<st_if>  if_statement
%nterm	<st_if>  condition_statement

	/* while statement */
%nterm	<st_while>  iteration_statement

	/* jump statement */
%nterm	<st_jump>  jump_statement





	/* rule implementations */

%start declaration_list
%%

declaration_list
	: declaration
	| declaration_list declaration
	;

declaration
	: { context_skip(context, 1); }
      declaration_
	;

declaration_
	: structure_declaration 			
		{ 	
			// pass 1: import only name. skip from { to matching }
			// pass 2: import the members
			// pass 3: pass 2
			ast_declare(&context->ast, 
				DC_STRUCTURE, TOKEN_STRUCTURE_NAME, 
				$structure_declaration->name,
				$structure_declaration); 
		}

    | enum_declaration					
		{ 	
			// pass 1: import only name. skip from { to matching }
			// pass 2: import the members
			// pass 3: pass 2
			ast_declare(&context->ast,
				DC_ENUM, TOKEN_ENUM_NAME, 
				$enum_declaration->name,
				$enum_declaration); 
		}

	| alias_declaration	
		{
			// pass 1: import only name. skip until ;
			// pass 2: import the target
			// pass 3: pass 2
			ast_declare(&context->ast,
				DC_ALIAS, TOKEN_ALIAS_NAME, 
				$alias_declaration->name,
				$alias_declaration); 
		}

	| function
		{ 	
			// pass 1: skip entirely until ;
			// pass 2: import with no body
			// pass 3: import with body
			ast_declare(&context->ast, 			
				DC_FUNCTION, TOKEN_FUNCTION_NAME, 
				$function->name,
				$function); 
		}

    | global_variable_declaration_statement	
		{ 	
			// pass 1: skip entirely until ;
			// pass 2: import with no body
			// pass 3: import with body
			ast_declare(&context->ast,			
				DC_ST_VARIABLE, TOKEN_VARIABLE_NAME, 
				$global_variable_declaration_statement->name,
				$global_variable_declaration_statement); 
		}

	| import_declaration
		{ 
			// non-native
			// pass 1: do import pass 1
			// pass 2: do import pass 2
			// pass 3: do sequentially pass 1 and pass 2

			// native
			// pass 1: import
			// pass 2: no-op
			// pass 3: no-op

			if ($import_declaration->is_native) {
				if (context->pass == 1) {
					ast_add_declaration(&context->ast, DC_IMPORT, $import_declaration);
				}
			} else {
				context_import(context, $import_declaration);
			}
		}
	;

function_or_variable_name
	: IDENTIFIER
	| FUNCTION_NAME
		{ 
			/* todo assert not full here? */ 
			$$ = $FUNCTION_NAME->name;
		}
	| VARIABLE_NAME
		{
			/* todo assert not full here? */
			$$ = $VARIABLE_NAME->name;
		}
	;

type_and_name
	: type function_or_variable_name
		{
			$$.is_full = true;
			$$.type = $type;
			$$.name = $function_or_variable_name;
		}
	| IDENTIFIER function_or_variable_name
		{
			$$.is_full = false;
			$$.name = $function_or_variable_name;
		}
	;


function
	: function_declaration
	| function_definition
	;

function_prefix
	: type_and_name function_parameters
		{	
			context_enter(context, SCTX_FUNCTION); 

			iterate_array(i, $function_parameters.value.size) {
				ast_local_declaration dc = {
					.kind = TOKEN_FUNCTION_PARAMETER_NAME,
					.name = $function_parameters.value.data[i].name,
					.u_function_parameter = &$function_parameters.value.data[i]
				};

				arl_add(ast_local_declaration, context_current(context)->u_locals, dc);
			}

			context_skip(context, 2);

			$$ = allocate(dc_function);
			$$->is_full		=  $type_and_name.is_full;
			$$->is_extern	=  false;
			$$->name 	    =  $type_and_name.name;
			$$->return_type =  $type_and_name.type;
			$$->parameters  =  $function_parameters;
		}
	;

function_definition
	: function_prefix compound_statement
		{		
			$$ = $function_prefix;
			$$->body = $compound_statement;

			context_exit(context);
		}
	| function_prefix SKIPPED_BODY
		{
			$$ = $function_prefix;
			$$->is_full = false;

			context_exit(context);
		}
	| type_and_name SKIPPED_PARAMETERS SKIPPED_BODY
		{
			$$ = allocate(dc_function);
			$$->is_full		=  false;
			$$->is_extern	=  false;
			$$->name 	    =  $type_and_name.name;
			$$->return_type =  $type_and_name.type;
			$$->parameters.is_c_vararg = false;
			li_init_empty(dc_function_parameter, $$->parameters.value);
		}
	;

function_declaration
	: EXTERN type_and_name extern_function_parameters ';'
		{
			$$ = allocate(dc_function);
			$$->is_full		=  $type_and_name.is_full;
			$$->is_extern   =  true;
			$$->name 	    =  $type_and_name.name;
			$$->return_type =  $type_and_name.type;
			$$->parameters  =  $extern_function_parameters;
		}
	| EXTERN type_and_name SKIPPED_PARAMETERS ';'
		{
			$$ = allocate(dc_function);
			$$->is_full		=  false;
			$$->is_extern   =  true;
			$$->name 	    =  $type_and_name.name;
			$$->return_type =  $type_and_name.type;
			$$->parameters.is_c_vararg = false;
			li_init_empty(dc_function_parameter, $$->parameters.value);
		}
	;

extern_function_parameters
	: function_parameters

	| '(' function_parameter_list ',' C_VARARG ')'
		{ 
			li_init_from(dc_function_parameter, $$.value, $function_parameter_list);
			$$.is_c_vararg = true;
		}

	| '(' C_VARARG ')' 				   
		{ 
			li_init_empty(dc_function_parameter, $$.value);
			$$.is_c_vararg = true;
		}
	;

function_parameters
	: '(' function_parameter_list ')'
		{ 
			li_init_from(dc_function_parameter, $$.value, $function_parameter_list);
			$$.is_c_vararg = false;
		}

	| '(' ')' 				   
		{ 
			li_init_empty(dc_function_parameter, $$.value);
			$$.is_c_vararg = false;
		}
	;

function_parameter_list
	: function_parameter { arraylist_init_with(dc_function_parameter)(&$$, $function_parameter); }
	| function_parameter_list[value] ',' function_parameter { arl_assign_add(dc_function_parameter, $$, $function_parameter, $value); }
	;

function_parameter
	: type IDENTIFIER 
		{ 
			$$.type = $type; 
			$$.name = $IDENTIFIER;
		}
	;

structure_name
	: IDENTIFIER
	| STRUCTURE_NAME
		{ 
			/* todo assert not full here? */ 
			$$ = $STRUCTURE_NAME->name;
		}
	;

structure_declaration
	: TYPE structure_name structure_body ';'
		{
			$$ = allocate(dc_structure);
			$$->is_full = true;
			$$->name = $structure_name;
			$$->member_list = $structure_body;
		}
	| TYPE structure_name SKIPPED_BODY ';'
		{
			$$ = allocate(dc_structure);
			$$->is_full = false;
			$$->name = $structure_name;
			li_init_empty(dc_structure_member, $$->member_list);
		}
	;

structure_body
	: '{' structure_member_list '}'
		{ li_init_from(dc_structure_member, $$, $structure_member_list); }

	| '{' '}' 				   
		{ li_init_empty(dc_structure_member, $$); }
	;

structure_member_list
	: structure_member { arraylist_init_with(dc_structure_member)(&$$, $structure_member); }
	| structure_member_list[value] structure_member { arl_assign_add(dc_structure_member, $$, $structure_member, $value); }
	;

structure_member
	: type IDENTIFIER ';'
		{
			$$.type = $type;
			$$.name = $IDENTIFIER;
		}
	;

enum_name
	: IDENTIFIER
	| ENUM_NAME
		{ /* todo assert not full here? */
			$$ = $ENUM_NAME->name;
		}
	;

enum_declaration
	: ENUM enum_name 
		{ context_enter(context, SCTX_ENUM); }
	  enum_body ';'
		{
			$$ = context_get(context, SCTX_ENUM)->u_enum_context.value;
			$$->is_full = true;
			$$->name = $enum_name;
			$$->member_list = $enum_body;

			context_exit(context);
		}
	| ENUM enum_name 
		{ context_enter(context, SCTX_ENUM); }
	  SKIPPED_BODY ';'
	  	{
			$$ = allocate(dc_enum);
			$$->is_full = false;
			$$->name = $enum_name;
			li_init_empty(dc_enum_member, $$->member_list);

			context_exit(context);
		}
	;

enum_body
	: '{' enum_member_list '}'
		{ li_init_from(dc_enum_member, $$, $enum_member_list); }

	| '{' '}' 				   
		{ li_init_empty(dc_enum_member, $$); }
	;

enum_member_list
	: enum_member
		{ arraylist_init_with(dc_enum_member)(&$$, $enum_member); }

	| enum_member_list[value] ',' enum_member
		{ arl_assign_add(dc_enum_member, $$, $enum_member, $value); }
	;

enum_member
	: IDENTIFIER '=' expression
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;

			expect(parent_enum->kind != ENUM_KIND_IMPLICIT)
				otherwise("enum with implicit values already defined cannot have explicit values");

			$$.value = $expression->properties->constant;

			expect($$.value.kind != EX_C_ARRAY &&
				   $$.value.kind != EX_C_STRUCTURE &&
				   $$.value.kind != EX_C_DYNAMIC) 
				otherwise("invalid constant value for an enum member");

			$$.parent = parent_enum->value;

			parent_enum->member_index++;
			parent_enum->kind = ENUM_KIND_EXPLICIT;
		}
	| IDENTIFIER
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;

			expect(parent_enum->kind != ENUM_KIND_EXPLICIT)
				otherwise("enum with explicit values already defined cannot contain implicit values");

			$$.value.kind = EX_C_INT; //todo check current index to be in bounds
			$$.value.u_int = parent_enum->member_index;
			//todo use long?
			//todo go back to multiple ConstExpr types
			$$.value.origin = NULL;

			$$.parent = parent_enum->value;

			parent_enum->member_index++;
			parent_enum->kind = ENUM_KIND_IMPLICIT;
		}
	;

global_variable_declaration_statement
	: type_and_name '=' expression_block ';'
		{
			$$ = allocate(dc_st_variable);
			$$->is_full = $type_and_name.is_full;
			$$->type = $type_and_name.type;
			$$->name = $type_and_name.name;
			$$->value = $expression_block;

			expect(ast_type_can_merge(&$$->type, &$expression_block.value->properties->type))
				otherwise("variable declaration with illegal assignment from type \"%s\" to \"%s\"",
							ast_type_to_string(&$expression_block.value->properties->type), ast_type_to_string(&$$->type));
		}
	;

variable_declaration_statement
	: type IDENTIFIER '=' expression_block ';'
		{
			$$ = allocate(dc_st_variable);
			$$->is_full = true;
			$$->type = $type;
			$$->name = $IDENTIFIER;
			$$->value = $expression_block;

			expect(ast_type_can_merge(&$$->type, &$expression_block.value->properties->type))
				otherwise("variable declaration with illegal assignment from type \"%s\" to \"%s\"",
							ast_type_to_string(&$expression_block.value->properties->type), ast_type_to_string(&$$->type));

			se_context_level* function = context_find(context, SCTX_FUNCTION);
			if (function != NULL) {
				ast_local_declaration dc = {
					.kind = TOKEN_VARIABLE_NAME,
					.name = $$->name,
					.u_variable = $$
				};

				arl_add(ast_local_declaration, function->u_locals, dc);
			}
		}
	;

alias_name
	: IDENTIFIER
	| ALIAS_NAME
		{ 
			/* todo assert not full here? */ 
			$$ = $ALIAS_NAME->name;
		}
	;

alias_declaration
	: ALIAS alias_name '=' type ';'
		{
			$$ = allocate(dc_alias);
			$$->is_full = true;
			$$->name = $alias_name;
			$$->target = $type;
		}
	| ALIAS alias_name SKIPPED_STATEMENT
		{
			$$ = allocate(dc_alias);
			$$->is_full = false;
			$$->name = $alias_name;
		}
	;

import_declaration_
	: NATIVE import_node_list ';'
			{
				$$ = allocate(dc_import);
				$$->is_native = true;
				li_init_from(dc_import_node, $$->path, $import_node_list);
			}
	| import_node_list ';'
			{
				$$ = allocate(dc_import);
				$$->is_native = false;
				li_init_from(dc_import_node, $$->path, $import_node_list);
			}
	;

import_declaration
	: { context_enter(context, SCTX_IMPORT); }
		IMPORT import_declaration_[value]
	  { 
		$$ = $value;
		context_exit(context);
      }
	;

import_node_list
	: import_node
		{ arraylist_init_with(dc_import_node)(&$$, $import_node); }

	| import_node_list[value] '.' import_node
		{ arl_assign_add(dc_import_node, $$, $import_node, $value); }
	;

import_node
	: IDENTIFIER
		{ $$ = $IDENTIFIER; }
	;






boolean_expression
	: TRUE 
	| FALSE
	;

number_expression
	: LONG_CONSTANT	  { inherit_extern(number, long,   $$, $LONG_CONSTANT);   }
	| DOUBLE_CONSTANT { inherit_extern(number, double, $$, $DOUBLE_CONSTANT); }
	;

constructor_expression_basic
	: type invocation_expression 
		{ 
			$$ = allocate(ex_constructor);
			$$->type = allocate(ast_type);
			*$$->type = $type;
			$$->argument_list = $invocation_expression;

			if (ast_type_is_constant_array(&$type)) {
				$$->is_array = true;
				$$->u_array_size = ast_type_constant_array_size(&$type);
				arl_pop(ast_type_level, (*$$->type).level_list);
			} else {
				$$->is_array = false;
			}
		}
	;

constructor_expression_
	: constructor_expression_basic
		{ 
			$$ = $constructor_expression_basic;
			$$->is_new = false;
		}
	| NEW constructor_expression_basic 
		{
			$$ = $constructor_expression_basic;
			$$->is_new = true;
		}
	;

constructor_expression
	: constructor_expression_
		{
			$$ = $constructor_expression_;

			ex_constructor_type_check($$);
			arl_add(ex_constructor_ptr, context_get(context, SCTX_EXPRESSION)->u_ex_block.constructors, $$);
		}
	;

enum_member_expression
	: ENUM_NAME '.' IDENTIFIER
		{
			arl_find_by_name(
				in_list(dc_enum_member, $ENUM_NAME->member_list), 
				find_and_assign($IDENTIFIER, $$),
				on_error("enum \"%s\" has no member \"%s\"", $ENUM_NAME->name, $IDENTIFIER)
			);
		}
	;

basic_expression
	: VARIABLE_NAME
		{ inherit_extern(basic, variable, $$, $VARIABLE_NAME); }
		
	| FUNCTION_NAME
		{ inherit_extern(basic, function, $$, $FUNCTION_NAME); }

	| FUNCTION_PARAMETER_NAME
		{ inherit_extern(basic, function_parameter, $$, $FUNCTION_PARAMETER_NAME); }

	| boolean_expression
		{ inherit_extern(basic, boolean, $$, $boolean_expression); }

	| number_expression
		{ inherit_expression(basic, number, $$, $number_expression); }

	| CHAR_CONSTANT
		{ inherit_extern(basic, character, $$, $CHAR_CONSTANT); }
		
	| STRING_LITERAL	
		{ inherit_extern(basic, string, $$, $STRING_LITERAL); }

	| constructor_expression
		{ inherit_extern(basic, ex_constructor, $$, $constructor_expression);}

	| enum_member_expression
		{ inherit_extern(basic, enum_member, $$, $enum_member_expression); }

	| '(' expression ')'
		{ inherit_expression(basic, expression, $$, (*$expression)); }
	;

postfix_expression_recursive
	: basic_expression
		{ inherit_expression(postfix, basic, $$, $basic_expression); }

	| postfix_expression_recursive[value] '[' expression ']'
		{ inherit_self_with_ex(postfix, index, $$, $value, (*$expression)); }

	| postfix_expression_recursive[value] invocation_expression
		{ inherit_self_with(postfix, invocation, $$, $value, $invocation_expression); }

	| postfix_expression_recursive[value] '.' IDENTIFIER
		{ inherit_self_with_before(postfix, property, $$, $value, dc_structure_member*, $IDENTIFIER); }
	
	| postfix_expression_recursive[value] OP_POINTER_PROPERTY IDENTIFIER
		{ inherit_self_with_before(postfix, pointer_property, $$, $value, dc_structure_member*, $IDENTIFIER); }
	;

postfix_expression
	: postfix_expression_recursive
		{ inherit_self(postfix, plain, $$, $postfix_expression_recursive); }

	| postfix_expression_recursive OP_INCREMENT
		{ inherit_self(postfix, increment, $$, $postfix_expression_recursive); }

	| postfix_expression_recursive OP_DECREMENT
		{ inherit_self(postfix, decrement, $$, $postfix_expression_recursive); }
	;

invocation_expression
	: '(' invocation_argument_list ')' 
		{ li_init_from(expression_ptr, $$, $invocation_argument_list); }

	| '(' ')'						   
		{ li_init_empty(expression_ptr, $$); }

invocation_argument_list
	: expression { arraylist_init_with(expression_ptr)(&$$, $expression); }
	| invocation_argument_list[value] ',' expression { arl_assign_add(expression_ptr, $$, $expression, $value); }
	;

unary_expression_recursive
	: postfix_expression
		{ inherit_expression(unary, postfix, $$, $postfix_expression); }

	| '&' unary_expression_recursive[value]
		{ inherit_self(unary, reference, $$, $value); }

	| '*' unary_expression_recursive[value]
		{ inherit_self(unary, dereference, $$, $value); }

	| '~' unary_expression_recursive[value]
		{ inherit_self(unary, binary_not, $$, $value); }

	| '!' unary_expression_recursive[value]
		{ inherit_self(unary, logic_not, $$, $value); }
	;

unary_expression
	: unary_expression_recursive
		{ inherit_self(unary, plain, $$, $unary_expression_recursive); }

	| '+' unary_expression_recursive
		{ inherit_self(unary, plus, $$, $unary_expression_recursive); }

	| '-' unary_expression_recursive
		{ inherit_self(unary, minus, $$, $unary_expression_recursive); }

	| OP_INCREMENT unary_expression_recursive
		{ inherit_self(unary, increment, $$, $unary_expression_recursive); }

	| OP_DECREMENT unary_expression_recursive
		{ inherit_self(unary, decrement, $$, $unary_expression_recursive); }
	;

cast_expression
	: unary_expression
		{ inherit_expression(cast, unary, $$, $unary_expression); }

	| cast_expression[value] AS '(' type ')'
		{ inherit_self_with(cast, cast, $$, $value, $type); }
	;

binary_expression
	: cast_expression[value] /* todo naming "value", "parent" */
		{ inherit_expression(binary, cast, $$, $value); }

		/* multiplication */
	| binary_expression[a] '*' binary_expression[b] { iapi_inherit_binary(multiply, MULTIPLY, $$, $a, $b); }
	| binary_expression[a] '/' binary_expression[b] { iapi_inherit_binary(divide,   DIVIDE,   $$, $a, $b); }
	| binary_expression[a] '%' binary_expression[b] { iapi_inherit_binary(modulo,   MODULO,   $$, $a, $b); }

		/* addition */
	| binary_expression[a] '+' binary_expression[b] { iapi_inherit_binary(add, 	   ADD,      $$, $a, $b); }
	| binary_expression[a] '-' binary_expression[b] { iapi_inherit_binary(subtract, SUBTRACT, $$, $a, $b); }

		/* bit shift */
	| binary_expression[a] OP_SHIFT_LEFT  binary_expression[b] { iapi_inherit_binary(shift_left,  SHIFT_LEFT,  $$, $a, $b); }
	| binary_expression[a] OP_SHIFT_RIGHT binary_expression[b] { iapi_inherit_binary(shift_right, SHIFT_RIGHT, $$, $a, $b); }

		/* comparison */
	| binary_expression[a] '<' 			 	binary_expression[b] { iapi_inherit_binary(less_than,     LESS_THAN,     $$, $a, $b); }
	| binary_expression[a] OP_LESS_EQUAL	binary_expression[b] { iapi_inherit_binary(less_equal,    LESS_EQUAL,    $$, $a, $b); }
	| binary_expression[a] '>' 			 	binary_expression[b] { iapi_inherit_binary(greater_than,  GREATER_THAN,  $$, $a, $b); }
	| binary_expression[a] OP_GREATER_EQUAL binary_expression[b] { iapi_inherit_binary(greater_equal, GREATER_EQUAL, $$, $a, $b); }

		/* equality */
	| binary_expression[a] OP_EQUAL 	binary_expression[b] { iapi_inherit_binary(equal,     EQUAL,     $$, $a, $b); }
	| binary_expression[a] OP_NOT_EQUAL	binary_expression[b] { iapi_inherit_binary(not_equal, NOT_EQUAL, $$, $a, $b); }

		/* bit and */
	| binary_expression[a] '&' binary_expression[b] 		 { iapi_inherit_binary(binary_and, BINARY_AND, $$, $a, $b); }

		/* bit xor */
	| binary_expression[a] '^' binary_expression[b] 		 { iapi_inherit_binary(binary_xor, BINARY_XOR, $$, $a, $b); }

		/* bit or */
	| binary_expression[a] '|' binary_expression[b] 		 { iapi_inherit_binary(binary_or,  BINARY_OR,  $$, $a, $b); }

		/* logic and */
	| binary_expression[a] OP_LOGIC_AND binary_expression[b] { iapi_inherit_binary(logic_and,  LOGIC_AND,  $$, $a, $b); }

		/* logic or */
	| binary_expression[a] OP_LOGIC_OR  binary_expression[b] { iapi_inherit_binary(logic_or,   LOGIC_OR,   $$, $a, $b); }
	;

conditional_expression
	: binary_expression[parent]
		{ inherit_expression(condition, binary, $$, $parent); }

	| binary_expression[condition] '?' expression[if] ':' conditional_expression[parent]
		{ inherit_self_with_ex_and_ex(condition, condition, $$, $parent, $condition, (*$if)); }
	;

expression
	: conditional_expression[parent]
		{ 
			$$ = allocate(expression);
			inherit_expression(expression, condition, (*$$), $parent); 
		}

	| unary_expression[assignee] assignment_operator[operator] expression[parent]
		{
			$$ = allocate(expression);
			inherit_self_with_ex_and(expression, assignment, (*$$), (*$parent), $assignee, $operator); 
		}
	;

/**
 * Expression blocks only can be placed
 * inside a compound statement
 * to maintain context
 */
expression_block
	: expression
	  	{
			$$ = context_get(context, SCTX_EXPRESSION)->u_ex_block;
			if (!arraylist_is_empty($$.constructors)) {
				arl_trim(ex_constructor_ptr, $$.constructors);
			}
			$$.value = $expression;
			
		  	context_exit(context);
		
			context_enter(context, SCTX_EXPRESSION); 
		}
	;

assignment_operator
	: '=' 		{ $$ = OP_A_PLAIN; }
	| OP_ASSIGN { $$ = $OP_ASSIGN; }
	;






type
	: type_recursive
		{
			$$ = $type_recursive;
			if (!arraylist_is_empty($$.level_list)) {
				arl_trim(ast_type_level, $$.level_list);
			}
		}
	| UNSIGNED
		{
			context_enter(context, SCTX_FLAG); //todo potential issue - context not exited?
			flag_context_set_unsigned(context_current(context)->u_flag_context);
		}
	  type_recursive
	  	{
			$$ = $type_recursive;
			if (!arraylist_is_empty($$.level_list)) {
				arl_trim(ast_type_level, $$.level_list);
			}
		}
	;

type_recursive
	: type_terminal
		{ $$ = $type_terminal; }
	
	| type_recursive[value] '*'	   
		{ $$ = $value; ast_type_pointer_wrap(&$$); }

	| type_recursive[value] '[' ']' 
		{ $$ = $value; ast_type_array_wrap(&$$); }
	
	| type_recursive[value] '[' expression ']' 
		{ $$ = $value; ast_type_constant_array_wrap(&$$, &$expression->data); }
	;

type_terminal
    : PRIMITIVE_NAME 
		{ ast_type_init(&$$, AST_TYPE_PRIMITIVE, $PRIMITIVE_NAME); }

	| STRUCTURE_NAME
		{ ast_type_init(&$$, AST_TYPE_STRUCTURE, $STRUCTURE_NAME); }

	| ENUM_NAME
		{ ast_type_init(&$$, AST_TYPE_ENUM, $ENUM_NAME); }

	| ALIAS_NAME
		{ ast_type_clone_to(&$$, $ALIAS_NAME->target); }
	;





statement
	: compound_statement	{ u_init_ptr($$, ST_COMPOUND)  ->u_st_compound   = $compound_statement;   }
	| expression_statement	{ u_init_ptr($$, ST_EXPRESSION)->u_st_expression = $expression_statement; }
	| condition_statement	{ u_init_ptr($$, ST_IF)        ->u_st_if         = $condition_statement;  }
	| iteration_statement	{ u_init_ptr($$, ST_WHILE)     ->u_st_while      = $iteration_statement;  }
	| jump_statement		{ u_init_ptr($$, ST_JUMP)      ->u_st_jump       = $jump_statement;       }
	;

compound_statement
	: '{' 
		{ context_enter(context, SCTX_EXPRESSION); }
	   compound_statement_item_list '}'
		{ 
			context_exit(context);
			li_init_from(st_compound_item, $$, $compound_statement_item_list);
		}

	| '{' '}'
		{ li_init_empty(st_compound_item, $$); }
	;

compound_statement_item_list
	: compound_statement_item { arraylist_init_with(st_compound_item)(&$$, $compound_statement_item); }
	| compound_statement_item_list[value] compound_statement_item { arl_assign_add(st_compound_item, $$, $compound_statement_item, $value); }
	;

compound_statement_item
	: variable_declaration_statement { u_init($$, ST_C_ST_VARIABLE).u_variable  = $variable_declaration_statement; }
	| statement 					 { u_init($$, ST_C_STATEMENT)  .u_statement = $statement; 					   }
	;

expression_statement
	: ';'
		{ 
			arraylist_init_empty(ex_constructor_ptr)(&$$.constructors);
			$$.value = NULL;
		}

	| expression_block ';'
		{ $$ = $expression_block; }
	;

if_statement
	: IF '(' expression_block ')' statement
		{
			$$.condition = $expression_block;
			$$.body = $statement;
		}
	;

condition_statement
	: if_statement ELSE statement
		{
			$$ = $if_statement;
			$$.u_else = $statement;
			$$.has_else = true;
		}

	| if_statement
		{ 
			$$ = $if_statement; 
			$$.has_else = false;
		}
	;

iteration_statement
	: WHILE '(' expression_block ')' statement
		{
			$$.condition = $expression_block;
			$$.body = $statement;
		}
	;

jump_statement
	: CONTINUE ';' { $$.kind = ST_J_CONTINUE;    }
	| BREAK ';'	   { $$.kind = ST_J_BREAK;    	 }
	| RETURN ';'   { $$.kind = ST_J_RETURN_VOID; }

	| RETURN expression_block ';'
		{
			$$.kind = ST_J_RETURN;
			$$.u_return_value = $expression_block;
		}
	;





%%

/**
 * Throws a parser error
 *
 * @param[in] location Location of the error
 * @param[in] scanner The token scanner
 * @param[in] context Current compiler context
 * @param[in] message The error message
 */
void yyerror(MYYLTYPE* location, void* scanner, se_context* context, const char* message) {
	error_syntax("%s", message);
}

char* error_format_string(int argc) {
	switch (argc) {
		case 0: return "%@: syntax error";
		case 1: return "%@: syntax error: unexpected %u";
		case 2: return "%@: syntax error: expected %0e before %u";
		case 3: return "%@: syntax error: expected %0e or %1e before %u";
		case 4: return "%@: syntax error: expected %0e or %1e or %2e before %u";
		case 5: return "%@: syntax error: expected %0e or %1e or %2e or %3e before %u";
		case 6: return "%@: syntax error: expected %0e or %1e or %2e or %3e or %4e before %u";
		case 7: return "%@: syntax error: expected %0e or %1e or %2e or %3e or %4e or %5e before %u";
		case 8: return "%@: syntax error: expected %0e or %1e or %2e or %3e or %4e or %5e etc., before %u";
		otherwise_error
  	}
}

void location_print (FILE *out, MYYLTYPE const * const loc) {
  fprintf (out, "%d.%d", loc->first_line, loc->first_column);

  int end_col = 0 != loc->last_column ? loc->last_column - 1 : 0;
  if (loc->first_line < loc->last_line)
    fprintf (out, "-%d.%d", loc->last_line, end_col);
  else if (loc->first_column < end_col)
    fprintf (out, "-%d", end_col);
}

int yyreport_syntax_error(const yypcontext_t* yyctx, void* yyscanner, se_context* context) {
  enum { ARGS_MAX = 6 };
  yysymbol_kind_t arg[ARGS_MAX];

  int argsize = yypcontext_expected_tokens (yyctx, arg, ARGS_MAX);
  if (argsize < 0)
    return argsize;

  const int too_many_expected_tokens = argsize == 0 && arg[0] != YYSYMBOL_YYEMPTY;
  if (too_many_expected_tokens)
    argsize = ARGS_MAX;
  const char *format = error_format_string(1 + argsize + too_many_expected_tokens);

  const MYYLTYPE *loc = yypcontext_location (yyctx);
  while (*format)
    // %@: location.
    if (format[0] == '%' && format[1] == '@')
      {
        location_print (stderr, loc);
        format += 2;
      }
    // %u: unexpected token.
    else if (format[0] == '%' && format[1] == 'u')
      {
        fputs (yysymbol_name (yypcontext_token (yyctx)), stderr);
        format += 2;
      }
    // %0e, %1e...: expected token.
    else if (format[0] == '%'
             && isdigit ((unsigned char) format[1])
             && format[2] == 'e'
             && (format[1] - '0') < argsize)
      {
        int i = format[1] - '0';
        fputs (yysymbol_name (arg[i]), stderr);
        format += 3;
      }
    else
      {
        fputc (*format, stderr);
        ++format;
      }
  fputc ('\n', stderr);

  // Quote the source line.
  {
    fprintf (stderr, "%5d | %d\n", loc->first_line, loc->last_line);
    fprintf (stderr, "%5s | %*s", "", loc->first_column, "^");
    for (int i = loc->last_column - loc->first_column - 1; 0 < i; --i)
      putc ('~', stderr);
    putc ('\n', stderr);
  }
  return 0;
}