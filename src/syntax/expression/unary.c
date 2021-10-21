/**
 * @file unary.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.3
 * @date 2021-08-27
 * 
 *  Unary expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

        /* inheritance */
        
    /* {DATA} UNARY <-< POSTFIX */
iapi_init_from_parent(unary, postfix) {
    this->value = *parent;
    arl_init(op_unary, this->op_list);
}

    /* {PROPERTIES} UNARY < [REFERENCE] */
iapi_append_operator(U, unary, reference, REFERENCE) {
    type_assignment(assign pointer) {
        this->type = parent->type;
        ast_type_pointer_wrap(&this->type);
    }
}

    /* {PROPERTIES} UNARY < [DEREFERENCE] */
iapi_append_operator(U, unary, dereference, DEREFERENCE) {
    parent_expect(pointer) {
        expect(ast_type_is_pointer(&parent->type))
            otherwise("cannot dereference a non-pointer of type %s", 
                ast_type_to_string(&parent->type));
    }
    
    type_assignment(assign pop) {
        this->type = parent->type;
        arl_pop(ast_type_level, this->type.level_list);
    }
}

   /* {PROPERTIES} UNARY < [BINARY NOT] */
iapi_append_operator(U, unary, binary_not, BINARY_NOT) {
    parameter_expect(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot apply binary not to a non-number of type \"%s\"", 
                ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [LOGIC NOT] */
iapi_append_operator(U, unary, logic_not, LOGIC_NOT) {
    parameter_expect(boolean) {
        expect(ast_type_is_pp_boolean(&parent->type))
            otherwise("cannot apply logic not to a non-boolean of type \"%s\"", 
                ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [INCREMENT] */
iapi_init_with_kind(U, unary, increment, INCREMENT) {
    parent_expect(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot increment a non-number of type \"%s\"", 
                ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [DECREMENT] */
iapi_init_with_kind(U, unary, decrement, DECREMENT) {
    parent_expect(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot decrement a non-number of type \"%s\"", 
                ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [PLUS] */
iapi_init_with_kind(U, unary, plus, PLUS) {
    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [MINUS] */
iapi_init_with_kind(U, unary, minus, MINUS) {
    parent_expect(number) {
        expect(ast_type_is_pp_number(&parent->type))
                otherwise("cannot negate a non-number of type \"%s\"", 
                    ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}


    /* {PROPERTIES} UNARY < [PLAIN] */
iapi_init_with_kind(U, unary, plain, PLAIN) {
    type_assignment(assign) {
        this->type = parent->type;
    }
}