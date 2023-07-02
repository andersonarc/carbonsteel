/**
 * @file unary.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-27
 * 
 *  Unary expression types file
 * 
 *  Unary expressions include:
 *   - ex_constructor, constructor expression
 *   - ex_basic, basic expression
 *   - ex_postfix, postfix unary operation expression
 *   - ex_unary, prefix unary operation expression
 *   - ex_cast, casted unary expression
 * 
 *  After unary expressions, binary expressions are processed.
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_UNARY_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_UNARY_H

    /* includes */
#include "syntax/predeclaration.h"         /* predeclarations */
#include "syntax/expression/primitive.h"   /* primitive expressions */
#include "syntax/expression/operator.h"    /* operators */
#include "syntax/expression/inheritance/interface.h" /* expression inheritance */

    /* typedefs */
/**
 * Constructor expression for a 
 * structure, primitive type or an array
 * 
 * By default, the constructed value is
 * allocated on stack, but by adding a
 * "new" prefix it can be allocated in
 * the heap.
 */
struct ex_constructor {
    bool is_new;
    bool is_array;
    char* u_variable_name;
    expression_data* u_array_size;
    ast_type* type;
    list(expression_ptr) argument_list;
};

void ex_constructor_type_check(ex_constructor* this);



/**
 * Basic expression could be
 * a variable or a function reference,
 * a numerical or a boolean expression,
 * a string literal, a constructor expression,
 * an enum member expression,
 * a function parameter reference or another
 * expression enclosed in brackets
 */
enum ex_basic_kind {
    EX_B_VARIABLE, EX_B_FUNCTION,
    EX_B_NUMBER, EX_B_BOOLEAN, EX_B_CHARACTER,
    EX_B_STRING, EX_B_EX_CONSTRUCTOR, 
    EX_B_ENUM_MEMBER,
    EX_B_FUNCTION_PARAMETER, EX_B_EXPRESSION
};

struct ex_basic_data {
    ex_basic_kind kind;
    union {
        dc_st_variable* u_variable;
        dc_function* u_function;
        ex_number_data u_number;
        bool u_boolean;
        ex_character u_character;
        ex_enum_member* u_enum_member;
        char* u_string;
        ex_constructor* u_ex_constructor;
        dc_function_parameter* u_function_parameter;
        expression_data* u_expression;
    };
};

declare_expression(basic);
extern_inheritance(basic, variable, dc_st_variable*);
extern_inheritance(basic, function, dc_function*);
expression_inheritance(basic, number);
extern_inheritance(basic, boolean, bool);
extern_inheritance(basic, character, ex_character);
extern_inheritance(basic, enum_member, ex_enum_member*);
extern_inheritance(basic, string, char*);
extern_inheritance(basic, ex_constructor, ex_constructor*);
extern_inheritance(basic, function_parameter, dc_function_parameter*);
expression_inheritance(basic, expression);

/**
 * Postfix expression consists of
 * an arraylist of levels, which have
 * their kinds, and a final action
 * (none, increment or a decrement)
 * 
 * Postfix expression levels are:
 *  - Index operation       "foo[4]"
 *      - Restricted to arrays
 *      - u_index can only be an expression with a numerical type
 * 
 *  - Invocation operation  "foo(1, bar)"
 *      - Restricted to functions
 *      - Invocation arguments should match corresponding
 *          function parameters by count and type
 * 
 *  - Property access       "foo.bar"
 *      - Restricted to structures
 *      - Accessed property should be a valid structure member
 */
enum ex_postfix_level_kind {
    EX_PL_INDEX, EX_PL_INVOCATION,
    EX_PL_PROPERTY, EX_PL_POINTER_PROPERTY
};
arraylist_declare_functions(ex_postfix_level_kind);

struct ex_postfix_level {
    ex_postfix_level_kind kind;
    union {
        expression_data* u_index;
        list(expression_ptr) u_invocation;
        dc_structure_member* u_property;
        dc_structure_member* u_pointer_property; /* for macro compatibility */
    };
};
arraylist_declare_functions(ex_postfix_level);


enum ex_postfix_kind {
    EX_P_PLAIN, EX_P_INCREMENT,
    EX_P_DECREMENT
};

struct ex_postfix_data {
    ex_basic_data value;
    ex_postfix_kind kind;
    arraylist(ex_postfix_level) level_list;
};

declare_expression(postfix);
expression_inheritance(postfix, basic);
self_inheritance_with_ex(postfix, index, expression, index);
self_inheritance_with(postfix, invocation, list(expression_ptr));
self_inheritance_with_before(postfix, property, dc_structure_member*, char*);
self_inheritance_with_before(postfix, pointer_property, dc_structure_member*, char*);
self_inheritance(postfix, increment);
self_inheritance(postfix, decrement);
self_inheritance(postfix, plain);


/**
 * Unary expression is prefixed with
 * a list of unary operators and one
 * final operator (none, increment, decrement,
 * plus (no-op), minus (invert number))
 * 
 * Unary operators are:
 *  - Reference     "&"
 *      - Restricted to lvalues, but the check is performed
 *          by the backend compiler
 *      - Returns a pointer to a value
 * 
 *  - Dereference   "*"
 *      - Resticted to pointers
 *      - Returns a value of the pointer
 * 
 *  - Binary NOT   "~"
 *      - Restricted to numbers
 *      - Returns a result of a bitwise NOT operation
 * 
 *  - Logic NOT   "!"
 *      - Restricted to booleans
 *      - Returns a result of a logic NOT operation
 */
enum ex_unary_kind {
    EX_U_PLAIN, EX_U_INCREMENT, EX_U_DECREMENT,
    EX_U_PLUS, EX_U_MINUS
};

struct ex_unary_data {
    ex_postfix_data value;
    ex_unary_kind kind;
    arraylist(op_unary) op_list;
};

declare_expression(unary);
expression_inheritance(unary, postfix);
self_inheritance(unary, reference);
self_inheritance(unary, dereference);
self_inheritance(unary, binary_not);
self_inheritance(unary, logic_not);
self_inheritance(unary, increment);
self_inheritance(unary, decrement);
self_inheritance(unary, plus);
self_inheritance(unary, minus);
self_inheritance(unary, plain);


/**
 * Cast expression allows you to
 * explicitly change a type of an
 * expression to another
 * 
 * @warn There is no guarantee that cast result would be valid
 */
struct ex_cast_data {
    ex_unary_data value;
    arraylist(ast_type) cast_list;
};

declare_expression(cast);
arraylist_declare_functions(ex_cast);
expression_inheritance(cast, unary);
self_inheritance_with(cast, cast, ast_type);


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_UNARY_H */