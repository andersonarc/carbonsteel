/**
 * @file resolve.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Default type values for constants
 *  and type resolution for primitive expressions
 */
    /* header guard */
#ifndef AST_TYPE_DEFAULT_H
#define AST_TYPE_DEFAULT_H

    /* includes */
#include "ast/type/type.h" /* lexical type */
#include "syntax/expression/primitive.h" /* primitive expressions */

    /* functions */
/**
 * Primitive expression type resolution functions
 * 
 * ast_type_of_string    - intializes byte[]
 * ast_type_of_boolean   - intializes bool
 * ast_type_of_character - intializes byte
 * 
 * ast_type_of_ex_number - intializes the default numerical constant type
 *                          (float - float constant)
 *                          (int - integer constant)
 * 
 * ast_type_of_ex_constructor - intializes the expression type
 *                               (type + [] if is_array)
 *                               (type + *  if is_new)
 */
void ast_type_of_string(ast_type* this);
void ast_type_of_boolean(ast_type* this);
void ast_type_of_character(ast_type* this);
void ast_type_of_ex_number(ast_type* this, ex_number_kind kind);
void ast_type_of_ex_constructor(ast_type* this, ex_constructor* ex);


#endif /* AST_TYPE_RESOLVE_H */