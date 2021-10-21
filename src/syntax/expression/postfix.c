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
    parameter_expect(number) {
        expect(ast_type_is_pp_number(&index->type))
            otherwise("expected a numerical index for [] operation, got type \"%s\"", 
                        ast_type_to_string(&index->type));
    }

    parent_expect(array) {
        expect(ast_type_is_array(&parent->type))
            otherwise("expected an array for [] operation, got type \"%s\"", 
                        ast_type_to_string(&parent->type));
    }

    type_assignment(assign pop) {
        this->type = parent->type;
        arl_pop(ast_type_level, this->type.level_list);
    }
}

    /* {PROPERTIES} POSTFIX < [INVOCATION] LIST<EXPRESSION> */
iapi_append_level(PL, postfix, invocation, INVOCATION, list(expression_ptr)) {
    parent_expect(function) {
        expect(parent->type.kind == AST_TYPE_FUNCTION)
            otherwise("expected a function for a () operation, got \"%s\"", 
                        ast_type_to_string(&parent->type));
    }

    dc_function* function = parent->type.u_function;
    list(dc_function_parameter) parameter_list = function->parameters.value;
    
    /* c-style variadic arguments (...) compatibility */
    parameter_expect(invocation argument count) {
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

    parameter_expect(invocation argument types) {
        iterate_array(i, function->parameters.value.size) {
            dc_function_parameter* parameter = &parameter_list.data[i];
            expression* argument = invocation.data[i];
            
            expect(ast_type_can_merge(&parameter->type, &argument->properties->type))
                otherwise("expected type \"%s\" for parameter \"%s\" of function \"%s\", got type \"%s\"",
                                ast_type_to_string(&parameter->type), 
                                parameter->name, 
                                function->name, 
                                ast_type_to_string(&argument->properties->type));
        }
    }

    type_assignment(clone) {
        ast_type_clone_to(&this->type, function->return_type);
    }
}

    /* POSTFIX < [PROPERTY] {STRING -> STRUCTURE MEMBER} */
self_inheritance_before(postfix, property, dc_structure_member*, char* property_name) {
    parent_expect(plain) {
        expect(ast_type_is_plain(&parent->type))
            otherwise("expected a plain structure for . operation, got type \"%s\"", 
                        ast_type_to_string(&parent->type));
    }

    parent_expect(structure) {
        expect(parent->type.kind == AST_TYPE_STRUCTURE)
            otherwise("expected a structure for . operation, got type \"%s\"", 
                        ast_type_to_string(&parent->type));
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
    type_assignment(clone) {
        ast_type_clone_to(&this->type, property->type);
    }
}

    /* POSTFIX < [POINTER PROPERTY] {STRING -> STRUCTURE MEMBER} */
self_inheritance_before(postfix, pointer_property, dc_structure_member*, char* property_name) {
    parent_expect(single pointer) {
        expect(ast_type_is_single_pointer(&parent->type))
            otherwise("expected a structure pointer for -> operation, got type \"%s\"", 
                        ast_type_to_string(&parent->type));
    }

    parent_expect(structure) {
        expect(parent->type.kind == AST_TYPE_STRUCTURE)
            otherwise("expected a structure pointer for -> operation, got type \"%s\"", 
                        ast_type_to_string(&parent->type));
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
    type_assignment(clone) {
        ast_type_clone_to(&this->type, pointer_property->type);
    }
}

    /* {PROPERTIES} POSTFIX < [INCREMENT] */
iapi_init_with_kind(P, postfix, increment, INCREMENT) {
    parent_expect(number) {
        expect(ast_type_is_pp_number(&parent->type))
                    otherwise("cannot increment a non-number of type \"%s\"", 
                        ast_type_to_string(&parent->type));
    }

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} POSTFIX < [DECREMENT] */
iapi_init_with_kind(P, postfix, decrement, DECREMENT) {
    expect(ast_type_is_pp_number(&parent->type))
            otherwise("cannot decrement a non-number of type \"%s\"", 
                ast_type_to_string(&parent->type));

    type_assignment(assign) {
        this->type = parent->type;
    }
}

    /* {PROPERTIES} POSTFIX < [PLAIN] */
iapi_init_with_kind(P, postfix, plain, PLAIN) {
    type_assignment(assign) { //todo inline type_assignment e.g. type_assignment(assign)(parent->type...)
        this->type = parent->type;
    }
}