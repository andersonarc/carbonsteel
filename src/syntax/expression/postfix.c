/**
 * @file postfix.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-10-02
 * 
 *  Postfix expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

        /* inheritance */

    /* {DATA} POSTFIX <-< BASIC */
iapi_init_from_parent(postfix, basic) {
    this->value = *parent;
    arl_init(ex_postfix_level, this->level_list);
}

    /* {PROPERTIES} POSTFIX < [INDEX] EXPRESSION */
iapi_append_level_from_expression(PL, postfix, index, INDEX, expression) {
    iexpect_parameter(number) {
        expect(ast_type_is_pp_number(&index->type))
            otherwise("expected a numerical index for [] operation, got type \"%s\"", 
                        ast_type_display_name(&index->type));
    }

    iexpect_parent(array) {
        expect(ast_type_is_array(&parent->type))
            otherwise("expected an array for [] operation, got type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    iset_type(assign pop) {
        this->type = parent->type;
        arl_pop(ast_type_level, this->type.level_list);
    }

    irequire_constant(unsigned)(index) {
        iset_inherit_constant(unsigned_assign) {
            ex_constant_unsigned_assign(this->constant, parent->constant.u_array.data[, ]);
        }
    }
}

    /* {PROPERTIES} POSTFIX < [INVOCATION] LIST<EXPRESSION> */
iapi_append_level(PL, postfix, invocation, INVOCATION, list(expression_ptr)) {
    iexpect_parent(function) {
        expect(parent->type.kind == AST_TYPE_FUNCTION)
            otherwise("expected a function for a () operation, got \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    dc_function* function = parent->type.u_function;
    list(dc_function_parameter) parameter_list = function->parameters.value;
    
    /* c-style variadic arguments (...) compatibility */
    iexpect_parameter(invocation argument count) {
        if (!function->parameters.is_c_vararg) {
            expect(invocation.size == parameter_list.size)
                otherwise("invalid argument count for a function \"%s\", expected %zu, got %zu", 
                                    function->name, 
                                    parameter_list.size, 
                                    invocation.size);
        } else {
            expect(invocation.size >= parameter_list.size)
                otherwise("invalid argument count for a function \"%s\", expected more than/exactly %zu, got %zu", 
                                    function->name, 
                                    parameter_list.size, 
                                    invocation.size);
        }
    }

    iexpect_parameter(invocation argument types) {
        iterate_array(i, function->parameters.value.size) {
            dc_function_parameter* parameter = &parameter_list.data[i];
            expression* argument = invocation.data[i];
            
            expect(ast_type_can_merge(&parameter->type, &argument->properties->type))
                otherwise("expected type \"%s\" for parameter \"%s\" of function \"%s\", got type \"%s\"",
                                ast_type_display_name(&parameter->type), 
                                parameter->name, 
                                function->name, 
                                ast_type_display_name(&argument->properties->type));
        }
    }

    iset_type(clone) {
        ast_type_clone_to(&this->type, function->return_type);
    }

    iset_constant(dynamic) {
        ex_constant_dynamic(&this->constant);
    }
}

    /* POSTFIX < [PROPERTY] {STRING -> STRUCTURE MEMBER} */
self_inheritance_before(postfix, property, dc_structure_member*, char* property_name) {
    iexpect_parent(plain) {
        expect(ast_type_is_plain(&parent->type))
            otherwise("expected a plain structure for . operation, got type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    iexpect_parent(structure) {
        expect(parent->type.kind == AST_TYPE_STRUCTURE)
            otherwise("expected a structure for . operation, got type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    arl_find_by_name(
        in_list(dc_structure_member, parent->type.u_structure->member_list),
        find_and_assign(property_name, (*value)),
        on_error("structure \"%s\" has no member \"%s\"",
                    parent->type.u_structure->name,
                    property_name)
    );
}
iapi_append_level(PL, postfix, property, PROPERTY, dc_structure_member*) {
    iset_type(clone) {
        ast_type_clone_to(&this->type, property->type);
    }
    //todo
    iset_constant(dynamic) {
        ex_constant_dynamic(&this->constant);
    }
}

    /* POSTFIX < [POINTER PROPERTY] {STRING -> STRUCTURE MEMBER} */
self_inheritance_before(postfix, pointer_property, dc_structure_member*, char* property_name) {
    iexpect_parent(single pointer) {
        expect(ast_type_is_single_pointer(&parent->type))
            otherwise("expected a structure pointer for -> operation, got type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    iexpect_parent(structure) {
        expect(parent->type.kind == AST_TYPE_STRUCTURE)
            otherwise("expected a structure pointer for -> operation, got type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    arl_find_by_name(
        in_list(dc_structure_member, parent->type.u_structure->member_list),
        find_and_assign(property_name, (*value)),
        on_error("structure \"%s\" has no member \"%s\"",
                    parent->type.u_structure->name,
                    property_name)
    );
}
iapi_append_level(PL, postfix, pointer_property, POINTER_PROPERTY, dc_structure_member*) {
    iset_type(clone) {
        ast_type_clone_to(&this->type, pointer_property->type);
    }
    iset_constant(dynamic) {
        ex_constant_dynamic(&this->constant);
    }
}

    /* {PROPERTIES} POSTFIX < [INCREMENT] */
iapi_init_with_kind(P, postfix, increment, INCREMENT) {
    iexpect_parent(number) {
        expect(ast_type_is_pp_number(&parent->type))
                    otherwise("cannot increment a non-number of type \"%s\"", 
                        ast_type_display_name(&parent->type));
    }

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
    iset_constant_origin(this, increment) {
        ex_constant_apply_postfix((*this->constant.origin), + 1);
        //todo is increment of the original value correct?
    }
}

    /* {PROPERTIES} POSTFIX < [DECREMENT] */
iapi_init_with_kind(P, postfix, decrement, DECREMENT) {
    expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot decrement a non-number of type \"%s\"", 
                ast_type_display_name(&parent->type));

    iset_type(assign) {
        this->type = parent->type;
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
    iset_constant_origin(this, increment) {
        ex_constant_apply_postfix((*this->constant.origin), - 1);
        //todo is increment of the original value correct?
    }
}

    /* {PROPERTIES} POSTFIX < [PLAIN] */
iapi_init_with_kind(P, postfix, plain, PLAIN) {
    iset_type(assign) { //todo inline iset_type e.g. iset_type(assign)(parent->type...)
        this->type = parent->type;
    }
    iset_constant(assign) {
        this->constant = parent->constant;
    }
}