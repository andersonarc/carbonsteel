/**
 * @file properties.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-21
 * 
 *  Expression properties structure
 *  that contains metadata for constant
 *  expression evaluation and type of the expression
 * 
 *  Expression properties are stored alongside
 *  expression data to optimize memory usage
 *  (no duplicated properties for each expression level)
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_PROPERTIES_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_PROPERTIES_H

    /* includes */
#include "syntax/expression/constant/core.h"  /* constant expressions */
#include "ast/type/type.h"               /* lexical type */

    /* typedefs */
/**
 * Expression properties structure
 * whuch contains the expression type
 * and a constant expression value pointer
 * that can be NULL if the expression value
 * cannot be determined at compile time 
 */
struct expression_properties {
    ast_type type;
    ex_constant constant;
};


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_PROPERTIES_H */