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
#include "syntax/expression/primitive.h" /* primitive expressions */
#include "ast/type/type.h"               /* lexical type */

    /* typedefs */
/**
 * Constant expression data
 * that can be either a numerical
 * constant, a character or a string
 * literal, an array or a structure 
 * whose members are constant expressions
 */
enum expression_constant_kind {
    EX_C_NUMBER, EX_C_CHARACTER,
    EX_C_STRING, EX_C_ARRAY,
    EX_C_STRUCTURE
};

struct expression_constant {
    expression_constant_kind kind;
    union {
        ex_number u_number;
        char u_character;
        char* u_string;
        expression_constant* u_members;
        struct {
            ex_number u_size;
            expression_constant* u_members;
        } u_array;
    };
};

/**
 * Expression properties structure
 * whuch contains the expression type
 * and a constant expression value pointer
 * that can be NULL if the expression value
 * cannot be determined at compile time 
 */
struct expression_properties {
    ast_type type;
    expression_constant* u_value;
};

    
#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_PROPERTIES_H */