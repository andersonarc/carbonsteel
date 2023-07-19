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

#include <float.h> /* maximum float values */

    /* global variables */
/**
 * Primitive type list
 */
list(ast_type_primitive) primitive_list;

    /* defines */
/**
 * Helper macro for initializing primitive type list
 * 
 * @param[in] _index_name Index name
 * @param[in] _name       Primitive type name
 * @param[in] _code_name  Primitive type actual (code) name
 * @param[in] _size       Primitive type size
 * @param[in] _capacity   Maximum numerical value of the type
 *                          or 0 if inappicable
 * @param[in] _is_allowed_in_native Some primitives are not primitives in C
 */
#define ast_declare_primitive(_index_name, _name, _code_name, _size, _capacity, _is_allowed_in_native) \
    primitive_list.data[PRIMITIVE_INDEX_##_index_name].name = _name;            \
    primitive_list.data[PRIMITIVE_INDEX_##_index_name].code_name = _code_name;  \
    primitive_list.data[PRIMITIVE_INDEX_##_index_name].size = _size;            \
    primitive_list.data[PRIMITIVE_INDEX_##_index_name].capacity = _capacity;    \
    primitive_list.data[PRIMITIVE_INDEX_##_index_name].is_allowed_in_native = _is_allowed_in_native;

    /* functions */
/**
 * Allocates and fills the global primitive type list
 * with predefined type structures
 */
void primitive_list_init() {
    /* ! DO NOT CHANGE THE ORDER, PRIMITIVE LOOKUP DEPENDS ON IT ! */
    //todo - add "c" primitive types that codegen to native C types
    li_init(ast_type_primitive, primitive_list, _PRIMITIVE_INDEX_MAX + 1);
    ast_declare_primitive(VOID,   "void",   "void",     0, 0, true);
    ast_declare_primitive(BOOLEAN,   "bool",   "int8_t",   1, 0, false);

    ast_declare_primitive(CHAR,   "char",   "int8_t",   1, __INT8_MAX__, true);
    ast_declare_primitive(BYTE,   "byte",   "int8_t",   1, __INT8_MAX__, false);
    ast_declare_primitive(SHORT,  "short",  "int16_t",  2, __INT16_MAX__, true);
    ast_declare_primitive(INT,    "int",    "int32_t",  4, __INT32_MAX__, true);
    ast_declare_primitive(LONG,   "long",   "int64_t",  8, __INT64_MAX__, true);

    ast_declare_primitive(UCHAR,  "uchar",   "uint8_t",  1, __UINT8_MAX__, false);
    ast_declare_primitive(UBYTE,  "ubyte",   "uint8_t",  1, __UINT8_MAX__, false);
    ast_declare_primitive(USHORT, "ushort",  "uint16_t", 2, __UINT16_MAX__, false);
    ast_declare_primitive(UINT,   "uint",    "uint32_t", 4, __UINT32_MAX__, false);
    ast_declare_primitive(ULONG,  "ulong",   "uint64_t", 8, __UINT64_MAX__, false);

    ast_declare_primitive(FLOAT,  "float",  "float",    4, 16777216UL /* 2 ^ 24 (mantissa bits) */, true);
    ast_declare_primitive(DOUBLE, "double", "double",   8, 9007199254740992UL /* 2 ^ 53 (mantissa bits) */, true);

    ast_declare_primitive(ANY, "<__cst_any>", "<__cst_any>", 0, 0, false);
}

/**
 * Primitive type index calculation function
 *
 * @param[in] value Pointer to the primitive type
 * 
 * @return Index of the primitive type 
 */
index_t ast_type_primitive_get_index(ast_type_primitive* value) { 
    return value - primitive_list.data; 
}

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
bool ast_type_primitive_is_void(ast_type_primitive* value) { 
    return ast_type_primitive_get_index(value) == PRIMITIVE_INDEX_VOID;
}

bool ast_type_primitive_is_boolean(ast_type_primitive* value) {
    return ast_type_primitive_get_index(value) == PRIMITIVE_INDEX_BOOLEAN;
}

bool ast_type_primitive_is_number(ast_type_primitive* value) {
    return primitive_type_in_range(value, NUMBER);
}

bool ast_type_primitive_is_integer (ast_type_primitive* value) {
    return primitive_type_in_range(value, INTEGER);
}
bool ast_type_primitive_is_signed  (ast_type_primitive* value) {
    return primitive_type_in_range(value, SIGNED);
}
bool ast_type_primitive_is_unsigned(ast_type_primitive* value) {
    return primitive_type_in_range(value, UNSIGNED);
}

bool ast_type_primitive_is_floating(ast_type_primitive* value) {
    return primitive_type_in_range(value, FLOATING);
}
