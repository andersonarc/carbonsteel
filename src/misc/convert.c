/**
 * @file string.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-16
 * 
 *  Type conversion utilities
 */
    /* includes */
#include "misc/convert.h" /* this */

#include <math.h> /* math functions */
#include <stdio.h> /* print functions */

#include "misc/memory.h" /* memory allocation */

    /* internal functions */
/**
 * Returns the length of the string
 * representation of the given
 * size_t integer
 * 
 * @param[in] input The size_t integer
 * 
 * @return The length
 */
size_t size2length(size_t input) {
    if (input < 10) return 1;
    if (input < 100) return 2;
    if (input < 1000) return 3;
    return (size_t) floor(log10((double) input)) + 1;
}


    /* functions */
/**
 * Converts the size_t integer to
 * a string allocated with malloc
 * and returns it
 * 
 * @param[in] input The size_t integer
 * 
 * @return The converted string
 */
char* size2string(size_t input) {
    size_t length = size2length(input) + 1;
    char* buffer = allocate_array(char, length);
    snprintf(buffer, length, "%zu", input);
    return buffer;
}