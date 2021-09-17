/**
 * @file convert.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-16
 * 
 *  Type conversion utilities
 */
    /* header guard */
#ifndef CARBONSTEEL_MISC_CONVERT_H
#define CARBONSTEEL_MISC_CONVERT_H

    /* includes */
#include <stdbool.h> /* boolean */
#include <stdlib.h> /* size_t */

    /* functions */
/**
 * Converts a size_t integer to
 * a string allocated with malloc
 * and returns it
 * 
 * @param[in] input The size_t integer
 * 
 * @return The converted string
 */
char* size2string(size_t input);

#endif /* CARBONSTEEL_MISC_CONVERT_H */