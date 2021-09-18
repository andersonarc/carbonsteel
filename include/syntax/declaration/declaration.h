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
#include "syntax/predeclaration.h" /* predeclarations */
#include "parser/context.h" /* parser context */
#include "ast/type.h" /* lexical type */

    /* typedefs */
/**
 * Structure is a named list of members,
 * which have a type and a name
 */
struct dc_structure {
    char* name;
    list(dc_structure_member) member_list;
};
struct dc_structure_member {
    char* name;
    ast_type type;
};


/**
 * Enumeration is a named list of members,
 * which have a name and a numerical value
 */
struct dc_enum {
    char* name;
    list(dc_enum_member) member_list;
};
struct dc_enum_member {
    char* name;
    ex_number value;
    dc_enum* parent;
};


/**
 * Alias is an alias for another type
 * with its own name
 */
struct dc_alias {
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
struct dc_function {
    char* name;
    st_compound body;
    ast_type return_type;
    list(dc_function_parameter) parameter_list;
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
typedef list(dc_import_node) dc_import;


/**
 * Base type that contains a declaration 
 * of specified kind
 */
enum declaration_kind {
    DC_IMPORT, DC_ALIAS, DC_STRUCTURE,
    DC_ENUM, DC_FUNCTION, DC_ST_VARIABLE
};
struct declaration {
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
    };
};


    /* functions */

void new_type_declaration(ast_root* ast, declaration* this, declaration_kind kind, int token_kind, void* value, char* name);

void new_declaration(ast_root* ast, declaration* this, declaration_kind kind, void* value);


#endif /* CARBONSTEEL_SYNTAX_DECLARATION_H */