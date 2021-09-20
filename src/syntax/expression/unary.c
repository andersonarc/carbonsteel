/**
 * @file unary.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-27
 * 
 *  Unary expression parser functions
 *  implementation file
 */
    /* includes */
#include "syntax/expression/unary.h" /* this */

#include <string.h> /* string functions */

#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/expression/complex.h" /* expressions */
#include "ast/type.h" /* type functions */

    /* functions */

void ex_constructor_type_check(ex_constructor* this) {
    /* check plain constructors */
    if (arraylist_is_empty(this->type->level_list) && !this->is_array) {
        /* no-enum restriction */
        if (this->type->kind == AST_TYPE_ENUM) {
            error_syntax("cannot construct an enum \"%s\"", ast_type_to_string(this->type))
        }

        /* no-function restriction */
        if (this->type->kind == AST_TYPE_FUNCTION) {
            error_syntax("cannot construct a function \"%s\"", ast_type_to_string(this->type))
        }

        /* check primitive constructor argument */
        if (this->type->kind == AST_TYPE_PRIMITIVE) {
            /* count check */
            if (this->argument_list.size != 1) {
                error_syntax("invalid argument count for \"%s\" primitive type constructor: expected 1, got %zu",
                                ast_type_to_string(this->type), this->argument_list.size);
            }

            /* type check */
            ast_type* type = this->argument_list.data[0]->type;
            if (!ast_type_can_merge(this->type, type)) {
                error_syntax("invalid argument for \"%s\" primitive type constructor: expected \"%s\", got \"%s\"",
                                ast_type_to_string(this->type), ast_type_to_string(this->type), ast_type_to_string(type));
            }
        }

        /* check structure constructor arguments */
        if (this->type->kind == AST_TYPE_STRUCTURE) {
            dc_structure* structure = this->type->u_structure;

            /* check count */
            if (structure->member_list.size != this->argument_list.size) {
                error_syntax("invalid argument count for \"%s\" structure type constructor: expected %zu, got %zu",
                                ast_type_to_string(this->type), structure->member_list.size, this->argument_list.size);
            }

            /* check argument types */
            iterate_array(i, this->argument_list.size) {
                dc_structure_member member = structure->member_list.data[i];
                ast_type* type = this->argument_list.data[i]->type;

                if (!ast_type_can_merge(type, &member.type)) {
                    error_syntax("invalid constructor argument \"%s\": expected type \"%s\", got \"%s\"",
                                    member.name, ast_type_to_string(&member.type), ast_type_to_string(type));
                }
            }
        }
    } else {
        /* pointer constructor validation */
        if (ast_type_last_level_is(this->type, AT_LEVEL_POINTER)) {
            /* count check */
            if (this->argument_list.size != 1) {
                error_syntax("invalid argument count for \"%s\" pointer type constructor: expected 1, got %zu",
                                ast_type_to_string(this->type), this->argument_list.size);
            }

            /* type check */
            ast_type* type = this->argument_list.data[0]->type;
            if (!ast_type_can_merge(this->type, type)) {
                error_syntax("invalid argument for \"%s\" pointer type constructor: expected \"%s\", got \"%s\"",
                                ast_type_to_string(this->type), ast_type_to_string(this->type), ast_type_to_string(type));
            }
        }

        /**
         * @todo 
         * Array constructor check:
         * - if not New, validate a constant expression size
         */
    }
}


void ex_postfix_type_check_index(ex_postfix* this) {
    ex_postfix_level* level = &arraylist_last(this->level_list);

    /* array restriction check */
    if (!ast_type_last_level_is(this->type, AT_LEVEL_ARRAY)) {
        error_syntax("expected an array for [] operation, got type \"%s\"", ast_type_to_string(this->type));
    }

    /* number restriction check */
    if (!ast_type_is_number(level->u_index->type)) {
        error_syntax("expected a numerical index for [] operation, got type \"%s\"", ast_type_to_string(level->u_index->type));
    }
}


void ex_postfix_type_check_invocation(ex_postfix* this) {
    expect(this->type->kind == AST_TYPE_FUNCTION)
        otherwise("expected a function for a () operation, got \"%s\"", ast_type_to_string(this->type));

    ex_postfix_level* level = &arraylist_last(this->level_list);

    dc_function* function = this->type->u_function;
    if (!function->parameters.is_c_vararg) {
        expect(level->u_argument_list.size == function->parameters.value.size)
            otherwise("invalid argument count for a function \"%s\", expected %zu, got %zu", 
                                function->name, 
                                function->parameters.value.size, 
                                level->u_argument_list.size);
    } else {
        expect(level->u_argument_list.size >= function->parameters.value.size)
            otherwise("invalid argument count for a function \"%s\", expected more than/exactly %zu, got %zu", 
                                function->name, 
                                function->parameters.value.size, 
                                level->u_argument_list.size);
    }

    iterate_array(i, function->parameters.value.size) {
        dc_function_parameter* parameter = &this->type->u_function->parameters.value.data[i];
        expression* argument = level->u_argument_list.data[i];
        
        expect(ast_type_can_merge(&parameter->type, argument->type))
            otherwise("expected type \"%s\" for parameter \"%s\" of function \"%s\", got type \"%s\"",
                            ast_type_to_string(&parameter->type), 
                            parameter->name, 
                            this->type->u_function->name, 
                            ast_type_to_string(argument->type));
    }
}
    

void ex_postfix_type_precheck_property(ex_postfix* this) {
    expect(arraylist_is_empty(this->type->level_list))
        otherwise("expected a structure for . operation, got array/pointer type \"%s\"", ast_type_to_string(this->type));

    expect(this->type->kind == AST_TYPE_STRUCTURE)
        otherwise("expected a structure for . operation, got type \"%s\"", ast_type_to_string(this->type));
}


void ex_postfix_type_precheck_pointer_property(ex_postfix* this) {
    expect(ast_type_last_level_is(this->type, AT_LEVEL_POINTER))
        otherwise("expected a pointer to a structure for -> operation, got type \"%s\"", ast_type_to_string(this->type));

    expect(this->type->kind == AST_TYPE_STRUCTURE)
        otherwise("expected a structure for -> operation, got type \"%s\"", ast_type_to_string(this->type));
}