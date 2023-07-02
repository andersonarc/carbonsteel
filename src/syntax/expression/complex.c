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
    iexpect_parameter(boolean) {
        expect(ast_type_is_pp_boolean(&condition->type))
            otherwise("conditional expression has non-boolean condition of type \"%s\"", 
                        ast_type_to_string(&condition->type));
    }

    //todo :: an error buffer for merge_extend function (accept and return plain values)?
    //todo change type API to plain values?

    ast_type* tmp;

    iexpect_parent_and_parameter(mergeable) {
        expect((tmp = ast_type_merge_extend(&if_branch->type, &parent->type)) != NULL)
            otherwise("conditional expression values are not equal: first value has type \"%s\", but second has type \"%s\"",
                        ast_type_to_string(&if_branch->type),
                        ast_type_to_string(&parent->type));
    }

    iset_type(merge) {
        this->type = *tmp;
    }

    iset_inherit_constant(conditional) irequire_constant()(condition) irequire_constant()(if_branch) {
        switch (condition->constant.kind) {
            case EX_C_BOOLEAN:
                if (condition->constant.u_boolean) {
                    this->constant = if_branch->constant; //todo is it correct?
                } else {
                    this->constant = parent->constant;
                }
                break;
            otherwise_error
        }
    }

    //todo
    iset_constant(stub) {
        ex_constant_dynamic(&this->constant);
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
    iexpect_parent_and_parameter(assignable) {
        expect(ast_type_can_assign(&assignee->type, &parent->type))
            otherwise("illegal assignment to type \"%s\" from type \"%s\"",
                            ast_type_to_string(&assignee->type), 
                            ast_type_to_string(&parent->type));
    }

    iexpect_parameter(number if not plain assignment) {
        if (operator != OP_A_PLAIN) {
            expect(ast_type_is_pp_number(&assignee->type))
                otherwise("assignment operator \"%s\" can only be applied to numbers, got type \"%s\"", 
                            op_assign_strings[operator], 
                            ast_type_to_string(&assignee->type));
        }
    }

    //todo support this for constant expressions
    iset_constant(stub) {
        ex_constant_dynamic(&this->constant);
    }
}