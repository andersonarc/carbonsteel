/**
 * @file error.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-07-26
 * 
 *  Parser error throwing functions
 */
    /* header guard */
#ifndef CARBONSTEEL_ERROR_H
#define CARBONSTEEL_ERROR_H

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
 */
#define error_syntax(message, ...) logfe("syntax error: \n --- " message, ##__VA_ARGS__);

/**
 * Throws an internal error with
 * a specified message
 * 
 * @param[in] message The message string
 */
#define error_internal(message, ...) logfe("internal error: \n --- " message, ##__VA_ARGS__);

#endif /* CARBONSTEEL_ERROR_H */