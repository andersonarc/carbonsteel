/**
 * @file declaration.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-09-01
 * 
 *  Abstract syntax tree local and global
 *  declaration type definitions
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_DECLARATION_H
#define CARBONSTEEL_AST_DECLARATION_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */
#include "ast/type/primitive.h"    /* primitive types */
#include "ast/type/type.h"         /* lexical type */
#include "language/context.h"      /* parser context */

    /* typedefs */
/**
 * Local named declaration entry for
 * context lookup
 * 
 * Local declarations are stored in
 * parser context and are looked up by name.
 */
typedef struct ast_local_declaration {
    int kind;
    char* name;
    union {
        /* warning: do not add any non-pointer types */
        void* u__any;
        dc_function_parameter* u_function_parameter;
        dc_st_variable* u_variable;
    };
} ast_local_declaration;
arraylist_declare(ast_local_declaration);
/* todo - needs to be reworked, as currently there are declaration conflicts between scopes
    e.g. for if expressions inside functions */


#endif /* CARBONSTEEL_AST_DECLARATION_H */