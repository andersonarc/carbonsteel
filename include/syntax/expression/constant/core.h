/**
 * @file core.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-11-16
 * 
 *  Constant expression structure definition
 *  and core functions such as initialization, 
 *  printing, cloning and type checking
 * 
 *  Constant expression evaluation is used
 *  for compile-time casts and checks
 *  in expression inheritance.
 * 
 *  Pointer and array levels are not tracked
 *  and certain type checks are not done
 *  (e.g. number check) because the constant
 *  expression is tightly coupled with its
 *  lexical type and is mostly useless without it.
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_CORE_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_CORE_H

    /* includes */
#include "syntax/expression/constant/size.h" /* constant expression size */
#include "syntax/expression/constant/transform.h" /* constant expression transformations */
#include "misc/list.h" /* list utilities */
#include "ast/search.h" /* hash table */

#include <stdint.h> /* integer types */
#include <assert.h> /* assertions */

    /* typedefs */
/**
 * The constant expression structure definition
 * 
 * Each numerical type is stored in a separate field
 * so as to minimize the difference between constant
 * expression and runtime evaluation.
 */
enum ex_constant_kind {
    /* primitive values */
    EX_C_BOOLEAN,
    EX_C_CHAR, EX_C_BYTE, EX_C_SHORT, EX_C_INT, EX_C_LONG,
    EX_C_UCHAR, EX_C_UBYTE, EX_C_USHORT, EX_C_UINT, EX_C_ULONG,
    EX_C_FLOAT, EX_C_DOUBLE,
    
    /* containers of primitive values */
    EX_C_ARRAY, EX_C_STRUCTURE,

    /* non-constant value */
    EX_C_DYNAMIC
};

struct ex_constant {
    ex_constant_kind kind;
    union {
        /* / offset of the first union byte within the structure */
        char _union_offset;

        /* primitive values */
        uint8_t u_boolean;
        int8_t u_byte;
        int16_t u_short;
        int32_t u_int;
        int64_t u_long;
        uint8_t u_ubyte;
        uint16_t u_ushort;
        uint32_t u_uint;
        uint64_t u_ulong;
        float u_float;
        double u_double;

        /* containers of primitive values */
        list(ex_constant) u_array;
        struct hsearch_data u_structure;

        /* dynamic has no value */
    };
    ex_constant* origin; /* @todo nullable annotations */
};


    /* defines */
/**
 * Initializes a constant expression
 * with a kind and a value which is
 * asserted to be of suitable size
 * for the specified kind
 * 
 * @param[out] this  The constant expression
 * @param[in]  kind  Kind of the constant expression
 * @param[in]  value Value of the constant expression
 */
#define ex_constant_init(this, kind_upper, kind_lower, value)                                                                                    \
    static_assert(sizeof(value) == ex_constant_size_static(EX_C_##kind_upper),                                                                           \
                    "invalid constant expression value of size " macro_stringify(sizeof(value))                                                  \
                    " for kind " macro_stringify(EX_C_##kind_upper) " (expected size " macro_stringify(ex_c_union_size(EX_C_##kind_upper)) ")"); \
    (this).kind = EX_C_##kind_upper;                                                                                                            \
    (this).u_##kind_lower = value;                                                                                                              \
    (this).origin = NULL;

    /* functions */
/**
 * Initializes a dynamic constant expression
 * that has no value
 * 
 * @param[out] this The constant expression
 */
void ex_constant_dynamic(ex_constant* this);

/**
 * Initializes a constant expression
 * from an array of bytes while interpreting
 * each N bytes as a constant expression of
 * specified kind until the end of the array
 * 
 * @param[out] this   The constant expression
 * @param[in]  kind   Kind of the array elements
 * @param[in]  buffer Byte array to be interpreted as a list of constant expressions
 * @param[in]  size   Size of the array
 */
void ex_constant_from_array(ex_constant* this, ex_constant_kind kind, char* buffer, size_t size);

/**
 * Copies a constant expression value
 * and references the source in the origin field
 * 
 * @param[out] this The constant expression
 * @param[in]  origin The constant expression to be cloned
 */
void ex_constant_reference(ex_constant* this, ex_constant* origin);

/**
 * Copies a constant expression value
 * *without* referencing the source
 * 
 * @param[out] this The constant expression
 * @param[in]  source The constant expression to be cloned
 */
void ex_constant_clone(ex_constant* this, ex_constant* source);

/**
 * Prints a constant expression value
 * to the standard output stream
 * 
 * @param[in] this The constant expression
 */
void ex_constant_print(ex_constant* this);

/**
 * Constant expression type check functions
 * 
 *  ex_constant_is_number   - integer or a floating-point number
 *  ex_constant_is_integer  - integer number
 *  ex_constant_is_floating - floating-point number
 *  ex_constant_is_signed   - signed integer
 *  ex_constant_is_unsigned - unsigned integer
 * 
 * @param[in] this The constant expression
 */
bool ex_constant_is_number(ex_constant* this);
bool ex_constant_is_integer(ex_constant* this);
bool ex_constant_is_floating(ex_constant* this);
bool ex_constant_is_signed(ex_constant* this);
bool ex_constant_is_unsigned(ex_constant* this);


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_CORE_H */