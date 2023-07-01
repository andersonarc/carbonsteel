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

    /* defines */
/**
 * Binary operator name lookup table
 */
#define lookup_op_binary(action_name) _IMPL_OP_BINARY_##action_name
#define _IMPL_OP_BINARY_multiply      *
#define _IMPL_OP_BINARY_divide        /
#define _IMPL_OP_BINARY_modulo        %
#define _IMPL_OP_BINARY_add           +
#define _IMPL_OP_BINARY_subtract      -
#define _IMPL_OP_BINARY_shift_left    <<
#define _IMPL_OP_BINARY_shift_right   >>
#define _IMPL_OP_BINARY_greater_than  >
#define _IMPL_OP_BINARY_greater_equal >=
#define _IMPL_OP_BINARY_less_than     <
#define _IMPL_OP_BINARY_less_equal    <=
#define _IMPL_OP_BINARY_equal         ==
#define _IMPL_OP_BINARY_not_equal     !=
#define _IMPL_OP_BINARY_binary_and    &
#define _IMPL_OP_BINARY_binary_xor    ^
#define _IMPL_OP_BINARY_binary_or     |
#define _IMPL_OP_BINARY_logic_and     &&
#define _IMPL_OP_BINARY_logic_or      ||

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
iapi_binary_properties(binary_and);
iapi_binary_properties(binary_xor);
iapi_binary_properties(binary_or);
iapi_binary_properties(logic_and);
iapi_binary_properties(logic_or);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_BINARY_H */