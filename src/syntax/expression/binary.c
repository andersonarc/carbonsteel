/**
 * @file binary.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-28
 * 
 *  Binary expression parser functions
 *  implementation file
 */
    /* includes */
#include "syntax/expression/binary.h" /* this */

#include "misc/error.h" /* syntax errors */
#include "ast/type.h" /* type functions */
#include "misc/memory.h" /* memory allocation */

    /* functions */

static inline void ex_binary_check_number(op_binary operator, ast_type* type) {
    if (!ast_type_is_number(type)) {
        error_syntax("expected a number for a binary operation \"%s\", got \"%s\"",
            op_binary_strings[operator], ast_type_to_string(type));
    }
}


static inline void ex_binary_check_boolean(op_binary operator, ast_type* type) {
    if (!ast_type_is_boolean(type)) {
        error_syntax("expected a boolean for a binary operation \"%s\", got \"%s\"",
            op_binary_strings[operator], ast_type_to_string(type));
    }
}


static inline void ex_binary_check_equal(op_binary operator, ast_type* a, ast_type* b) {
    if (ast_type_can_merge(a, b)) {
        error_syntax("expected equal types for a binary operation \"%s\", got \"%s\" and \"%s\"",
                    op_binary_strings[operator], ast_type_to_string(a), ast_type_to_string(b));
    }
}


void ex_binary_add(ex_binary* this, op_binary operator, ex_binary* operand) {
    if (!this->v.has_operation) {
        /* wrap the value as the first operand */
        ex_binary_inline tmp = this->v;
        this->v.a = allocate(ex_binary_inline);
        *this->v.a = tmp;
        this->v.has_operation = true;
    }

    /* wrap the operand */
    this->v.b = allocate(ex_binary_inline);
    *this->v.b = operand->v;

    /* add the operator */
    this->v.operator = operator;

    /* do type check */
    switch (operator) {
        case OP_B_MULTIPLY:
        case OP_B_DIVIDE:
        case OP_B_MODULO:
        case OP_B_ADD:
        case OP_B_SUBTRACT:
        case OP_B_SHIFT_LEFT:
        case OP_B_SHIFT_RIGHT:
        case OP_B_BINARY_AND:
        case OP_B_BINARY_OR:
        case OP_B_BINARY_XOR:
            /* first case: (number, number) => number */
            ex_binary_check_number(operator, this->type);
            ex_binary_check_number(operator, operand->type);
            /* type is still a number, no changes */
            break;

        case OP_B_GREATER_EQUAL:
        case OP_B_GREATER_THAN:
        case OP_B_LESS_EQUAL:
        case OP_B_LESS_THAN:
            /* second case: (number, number) => boolean */
            ex_binary_check_number(operator, this->type);
            ex_binary_check_number(operator, operand->type);
            this->type = ast_type_of_boolean();
            break;

        case OP_B_EQUAL:
        case OP_B_NOT_EQUAL:
            /* third case: (type_a, type_a) => boolean */
            ex_binary_check_equal(operator, this->type, operand->type);
            this->type = ast_type_of_boolean();
            break;

        case OP_B_LOGIC_AND:
        case OP_B_LOGIC_OR:
            /* fourth case: (boolean, boolean) => boolean */
            ex_binary_check_boolean(operator, this->type);
            ex_binary_check_boolean(operator, operand->type);
            this->type = ast_type_of_boolean();
            break;
        
        otherwise_error
    }
}