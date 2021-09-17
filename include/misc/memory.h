/**
 * @file memory.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-28
 * 
 *  Memory allocation and duplication functions
 */
    /* header guard */
#ifndef CARBONSTEEL_MISC_MEMORY_H
#define CARBONSTEEL_MISC_MEMORY_H

    /* includes */
#include <string.h> /* string functions */
#include <stdlib.h> /* memory allocation */

#include "misc/error.h" /* error throw */

    /* defines */
/**
 * Allocates memory for
 * a structure of specified type
 * and throws an internal error if
 * the allocation fails
 * 
 * @param[in] type The type
 */
#define allocate(type) checked_malloc(sizeof(type))

/**
 * Allocates memory for
 * an array of specified type
 * and throws an internal error if
 * the allocation fails
 * 
 * @param[in] type  The type
 * @param[in] count Number of elements
 */
#define allocate_array(type, count) checked_malloc(sizeof(type) * count)


    /* functions */
/**
 * Duplicates a string or throws 
 * an internal error on failure
 * 
 * @param[in] string The string
 * 
 * @return Copy of the string
 */
static inline char* copy_string(const char* string) {
    char* result = strdup(string);
    if (result == NULL) {
        error_internal("failed to duplicate a string \"%s\"", string);
    }
    return result;
}

/**
 * Allocates a memory region
 * of specified size and
 * throws an internal error if
 * the allocation fails
 * 
 * @param[in] size The size of memory region
 * 
 * @return The allocated memory region
 */
static inline void* checked_malloc(size_t size) {
    void* result = malloc(size);
    if (result == NULL) {
        error_internal("failed to allocated a memory region of size %zu bytes", size);
    }
    return result;
}

#endif /* CARBONSTEEL_MISC_MEMORY_H */