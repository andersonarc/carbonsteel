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
	#include "language/parser.h" /* parser header */
	#include "misc/union.h" /* union initialization */

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




		/* tokens */

	/* identifier */
%token <char*>  IDENTIFIER 			"identifier"

	/* constants */
%token <int>   	INT_CONSTANT   		"int constant"
%token <char>   CHAR_CONSTANT   	"char constant"
%token <float>  FLOAT_CONSTANT 		"float constant"
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

	/* type references */
%token <dc_structure*> 			STRUCTURE_NAME 		"structure name"
%token <dc_alias*> 				ALIAS_NAME 			"alias name"
%token <ast_type_primitive*>	PRIMITIVE_NAME		"primitive type name"
%token <dc_enum*>				ENUM_NAME			"enum name"

	/* value references */
%token <dc_st_variable*>		VARIABLE_NAME  			"variable name"
%token <dc_function*> 			FUNCTION_NAME 			"function name"
%token <dc_function_parameter*> FUNCTION_PARAMETER_NAME	"function parameter name"

	/* special token */
%token <void*> ANY_NAME /* should only be used by the ast_lex_token function */

	/* other tokens */
%token C_VARARG "..."




		/* rule types */

	/* declaration */
%nterm 	<declaration>  declaration

	/* function */
%nterm 	<dc_function*>  function
%nterm 	<dc_function*>  function_declaration
%nterm 	<dc_function*>  function_definition
%nterm 	<dc_function_parameters> 			function_parameters
%nterm 	<dc_function_parameters> 			extern_function_parameters
%nterm 	<arraylist(dc_function_parameter)>  function_parameter_list
%nterm 	<dc_function_parameter>			  	function_parameter

	/* structure */
%nterm 	<dc_structure*>  structure_declaration
%nterm 	<list(dc_structure_member)>		  structure_body
%nterm 	<arraylist(dc_structure_member)>  structure_member_list
%nterm	<dc_structure_member>			  structure_member

	/* enum */
%nterm 	<dc_enum*>  enum_declaration
%nterm 	<list(dc_enum_member)>		 enum_body
%nterm 	<arraylist(dc_enum_member)>  enum_member_list
%nterm 	<dc_enum_member>			 enum_member

	/* variable */
%nterm 	<dc_st_variable*>  variable_declaration_statement

	/* alias */
%nterm 	<dc_alias*>  alias_declaration

	/* import */
%nterm 	<dc_import*>  import_declaration
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
%nterm  <expression_block>  expression_block

	/* type */
%nterm  <ast_type>  type
%nterm  <ast_type>  type_

	/* statement */
%nterm 	 <statement*>  statement

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
	: structure_declaration 			{ 	new_type_declaration(&context->ast, &$$, 
												DC_STRUCTURE, TOKEN_STRUCTURE_NAME, 
												$structure_declaration, 
												$structure_declaration->name 
										); }

    | enum_declaration					{ 	new_type_declaration(&context->ast, &$$, 
												DC_ENUM, TOKEN_ENUM_NAME, 
												$enum_declaration, 
												$enum_declaration->name 
										); }

	| alias_declaration					{ 	new_type_declaration(&context->ast, &$$, 
												DC_ALIAS, TOKEN_ALIAS_NAME, 
												$alias_declaration, 
												$alias_declaration->name 
										); }

	| function  						{ 	new_type_declaration(&context->ast, &$$, 			
												DC_FUNCTION, TOKEN_FUNCTION_NAME, 
												$function, 	
												$function->name
										); }

    | variable_declaration_statement	{ 	new_type_declaration(&context->ast, &$$, 			
												DC_ST_VARIABLE, TOKEN_VARIABLE_NAME, 
												$variable_declaration_statement, 	
												$variable_declaration_statement->name
										); }

	| import_declaration				{	new_declaration(&context->ast, &$$, DC_IMPORT, $import_declaration); }
	;

function
	: function_declaration
	| function_definition
	;

function_definition
	: type IDENTIFIER function_parameters
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
		}
      compound_statement
		{
			$$ = allocate(dc_function);
			$$->is_extern	=  false;
			$$->name 	    =  $IDENTIFIER;
			$$->return_type =  $type;
			$$->parameters  =  $function_parameters;
			$$->body 		=  $compound_statement;

			context_exit(context);
		}
	;

function_declaration
	: EXTERN type IDENTIFIER extern_function_parameters ';'
		{
			$$ = allocate(dc_function);
			$$->is_extern   =  true;
			$$->name 	    =  $IDENTIFIER;
			$$->return_type =  $type;
			$$->parameters  =  $extern_function_parameters;
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
	: function_parameter 
		{ arl_init_add(dc_function_parameter, $$, $function_parameter); }

	| function_parameter_list[value] ',' function_parameter 
		{ arl_assign_add(dc_function_parameter, $$, $function_parameter, $value); }
	;

function_parameter
	: type IDENTIFIER 
		{ 
			$$.type = $type; 
			$$.name = $IDENTIFIER;
		}
	;

structure_declaration
	: TYPE IDENTIFIER structure_body ';'
		{
			$$ = allocate(dc_structure);
			$$->name = $IDENTIFIER;
			$$->member_list = $structure_body;
		}
	;

structure_body
	: '{' structure_member_list '}'
		{ li_init_from(dc_structure_member, $$, $structure_member_list); }

	| '{' '}' 				   
		{ li_init_empty(dc_structure_member, $$); }
	;

structure_member_list
	: structure_member
		{ arl_init_add(dc_structure_member, $$, $structure_member); }

	| structure_member_list[value] structure_member 
		{ arl_assign_add(dc_structure_member, $$, $structure_member, $value); }
	;

structure_member
	: type IDENTIFIER ';'
		{
			$$.type = $type;
			$$.name = $IDENTIFIER;
		}
	;

enum_declaration
	: ENUM IDENTIFIER 
		{ context_enter(context, SCTX_ENUM); }
			enum_body ';'
		{
			$$ = context_get(context, SCTX_ENUM)->u_enum_context.value;
			$$->name = $IDENTIFIER;
			$$->member_list = $enum_body;

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
		{ arl_init_add(dc_enum_member, $$, $enum_member); }

	| enum_member_list[value] ',' enum_member
		{ arl_assign_add(dc_enum_member, $$, $enum_member, $value); }
	;

enum_member
	: IDENTIFIER '=' number_expression
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;
			$$.value = $number_expression;
			$$.parent = parent_enum->value;

			parent_enum->member_index++;
		}
	| IDENTIFIER
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;
			$$.value.kind = EX_N_INT;
			$$.value.u_int_constant = parent_enum->member_index;
			$$.parent = parent_enum->value;

			parent_enum->member_index++;
		}
	;

variable_declaration_statement
	: type IDENTIFIER '=' expression_block ';'
		{
			$$ = allocate(dc_st_variable);
			$$->type = $type;
			$$->name = $IDENTIFIER;
			$$->value = $expression_block;

			expect(ast_type_can_merge(&$$->type, $$->value.value->type))
				otherwise("variable declaration with illegal assignment from type \"%s\" to \"%s\"",
							ast_type_to_string($$->value.value->type), ast_type_to_string(&$$->type));

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

alias_declaration
	: ALIAS IDENTIFIER '=' type ';'
		{
			$$ = allocate(dc_alias);
			$$->name = $IDENTIFIER;
			$$->target = $type;
		}
	;

import_declaration
	: 
			{ context_enter(context, SCTX_IMPORT); }
		IMPORT import_node_list ';'
			{
				$$ = allocate(dc_import);
				li_init_from(dc_import_node, (*$$), $import_node_list);

				context_exit(context);
			}
	;

import_node_list
	: import_node
		{ arl_init_add(dc_import_node, $$, $import_node); }

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
	: INT_CONSTANT	 { u_init($$, EX_N_INT)  .u_int_constant   = $INT_CONSTANT;   }
	| FLOAT_CONSTANT { u_init($$, EX_N_FLOAT).u_float_constant = $FLOAT_CONSTANT; }
	;

constructor_expression_basic
	: type invocation_expression 
		{ 
			$$ = allocate(ex_constructor);
			$$->type = allocate(ast_type);
			*$$->type = $type;
			$$->argument_list = $invocation_expression;
			$$->is_array = false;
		}
	| type '[' expression ']' invocation_expression
		{
			$$ = allocate(ex_constructor);
			$$->type = allocate(ast_type);
			*$$->type = $type;
			$$->argument_list = $invocation_expression;
			$$->is_array = true;
			$$->u_array_size = $expression;
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
			arl_add(ex_constructor_ptr, context_get(context, SCTX_EXPRESSION)->u_expression_block.constructors, $$);
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
	: FUNCTION_NAME 		
		{ 
			u_init($$, EX_B_FUNCTION).u_function = $FUNCTION_NAME;
			$$.type = ast_type_new(AST_TYPE_FUNCTION, $$.u_function);
		}

	| VARIABLE_NAME
		{ 
			u_init($$, EX_B_VARIABLE).u_variable = $VARIABLE_NAME;
			$$.type = ast_type_clone($$.u_variable->type);
		}
	
	| FUNCTION_PARAMETER_NAME 
		{ 
			u_init($$, EX_B_FUNCTION_PARAMETER).u_function_parameter = $FUNCTION_PARAMETER_NAME;
			$$.type = ast_type_clone($$.u_function_parameter->type); 
		}

	| boolean_expression 	
		{ 
			u_init($$, EX_B_BOOLEAN).u_boolean = $boolean_expression;
			$$.type = ast_type_of_boolean();
		}

	| number_expression		
		{ 
			u_init($$, EX_B_EX_NUMBER).u_ex_number = $number_expression;
			$$.type = ast_type_of_ex_number($$.u_ex_number.kind);
		}

	| CHAR_CONSTANT
		{
			u_init($$, EX_B_CHARACTER).u_character = $CHAR_CONSTANT;
			$$.type = ast_type_of_character();
		}

	| STRING_LITERAL	    
		{ 
			u_init($$, EX_B_STRING).u_string = $STRING_LITERAL;
			$$.type = ast_type_of_string();
		}

	| constructor_expression
		{ 
			u_init($$, EX_B_EX_CONSTRUCTOR).u_ex_constructor = $constructor_expression;
			$$.type = ast_type_of_ex_constructor($$.u_ex_constructor);
		}

	| enum_member_expression
		{ 
			u_init($$, EX_B_ENUM_MEMBER).u_enum_member = $enum_member_expression;
			$$.type = ast_type_new(AST_TYPE_ENUM, $$.u_enum_member->parent);   
		}

	| '(' expression ')'		
		{ 
			u_init($$, EX_B_EXPRESSION).u_expression = $expression;
			$$.type = $$.u_expression->type;
		}
	;

postfix_expression_recursive
	: basic_expression
		{
			$$.value = $basic_expression;
			$$.type = $basic_expression.type;
			arl_init_empty(ex_postfix_level, $$.level_list);
		}

	| postfix_expression_recursive[value] '[' expression ']'
		{
			$$ = $value;
			ex_postfix_level level = {
				.kind = EX_PL_INDEX,
				.u_index = $expression
			};

			arl_add(ex_postfix_level, $$.level_list, level);
			ex_postfix_type_check_index(&$$);
			arl_pop(ast_type_level, $$.type->level_list);
		}

	| postfix_expression_recursive[value] invocation_expression
		{
			$$ = $value;
			ex_postfix_level level = {
				.kind = EX_PL_INVOCATION,
				.u_argument_list = $invocation_expression
			};

			arl_add(ex_postfix_level, $$.level_list, level);
			ex_postfix_type_check_invocation(&$$);
			ast_type_clone_to($$.type, $$.type->u_function->return_type);
		}

	| postfix_expression_recursive[value] '.' IDENTIFIER
		{
			$$ = $value;
			ex_postfix_level level = {
				.kind = EX_PL_PROPERTY
			};

			ex_postfix_type_precheck_property(&$$);
			arl_find_by_name(
				in_list(dc_structure_member, $$.type->u_structure->member_list),
				find_and_assign($IDENTIFIER, level.u_property),
				on_error("structure \"%s\" has no member \"%s\"", $$.type->u_structure->name, $IDENTIFIER)
			)
			
			arl_add(ex_postfix_level, $$.level_list, level);
			ast_type_clone_to($$.type, level.u_property->type);
		}
	
	| postfix_expression_recursive[value] OP_POINTER_PROPERTY IDENTIFIER
		{
			$$ = $value;
			ex_postfix_level level = {
				.kind = EX_PL_POINTER_PROPERTY
			};

			ex_postfix_type_precheck_pointer_property(&$$);
			arl_find_by_name(
				in_list(dc_structure_member, $$.type->u_structure->member_list),
				find_and_assign($IDENTIFIER, level.u_property),
				on_error("structure pointer \"%s\" has no member \"%s\"", $$.type->u_structure->name, $IDENTIFIER)
			)
			
			arl_add(ex_postfix_level, $$.level_list, level);
			ast_type_clone_to($$.type, level.u_property->type);
		}
	;

postfix_expression
	: postfix_expression_recursive
		{
			$$ = $postfix_expression_recursive;
			$$.kind = EX_P_PLAIN;
		}

	| postfix_expression_recursive OP_INCREMENT
		{
			$$ = $postfix_expression_recursive;
			$$.kind = EX_P_INCREMENT;

			expect(ast_type_is_number($$.type))
                otherwise("cannot increment a non-number of type \"%s\"", ast_type_to_string($$.type));
		}

	| postfix_expression_recursive OP_DECREMENT
		{
			$$ = $postfix_expression_recursive;
			$$.kind = EX_P_DECREMENT;

			expect(ast_type_is_number($$.type))
                otherwise("cannot decrement a non-number of type \"%s\"", ast_type_to_string($$.type));
		}
	;

invocation_expression
	: '(' invocation_argument_list ')' 
		{ li_init_from(expression_ptr, $$, $invocation_argument_list); }

	| '(' ')'						   
		{ li_init_empty(expression_ptr, $$); }

invocation_argument_list
	: expression
		{ arl_init_add(expression_ptr, $$, $expression); }

	| invocation_argument_list[value] ',' expression 
		{ arl_assign_add(expression_ptr, $$, $expression, $value); }
	;

unary_expression_recursive
	: postfix_expression
		{
			$$.value = $postfix_expression;
			$$.type = $$.value.type;
			arl_init_empty(op_unary, $$.op_list);
		}

	| '&' unary_expression_recursive[value]
		{
			$$ = $value;
			arl_add(op_unary, $$.op_list, OP_U_REFERENCE);

			ast_type_level level = { 
				.kind = AT_LEVEL_POINTER 
			};
            arl_add(ast_type_level, $$.type->level_list, level);
		}

	| '*' unary_expression_recursive[value]
		{
			$$ = $value;
			arl_add(op_unary, $$.op_list, OP_U_DEREFERENCE);

			expect(ast_type_last_level_is($$.type, AT_LEVEL_POINTER))
            	otherwise("cannot dereference a non-pointer of type %s", ast_type_to_string($$.type));
			arl_pop(ast_type_level, $$.type->level_list);
		}

	| '~' unary_expression_recursive[value]
		{
			$$ = $value;
			arl_add(op_unary, $$.op_list, OP_U_BINARY_NOT);

			expect(ast_type_is_number($$.type))
                otherwise("cannot apply binary not to a non-number of type \"%s\"", ast_type_to_string($$.type));
		}

	| '!' unary_expression_recursive[value]
		{
			$$ = $value;
			arl_add(op_unary, $$.op_list, OP_U_LOGIC_NOT);

			expect(ast_type_is_boolean($$.type))
                otherwise("cannot apply logic not to a non-boolean of type \"%s\"", ast_type_to_string($$.type));
		}
	;

unary_expression
	: unary_expression_recursive
		{
			$$ = $unary_expression_recursive;
			$$.kind = EX_U_PLAIN;
		}

	| '+' unary_expression_recursive
		{
			$$ = $unary_expression_recursive;
			$$.kind = EX_U_PLUS;
		}

	| '-' unary_expression_recursive
		{
			$$ = $unary_expression_recursive;
			$$.kind = EX_U_MINUS;

			expect(ast_type_is_number($$.type))
                otherwise("cannot negate a non-number of type \"%s\"", ast_type_to_string($$.type));
		}

	| OP_INCREMENT unary_expression_recursive
		{
			$$ = $unary_expression_recursive;
			$$.kind = EX_U_INCREMENT;

			expect(ast_type_is_number($$.type))
                otherwise("cannot increment a non-number of type \"%s\"", ast_type_to_string($$.type));
		}

	| OP_DECREMENT unary_expression_recursive
		{
			$$ = $unary_expression_recursive;
			$$.kind = EX_U_DECREMENT;

			expect(ast_type_is_number($$.type))
                otherwise("cannot decrement a non-number of type \"%s\"", ast_type_to_string($$.type));
		}
	;

cast_expression
	: unary_expression
		{
			$$.value = $unary_expression;
			$$.type = $$.value.type;
			arl_init_empty(ast_type, $$.cast_list);
		}

	| cast_expression[value] AS '(' type ')'
		{
			$$ = $value;
			ast_type_clone_to($$.type, $type);
			arl_add(ast_type, $$.cast_list, $type);
		}
	;

binary_expression
	: cast_expression[value]
		{
			$$.type = $value.type;
			$$.v.has_operation = false;
			$$.v.value = $value;
		}

		/* multiplication */
	| binary_expression[a] '*' binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_MULTIPLY, &$b); }
	| binary_expression[a] '/' binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_DIVIDE  , &$b); }
	| binary_expression[a] '%' binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_MODULO  , &$b); }

		/* addition */
	| binary_expression[a] '+' binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_ADD     , &$b); }
	| binary_expression[a] '-' binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_SUBTRACT, &$b); }

		/* bit shift */
	| binary_expression[a] OP_SHIFT_LEFT  binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_SHIFT_LEFT , &$b); }
	| binary_expression[a] OP_SHIFT_RIGHT binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_SHIFT_RIGHT, &$b); }

		/* comparison */
	| binary_expression[a] '<' 			 	binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_LESS_THAN    , &$b); }
	| binary_expression[a] '>' 			 	binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_GREATER_THAN , &$b); }
	| binary_expression[a] OP_LESS_EQUAL	binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_LESS_EQUAL   , &$b); }
	| binary_expression[a] OP_GREATER_EQUAL binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_GREATER_EQUAL, &$b); }

		/* equality */
	| binary_expression[a] OP_EQUAL 	binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_EQUAL,     &$b); }
	| binary_expression[a] OP_NOT_EQUAL	binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_NOT_EQUAL, &$b); }

		/* bit and */
	| binary_expression[a] '&' binary_expression[b] 		 { $$ = $a; ex_binary_add(&$$, OP_B_BINARY_AND, &$b); }

		/* bit xor */
	| binary_expression[a] '^' binary_expression[b] 		 { $$ = $a; ex_binary_add(&$$, OP_B_BINARY_XOR, &$b); }

		/* bit or */
	| binary_expression[a] '|' binary_expression[b] 		 { $$ = $a; ex_binary_add(&$$, OP_B_BINARY_OR , &$b); }

		/* logic and */
	| binary_expression[a] OP_LOGIC_AND binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_LOGIC_AND, &$b); }

		/* logic or */
	| binary_expression[a] OP_LOGIC_OR  binary_expression[b] { $$ = $a; ex_binary_add(&$$, OP_B_LOGIC_OR , &$b); }
	;

conditional_expression
	: binary_expression
		{
			$$.value = $binary_expression;

			$$.has_condition = false;
			$$.type = $$.value.type;
		}

	| binary_expression '?' expression ':' conditional_expression[value]
		{
			$$.value = $binary_expression;
			
			$$.u_if = $expression;
			$$.u_else = allocate(ex_condition);
			*$$.u_else = $value;

			$$.has_condition = true;

			expect(($$.type = ast_type_merge_extend($$.u_if->type, $$.u_else->type)) != NULL)
				otherwise("conditional expression values are not equal: first value has type \"%s\", but second has type \"%s\"",
							ast_type_to_string($$.u_if->type),
							ast_type_to_string($$.u_else->type));

			expect(ast_type_is_boolean($$.value.type))
				otherwise("conditional expression has non-boolean condition of type \"%s\"", 
							ast_type_to_string($$.value.type));
		}
	;

expression
	: conditional_expression
		{
			$$ = allocate(expression);
			$$->u_value = $conditional_expression;

			$$->has_assignment = false;
			$$->type = $$->u_value.type;
		}

	| unary_expression assignment_operator expression[value]
		{
			$$ = allocate(expression);
			$$->u_assignment.assignee = $unary_expression;
			$$->u_assignment.operator = $assignment_operator;
			$$->u_assignment.value = $value;

			$$->has_assignment = true;

			expect(($$->type = ast_type_merge_prioritized($$->u_assignment.assignee.type, $$->u_assignment.value->type)) != NULL)
				otherwise("illegal assignment to type \"%s\" from type \"%s\"",
								ast_type_to_string($$->u_assignment.assignee.type), 
								ast_type_to_string($$->u_assignment.value->type));

			if ($$->u_assignment.operator != OP_A_PLAIN) {
				expect(ast_type_is_number($$->u_assignment.assignee.type))
					otherwise("assignment operator \"%s\" can only be applied to numbers, got type \"%s\"", 
								op_assign_strings[$$->u_assignment.operator], 
								ast_type_to_string($$->u_assignment.assignee.type));
			}
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
			$$ = context_get(context, SCTX_EXPRESSION)->u_expression_block;
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
	: type_
		{
			$$ = $type_;
			if (!arraylist_is_empty($$.level_list)) {
				arl_trim(ast_type_level, $$.level_list);
			}
		}
	;

type_
    : PRIMITIVE_NAME 
		{ ast_type_init(&$$, AST_TYPE_PRIMITIVE, $PRIMITIVE_NAME); }

	| STRUCTURE_NAME
		{ ast_type_init(&$$, AST_TYPE_STRUCTURE, $STRUCTURE_NAME); }

	| ENUM_NAME
		{ ast_type_init(&$$, AST_TYPE_ENUM, $ENUM_NAME); }

	| ALIAS_NAME
		{ ast_type_clone_to(&$$, $ALIAS_NAME->target); }

    | type '*'	   
		{ 
			$$ = $type;
			ast_type_level level = { .kind = AT_LEVEL_POINTER };
			arl_add(ast_type_level, $$.level_list, level); 
		}

	| type '[' ']' 
		{ 
			$$ = $type;
			ast_type_level level = { .kind = AT_LEVEL_ARRAY };
			arl_add(ast_type_level, $$.level_list, level); 
		}
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
	: compound_statement_item
		{ arl_init_add(st_compound_item, $$, $compound_statement_item); }

	| compound_statement_item_list[value] compound_statement_item
		{ arl_assign_add(st_compound_item, $$, $compound_statement_item, $value); }
	;

compound_statement_item
	: variable_declaration_statement { u_init($$, ST_C_ST_VARIABLE).u_variable  = $variable_declaration_statement; }
	| statement 					 { u_init($$, ST_C_STATEMENT)  .u_statement = $statement; 					   }
	;

expression_statement
	: ';'
		{ 
			arl_init_empty(ex_constructor_ptr, $$.constructors);
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