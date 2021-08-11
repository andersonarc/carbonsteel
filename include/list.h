/**
 * @file list.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-07-28
 * 
 *  Arraylist and list routine 
 *  operations with assertions
 */
    /* header guard */
#ifndef CARBONSTEEL_LIST_H
#define CARBONSTEEL_LIST_H

    /* includes */
#include "ctool/type/list.h" /* list */
#include "ctool/type/arraylist.h" /* arraylist */
#include "error.h" /* error throw */

    /* defines */
/**
 * Default arraylist init size
 */
#define ARRAYLIST_SIZE 2

/**
 * Asserts that a list is initialized
 * successfully with specified size
 * 
 * @param[in] type Type of the list
 * @param[in] list The list
 * @param[in] size Size of the list
 */
#define assert_list_init(type, list, size)   \
if (list_init(type)(&list, size) != ST_OK) { \
    error_internal("list " #list " of type " #type " with size " #size " failed to initialize"); \
}

/**
 * Asserts that a list is resized
 * successfully to specified size
 * 
 * @param[in] type     Type of the list
 * @param[in] list     The list
 * @param[in] new_size New size for the list
 */
#define assert_list_resize(type, list, new_size)   \
if (list_resize(type)(&list, new_size) != ST_OK) { \
    error_internal("list " #list " of type " #type " with size %zu failed to resize to new size " #new_size, list.size); \
}

/**
 * Asserts that an arraylist is not empty
 * 
 * @param[in] list The arraylist
 */
#define assert_arraylist_not_empty(list) \
if (arraylist_is_empty(list)) {          \
    error_internal(arraylist_to_string(type, list) " is empty, expected to be not empty", arraylist_to_string_format(list)); \
}

/**
 * Asserts that an arraylist is initialized
 * successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define assert_arraylist_init(type, list)   \
if (arraylist_init(type)(&list, ARRAYLIST_SIZE) != ST_OK) { \
    error_internal(arraylist_to_string(type, list) " failed to initialize", arraylist_to_string_format(list)); \
}

/**
 * Asserts that an arraylist is initialized
 * with empty size successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define assert_arraylist_init_empty(type, list)   \
if (arraylist_init(type)(&list, 0) != ST_OK) { \
    error_internal(arraylist_to_string(type, list) " failed to initialize", arraylist_to_string_format(list)); \
}

/**
 * Asserts that an arraylist is
 * trimmed successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define assert_arraylist_trim(type, list)   \
if (arraylist_trim(type)(&list) != ST_OK) { \
    error_internal("failed to trim " arraylist_to_string(type, list), arraylist_to_string_format(list)); \
}

/**
 * Asserts that an element is added
 * to an arraylist successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 * @param[in] element The element
 */
#define assert_arraylist_add(type, list, element)   \
if (arraylist_add(type)(&list, element) != ST_OK) { \
    error_internal("failed to add new element " #element " to " arraylist_to_string(type, list), arraylist_to_string_format(list)); \
}

/**
 * Asserts that an element is removed
 * from an arraylist successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 * @param[in] index Index of the element
 */
#define assert_arraylist_remove(type, list, index)   \
if (arraylist_remove(type)(&list, index) != ST_OK) { \
    error_internal("failed to remove element at index " #index " from " arraylist_to_string(type, list), arraylist_to_string_format(list)); \
}

/**
 * Asserts that an element is removed
 * from top of an arraylist successfully
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define assert_arraylist_pop(type, list)   \
if (arraylist_pop(type)(&list) != ST_OK) { \
    error_internal("failed to remove element from top of " arraylist_to_string(type, list), arraylist_to_string_format(list)); \
}

#endif /* CARBONSTEEL_LIST_H */