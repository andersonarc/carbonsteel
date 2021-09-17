/**
 * @file list.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-28
 * 
 *  Arraylist and list routine 
 *  operations with assertions
 */
    /* header guard */
#ifndef CARBONSTEEL_MISC_LIST_H
#define CARBONSTEEL_MISC_LIST_H

    /* includes */
#include <string.h> /* string functions */

#include "ctool/type/arraylist.h" /* arraylist */
#include "ctool/type/list.h" /* list */
#include "misc/error.h" /* error throw */

    /* defines */
/**
 * Default arraylist initialization size
 */
#define ARRAYLIST_DEFAULT_SIZE 2


/**
 * Initializes a list with specified size
 * 
 * @param[in]  type Type of the list
 * @param[out] list The list
 * @param[in]  size Size of the list
 */
#define li_init(type, list, size)                                                                       \
    if (list_init(type)(&list, size) != ST_OK) {                                                        \
        error_internal("list " #list " of type " #type " with size " #size " failed to initialize");    \
    }


/**
 * Initializes an empty list
 * 
 * @param[in]  type Type of the list
 * @param[out] list The list
 */
#define li_init_empty(type, list) li_init(type, list, 0)


/**
 * Initializes a list from an arraylist
 * 
 * @param[in]  type Type of the list and arraylist
 * @param[out] list The list
 * @param[in]  arraylsit The arraylist
 */
#define li_init_from(type, list, arraylist)                                                                             \
    if (arraylist_to_list(type)(&arraylist, &list) != ST_OK) {                                                          \
        error_internal("unable to convert list " #list " of type " #type " with size %zu to an arraylist", list.size);  \
    }


/**
 * Initializes an arraylist with specified size
 * 
 * @param[in]  type Type of the arraylist
 * @param[out] list The arraylist
 * @param[in]  size Size of the arraylist
 */
#define arl_init_with_size(type, list, size)                                                                        \
    if (arraylist_init(type)(&list, size) != ST_OK) {                                                               \
        error_internal(arraylist_to_string(type, list) " failed to initialize", arraylist_to_string_format(list));  \
    }
    

/**
 * Initializes an arraylist with default size
 * 
 * @param[in]  type Type of the arraylist
 * @param[out] list The arraylist
 */
#define arl_init(type, list) arl_init_with_size(type, list, ARRAYLIST_DEFAULT_SIZE)


/**
 * Initializes an empty arraylist
 * 
 * @param[in]  type Type of the arraylist
 * @param[out] list The arraylist
 */
#define arl_init_empty(type, list) arl_init_with_size(type, list, 0)


/**
 * Trims an arraylist
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define arl_trim(type, list)                                                                                    \
    if (arraylist_trim(type)(&list) != ST_OK) {                                                                 \
        error_internal("failed to trim " arraylist_to_string(type, list), arraylist_to_string_format(list));    \
    }


/**
 * Adds an element to an arraylist
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 * @param[in] element The element
 */
#define arl_add(type, list, element)                                                                                                    \
    if (arraylist_add(type)(&list, element) != ST_OK) {                                                                                 \
        error_internal("failed to add new element " #element " to " arraylist_to_string(type, list), arraylist_to_string_format(list)); \
    }


/**
 * Removes an element from top of an arraylist
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 */
#define arl_pop(type, list)                                                                                                         \
    if (arraylist_pop(type)(&list) != ST_OK) {                                                                                      \
        error_internal("failed to remove element from top of " arraylist_to_string(type, list), arraylist_to_string_format(list));  \
    }


/**
 * Initializes an arraylist with default
 * size and adds a new element
 * 
 * @param[in] type Type of the arraylist
 * @param[in] list The arraylist
 * @param[in] element The element to append
 */
#define arl_init_add(type, list, element) arl_init(type, list); arl_add(type, list, element)


/**
 * Assigns an initialized arraylist
 * to specified variable and adds a new element
 * 
 * @param[in]  type Type of the arraylist
 * @param[in]  list The arraylist
 * @param[in]  element The element to append
 * @param[out] variable The variable to be assigned
 */
#define arl_assign_add(type, list, element, variable) variable = list; arl_add(type, list, element)


/**
 * Finds a first arraylist element
 * that matches a given name and then
 * assigns it to the specified variable
 * or throws the specified syntax error error
 * 
 * @param[in]  type Type of the arraylist
 * @param[in]  list The arraylist
 * @param[in]  expected_name Name of the element to be found
 * @param[out] result The variable to be assigned
 * @param[in]  error  Error message to print on failure
 */
#define in_list(type, list)                    type, list
#define find_and_assign(expected_name, result) expected_name, result 
#define on_error(error, ...)                   (error, __VA_ARGS__)

#define arl_find_by_name( type__list, expected_name__result, error) \
        arl_find_by_name_(type__list, expected_name__result, error)

#define arl_find_by_name_(type, list, expected_name, result, error) \
    result = NULL;                                                  \
    iterate_array(i, list.size) {                                   \
        if (strcmp(list.data[i].name, expected_name) == 0) {        \
            result = &list.data[i];                                 \
            break;                                                  \
        }                                                           \
    }                                                               \
    if (result == NULL) {                                           \
        error_syntax error;                                         \
    }

#endif /* CARBONSTEEL_MISC_LIST_H */