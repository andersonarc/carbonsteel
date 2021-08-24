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

struct dc_enum_member {
    char* name;
    ex_number* value;
};

struct dc_enum {
    char* name;
    list(dc_enum_member) member_list;
};


    /** PRIMITIVE **/

struct dc_type_primitive {
    char* name;
    char* code_name;
    size_t size;
};


    /** ALIAS **/

struct dc_alias {
    char* name;
    dc_type* target;
};


    /** TYPE **/

enum dc_type_level_kind {
    DC_TL_POINTER, DC_TL_ARRAY
};

struct dc_type_level {
    dc_type_level_kind kind;
};

struct dc_type {
    enum {
        DC_TK_PRIMITIVE, DC_TK_STRUCTURE,
        DC_TK_ENUM, DC_TK_ALIAS
    } kind;
    arraylist(dc_type_level) level_list;
    union {
        dc_type_primitive* u_primitive;
        dc_structure* u_structure;
        dc_enum* u_enum;
        dc_alias* u_alias;
    };
};


    /** FUNCTION **/

struct dc_function_parameter {
    char* name;
    dc_type type;
};

struct dc_function {
    char* name;
    dc_type return_type;
    list(dc_function_parameter) parameter_list;
    st_compound body;
    arraylist(st_variable) _variable_stack;
};


    /** STRUCTURE **/

struct dc_structure_member {
    char* name;
    dc_type type;
};

struct dc_structure {
    char* name;
    dc_function constructor;
    list(dc_structure_member) member_list;
};


    /** IMPORT **/

struct dc_import {
    arraylist(string) node_list;
};


    /** DECLARATION **/

enum declaration_kind {
    DC_IMPORT, DC_ALIAS, DC_STRUCTURE,
    DC_ENUM, DC_FUNCTION, DC_ST_VARIABLE
};
    
struct declaration {
    declaration_kind kind;
    char* name;
    union {
        dc_import u_import;
        dc_alias u_alias;
        dc_structure u_structure;
        dc_enum u_enum;
        dc_function u_function;
        st_variable* u_st_variable;
    };
};

    /* stack functions */
/**
 * @pop NAME
 * @init dc_structure (.name)
 * @push STRUCTURE
 */
void start_structure();

/**
 * @walk from STRUCTURE
 *  @get TYPE
 *  @get NAME
 * @init dc_structure (.member_list, .constructor) full
 * @pop STRUCTURE
 */
void end_structure();

/**
 * @find TYPE
 * @init dc_type (.level_list)
 */
void add_type_level(dc_type_level_kind kind);

/**
 * @pop NAME
 * @init dc_enum (.name)
 * @push ENUM
 */
void start_enum();

/**
 * @walk from ENUM
 *  @get NAME
 *  @get? NUMBER
 * @init dc_enum (.member_list) full
 * @pop ENUM
 */
void end_enum();

/**
 * @pop NAME
 * @pop TYPE
 * @init dc_function (.name, .return_type)
 * @push FUNCTION
 */
void start_function();

/**
 * @walk from FUNCTION
 *  @pop TYPE
 *  @pop NAME
 * @init dc_function (.parameter_list) full
 */
void add_function_parameters();

/**
 * @pop ST_COMPOUND
 * @walk from FUNCTION
 *  @pop TYPE
 *  @pop NAME
 * @init dc_function (.body) full
 * @pop FUNCTION
 */
void end_function();

/**
 * @init dc_import
 * @push IMPORT
 */
void start_import();

/**
 * @walk
 *  @get NAME
 * @check size != 0
 * @pop IMPORT
 */
void end_import();

/**
 * @pop TYPE
 * @pop IDENTIFIER
 */
void add_alias();

/**
 * @pop IMPORT | ALIAS | STRUCTURE | ENUM | FUNCTION | VARIABLE
 * @init declaration (.kind, .u_*)
 */
void add_declaration(declaration_kind kind);

#endif /* CARBONSTEEL_TYPE_H */