/**
 * @file check.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Lexical type comparison and check functions
 */
    /* header guard */
#ifndef AST_TYPE_CHECK_H
#define AST_TYPE_CHECK_H

    /* includes */
#include "ast/type/type.h" /* lexical type */

    /* functions */
/**
 * Type check functions
 * 
 *  ast_type_is_array   - type is an array
 *  ast_type_is_pointer - type is a pointer
 *  ast_type_is_single_pointer - type is a single pointer
 *  ast_type_is_plain   - type is plain (not array or pointer)
 * 
 *  Plain and primitive types are referred as [pp]
 * 
 *  ast_type_is_pp        - type is [p]lain and [p]rimitive
 *  ast_type_is_pp_number - type is [pp] and a number
 *  ast_type_is_pp_bool   - type is [pp] and a boolean
 * 
 * @param[in] value Pointer to the type
 * 
 * @return true if the type matches the specified condition
 */
bool ast_type_is_array  (ast_type* value);
bool ast_type_is_pointer(ast_type* value);
bool ast_type_is_single_pointer(ast_type* value);
bool ast_type_is_plain  (ast_type* value);

bool ast_type_is_pp        (ast_type* value);
bool ast_type_is_pp_number (ast_type* value);
bool ast_type_is_pp_boolean(ast_type* value);


/**
 * Advanced type comparison functions
 * which support implicit number casts
 * 
 * Implicit number casts are performed
 * as described below:
 * 
 * When merge is [prioritized], the second type is possibly
 * !truncated! and implicitly casted to the first type
 * 
 * When merge is [extend], the smaller type is casted 
 * to the bigger type, so truncation is impossible
 * 
 * @todo Remove [prioritized] merges, use a workaround for constant expressions
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return(merge) Merge result, or NULL if types are incompatible
 * 
 * @return(can_merge) true if the types are compatible
 */
ast_type* ast_type_merge_prioritized(ast_type* a, ast_type* b);
ast_type* ast_type_merge_extend     (ast_type* a, ast_type* b);

bool ast_type_can_merge(ast_type* a, ast_type* b);


/**
 * Simple deep type comparison
 * without any implicit casting
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return true if the types are equal
 */
bool ast_type_is_equal(ast_type* a, ast_type* b);


#endif /* AST_TYPE_CHECK_H */