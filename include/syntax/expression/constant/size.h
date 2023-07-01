/**
 * @file size.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-11-17
 * 
 *  Constant expression union size
 *  compile-time and runtime lookup functions
 */
    /* header guard */
#ifndef CARBONSTEEL_EXPRESSION_CONSTANT_SIZE_H
#define CARBONSTEEL_EXPRESSION_CONSTANT_SIZE_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */

    /* defines */
/**
 * Union size table that is used to determine
 * the size of a constant expression union
 * if its kind is known in either runtime or compile-time
 * 
 * The purpose of this functionality is to
 * be able to perform generic conversions
 * on constant expression values and
 * deserialize them from byte buffers.
 * 
 * @param[in] ckind Compile-time kind of the constant expression
 * @param[in] kind Kind of the constant expression
 */
#define ex_constant_size_static(ckind)  _##ckind##_UNION_SIZE
#define ex_constant_size_dynamic(kind) ex_constant_size_table[kind]

#define _EX_C_BOOLEAN_UNION_SIZE    1
#define _EX_C_BYTE_UNION_SIZE       1
#define _EX_C_SHORT_UNION_SIZE      2
#define _EX_C_INT_UNION_SIZE        4
#define _EX_C_LONG_UNION_SIZE       8
#define _EX_C_UBYTE_UNION_SIZE      1
#define _EX_C_USHORT_UNION_SIZE     2
#define _EX_C_UINT_UNION_SIZE       4
#define _EX_C_ULONG_UNION_SIZE      8
#define _EX_C_FLOAT_UNION_SIZE      4
#define _EX_C_DOUBLE_UNION_SIZE     8
#define _EX_C_ARRAY_UNION_SIZE      sizeof(list(ex_constant))
#define _EX_C_STRUCTURE_UNION_SIZE  sizeof(list(ex_constant))

    /* global variables */
/**
 * Union size table for constant expressions
 * with kind unknown at compile time
 */
extern size_t ex_constant_size_table[];

#endif /* CARBONSTEEL_EXPRESSION_CONSTANT_SIZE_H */