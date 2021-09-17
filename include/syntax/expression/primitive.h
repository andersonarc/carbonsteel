/**
 * @file primitive.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-02
 * 
 *  Primitive expression type definitions
 * 
 *  Primitive expressions include:
 *   - ex_number, numerical expression
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_PRIMITIVE_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_PRIMITIVE_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */

    /* typedefs */
/**
 * Numerical expression that is either 
 * an integer or a float constant
 */
enum ex_number_kind {
    EX_N_INT, EX_N_FLOAT
};
struct ex_number {
    ex_number_kind kind;
    union {
        int u_int_constant;
        float u_float_constant;
    };
};

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_PRIMITIVE_H */