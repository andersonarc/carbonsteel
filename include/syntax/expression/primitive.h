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
#include "syntax/expression/inheritance/interface.h" /* inheritance */

#include <stdint.h> /* int types */

    /* typedefs */
/**
 * Numerical expression that is either 
 * an integer or a float constant
 */
enum ex_number_kind {
    EX_N_LONG, EX_N_DOUBLE
};

struct ex_number_data {
    ex_number_kind kind;
    union {
        long long u_long;
        double u_double;
    };
};

declare_expression(number);
extern_inheritance(number, long, long long);
extern_inheritance(number, double, double);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_PRIMITIVE_H */