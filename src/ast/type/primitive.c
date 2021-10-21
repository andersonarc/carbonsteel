/**
 * @file primitive.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Primitive type list storage location
 *  and initialization function implementation
 */
    /* includes */
#include "ast/type/primitive.h" /* this */

    /* global variables */
/**
 * Primitive type list
 */
list(ast_type_primitive) primitive_list;

    /* defines */
/**
 * Helper macro for initializing primitive type list
 * 
 * @param[in] index      Index in the primitive type list
 * @param[in] _name      Primitive type name
 * @param[in] _code_name Primitive type actual (code) name
 * @param[in] _size      Primitive type size
 */
#define ast_declare_primitive(index, _name, _code_name, _size)              \
    primitive_list.data[index].name = _name;                                \
    primitive_list.data[index].code_name = _code_name;                      \
    primitive_list.data[index].size = _size;

    /* functions */
/**
 * Allocates and fills the global primitive type list
 * with predefined type structures
 */
void primitive_list_init() {
    li_init(ast_type_primitive, primitive_list, 8);
    ast_declare_primitive(0, "void",   "void",    0);
    ast_declare_primitive(1, "bool",   "int8_t",  1);
    ast_declare_primitive(2, "byte",   "int8_t",  1);
    ast_declare_primitive(3, "short",  "int16_t", 2);
    ast_declare_primitive(4, "int",    "int32_t", 4);
    ast_declare_primitive(5, "long",   "int64_t", 8);
    ast_declare_primitive(6, "float",  "float",   4);
    ast_declare_primitive(7, "double", "double",  8);
}