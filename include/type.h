/**
 * @file type.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-25
 * 
 *  Type definitions and
 *  stack manipulation function for
 *  structures, enums, aliases, imports,
 *  types and other lexical types
 */
    /* header guard */
#ifndef CARBONSTEEL_TYPE_H
#define CARBONSTEEL_TYPE_H

    /* includes */
#include "declaration.h" /* type declarations */

    /* declarations */
typedef char* string;
arraylist_declare(string);

        /* typedefs */

    /** ENUM **/

struct ast_enum_member {
    char* name;
    ex_number* value;
};

struct ast_enum {
    char* name;
    list(ast_enum_member) member_list;
};


    /** PRIMITIVE **/

struct ast_primitive {
    char* name;
    char* code_name;
    size_t size;
};


    /** ALIAS **/

struct ast_alias {
    char* name;
    ast_type* target;
};


    /** TYPE **/

enum ast_type_level {
    AST_TL_POINTER
};

struct ast_type {
    enum {
        AST_TK_PRIMITIVE, AST_TK_STRUCTURE,
        AST_TK_ENUM, AST_TK_ALIAS
    } kind;
    arraylist(ast_type_level) level_list;
    union {
        ast_primitive* u_primitive;
        ast_structure* u_structure;
        ast_enum* u_enum;
        ast_alias* u_alias;
    };
};


    /** FUNCTION **/

struct ast_function_parameter {
    char* name;
    ast_type type;
};

struct ast_function {
    char* name;
    ast_type return_type;
    list(ast_function_parameter) parameter_list;
    st_compound body;
    arraylist(st_variable) _variable_stack;
};


    /** STRUCTURE **/

struct ast_structure_member {
    char* name;
    ast_type type;
};

struct ast_structure {
    char* name;
    ast_function constructor;
    list(ast_structure_member) member_list;
};


    /** IMPORT **/

struct ast_import {
    arraylist(string) node_list;
};

    /* stack functions */
/**
 * @pop NAME
 * @init ast_structure (.name)
 * @push STRUCTURE
 */
void start_structure();

/**
 * @walk from STRUCTURE
 *  @get TYPE
 *  @get NAME
 * @init ast_structure (.member_list, .constructor) full
 * @pop STRUCTURE
 * @declare ast_structure
 */
void end_structure();

/**
 * @peek TYPE
 * @init ast_type (.level_list)
 */
void add_type_level();

/**
 * @pop NAME
 * @init ast_enum (.name)
 * @push ENUM
 */
void start_enum();

/**
 * @walk from ENUM
 *  @get NAME
 *  @get? NUMBER
 * @init ast_enum (.member_list) full
 * @pop ENUM
 * @declare ast_enum
 */
void end_enum();

/**
 * @pop NAME
 * @pop TYPE
 * @init ast_function (.name, .return_type)
 * @push FUNCTION
 */
void start_function();

/**
 * @walk from FUNCTION
 *  @pop TYPE
 *  @pop NAME
 * @init ast_function (.parameter_list) full
 */
void add_function_parameters();

/**
 * @pop ST_COMPOUND
 * @walk from FUNCTION
 *  @pop TYPE
 *  @pop NAME
 * @init ast_function (.body) full
 * @pop FUNCTION
 * @declare ast_function
 */
void end_function();

/**
 * @init ast_import
 * @push IMPORT
 */
void start_import();

/**
 * @walk
 *  @get NAME
 * @check size != 0
 * @declare ast_import
 * @pop IMPORT
 */
void end_import();

/**
 * @pop TYPE
 * @pop IDENTIFIER
 * @declare ast_alias
 */
void add_alias();

#endif /* CARBONSTEEL_TYPE_H */