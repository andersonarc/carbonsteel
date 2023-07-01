/**
 * @file number.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-22
 * 
 *  Number expression inheritance schemes
 */
    /* includes */
#include "syntax/expression/primitive.h" /* primitive expressions */
#include "syntax/expression/properties.h" /* expression properties */
#include "ast/type/primitive.h" /* primitive lexical type */
#include "ast/type/type.h" /* lexical type */

#include <math.h> /* floating-point number operations */
#include <float.h> /* floating-point number constants */

    /* defines */
/**
 * Utility definition for assigning a primitive type
 * to the expression properties value and type
 * 
 * @param[in] primitive_lower Primitive number in lower case
 * @param[in] primitive_upper Primitive number in upper case
 */
#define _primitive_properties_of(primitive_lower, primitive_upper)      \
    this->constant.u_##primitive_lower = value;                         \
    this->constant.kind                = EX_C_##primitive_upper;        \
    primitive = &primitive_list.data[PRIMITIVE_INDEX_##primitive_upper];

    /* internal functions */
    /* @todo unsigned/signed support */
/**
 * Calculates the minimum size of an integer
 * container for a given long value and
 * initializes expression properties according to it
 * 
 * @param[out] this The expression properties
 * @param[in]  value The long value
 */
void expression_properties_of_long(expression_properties* this, long value) {
    ast_type_primitive* primitive;

    if (value <= __INT8_MAX__) {
        _primitive_properties_of(byte, BYTE);
    } else if (value <= __INT16_MAX__) {
        _primitive_properties_of(short, SHORT);
    } else if (value <= __INT32_MAX__) {
        _primitive_properties_of(int, INT);
    } else {
        _primitive_properties_of(long, LONG);
    }

    ast_type_init(&this->type, AST_TYPE_PRIMITIVE, primitive);
}

/**
 * Calculates the minimum size of an floating-point number
 * container for a given double value and
 * initializes expression properties according to it
 * 
 * @param[out] this The expression properties
 * @param[in]  value The double value
 */
void expression_properties_of_double(expression_properties* this, double value) {
    if (rint(value) == value) {
        expression_properties_of_long(this, (long) value);
    } else {
        ast_type_primitive* primitive;

        if (value <= __FLT_MAX__) {
            _primitive_properties_of(float, FLOAT);
        } else {
            _primitive_properties_of(double, DOUBLE);
        }

        ast_type_init(&this->type, AST_TYPE_PRIMITIVE, primitive);
    }
}

        /* inheritance */
    
    /* {PROPERTIES} NUMBER << LONG LONG */
iapi_init_union_from_extern(N, number, long long, LONG, long) {
    iset_type_and_constant(long number) {
        expression_properties_of_long(this, value);
    }
}

    /* {PROPERTIES} NUMBER << DOUBLE */
iapi_init_union_from_extern(N, number, double, DOUBLE, double) {
    iset_type_and_constant(double number) {
        expression_properties_of_double(this, value);
    }
}