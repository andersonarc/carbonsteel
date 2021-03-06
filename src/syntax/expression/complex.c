/**
 * @file complex.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-10-14
 * 
 *  Complex expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/complex.h" /* this */

#include "ast/type/check.h" /* type comparison */
#include "ast/type/resolve.h" /* default types */
#include "misc/error.h"       /* syntax errors */
#include "misc/memory.h"      /* memory allocation */

        /* inheritance */

    /* {DATA} CONDITION <-< BINARY */
iapi_init_from_parent(condition, binary) {
    this->value = *parent;
    this->has_condition = false;
}

    /* CONDITION < [CONDITION] BINARY EXPRESSION + EXPRESSION */
data_self_inheritance_with_ex_and_ex(condition, condition, binary, condition, expression, if_branch) {
    this->value = *condition;
    this->u_if = if_branch;
    this->u_else = allocate(ex_condition);
    *this->u_else = *parent;
    this->has_condition = true;
}
properties_self_inheritance_with_ex_and_ex(condition, condition, condition, if_branch) {
    parameter_expect(boolean) {
        expect(ast_type_is_pp_boolean(&condition->type))
            otherwise("conditional expression has non-boolean condition of type \"%s\"", 
                        ast_type_to_string(&condition->type));
    }

    //todo :: an error buffer for merge_extend function (accept and return plain values)?
    //todo change type API to plain values?

    ast_type* tmp;

    parent_and_parameter_expect(mergeable) {
        expect((tmp = ast_type_merge_extend(&if_branch->type, &parent->type)) != NULL)
            otherwise("conditional expression values are not equal: first value has type \"%s\", but second has type \"%s\"",
                        ast_type_to_string(&if_branch->type),
                        ast_type_to_string(&parent->type));
    }

    type_assignment(merge) {
        this->type = *tmp;
    }
}

    /* {DATA} COMPLEX <-< CONDITION */
iapi_init_from_parent(expression, condition) {
    this->u_value = *parent;
    this->has_assignment = false;
}

    /* COMPLEX < [ASSIGNMENT] UNARY EXPRESSION + ASSIGNMENT OPERATOR */
data_self_inheritance_with_ex_and(expression, assignment, unary, assignee, op_assign operator) {
    this->u_assignment.assignee = *assignee;
    this->u_assignment.operator = operator;
    this->u_assignment.value = allocate(expression_data);
    *this->u_assignment.value = *parent;
    this->has_assignment = true;
}
properties_self_inheritance_with_ex_and(expression, assignment, assignee, op_assign operator) {
    ast_type* tmp;

    parent_and_parameter_expect(mergeable) {
        expect((tmp = ast_type_merge_prioritized(&assignee->type, &parent->type)) != NULL)
            otherwise("illegal assignment to type \"%s\" from type \"%s\"",
                            ast_type_to_string(&assignee->type), 
                            ast_type_to_string(&parent->type));
    }

    parameter_expect(number if not plain assignment) {
        if (operator != OP_A_PLAIN) {
            expect(ast_type_is_pp_number(&assignee->type))
                otherwise("assignment operator \"%s\" can only be applied to numbers, got type \"%s\"", 
                            op_assign_strings[operator], 
                            ast_type_to_string(&assignee->type));
        }
    }
}