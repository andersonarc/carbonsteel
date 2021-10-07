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
struct ex_binary_inline {
    bool has_operation;
    union {
        struct {
            ex_binary_inline* a;
            op_binary operator;
            ex_binary_inline* b;
        };
        ex_cast_data value;
    };
};
struct ex_binary {
    ast_type* type;
    ex_binary_inline v;
};

    /* functions */
void ex_binary_add(ex_binary* this, op_binary operator, ex_binary* operand);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_BINARY_H */