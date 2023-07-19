/**
 * @file resolve.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-09-20
 * 
 * 
 */
    /* includes */
#include "ast/type/resolve.h" /* this */

#include "ast/type/primitive.h" /* primitive type */
#include "syntax/expression/unary.h" /* unary expressions */

    /* functions */
/**
 * Primitive expression type resolution functions
 * 
 * ast_type_of_string    - initializes byte[]
 * ast_type_of_boolean   - initializes bool
 * ast_type_of_character - initializes byte
 * 
 * ast_type_of_signed_number   - \/
 * ast_type_of_unsigned_number - \/
 * ast_type_of_float_number    - initializes with the smallest available
 *                              numerical type without precision loss
 *                              
 * 
 * ast_type_of_ex_constructor - initializes the expression type
 *                               (type + [] if is_array)
 *                               (type + *  if is_new)
 */
void ast_type_of_string(ast_type* this) {
    ast_type_of_character(this);
    ast_type_array_wrap(this);
}

void ast_type_of_void(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_VOID]);
}

void ast_type_of_any(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_ANY]);
}

void ast_type_of_character(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_CHAR]);
}

void ast_type_of_boolean(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_BOOLEAN]);
}

void ast_type_of_float_number(ast_type* this, double value) {
    //todo
}

void ast_type_of_ex_constructor(ast_type* this, ex_constructor* ex) {
    ast_type_clone_to(this, *ex->type);
    if (ex->is_array) {
        ast_type_array_wrap(this);
    } else if (ex->is_new) {
        ast_type_pointer_wrap(this);
    }
}