/**
 * @file unary.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-27
 * 
 *  Unary expression types file
 * 
 *  Unary expressions include:
 *   - ex_enum_member, enum member expression
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
#include "syntax/predeclaration.h" /* predeclarations */
#include "syntax/expression/primitive.h" /* primitive expressions */
#include "syntax/expression/operator.h" /* operators */

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
    expression* u_array_size;
    ast_type* type;
    list(expression_ptr) argument_list;
};


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
    EX_B_EX_NUMBER, EX_B_BOOLEAN, EX_B_CHARACTER,
    EX_B_STRING, EX_B_EX_CONSTRUCTOR, 
    EX_B_ENUM_MEMBER,
    EX_B_FUNCTION_PARAMETER, EX_B_EXPRESSION
};
struct ex_basic {
    ast_type* type;
    ex_basic_kind kind;
    union {
        dc_st_variable* u_variable;
        dc_function* u_function;
        ex_number u_ex_number;
        ex_boolean u_boolean;
        ex_character u_character;
        ex_enum_member* u_enum_member;
        char* u_string;
        ex_constructor* u_ex_constructor;
        dc_function_parameter* u_function_parameter;
        expression* u_expression;
    };
};


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
struct ex_postfix_level {
    ex_postfix_level_kind kind;
    union {
        expression* u_index;
        list(expression_ptr) u_argument_list;
        dc_structure_member* u_property;
    };
};
enum ex_postfix_kind {
    EX_P_PLAIN, EX_P_INCREMENT,
    EX_P_DECREMENT
};
struct ex_postfix {
    ast_type* type;
    ex_postfix_kind kind;
    arraylist(ex_postfix_level) level_list;
    ex_basic value;
};


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
 *  - Bitwise NOT   "~"
 *      - Restricted to numbers
 *      - Returns a result of a bitwise NOT operation
 * 
 *  - Logical NOT   "!"
 *      - Restricted to booleans
 *      - Returns a result of a logic NOT operation
 */
enum ex_unary_kind {
    EX_U_PLAIN, EX_U_INCREMENT, EX_U_DECREMENT,
    EX_U_PLUS, EX_U_MINUS
};
struct ex_unary {
    ast_type* type;
    ex_unary_kind kind;
    arraylist(op_unary) op_list;
    ex_postfix value;
};


/**
 * Cast expression allows you to
 * explicitly change a type of an
 * expression to another
 * 
 * @warn There is no guarantee that cast result would be valid
 */
struct ex_cast {
    ast_type* type;
    ex_unary value;
    arraylist(ast_type) cast_list;
};


    /* functions */
    
void ex_constructor_type_check(ex_constructor* this);

void ex_postfix_type_check_index(ex_postfix* this);
void ex_postfix_type_check_invocation(ex_postfix* this);
void ex_postfix_type_precheck_property(ex_postfix* this);
void ex_postfix_type_precheck_pointer_property(ex_postfix* this);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_UNARY_H */