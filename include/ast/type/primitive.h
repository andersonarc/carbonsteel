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
 */
#define PRIMITIVE_INDEX_VOID   0
#define PRIMITIVE_INDEX_BOOL   1
#define PRIMITIVE_INDEX_BYTE   2
#define PRIMITIVE_INDEX_SHORT  3
#define PRIMITIVE_INDEX_INT    4
#define PRIMITIVE_INDEX_LONG   5
#define PRIMITIVE_INDEX_FLOAT  6
#define PRIMITIVE_INDEX_DOUBLE 7

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


#endif /* CARBONSTEEL_AST_TYPE_PRIMITIVE_H */