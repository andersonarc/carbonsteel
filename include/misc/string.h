/**
 * @file string.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-17
 * 
 *  Utility functions for working with strings
 */
    /* header guard*/
#ifndef CARBONSTEEL_MISC_STRING_H
#define CARBONSTEEL_MISC_STRING_H

    /* functions */
/**
 * Concatenates two strings
 * 
 * @return The result string allocated by malloc
 */
char* cst_strconcat(const char* a, const char* b);

#endif /* CARBONSTEEL_MISC_STRING_H */