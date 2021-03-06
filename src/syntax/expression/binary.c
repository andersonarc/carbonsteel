/**
 * @file binary.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.3
 * @date 2021-08-28
 * 
 *  Binary expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/binary.h" /* this */

#include "ast/type/check.h" /* type comparison */
#include "ast/type/resolve.h" /* default types */
#include "misc/error.h"       /* syntax errors */
#include "misc/memory.h"      /* memory allocation */


    /* internal functions */
static inline void ex_binary_check_number(op_binary operator, ast_type* type) {
    expect(ast_type_is_pp_number(type))
        otherwise("expected a number for a binary operation \"%s\", got \"%s\"",
            op_binary_strings[operator], 
            ast_type_to_string(type));
}


static inline void ex_binary_check_boolean(op_binary operator, ast_type* type) {
    expect(ast_type_is_pp_boolean(type))
        otherwise("expected a boolean for a binary operation \"%s\", got \"%s\"",
            op_binary_strings[operator],
            ast_type_to_string(type));
}


static inline void ex_binary_check_equal(op_binary operator, ast_type* a, ast_type* b) {
    expect(ast_type_can_merge(a, b))
        otherwise("expected equal types for a binary operation \"%s\", got \"%s\" and \"%s\"",
                    op_binary_strings[operator], 
                    ast_type_to_string(a), 
                    ast_type_to_string(b));
}


        /* inheritance */

    /* {DATA} BINARY <-< CAST */
iapi_init_from_parent(binary, cast) {
	this->value = *parent;
	this->has_operation = false;
}

    /* {DATA} BINARY < BINARY + OPERATOR */
iapi_binary_data() {
    if (!this->has_operation) {
        this->a = allocate(ex_binary_data);
        *this->a = *parent;

        this->has_operation = true;
    }

    this->b = allocate(ex_binary_data);
    *this->b = *other;

    /* add the operator */
    this->operator = operator;
}

        /* common inheritance functions */

    /* {PROPERTIES} BINARY < [NUMERICAL] BINARY + OPERATOR */
iapi_binary_properties(numerical) {
    parent_expect(number in binary expression) {
        ex_binary_check_number(operator, &parent->type);
    }
    parameter_expect(number in binary expression) {
        ex_binary_check_number(operator, &other->type);
    }

    ast_type* tmp;

    parent_and_parameter_expect(mergeable) {
        expect((tmp = ast_type_merge_extend(&parent->type, &other->type)) != NULL)
            otherwise("incompatible types for a binary operation \"%s\": \"%s\" and \"%s\" require an explicit cast because of integer truncation",
                        op_binary_strings[operator],
                        ast_type_to_string(&parent->type),
                        ast_type_to_string(&other->type));
    }

    type_assignment(merge) {
        this->type = *tmp;
    }
}
iapi_binary_properties_from_common(multiply,    numerical);
iapi_binary_properties_from_common(divide,      numerical);
iapi_binary_properties_from_common(modulo,      numerical);
iapi_binary_properties_from_common(add,         numerical);
iapi_binary_properties_from_common(subtract,    numerical);
iapi_binary_properties_from_common(shift_left,  numerical);
iapi_binary_properties_from_common(shift_right, numerical);
iapi_binary_properties_from_common(binary_and,  numerical);
iapi_binary_properties_from_common(binary_or,   numerical);
iapi_binary_properties_from_common(binary_xor,  numerical);


    /* {PROPERTIES} BINARY < [NUMBER_COMPARISON] BINARY + OPERATOR */
iapi_binary_properties(number_comparison) {
    parent_expect(number in binary expression) {
        ex_binary_check_number(operator, &parent->type);
    }
    parameter_expect(number in binary expression) {
        ex_binary_check_number(operator, &other->type);
    }

    type_assignment(boolean) {
        ast_type_of_boolean(&this->type);
    }
}
iapi_binary_properties_from_common(greater_than,  number_comparison);
iapi_binary_properties_from_common(greater_equal, number_comparison);
iapi_binary_properties_from_common(less_than,     number_comparison);
iapi_binary_properties_from_common(less_equal,    number_comparison);


    /* {PROPERTIES} BINARY < [COMPARISON] BINARY + OPERATOR */
iapi_binary_properties(comparison) {
    parent_and_parameter_expect(equal) {
        ex_binary_check_equal(operator, &parent->type, &other->type);
    }

    type_assignment(boolean) {
        ast_type_of_boolean(&this->type);
    }
}
iapi_binary_properties_from_common(equal,     comparison);
iapi_binary_properties_from_common(not_equal, comparison);


    /* {PROPERTIES} BINARY < [LOGIC] BINARY + OPERATOR */
iapi_binary_properties(logic) {
    parent_expect(boolean in binary expression) {
        ex_binary_check_boolean(operator, &parent->type);
    }
    parameter_expect(boolean in binary expression) {
        ex_binary_check_boolean(operator, &other->type);
    }

    type_assignment(boolean) {
        ast_type_of_boolean(&this->type);
    }
}
iapi_binary_properties_from_common(logic_and, logic);
iapi_binary_properties_from_common(logic_or,  logic);