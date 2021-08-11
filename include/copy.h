/**
 * @file copy.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-28
 * 
 *  Memory and string duplication functions
 */
    /* header guard */
#ifndef CARBONSTEEL_COPY_H
#define CARBONSTEEL_COPY_H

    /* includes */
#include <string.h> /* string functions */
#include <stdlib.h> /* memory allocation */
#include "error.h" /* error throw */

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
        error_internal("failed to duplicate a string");
    }
    return result;
}

/**
 * Duplicates a buffer or throws
 * an internal error on failure
 * 
 * @param[in] buffer The buffer
 * @param[in] size Size of the buffer in bytes
 * 
 * @return Copy of the buffer
 */
static inline void* copy_memory(void* buffer, size_t size) {
    void* result = malloc(size);
    if (result == NULL) {
        error_internal("failed to duplicate a memory buffer");
    }
    return memcpy(result, buffer, size);
}

    /* defines */
/**
 * Duplicates a structure or throws
 * an internal error on failure
 * 
 * @param[in] struct The structure (not a pointer)
 * 
 * @return Copy of the structure
 */
#define copy_structure(struct) copy_memory(&struct, sizeof(struct))

#endif /* CARBONSTEEL_COPY_H */