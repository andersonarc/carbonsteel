/**
 * @file cast.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-29
 * 
 *  Cast expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

        /* inheritance */
 
    /* {DATA} CAST <-< UNARY */
iapi_init_from_parent(cast, unary) {
    this->value = *parent;
    arl_init(ast_type, this->cast_list);
}

    /* CAST < [CAST] TYPE */
data_self_inheritance_with(cast, cast, ast_type type) {
    *this = *parent;
    arl_add(ast_type, this->cast_list, type);
}
properties_self_inheritance_with(cast, cast, ast_type type) {
    type_assignment(parameter) {
        this->type = type;
    }
}