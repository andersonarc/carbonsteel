/**
 * @file constructor.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-04
 * 
 *  Constructor expression inheritance schemes
 */
    /* includes */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

    /* inheritance */

/* todo */
/**
 * @todo variadic extern inheritance
 * from expression_list + type
 * from expression_list + expression + type
 * 
 * from SELF(plain)
 * from SELF(new)
 * 
 */

void ex_constructor_type_check_plain(ex_constructor* this) {
    switch (this->type->kind) {
        case AST_TYPE_ENUM:
            error_syntax("cannot construct an enum \"%s\"", ast_type_display_name(this->type));
            break;

        case AST_TYPE_GENERIC:
            error_syntax("cannot construct a generic type \"%s\"", ast_type_display_name(this->type));
            break;

        case AST_TYPE_FUNCTION:
            error_syntax("cannot construct a function \"%s\"", ast_type_display_name(this->type));
            break;

        case AST_TYPE_PRIMITIVE:
            expect(this->argument_list.size == 1)
                otherwise("invalid argument count for \"%s\" primitive type constructor: expected 1, got %zu",
                            ast_type_display_name(this->type), 
                            this->argument_list.size);

            ast_type* type = &this->argument_list.data[0]->properties->type;
            expect(ast_type_can_merge(this->type, type))
                otherwise("invalid argument for \"%s\" primitive type constructor: expected \"%s\", got \"%s\"",
                            ast_type_display_name(this->type), 
                            ast_type_display_name(this->type), 
                            ast_type_display_name(type));
            break;

        case AST_TYPE_STRUCTURE:
            dc_structure* structure = this->type->u_structure;
            expect(structure->member_list.size == this->argument_list.size)
                otherwise("invalid argument count for \"%s\" structure type constructor: expected %zu, got %zu",
                            ast_type_display_name(this->type), 
                            structure->member_list.size, 
                            this->argument_list.size);

            
            if (structure->_generic_impls.size != 0) {
                /* assign the implementation used */
                list(ast_type) impl = structure->_generic_impls.data[this->type->_generic_impl_index];
                dc_structure_generic_apply_impl(structure, impl);
            }

            iterate_array(i, this->argument_list.size) {
                dc_structure_member member = structure->member_list.data[i];
                ast_type* type = &this->argument_list.data[i]->properties->type;

                expect(ast_type_can_merge(type, &member.type))
                    otherwise("invalid constructor argument \"%s\": expected type \"%s\", got \"%s\"",
                                member.name, 
                                ast_type_display_name(&member.type), 
                                ast_type_display_name(type));
            }
            break;

        otherwise_error;
    }
}

void ex_constructor_type_check_pointer(ex_constructor* this) {
    expect(this->argument_list.size == 1)
        otherwise("invalid argument count for \"%s\" pointer type constructor: expected 1, got %zu",
                        ast_type_display_name(this->type), 
                        this->argument_list.size);

    ast_type* type = &this->argument_list.data[0]->properties->type;
    expect(ast_type_can_merge(this->type, type))
        otherwise("invalid argument for \"%s\" pointer type constructor: expected \"%s\", got \"%s\"",
                        ast_type_display_name(this->type), 
                        ast_type_display_name(this->type), 
                        ast_type_display_name(type));
}

void ex_constructor_type_check(ex_constructor* this) {
    if (ast_type_is_plain(this->type) && !this->is_array) {
        ex_constructor_type_check_plain(this);
    } else if (ast_type_is_pointer(this->type)) {
        ex_constructor_type_check_pointer(this);
    }

    /**
     * @todo 
     * Array constructor check:
     * - if not New, validate a constant expression size
     */
}