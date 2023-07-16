/**
 * @file declaration.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-27
 * 
 *  Declaration types file
 * 
 *  Declarations include:
 *   - dc_structure, structure declaration
 *   - dc_enum, enum declaration
 *   - dc_alias, alias declaration
 *   - dc_function, function declaration
 *   - dc_import, import declaration
 * 
 *  Declaration structure consists of a
 *  kind and a value - one of the declarations.
 * 
 *  All declarations have a name, except for the
 *  dc_import, which has a NULL name.
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_DECLARATION_H
#define CARBONSTEEL_SYNTAX_DECLARATION_H

    /* includes */
#include "syntax/expression/primitive.h" /* primitive expressions */
#include "syntax/predeclaration.h"       /* predeclarations */
#include "language/context.h"            /* parser context */
#include "ast/type/type.h"               /* lexical type */

    /* typedefs */
/**
 * Structure is a named list of members,
 * which have a type and a name
 */
struct dc_structure {
    bool is_full; /* marks partial declarations */
    char* name;
    list(dc_structure_member) member_list;
};

struct dc_structure_member {
    char* name;
    ast_type type;
};
arraylist_declare_functions(dc_structure_member);


/**
 * Enumeration is a named list of members,
 * which have a name and a numerical value
 */
struct dc_enum {
    bool is_full; /* marks partial declarations */
    char* name;
    list(dc_enum_member) member_list;
};

struct dc_enum_member {
    char* name;
    ex_constant value;
    dc_enum* parent;
};
arraylist_declare_functions(dc_enum_member);


/**
 * Alias is an alias for another type
 * with its own name
 */
struct dc_alias {
    bool is_full; /* marks partial declarations */
    char* name;
    ast_type target;
};


/**
 * Function has a name, a return type,
 * a parameter list an a compound statement body
 * 
 * Local variables are looked up using
 * _variable_stack which is filled when
 * a variable statement is encountered.
 */
struct dc_function_parameter {
    char* name;
    ast_type type;
};
arraylist_declare_functions(dc_function_parameter);

struct dc_function_parameters {
    list(dc_function_parameter) value;
    bool is_c_vararg;
};

struct dc_function {
    bool is_full; /* marks partial declarations */
    char* name;
    bool is_extern;
    st_compound body;
    ast_type return_type;
    dc_function_parameters parameters;
};


/**
 * Import declaration is a list of
 * strings which are resolved as a path
 * relative to the source file
 * 
 * The file at that path is parsed
 * and its declarations are included
 * in local AST.
 * 
 * @warn empty imports are not allowed
 */
struct dc_import {
    list(dc_import_node) path;
    bool is_native;
};


/**
 * Declaration of generic type
 */
struct dc_generic {
    char* name;
    ast_type _impl; /* used in codegen, NULL by default */
};


/**
 * Base type that contains a declaration 
 * of specified kind
 */
enum declaration_kind {
    DC_IMPORT, DC_ALIAS, DC_STRUCTURE,
    DC_ENUM, DC_FUNCTION, DC_ST_VARIABLE,
    DC_PRIMITIVE
};

struct declaration {
    bool is_full; /* marks partial declarations */
    bool is_native; /* native declarations are not generated in code */
    arraylist(char_ptr) native_filename_list; /* a fix for header guard absence in native files */
    char* name; /* may be null */
    int token;
    int ctoken;
    declaration_kind kind;
    union {
        /* warning: do not add any non-pointer types */
        void* u__any;
        dc_import* u_import;
        dc_alias* u_alias;
        dc_structure* u_structure;
        dc_enum* u_enum;
        dc_function* u_function;
        dc_st_variable* u_variable;
        ast_type_primitive* u_primitive;
    };
};
arraylist_declare_functions(declaration);


/**
 * Local (scope-wide) declaration structure
 * stored in the parser context
 */
enum local_declaration_kind {
    DC_L_FUNCTION_PARAMETER, DC_L_VARIABLE,
    DC_L_GENERIC_NAME
};

struct local_declaration {
    char* name;
    int token;
    local_declaration_kind kind;
    union {
        /* warning: do not add any non-pointer types */
        void* u__any;
        dc_function_parameter* u_function_parameter;
        dc_st_variable* u_variable;
        dc_generic* u_generic;
    };
};
arraylist_declare_functions(local_declaration);
/* todo - needs to be reworked, as currently there are declaration conflicts between scopes
    e.g. for if expressions inside functions */

#endif /* CARBONSTEEL_SYNTAX_DECLARATION_H */