/**
 * @file string.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-17
 * 
 *  Utility functions for working with strings
 */
    /* includes */
#include "misc/string.h"
#include "misc/memory.h"
#include <string.h>

/**
 * Concatenates two strings
 * 
 * @return The result string allocated by malloc
 */
char* cst_strconcat(const char* a, const char* b) {
    size_t length_a = strlen(a);
    size_t length_b = strlen(b);
    char* buffer = allocate_array(char, length_a + length_b + 1);
    strncpy(buffer, a, length_a);
    strncpy(buffer + length_a, b, length_b);
    buffer[length_a + length_b] = 0;
    return buffer;
}