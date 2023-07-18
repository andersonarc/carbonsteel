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
    iset_type(assign pointer) {
        this->type = parent->type;
        ast_type_pointer_wrap(&this->type);
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
}

    /* {PROPERTIES} UNARY < [DEREFERENCE] */
iapi_append_operator(U, unary, dereference, DEREFERENCE) {
    iexpect_parent(pointer) {
        expect(ast_type_is_pointer(&parent->type))
            otherwise("cannot dereference a non-pointer of type %s", 
                ast_type_display_name(&parent->type));
    }
    
    iset_type(assign pop) {
        this->type = parent->type;
        arl_pop(ast_type_level, this->type.level_list);
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
}

   /* {PROPERTIES} UNARY < [BINARY NOT] */
iapi_append_operator(U, unary, binary_not, BINARY_NOT) {
    iexpect_parameter(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot apply binary not to a non-number of type \"%s\"", 
                ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} UNARY < [LOGIC NOT] */
iapi_append_operator(U, unary, logic_not, LOGIC_NOT) {
    iexpect_parameter(boolean) {
        expect(ast_type_is_pp_boolean(&parent->type))
            otherwise("cannot apply logic not to a non-boolean of type \"%s\"", 
                ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_inherit_constant(logic not) {
        ex_constant_init(this->constant, BOOLEAN, boolean, ((uint8_t) !parent->constant.u_boolean));
    }
}

    /* {PROPERTIES} UNARY < [INCREMENT] */
iapi_init_with_kind(U, unary, increment, INCREMENT) {
    iexpect_parent(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot increment a non-number of type \"%s\"", 
                ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_inherit_constant(increment) {
        ex_constant_inherit_prefix(this->constant, parent->constant, ++);
    }
    iset_constant_origin(this, increment) {
        ex_constant_apply_postfix((*this->constant.origin), + 1);
    }
}

    /* {PROPERTIES} UNARY < [DECREMENT] */
iapi_init_with_kind(U, unary, decrement, DECREMENT) {
    iexpect_parent(number) {
        expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot decrement a non-number of type \"%s\"", 
                ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_inherit_constant(increment) {
        ex_constant_inherit_prefix(this->constant, parent->constant, --);
    }
    iset_constant_origin(this, increment) {
        ex_constant_apply_postfix((*this->constant.origin), - 1);
    }
}

    /* {PROPERTIES} UNARY < [PLUS] */
iapi_init_with_kind(U, unary, plus, PLUS) {
    iset_type(assign) {
        this->type = parent->type;
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
}

    /* {PROPERTIES} UNARY < [MINUS] */
iapi_init_with_kind(U, unary, minus, MINUS) {
    iexpect_parent(number) {
        expect(ast_type_is_pp_number(&parent->type))
                otherwise("cannot negate a non-number of type \"%s\"", 
                    ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_inherit_constant(negate) {
        ex_constant_inherit_prefix(this->constant, parent->constant, -);
    }
}


    /* {PROPERTIES} UNARY < [PLAIN] */
iapi_init_with_kind(U, unary, plain, PLAIN) {
    iset_type(assign) {
        this->type = parent->type;
    }
    iset_constant(assign) {
       this->constant = parent->constant;
    }
}