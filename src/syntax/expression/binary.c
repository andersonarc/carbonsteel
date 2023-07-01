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


static inline void ex_binary_check_integer(op_binary operator, ast_type* type) {
    expect(ast_type_is_pp_integer(type))
        otherwise("expected an integer for a binary operation \"%s\", got \"%s\"",
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
    iexpect_parent(number in binary expression) {
        ex_binary_check_number(operator, &parent->type);
    }
    iexpect_parameter(number in binary expression) {
        ex_binary_check_number(operator, &other->type);
    }

    ast_type* tmp;

    iexpect_parent_and_parameter(mergeable) {
        expect((tmp = ast_type_merge_extend(&parent->type, &other->type)) != NULL)
            otherwise("incompatible types for a binary operation \"%s\": \"%s\" and \"%s\" require an explicit cast because of integer truncation",
                        op_binary_strings[operator],
                        ast_type_to_string(&parent->type),
                        ast_type_to_string(&other->type));
    }

    iset_type(merge) {
        this->type = *tmp;
    }
}

iapi_binary_properties_from_common(multiply,    numerical, numerical);
iapi_binary_properties_from_common(divide,      numerical, numerical);
iapi_binary_properties(add) {
    ex_binary_inherit_properties_on_numerical(this, parent, other, operator);

    iset_constant(binary_add) {
        switch (parent->constant.kind) { case EX_C_BYTE: 
        switch (other->constant.kind) { 
            case EX_C_BYTE: this->constant.u_byte = (parent->constant.u_byte) + (other->constant.u_byte); break; 
            case EX_C_SHORT: this->constant.u_short = (parent->constant.u_byte) + (other->constant.u_short); break; 
            case EX_C_INT: this->constant.u_int = (parent->constant.u_byte) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_long = (parent->constant.u_byte) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_short = (parent->constant.u_byte) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_int = (parent->constant.u_byte) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_long = (parent->constant.u_byte) + (other->constant.u_uint); break;
 			case EX_C_FLOAT: this->constant.u_float = (parent->constant.u_byte) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_float = (parent->constant.u_byte) + (other->constant.u_double); break; otherwise_error } case EX_C_SHORT: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_short = (parent->constant.u_short) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_short = (parent->constant.u_short) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_int = (parent->constant.u_short) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_long = (parent->constant.u_short) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_short = (parent->constant.u_short) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_int = (parent->constant.u_short) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_long = (parent->constant.u_short) + (other->constant.u_uint); break;
 			case EX_C_FLOAT: this->constant.u_float = (parent->constant.u_short) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_float = (parent->constant.u_short) + (other->constant.u_double); break; otherwise_error } case EX_C_INT: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_int = (parent->constant.u_int) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_int = (parent->constant.u_int) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_int = (parent->constant.u_int) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_long = (parent->constant.u_int) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_int = (parent->constant.u_int) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_int = (parent->constant.u_int) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_long = (parent->constant.u_int) + (other->constant.u_uint); break;
 			case EX_C_FLOAT: this->constant.u_double = (parent->constant.u_int) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_double = (parent->constant.u_int) + (other->constant.u_double); break; otherwise_error } case EX_C_LONG: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_long = (parent->constant.u_long) + (other->constant.u_uint); break; otherwise_error } case EX_C_UBYTE: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_short = (parent->constant.u_ubyte) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_short = (parent->constant.u_ubyte) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_short = (parent->constant.u_ubyte) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_short = (parent->constant.u_ubyte) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_ubyte = (parent->constant.u_ubyte) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_ushort = (parent->constant.u_ubyte) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_uint = (parent->constant.u_ubyte) + (other->constant.u_uint); break;
 			case EX_C_ULONG: this->constant.u_ulong = (parent->constant.u_ubyte) + (other->constant.u_ulong); break;
 			case EX_C_FLOAT: this->constant.u_float = (parent->constant.u_ubyte) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_float = (parent->constant.u_ubyte) + (other->constant.u_double); break; otherwise_error } case EX_C_USHORT: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_int = (parent->constant.u_ushort) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_int = (parent->constant.u_ushort) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_int = (parent->constant.u_ushort) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_int = (parent->constant.u_ushort) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_ushort = (parent->constant.u_ushort) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_ushort = (parent->constant.u_ushort) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_uint = (parent->constant.u_ushort) + (other->constant.u_uint); break;
 			case EX_C_ULONG: this->constant.u_ulong = (parent->constant.u_ushort) + (other->constant.u_ulong); break;
 			case EX_C_FLOAT: this->constant.u_float = (parent->constant.u_ushort) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_float = (parent->constant.u_ushort) + (other->constant.u_double); break; otherwise_error } case EX_C_UINT: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_long = (parent->constant.u_uint) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_long = (parent->constant.u_uint) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_long = (parent->constant.u_uint) + (other->constant.u_int); break;
 			case EX_C_LONG: this->constant.u_long = (parent->constant.u_uint) + (other->constant.u_long); break;
 			case EX_C_UBYTE: this->constant.u_uint = (parent->constant.u_uint) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_uint = (parent->constant.u_uint) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_uint = (parent->constant.u_uint) + (other->constant.u_uint); break;
 			case EX_C_ULONG: this->constant.u_ulong = (parent->constant.u_uint) + (other->constant.u_ulong); break;
 			case EX_C_FLOAT: this->constant.u_double = (parent->constant.u_uint) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_double = (parent->constant.u_uint) + (other->constant.u_double); break; otherwise_error } case EX_C_ULONG: switch (other->constant.kind) { case EX_C_UBYTE: this->constant.u_ulong = (parent->constant.u_ulong) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_ulong = (parent->constant.u_ulong) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_ulong = (parent->constant.u_ulong) + (other->constant.u_uint); break;
 			//case EX_C_ULONG: this->constant.u_ulong = (parent->constant.u_ulong) + (other->constant.u_ulong); break; 
             otherwise_error 
             } case EX_C_FLOAT: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_float = (parent->constant.u_float) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_float = (parent->constant.u_float) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_double = (parent->constant.u_float) + (other->constant.u_int); break;
 			case EX_C_UBYTE: this->constant.u_float = (parent->constant.u_float) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_float = (parent->constant.u_float) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_double = (parent->constant.u_float) + (other->constant.u_uint); break;
 			case EX_C_FLOAT: this->constant.u_float = (parent->constant.u_float) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_double = (parent->constant.u_float) + (other->constant.u_double); break; otherwise_error } case EX_C_DOUBLE: switch (other->constant.kind) { case EX_C_BYTE: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_byte); break;
 			case EX_C_SHORT: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_short); break;
 			case EX_C_INT: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_int); break;
 			case EX_C_UBYTE: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_ubyte); break;
 			case EX_C_USHORT: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_ushort); break;
 			case EX_C_UINT: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_uint); break;
 			case EX_C_FLOAT: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_float); break;
 			case EX_C_DOUBLE: this->constant.u_double = (parent->constant.u_double) + (other->constant.u_double); break; otherwise_error } otherwise_error }
    }
}
iapi_binary_properties_from_common(subtract,    numerical, numerical);


    /* {PROPERTIES} BINARY < [INTEGER] BINARY + OPERATOR */
iapi_binary_properties(integer) {
    iexpect_parent(number in binary expression) {
        ex_binary_check_number(operator, &parent->type);
    }
    iexpect_parameter(integer in binary expression) {
        ex_binary_check_number(operator, &other->type);
    }

    ast_type* tmp;

    iexpect_parent_and_parameter(mergeable) {
        expect((tmp = ast_type_merge_extend(&parent->type, &other->type)) != NULL)
            otherwise("incompatible types for a binary operation \"%s\": \"%s\" and \"%s\" require an explicit cast because of integer truncation",
                        op_binary_strings[operator],
                        ast_type_to_string(&parent->type),
                        ast_type_to_string(&other->type));
    }

    iset_type(merge) {
        this->type = *tmp;
    }
}

iapi_binary_properties_from_common(modulo,      integer, integer);
iapi_binary_properties_from_common(shift_left,  integer, integer);
iapi_binary_properties_from_common(shift_right, integer, integer);
iapi_binary_properties_from_common(binary_and,  integer, integer);
iapi_binary_properties_from_common(binary_or,   integer, integer);
iapi_binary_properties_from_common(binary_xor,  integer, integer);


    /* {PROPERTIES} BINARY < [NUMBER_COMPARISON] BINARY + OPERATOR */
iapi_binary_properties(number_comparison) {
    iexpect_parent(number in binary expression) {
        ex_binary_check_number(operator, &parent->type);
    }
    iexpect_parameter(number in binary expression) {
        ex_binary_check_number(operator, &other->type);
    }

    iset_type(boolean) {
        ast_type_of_boolean(&this->type);
    }
}
iapi_binary_properties_from_common(greater_than,  number_comparison, numerical);
iapi_binary_properties_from_common(greater_equal, number_comparison, numerical);
iapi_binary_properties_from_common(less_than,     number_comparison, numerical);
iapi_binary_properties_from_common(less_equal,    number_comparison, numerical);


    /* {PROPERTIES} BINARY < [COMPARISON] BINARY + OPERATOR */
iapi_binary_properties(comparison) {
    iexpect_parent_and_parameter(equal) {
        ex_binary_check_equal(operator, &parent->type, &other->type);
    }

    iset_type(boolean) {
        ast_type_of_boolean(&this->type);
    }
}

iapi_binary_properties_from_common(equal, comparison, all);
iapi_binary_properties_from_common(not_equal, comparison, all);


    /* {PROPERTIES} BINARY < [LOGIC] BINARY + OPERATOR */
iapi_binary_properties(logic) {
    iexpect_parent(boolean in binary expression) {
        ex_binary_check_boolean(operator, &parent->type);
    }
    iexpect_parameter(boolean in binary expression) {
        ex_binary_check_boolean(operator, &other->type);
    }

    iset_type(boolean) {
        ast_type_of_boolean(&this->type);
    }
}
iapi_binary_properties_from_common(logic_and, logic, boolean);
iapi_binary_properties_from_common(logic_or,  logic, boolean);