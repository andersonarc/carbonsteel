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
void ast_type_of_string(ast_type* this) {
    ast_type_of_character(this);
    ast_type_array_wrap(this);
}

void ast_type_of_character(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[2]);
}

void ast_type_of_boolean(ast_type* this) {
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[1]);
}

void ast_type_of_ex_number(ast_type* this, ex_number_kind kind) {
    int index;
    switch (kind) {
        case EX_N_FLOAT:
            index = 7;
            break;

        case EX_N_INT:
            index = 4;
            break;

        otherwise_error
    }
    ast_type_init(this, AST_TYPE_PRIMITIVE, &primitive_list.data[index]);
}

void ast_type_of_ex_constructor(ast_type* this, ex_constructor* ex) {
    ast_type_clone_to(this, *ex->type);
    if (ex->is_array) {
        ast_type_array_wrap(this);
    } else if (ex->is_new) {
        ast_type_pointer_wrap(this);
    }
}