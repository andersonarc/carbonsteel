/**
 * @file operator.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-12
 * 
 *  Operator string and character values
 */
    /* includes */
#include "syntax/expression/operator.h" /* this */

    /* global variables */
/**
 * Unary operator character values
 */
const char op_unary_chars[] = {
    '&', '*', '~', '!'
};

    
/**
 * Binary operator string values
 */
const char* op_binary_strings[] = {
    "*", "/", "%",
    "+", "-",
    "<<", ">>",
    ">", ">=", "<", "<=",
    "==", "!=",
    "&", "^", "|",
    "&&", "||"
};


/**
 * Assignment operator string values
 */
const char* op_assign_strings[] = {
    "=", 
    "*=", "/=", "%=",
    "+=", "-=", 
    "<<=", ">>=",
    "&=", "^=", "|="
};