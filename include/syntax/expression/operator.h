/**
 * @file operator.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-12
 * 
 *  Unary, binary and assignment operator definitions
 *  and values for code generation
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_OPERATOR_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_OPERATOR_H

    /* includes */
#include "misc/list.h" /* list utilities */

    /* global variables */
/**
 * Unary operator character values
 */
extern const char op_unary_chars[];

/**
 * Binary operator string values
 */
extern const char* op_binary_strings[];

/**
 * Assignment operator string values
 */
extern const char* op_assign_strings[];

    /* typedefs */
/**
 * Unary operator list
 */
typedef enum op_unary {
    OP_U_REFERENCE,
    OP_U_DEREFERENCE,
    OP_U_BINARY_NOT,
    OP_U_LOGIC_NOT
} op_unary;


/**
 * Binary operator list
 */
typedef enum op_binary {
    /* multiplication */
    OP_B_MULTIPLY, OP_B_DIVIDE, OP_B_MODULO,

    /* addition */
    OP_B_ADD, OP_B_SUBTRACT,

    /* bit shift */
    OP_B_SHIFT_LEFT, OP_B_SHIFT_RIGHT,

    /* comparison */
    OP_B_GREATER_THAN, OP_B_GREATER_EQUAL, OP_B_LESS_THAN, OP_B_LESS_EQUAL,

    /* equality */
    OP_B_EQUAL, OP_B_NOT_EQUAL,

    /* bitwise operations */
    OP_B_BINARY_AND, OP_B_BINARY_XOR, OP_B_BINARY_OR,

    /* logic operations */
    OP_B_LOGIC_AND, OP_B_LOGIC_OR
} op_binary;


/**
 * Assignment operator list
 */
typedef enum op_assign {
    OP_A_PLAIN, 

    /* multiplication */
    OP_A_MULTIPLY, OP_A_DIVIDE, OP_A_MODULO,

    /* addition */
    OP_A_ADD, OP_A_SUBTRACT,

    /* bit shift */
    OP_A_SHIFT_LEFT, OP_A_SHIFT_RIGHT,

    /* bitwise operations */
    OP_A_BINARY_AND, OP_A_BINARY_XOR, OP_A_BINARY_OR
} op_assign;


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_OPERATOR_H */