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
 *  C-native parser grammar rules for Bison
 */


%code requires {
	/* header code */

			/* includes */
	#include "syntax/expression/complex.h" /* expressions */
	#include "syntax/statement/statement.h" /* statements */
	#include "syntax/declaration/declaration.h" /* declarations */
	#include "language/native/types.h"
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
	void cyyerror(CYYLTYPE* location, void* scanner, se_context* context, const char* message);
}

%{	
	/* source code */

		/* includes */
	#include <ctype.h> /* number checking */

	#include "language/context.h" /* parser context */
	#include "language/parser.h"  /* parser header */
	#include "language/native/parser.h"  /* parser header */
	#include "misc/union.h" 	  /* union initialization */
	#include "ast/type/check.h"	  /* type checking */
	#include "ast/type/resolve.h" /* default type values */
	#include "ast/lookup.h"
	#include "language/native/declaration.h"

	int cyylex(CYYSTYPE* yylval_param, CYYLTYPE* yylloc_param, void* yyscanner, se_context* context);
%}


	/* bison options */
%param {void* yyscanner}  /* pass the yyscanner parameter */
%param {se_context* context} /* track the context as a parameter */

%define api.token.prefix {CTOKEN_} /* add a token prefix to avoid collisions */
%define api.prefix {cyy}	       /* add an api prefix to support multiple parsers */
%define api.value.type union 	   /* use a union type */

%define parse.error custom  /* use custom error handling */
%define api.pure 	full 	/* use pure parser interface */
%define parse.lac 	full	/* accurate list of expected tokens */
%locations 				   	/* track token locations */
%define parse.trace




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
%token	TYPEDEF 	"typedef"
		EXTERN		"extern"
		STATIC 		"static"
		AUTO 		"auto"
		REGISTER 	"register"
		INLINE		"inline"
	
%token	CONST 		"const"
		RESTRICT 	"restrict"
		VOLATILE	"volatile"
	
%token	SIGNED 		"signed"
		UNSIGNED	"unsigned"
	
%token	COMPLEX 	"complex"
		IMAGINARY 	"imaginary"

%token	STRUCT 	"struct"
		UNION  	"union"
		ENUM 	"enum"
		BOOL	"bool"

%token	CASE		"case"
		DEFAULT	 "default"
		IF 			"if"
		ELSE		"else"
		SWITCH 		"switch"
		WHILE 		"while"
		DO			"do"
		FOR 		"for"
		GOTO		"goto"
		CONTINUE 	"continue"
		BREAK 		"break"
		RETURN		"return"

%token  FUNC_NAME	"__func__"
		SIZEOF		"sizeof"

%token	ALIGNAS 	"alignas"
		ALIGNOF 	"alignof"
		ATOMIC		"atomic"
		GENERIC 	"generic"
		NORETURN 	"noreturn"
		STATIC_ASSERT 	"static assert"
		THREAD_LOCAL	"thread-local"

	/* type references */
%token ENUMERATION_CONSTANT

	/* type references */
%token <dc_structure*> 			STRUCTURE_NAME 		"structure name"
%token <dc_alias*> 				ALIAS_NAME 			"alias name"
%token <ast_type_primitive*>	PRIMITIVE_NAME		"primitive type name"
%token <dc_enum*>				ENUM_NAME			"enum name"

	/* value references */
%token <dc_st_variable*>		VARIABLE_NAME  			"variable name"
%token <dc_function*> 			FUNCTION_NAME 			"function name"

	/* special tokens */
%token <void*> ANY_NAME /* should only be used by the ast_lex_token function */
%token SKIPPED_PARAMETERS
%token SKIPPED_BODY
%token SKIPPED_STATEMENT

	/* other tokens */
%token C_VARARG "..."




		/* rule types */

	/* C declarations */
%nterm  <c_declaration> external_declaration
%nterm  <c_declaration> function_definition
%nterm  <c_declaration> declaration
%nterm  <arraylist(c_declaration)> declaration_list

	/* C declaration specifiers */
%nterm <c_storage_class_specifier> 	 storage_class_specifier
%nterm <c_function_specifier> 		 function_specifier
%nterm <c_type_qualifier> 			 type_qualifier
%nterm <arraylist(c_type_qualifier)> type_qualifier_list
%nterm <c_type_specifier>			 type_specifier
%nterm <c_declaration_specifiers>    declaration_specifiers
%nterm <c_declaration_specifiers>	 specifier_qualifier_list

	/* C declarators */
%nterm  <int> pointer	
%nterm  <c_declarator>  init_declarator
%nterm  <arraylist(c_declarator)>	init_declarator_list
%nterm  <c_declarator>  struct_declarator
%nterm  <arraylist(c_declarator)>	struct_declarator_list
%nterm  <c_declarator>  declarator
%nterm  <c_declarator>  direct_declarator
%nterm  <c_declarator>  abstract_declarator
%nterm  <c_declarator>  direct_abstract_declarator

	/* function */
%nterm  <dc_function_parameters>  		   	function_parameters
%nterm  <arraylist(dc_function_parameter)> 	function_parameter_list
%nterm  <dc_function_parameter>			   	function_parameter
%nterm  <c_declaration>						parameter_declaration

	/* structure */
%nterm 	<char*> 							structure_name
%nterm  <dc_structure*>						struct_or_union_specifier
%nterm 	<list(dc_structure_member)>		  	structure_body
%nterm 	<arraylist(dc_structure_member)> 	structure_member_list
%nterm	<dc_structure_member>			  	structure_member
%nterm  <c_declaration>					  	struct_declaration

	/* enum */
%nterm 	<char*> 					enum_name
%nterm  <dc_enum*>					enum_specifier
%nterm  <list(dc_enum_member)> 		enum_body
%nterm  <arraylist(dc_enum_member)> enum_member_list
%nterm  <dc_enum_member>			enum_member

	/* expression */
%nterm 	<op_assign>    assignment_operator

	/* type */
%nterm  <ast_type>  type_terminal





	/* rule implementations */

%start translation_unit
%%

primary_expression
	: IDENTIFIER
	| constant
	| string
	| '(' expression ')'
	| generic_selection
	;

constant
	: LONG_CONSTANT
	| CHAR_CONSTANT
	| DOUBLE_CONSTANT
	| ENUMERATION_CONSTANT
	;

string
	: STRING_LITERAL
	| FUNC_NAME
	;

generic_selection
	: GENERIC '(' assignment_expression ',' generic_assoc_list ')'
	;

generic_assoc_list
	: generic_association
	| generic_assoc_list ',' generic_association
	;

generic_association
	: type_name ':' assignment_expression
	| DEFAULT ':' assignment_expression
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression OP_POINTER_PROPERTY IDENTIFIER
	| postfix_expression OP_INCREMENT
	| postfix_expression OP_DECREMENT
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| OP_INCREMENT unary_expression
	| OP_DECREMENT unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	| ALIGNOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression OP_SHIFT_LEFT additive_expression
	| shift_expression OP_SHIFT_RIGHT additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression OP_LESS_EQUAL shift_expression
	| relational_expression OP_GREATER_EQUAL shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression OP_EQUAL relational_expression
	| equality_expression OP_NOT_EQUAL relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression OP_LOGIC_AND inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OP_LOGIC_OR logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '=' 		{ $$ = OP_A_PLAIN; }
	| OP_ASSIGN { $$ = $OP_ASSIGN; }
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers ';'
		{ 
			$$.specs = $declaration_specifiers; 
			arraylist_init_empty(c_declarator)(&$$.declarators);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	
	| declaration_specifiers init_declarator_list ';'
		{
			$$.specs = $declaration_specifiers; 
			$$.declarators = $init_declarator_list;
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	
	| static_assert_declaration {	}
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers[value] 
		{
			$$ = $value;
			arl_add(c_storage_class_specifier, $$.storage_specs, $storage_class_specifier); 
		}
	
	| storage_class_specifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_storage_class_specifier, $$.storage_specs, $storage_class_specifier); 
		}

	| type_specifier declaration_specifiers[value]
		{
			$$ = $value;
			arl_add(c_type_specifier, $$.type_specs, $type_specifier);
		}

	| type_specifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_type_specifier, $$.type_specs, $type_specifier);
		}

	| type_qualifier declaration_specifiers[value] 
		{
			$$ = $value;
			arl_add(c_type_qualifier, $$.type_quals, $type_qualifier);
		}

	| type_qualifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_type_qualifier, $$.type_quals, $type_qualifier);
		}

	| function_specifier declaration_specifiers[value] 
		{ 
			$$ = $value;
			arl_add(c_function_specifier, $$.function_specs, $function_specifier);
		}

	| function_specifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_function_specifier, $$.function_specs, $function_specifier);
		}

	| alignment_specifier declaration_specifiers[value] { $$ = $value; }
	| alignment_specifier { c_declaration_specifier_init(&$$); }
	;

init_declarator_list
	: init_declarator 
		{ arraylist_init_with(c_declarator)(&$$, $init_declarator); }

	| init_declarator_list[value] ',' init_declarator 
		{ arl_assign_add(c_declarator, $$, $init_declarator, $value); }
	;

init_declarator
	: declarator '=' initializer { $$ = $declarator; }
	| declarator				 { $$ = $declarator; }
	;

storage_class_specifier
	: TYPEDEF		{ $$ = C_SCLS_TYPEDEF; 	}
	| EXTERN		{ $$ = C_SCLS_EXTERN;	}
	| STATIC		{ $$ = C_SCLS_STATIC;	}
	| THREAD_LOCAL	{ $$ = C_SCLS_THREAD_LOCAL; }
	| AUTO			{ $$ = C_SCLS_AUTO;		}
	| REGISTER		{ $$ = C_SCLS_REGISTER;	}
	;

type_specifier
	: SIGNED 	{ $$.kind = C_TS_SIGNED;    }
	| UNSIGNED  { $$.kind = C_TS_UNSIGNED;  }
	| COMPLEX   { $$.kind = C_TS_COMPLEX;   }
	| IMAGINARY { $$.kind = C_TS_IMAGINARY; }
	| atomic_type_specifier		{ $$.kind = C_TS_ATOMIC; }
	
	| struct_or_union_specifier { $$.kind = C_TS_TYPE; ast_type_init(&$$.u_type, AST_TYPE_STRUCTURE, $struct_or_union_specifier);}
	| enum_specifier			{ $$.kind = C_TS_TYPE; ast_type_init(&$$.u_type, AST_TYPE_ENUM, $enum_specifier); }
	| type_terminal				{ $$.kind = C_TS_TYPE; $$.u_type = $type_terminal; }
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

struct_or_union_prefix
	: STRUCT
	| UNION
	;

structure_name
	: IDENTIFIER
	| STRUCTURE_NAME { $$ = $STRUCTURE_NAME->name; }
	;

struct_or_union_specifier
	: struct_or_union_prefix structure_name structure_body
		{
			$$ = allocate(dc_structure);
			$$->is_full = true;
			$$->is_c_struct = true;
			$$->name = $structure_name;
			$$->member_list = $structure_body;
			li_init_empty(dc_generic_ptr, $$->generics);
			arraylist_init_empty(list(ast_type))(&$$->_generic_impls);
		}
	
	| struct_or_union_prefix structure_body
		{
			$$ = allocate(dc_structure);
			$$->is_full = true;
			$$->is_c_struct = true;
			$$->name = NULL;
			$$->member_list = $structure_body;
			li_init_empty(dc_generic_ptr, $$->generics);
			arraylist_init_empty(list(ast_type))(&$$->_generic_impls);
		}

	| struct_or_union_prefix structure_name
		{
			char* actual = cst_native_struct_name($structure_name);
			declaration* dc = ast_declaration_lookup(&context->ast, actual);
			if (dc == NULL) {
				dc_structure* st = allocate(dc_structure);
				st->is_full = false;
				st->is_c_struct = true;
				st->name = actual;
				li_init_empty(dc_generic_ptr, st->generics);
				li_init_empty(dc_structure_member, st->member_list);

				ast_declare_native(&context->ast, 
					DC_STRUCTURE, TOKEN_STRUCTURE_NAME, CTOKEN_STRUCTURE_NAME, 
					st->name, st,
					arraylist_last(context->file_list)->filename); 
				
				$$ = st;
			} else {
				if (dc->kind == DC_STRUCTURE) {
					$$ = dc->u_structure;
					free(actual);
				} else {
					logfe("expected a structure name (%s)", actual);
				}
			}
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
	: struct_declaration
		{ $$ = cst_native_declaration_to_structure_member(context, $struct_declaration); }
	;

struct_declaration
	: specifier_qualifier_list ';'
		{
			$$.specs = $specifier_qualifier_list;
			arraylist_init_empty(c_declarator)(&$$.declarators);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}

	| specifier_qualifier_list struct_declarator_list ';'
		{
			$$.specs = $specifier_qualifier_list;
			$$.declarators = $struct_declarator_list;
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	
	| static_assert_declaration {	}
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list[value]
		{
			$$ = $value;
			arl_add(c_type_specifier, $$.type_specs, $type_specifier);
		}

	| type_specifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_type_specifier, $$.type_specs, $type_specifier);
		}

	| type_qualifier specifier_qualifier_list[value] 
		{
			$$ = $value;
			arl_add(c_type_qualifier, $$.type_quals, $type_qualifier);
		}

	| type_qualifier 
		{ 
			c_declaration_specifier_init(&$$);
			arl_add(c_type_qualifier, $$.type_quals, $type_qualifier);
		}
	;

struct_declarator_list
	: struct_declarator
		{ arraylist_init_with(c_declarator)(&$$, $struct_declarator); }

	| struct_declarator_list[value] ',' struct_declarator
		{ arl_assign_add(c_declarator, $$, $struct_declarator, $value); }
	;

struct_declarator
	: ':' constant_expression				{	}
	| declarator ':' constant_expression	{ $$ = $declarator;	}
	| declarator							{ $$ = $declarator;	}
	;

enum_prefix
	: ENUM { context_enter(context, SCTX_ENUM); }
	;	

enum_name
	: IDENTIFIER
	| ENUM_NAME { $$ = $ENUM_NAME->name; }
	;

enum_specifier
	: enum_prefix enum_name enum_body
		{
			$$ = context_get(context, SCTX_ENUM)->u_enum_context.value;
			$$->is_full = true;
			$$->is_c_enum = true;
			$$->name = $enum_name;
			$$->member_list = $enum_body;

			context_exit(context);
		}
	| enum_prefix enum_body
		{
			$$ = context_get(context, SCTX_ENUM)->u_enum_context.value;
			$$->is_full = true;
			$$->is_c_enum = true;
			$$->name = NULL;
			$$->member_list = $enum_body;

			context_exit(context);
		}
	| enum_prefix enum_name
		{
			char* actual = cst_native_enum_name($enum_name);
			declaration* dc = ast_declaration_lookup(&context->ast, actual);
			if (dc == NULL) {
				dc_enum* st = allocate(dc_enum);
				st->is_full = false;
				st->is_c_enum = true;
				st->name = actual;
				li_init_empty(dc_enum_member, st->member_list);

				ast_declare_native(&context->ast, 
					DC_ENUM, TOKEN_ENUM_NAME, CTOKEN_ENUM_NAME, 
					st->name, st,
					arraylist_last(context->file_list)->filename); 
				
				$$ = st;
			} else {
				if (dc->kind == DC_ENUM) {
					$$ = dc->u_enum;
					free(actual);
				} else {
					logfe("expected a enum name (%s)", actual);
				}
			}
		}
	;

enum_start
	: '{'
	;

enum_end
	: ',' '}'
	| '}'
	;

enum_body
	: enum_start enum_member_list enum_end
		{ li_init_from(dc_enum_member, $$, $enum_member_list); }

	| enum_start enum_end				   
		{ li_init_empty(dc_enum_member, $$); }
	;

enum_member_list
	: enum_member
		{ arraylist_init_with(dc_enum_member)(&$$, $enum_member); }

	| enum_member_list[value] ',' enum_member
		{ arl_assign_add(dc_enum_member, $$, $enum_member, $value); }
	;

enum_member
	: IDENTIFIER '=' constant_expression
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;

			expect(parent_enum->kind != ENUM_KIND_IMPLICIT)
				otherwise("enum with implicit values already defined cannot have explicit values");

			$$.value.kind = EX_C_INT;
			$$.value.u_int = parent_enum->member_index; /* todo resolve enums properly */
			$$.value.origin = NULL;

			expect($$.value.kind != EX_C_ARRAY &&
				   $$.value.kind != EX_C_STRUCTURE &&
				   $$.value.kind != EX_C_DYNAMIC) 
				otherwise("invalid constant value for an enum member");

			$$.parent = parent_enum->value;

			parent_enum->member_index = $$.value.u_int + 1;
			parent_enum->kind = ENUM_KIND_EXPLICIT;
		}
	| IDENTIFIER
		{
			enum_context* parent_enum = &context_get(context, SCTX_ENUM)->u_enum_context;

			$$.name = $IDENTIFIER;

			$$.value.kind = EX_C_INT;
			$$.value.origin = NULL;
			$$.value.u_int = parent_enum->member_index;
			
			$$.parent = parent_enum->value;
			parent_enum->member_index++;
		}
	;

atomic_type_specifier
	: ATOMIC '(' type_name ')'
	;

type_qualifier
	: CONST		{ $$ = C_TQ_CONST; 		}
	| RESTRICT 	{ $$ = C_TQ_RESTRICT; 	}
	| VOLATILE	{ $$ = C_TQ_VOLATILE; 	}
	| ATOMIC	{ $$ = C_TQ_ATOMIC; 	}
	;

function_specifier
	: INLINE	{ $$ = C_FS_INLINE;		}
	| NORETURN	{ $$ = C_FS_NORETURN;	}
	;

alignment_specifier
	: ALIGNAS '(' type_name ')'
	| ALIGNAS '(' constant_expression ')'
	;

declarator
	: pointer direct_declarator
		{ 
			$$ = $direct_declarator;
			for (int i = 0; i < $pointer; i++) {
				ast_type_level level = { .kind = AT_LEVEL_POINTER };
				arl_add(ast_type_level, $$.level_list, level); 
			}
		}
	
	| direct_declarator { $$ = $direct_declarator; }
	;

declarator_array
	: '[' ']'
	| '[' '*' ']'
	| '[' STATIC type_qualifier_list assignment_expression ']'
	| '[' STATIC assignment_expression ']'
	| '[' type_qualifier_list '*' ']'
	| '[' type_qualifier_list STATIC assignment_expression ']'
	| '[' type_qualifier_list assignment_expression ']'
	| '[' type_qualifier_list ']'
	| '[' assignment_expression ']'
	;

direct_declarator
	: IDENTIFIER
		{ 
			$$.name = $IDENTIFIER; 
			$$.is_function = false;
			arraylist_init_empty(ast_type_level)(&$$.level_list);
		}
	
	| FUNCTION_NAME
		{ 
			$$.name = $FUNCTION_NAME->name; 
			$$.is_function = false;
			arraylist_init_empty(ast_type_level)(&$$.level_list);
		}

	| '(' declarator ')'
		{ $$ = $declarator; }

	| direct_declarator[value] declarator_array
		{ $$ = $value; 
			ast_type_level level = { .kind = AT_LEVEL_ARRAY, .u_array_size = 0 };
			arl_add(ast_type_level, $$.level_list, level); 
		}

	| direct_declarator[value] '(' function_parameters ')'
		{ $$ = $value; 
			$$.is_function = true;
			$$.u_parameters = $function_parameters;
		}

	| direct_declarator[value] '(' ')'
		{ $$ = $value; 
			$$.is_function = true;
		}
		
	| direct_declarator[value] '(' identifier_list ')'
		{ $$ = $value; 
			logfe("I DON'T KNOW WHAT TO DO");
		}
	;

pointer
	: '*' type_qualifier_list pointer[value] 	{ $$ = $value + 1; }
	| '*' type_qualifier_list 					{ $$ = 1; }
	| '*' pointer[value] 						{ $$ = $value + 1; }
	| '*' 										{ $$ = 1; }
	;

type_qualifier_list
	: type_qualifier
		{ arraylist_init_with(c_type_qualifier)(&$$, $type_qualifier); }

	| type_qualifier_list[value] type_qualifier
		{ arl_assign_add(c_type_qualifier, $$, $type_qualifier, $value); }
	;


function_parameters
	: function_parameter_list
		{ 
			li_init_from(dc_function_parameter, $$.value, $function_parameter_list);
			$$.is_c_vararg = false;
		}

	| function_parameter_list ',' C_VARARG
		{ 
			li_init_from(dc_function_parameter, $$.value, $function_parameter_list);
			$$.is_c_vararg = true;
		}
	;

function_parameter_list
	: function_parameter
		{ arraylist_init_with(dc_function_parameter)(&$$, $function_parameter); }

	| function_parameter_list[value] ',' function_parameter
		{ arl_assign_add(dc_function_parameter, $$, $function_parameter, $value); }
	;

function_parameter
	: parameter_declaration
		{ $$ = cst_native_declaration_to_function_parameter(context, $parameter_declaration); }
	;

parameter_declaration
	: declaration_specifiers declarator 
		{ 
			$$.specs = $declaration_specifiers; 
			arraylist_init_with(c_declarator)(&$$.declarators, $declarator);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	| declaration_specifiers abstract_declarator
		{ 
			$$.specs = $declaration_specifiers; 
			arraylist_init_with(c_declarator)(&$$.declarators, $abstract_declarator);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	| declaration_specifiers
		{ 
			$$.specs = $declaration_specifiers; 
			arraylist_init_empty(c_declarator)(&$$.declarators);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator
	: pointer direct_abstract_declarator
		{ 
			$$ = $direct_abstract_declarator;
			for (int i = 0; i < $pointer; i++) {
				ast_type_level level = { .kind = AT_LEVEL_POINTER };
				arl_add(ast_type_level, $$.level_list, level); 
			} 
		}

	| pointer
		{
			$$.name = NULL; 
			$$.is_function = false;
			arl_init(ast_type_level, $$.level_list);
			for (int i = 0; i < $pointer; i++) {
				ast_type_level level = { .kind = AT_LEVEL_POINTER };
				arl_add(ast_type_level, $$.level_list, level); 
			}
		}
	
	| direct_abstract_declarator
		{ $$ = $direct_abstract_declarator; }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
		{ $$ = $abstract_declarator; }
	
	| declarator_array
		{ 
			$$.name = NULL; 
			$$.is_function = false;
			ast_type_level level = { .kind = AT_LEVEL_ARRAY, .u_array_size = 0 };
			arraylist_init_with(ast_type_level)(&$$.level_list, level);
		}

	| direct_abstract_declarator[value] declarator_array
		{ $$ = $value;
			ast_type_level level = { .kind = AT_LEVEL_ARRAY, .u_array_size = 0 };
			arl_add(ast_type_level, $$.level_list, level);
		}
	
	| '(' ')'
		{
			$$.name = NULL; 
			$$.is_function = true;
			$$.u_parameters.is_c_vararg = false;
			li_init_empty(dc_function_parameter, $$.u_parameters.value);
			arraylist_init_empty(ast_type_level)(&$$.level_list);
		}

	| '(' function_parameters ')'
		{
			$$.name = NULL; 
			$$.is_function = true;
			$$.u_parameters = $function_parameters;
			arraylist_init_empty(ast_type_level)(&$$.level_list);
		}

	| direct_abstract_declarator[value] '(' ')'
		{ $$ = $value;
			$$.is_function = true;
			$$.u_parameters.is_c_vararg = false;
			li_init_empty(dc_function_parameter, $$.u_parameters.value);
		}

	| direct_abstract_declarator[value] '(' function_parameters ')'
		{ $$ = $value;
			$$.is_function = true;
			$$.u_parameters = $function_parameters;
		}
	;

initializer
	: '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	| assignment_expression
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list ',' designation initializer
	| initializer_list ',' initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;

static_assert_declaration
	: STATIC_ASSERT '(' constant_expression ',' STRING_LITERAL ')' ';'
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{'  block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration_terminal
	| translation_unit external_declaration_terminal
	;

external_declaration_terminal
	: external_declaration
		{ 
			arraylist(declaration) declarations = cst_native_declaration_translate(context, $external_declaration);
			logd("got %zu declarations", declarations.size);
			for (int i = 0; i < declarations.size; i++) {
				declaration dc = declarations.data[i];
				if (dc.name != NULL) {
					ast_declare_native(&context->ast,
						dc.kind, dc.token, dc.ctoken, 
						dc.name, dc.u__any, 
						arraylist_last(context->file_list)->filename);
				}
			}
		}
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
		{
			$$.specs = $declaration_specifiers;
			arraylist_init_with(c_declarator)(&$$.declarators, $declarator);
			$$.declarations = $declaration_list;
		}
	| declaration_specifiers declarator compound_statement
		{
			$$.specs = $declaration_specifiers;
			arraylist_init_with(c_declarator)(&$$.declarators, $declarator);
			arraylist_init_empty(c_declaration)(&$$.declarations);
		}
	;

declaration_list
	: declaration
		{ arraylist_init_with(c_declaration)(&$$, $declaration); }

	| declaration_list[value] declaration
		{ arl_assign_add(c_declaration, $$, $declaration, $value); }
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
void yyerror(CYYLTYPE* location, void* scanner, se_context* context, const char* message) {
	error_syntax("%s", message);
}

const char* error_format_string(int);
void location_print(FILE*, const CYYLTYPE*);

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

  const CYYLTYPE *loc = yypcontext_location (yyctx);
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