/**
 * @file error.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-07-26
 * 
 *  Error throwing functions
 */
    /* header guard */
#ifndef CARBONSTEEL_MISC_ERROR_H
#define CARBONSTEEL_MISC_ERROR_H

    /* includes */
#include "ctool/log.h" /* logging */

    /* defines */
/**
 * A switch-expression failsafe
 * for catching errors
 */
#define otherwise_error \
default:                      \
    error_internal("unexpected switch condition"); \
    break;                        \


/**
 * Throws a syntax error with
 * a specified message
 * 
 * @param[in] message The message string
 * @param[in] __VA_ARGS__ The message format
 */
#define error_syntax(message, ...) logfe("syntax error: \n --- " message, ##__VA_ARGS__);


/**
 * Throws an internal error with
 * a specified message
 * 
 * @param[in] message The message string
 * @param[in] __VA_ARGS__ The message format
 */
#define error_internal(message, ...) logfe("internal error: \n --- " message, ##__VA_ARGS__);


/**
 * Syntaxical sugar for checking conditions
 * to be true
 * 
 * Should be followed by an "otherwise" block
 * 
 * @param[in] condition The expected condition
 */
#define expect(condition) if (condition) {}


/**
 * Syntaxical sugar for throwing an error
 * after an "expect" statement
 * 
 * @param[in] error The syntax error
 * @param[in] __VA_ARGS__ The error format
 */
#define otherwise(error, ...) else { error_syntax(error, ##__VA_ARGS__); }


#endif /* CARBONSTEEL_MISC_ERROR_H */