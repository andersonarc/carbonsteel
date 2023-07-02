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
    iset_type(clone) {
        ast_type_clone_to(&this->type, value->type);
    }
    iset_constant(origin) {
        ex_constant_reference(&this->constant, &value->value.value->properties->constant);
    }//todo remove the origin-track system
}

    /* {PROPERTIES} BASIC << FUNCTION */
iapi_init_union_from_extern(B, basic, dc_function*, FUNCTION, function) {
    iset_type(init) {
        ast_type_init(&this->type, AST_TYPE_FUNCTION, value);
    }
    iset_constant(dynamic) {
        ex_constant_dynamic(&this->constant);
    }
}

    /* BASIC <-< NUMBER */
iapi_init_union_from_dereferenced_expression(B, basic, number, NUMBER, number);

    /* {PROPERTIES} BASIC << BOOLEAN */
iapi_init_union_from_extern(B, basic, bool, BOOLEAN, boolean) {
    iset_type(resolve) {
        ast_type_of_boolean(&this->type);
    }
    iset_constant(resolve) {
        ex_constant_init(this->constant, BOOLEAN, boolean, value);
    }
}

    /* {PROPERTIES} BASIC << CHARACTER */
iapi_init_union_from_extern(B, basic, ex_character, CHARACTER, character) {
    iset_type(resolve) {
        ast_type_of_character(&this->type);
    }
    iset_constant(resolve) {
        ex_constant_init(this->constant, BYTE, byte, value);
    }
}

    /* {PROPERTIES} BASIC << ENUM MEMBER */
iapi_init_union_from_extern(B, basic, ex_enum_member*, ENUM_MEMBER, enum_member) {
    iset_type(init) {
        ast_type_init(&this->type, AST_TYPE_ENUM, value->parent);
    }
    iset_constant(clone) {
        ex_constant_clone(&this->constant, &value->value);
    }
}

    /* {PROPERTIES} BASIC << STRING */
iapi_init_union_from_extern(B, basic, char*, STRING, string)  {
    iset_type(resolve) {
        ast_type_of_string(&this->type);
    }
    iset_constant(array) {
        ex_constant_from_array(&this->constant, EX_C_BYTE, value, strlen(value));
    }
}

    /* {PROPERTIES} << CONSTRUCTOR EXPRESSION */
iapi_init_union_from_extern(B, basic, ex_constructor*, EX_CONSTRUCTOR, ex_constructor) {
    iset_type(resolve) {
        ast_type_of_ex_constructor(&this->type, value);
    }
    iset_constant(dynamic) {
        //todo resolve
        ex_constant_dynamic(&this->constant);
    }
}

    /* {PROPERTIES} BASIC << FUNCTION PARAMETER */
iapi_init_union_from_extern(B, basic, dc_function_parameter*, FUNCTION_PARAMETER, function_parameter) {
    iset_type(clone) {
        ast_type_clone_to(&this->type, value->type);
    }
    iset_constant(dynamic) {
        ex_constant_dynamic(&this->constant);
    }
}

    /* BASIC <-< EXPRESSION */
iapi_init_union_from_expression(B, basic, expression, EXPRESSION, expression);