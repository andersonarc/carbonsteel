/**
 * @file binary.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-28
 * 
 *  Binary expression type definitions
 * 
 *  Binary expressions are either a unary
 *  expression or a list of unary expressions,
 *  separated by binary operators
 * 
 *  Some binary operators can only be applied
 *  to numerical/boolean values, so binary expression
 *  type is tracked and checked when adding a new operand.
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_BINARY_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_BINARY_H

    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */

    /* typedefs */
/**
 * Binary expression type definition
 * with a list of operands and operators
 * in order from highest to lowest priority
 */
struct ex_binary_data {
    bool has_operation;
    union {
        struct {
            ex_binary_data* a;
            op_binary operator;
            ex_binary_data* b;
        };
        ex_cast_data value;
    };
};

declare_expression(binary);
expression_inheritance(binary, cast);
iapi_binary_data();
iapi_binary_properties(multiply);
iapi_binary_properties(divide);
iapi_binary_properties(modulo);
iapi_binary_properties(add);
iapi_binary_properties(subtract);
iapi_binary_properties(shift_left);
iapi_binary_properties(shift_right);
iapi_binary_properties(greater_than);
iapi_binary_properties(greater_equal);
iapi_binary_properties(less_than);
iapi_binary_properties(less_equal);
iapi_binary_properties(equal);
iapi_binary_properties(not_equal);
iapi_binary_properties(less_than);
iapi_binary_properties(binary_and);
iapi_binary_properties(binary_xor);
iapi_binary_properties(binary_or);
iapi_binary_properties(logic_and);
iapi_binary_properties(logic_or);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_BINARY_H */