/**
 * @file expression.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Complex expression type definitions
 * 
 *  Complex expressions are the next level
 *  after binary expressions.
 *  
 *  They include:
 *      @todo replace with if expression-statement
 *   - ex_condition, conditional expression (a ? b : c) 
 *   - expression, assignment expression (a = b), which returns the assigned value
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_COMPLEX_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_COMPLEX_H

    /* includes */
#include "syntax/expression/binary.h" /* binary expressions */

    /* typedefs */
/**
 * Conditional expression (a ? b : c)
 * 
 * Conditional expressions are
 * deprecated and should be replaced
 * with if expression-statements in
 * later updates
 */
struct ex_condition_data {
    bool has_condition;
    ex_binary_data value;
    expression_data* u_if;
    struct ex_condition_data* u_else;
};

declare_expression(condition);
expression_inheritance(condition, binary);
self_inheritance_with_ex_and_ex(condition, condition, binary, condition, expression, if_branch);

/**
 * Assignment expression
 * 
 * Assigned value type should be equal to
 * the assignee type.
 */
struct expression_data {
    bool has_assignment;
    union {
        ex_condition_data u_value;
        struct {
            ex_unary_data assignee;
            op_assign operator;
            struct expression_data* value;
        } u_assignment;
    };
};

declare_expression(expression);
expression_inheritance(expression, condition);
self_inheritance_with_ex_and(expression, assignment, unary, assignee, op_assign);


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_COMPLEX_H */