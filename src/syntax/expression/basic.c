/**
 * @file basic.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-09-29
 * 
 *  Basic expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

        /* inheritance */

    /* {PROPERTIES} BASIC << VARIABLE */
iapi_init_union_from_extern(B, basic, dc_st_variable*, VARIABLE, variable) {
    type_assignment(clone) {
        ast_type_clone_to(&this->type, value->type);
    }
}


    /* {PROPERTIES} BASIC << FUNCTION */
iapi_init_union_from_extern(B, basic, dc_function*, FUNCTION, function) {
    type_assignment(init)
        ast_type_init(&this->type, AST_TYPE_FUNCTION, value);
}


    /* {PROPERTIES} BASIC << NUMBER */
iapi_init_union_from_extern(B, basic, ex_number, NUMBER, number) {
    type_assignment(resolve) {
        ast_type_of_ex_number(&this->type, value.kind);
    }
}


    /* {PROPERTIES} BASIC << BOOLEAN */
iapi_init_union_from_extern(B, basic, ex_boolean, BOOLEAN, boolean) {
    type_assignment(resolve) {
        ast_type_of_boolean(&this->type);
    }
}


    /* {PROPERTIES} BASIC << CHARACTER */
iapi_init_union_from_extern(B, basic, ex_character, CHARACTER, character) {
    type_assignment(resolve) {
        ast_type_of_character(&this->type);
    }
}


    /* {PROPERTIES} BASIC << ENUM MEMBER */
iapi_init_union_from_extern(B, basic, ex_enum_member*, ENUM_MEMBER, enum_member) {
    type_assignment(init) {
        ast_type_init(&this->type, AST_TYPE_ENUM, value->parent);
    }
}

    /* {PROPERTIES} BASIC << STRING */
iapi_init_union_from_extern(B, basic, char*, STRING, string)  {
    type_assignment(resolve) {
        ast_type_of_string(&this->type);
    }
}

    /* {PROPERTIES} << CONSTRUCTOR EXPRESSION */
iapi_init_union_from_extern(B, basic, ex_constructor*, EX_CONSTRUCTOR, ex_constructor) {
    type_assignment(resolve) {
        ast_type_of_ex_constructor(&this->type, value);
    }
}

    /* {PROPERTIES} BASIC << FUNCTON PARAMETER */
iapi_init_union_from_extern(B, basic, dc_function_parameter*, FUNCTION_PARAMETER, function_parameter) {
    type_assignment(clone) {
        ast_type_clone_to(&this->type, value->type);
    }
}

    /* {PROPERTIES} BASIC <-< EXPRESSION */
iapi_init_union_from_expression(B, basic, expression, EXPRESSION, expression) {
    /* no-op because properties are copied in expression inheritance */
}