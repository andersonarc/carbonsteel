/**
 * @file cast.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-29
 * 
 *  Cast expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/unary.h" /* unary expressions */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/statement/statement.h" /* statements */
#include "ast/type/resolve.h" /* type initialization */
#include "ast/type/check.h" /* type comparison */

    /* internal functions */
/**
 * Reinterprets a constant expression as a value of another type
 * which can be a primitive, a structure or an array
 * 
 * @param this The constant expression to initialize
 * @param parent The parent constant expression
 * @param type Type to which the expression is reinterpretated
 * 
 * @todo Support for primitive and structure casting
 */
#define _ex_constant_transform_case(this_kind_lower, parent_kind_upper, parent_kind_lower) \
    case EX_C_##parent_kind_upper:                                                         \
        this->u_##this_kind_lower = parent->u_##parent_kind_lower;                         \
        break;

#define _ex_constant_transform(kind_lower)                       \
    switch (parent->kind) {                                      \
       _ex_constant_transform_case(kind_lower, BOOLEAN, boolean) \
       _ex_constant_transform_case(kind_lower, BYTE, byte)       \
       _ex_constant_transform_case(kind_lower, SHORT, short)     \
       _ex_constant_transform_case(kind_lower, INT, int)         \
       _ex_constant_transform_case(kind_lower, LONG, long)       \
       _ex_constant_transform_case(kind_lower, UBYTE, ubyte)     \
       _ex_constant_transform_case(kind_lower, USHORT, ushort)   \
       _ex_constant_transform_case(kind_lower, UINT, uint)       \
       _ex_constant_transform_case(kind_lower, ULONG, ulong)     \
       _ex_constant_transform_case(kind_lower, FLOAT, float)     \
       _ex_constant_transform_case(kind_lower, DOUBLE, double)   \
       otherwise_error                                           \
    }

#define _ex_constant_primitive_to_kind(kind_upper, kind_lower) \
    case PRIMITIVE_INDEX_##kind_upper:                         \
        this->kind = EX_C_##kind_upper;                        \
        _ex_constant_transform(kind_lower);                    \
        break;

void ex_constant_reinterpret_cast(ex_constant* this, ex_constant* parent, ast_type* type) {
    if (ast_type_is_pp(type)) {
        switch (ast_type_primitive_get_index(type->u_primitive)) {
            _ex_constant_primitive_to_kind(BOOLEAN, boolean);
            _ex_constant_primitive_to_kind(BYTE,    byte);
            _ex_constant_primitive_to_kind(SHORT,   short);
            _ex_constant_primitive_to_kind(INT,     int);
            _ex_constant_primitive_to_kind(LONG,    long);
            _ex_constant_primitive_to_kind(UBYTE,   ubyte);
            _ex_constant_primitive_to_kind(USHORT,  ushort);
            _ex_constant_primitive_to_kind(UINT,    uint);
            _ex_constant_primitive_to_kind(ULONG,   ulong);
            _ex_constant_primitive_to_kind(FLOAT,   float);
            _ex_constant_primitive_to_kind(DOUBLE,  double);
            otherwise_error
        }
        //todo ast_type_is_structure
    } else {
        this->kind = EX_C_DYNAMIC;
    }
}

        /* inheritance */
 
    /* {DATA} CAST <-< UNARY */
iapi_init_from_parent(cast, unary) {
    this->value = *parent;
    arl_init(ast_type, this->cast_list);
}

    /* CAST < [CAST] TYPE */
data_self_inheritance_with(cast, cast, ast_type type) {
    *this = *parent;
    arl_add(ast_type, this->cast_list, type);
}
properties_self_inheritance_with(cast, cast, ast_type type) {
    iexpect_parameter(not void) {
        if (ast_type_is_pp(&type)) {
            expect(!ast_type_primitive_is_void(type.u_primitive))
                otherwise("cannot cast to a void type");
        }
    }
    iset_type(parameter) {
        this->type = type;
    }
    iset_constant(cast) {
        ex_constant_reinterpret_cast(&this->constant, &parent->constant, &type);
    }
}