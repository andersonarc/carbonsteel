/**
 * @file primitive.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Primitive type list storage variable
 *  and global initialization function
 * 
 *  Primitive types' order is constant
 *  and should not be changed, because many functions 
 *  (e.g. type detection/number casting) depend on it.
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_TYPE_PRIMITIVE_H
#define CARBONSTEEL_AST_TYPE_PRIMITIVE_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */

    /* defines */
/**
 * Primitive indices for primitive types
 * 
 * Primitive types must be stored in specific
 * order because certain type checks depend on it.
 * 
 * @param[in] index The primitive type index
 * @param[in] range Range name (NUMBER, INTEGER, SIGNED, UNSIGNED, FLOATING)
 */
#define primitive_index_min_range(range) (_PRIMITIVE_INDEX_MIN_##range)
#define primitive_index_max_range(range) (_PRIMITIVE_INDEX_MAX_##range)
#define primitive_index_in_range(index, range)   (((index) >= _PRIMITIVE_INDEX_MIN_##range) \
                                                && ((index) <= _PRIMITIVE_INDEX_MAX_##range))
#define primitive_type_in_range(value, range)   (((value - primitive_list.data) >= _PRIMITIVE_INDEX_MIN_##range) \
                                                && ((value - primitive_list.data) <= _PRIMITIVE_INDEX_MAX_##range))

#define primitive_signed_to_unsigned(value) (value + (_PRIMITIVE_INDEX_MIN_UNSIGNED - _PRIMITIVE_INDEX_MIN_SIGNED))

#define PRIMITIVE_INDEX_VOID   0
#define PRIMITIVE_INDEX_BOOLEAN   1

#define _PRIMITIVE_INDEX_MIN_NUMBER  PRIMITIVE_INDEX_CHAR
    #define _PRIMITIVE_INDEX_MIN_INTEGER PRIMITIVE_INDEX_CHAR
        #define _PRIMITIVE_INDEX_MIN_SIGNED  PRIMITIVE_INDEX_CHAR
            #define PRIMITIVE_INDEX_CHAR   2
            #define PRIMITIVE_INDEX_BYTE   3
            #define PRIMITIVE_INDEX_SHORT  4
            #define PRIMITIVE_INDEX_INT    5
            #define PRIMITIVE_INDEX_LONG   6
        #define _PRIMITIVE_INDEX_MAX_SIGNED   PRIMITIVE_INDEX_LONG

        #define _PRIMITIVE_INDEX_MIN_UNSIGNED PRIMITIVE_INDEX_UCHAR
            #define PRIMITIVE_INDEX_UCHAR  7
            #define PRIMITIVE_INDEX_UBYTE  8
            #define PRIMITIVE_INDEX_USHORT 9
            #define PRIMITIVE_INDEX_UINT   10
            #define PRIMITIVE_INDEX_ULONG  11
        #define _PRIMITIVE_INDEX_MAX_UNSIGNED PRIMITIVE_INDEX_ULONG
    #define _PRIMITIVE_INDEX_MAX_INTEGER  PRIMITIVE_INDEX_ULONG

    #define _PRIMITIVE_INDEX_MIN_FLOATING PRIMITIVE_INDEX_FLOAT
        #define PRIMITIVE_INDEX_FLOAT  12
        #define PRIMITIVE_INDEX_DOUBLE 13
    #define _PRIMITIVE_INDEX_MAX_FLOATING PRIMITIVE_INDEX_DOUBLE

    #define PRIMITIVE_INDEX_ANY 14
#define _PRIMITIVE_INDEX_MAX_NUMBER   PRIMITIVE_INDEX_ANY

#define _PRIMITIVE_INDEX_MAX          PRIMITIVE_INDEX_ANY

    /* typedefs */
/**
 * Primitive lexical type, either a
 * number, a boolean value or a void type
 * 
 * Code name is the actual name of the
 * type which is used solely for code
 * generation.
 * 
 * Size is the size of the primitive type
 * in bytes.
 */
struct ast_type_primitive {
    char* name;
    char* code_name;
    size_t size;
    unsigned long long capacity;
    bool is_allowed_in_native;
};

    /* global variables */
/**
 * Primitive type list
 */
extern list(ast_type_primitive) primitive_list;

    /* functions */
/**
 * Allocates and fills the global primitive type list
 * with predefined type structures
 */
void primitive_list_init();

/**
 * Primitive type index calculation function
 *
 * @param[in] value Pointer to the primitive type
 * 
 * @return Index of the primitive type 
 */
index_t ast_type_primitive_get_index(ast_type_primitive* value);

/**
 * Primitive type check functions
 * 
 *  ast_type_primitive_is_void    - type is void
 *  ast_type_primitive_is_boolean - type is boolean
 * 
 *  ast_type_primitive_is_number - type is any number
 *  |
 *  | - ast_type_primitive_is_integer - type is an integer with any sign
 *  |   | - ast_type_primitive_is_signed   - type is a signed integer
 *  |   | - ast_type_primitive_is_unsigned - type is an unsigned integer
 *  |
 *  | - ast_type_primitive_is_floating - type is a floating-point number
 * 
 * @param[in] value Pointer to the primitive type
 */
bool ast_type_primitive_is_void(ast_type_primitive* value);

bool ast_type_primitive_is_boolean(ast_type_primitive* value);

bool ast_type_primitive_is_number(ast_type_primitive* value);

bool ast_type_primitive_is_integer (ast_type_primitive* value);
bool ast_type_primitive_is_signed  (ast_type_primitive* value);
bool ast_type_primitive_is_unsigned(ast_type_primitive* value);

bool ast_type_primitive_is_floating(ast_type_primitive* value);


#endif /* CARBONSTEEL_AST_TYPE_PRIMITIVE_H */