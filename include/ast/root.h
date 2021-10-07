/**
 * @file root.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree type definition
 *  and modification functions
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_ROOT_H
#define CARBONSTEEL_AST_ROOT_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */
#include "misc/list.h"             /* list utilities */
#include "ast/search.h"            /* hash table search */

    /* typedefs */
/**
 * Abstract syntax tree root
 * that contains the list of all 
 * declarations and a lookup table
 */
typedef struct ast_root {
    arraylist(declaration) declaration_list;
    struct hsearch_data* hash_table;
} ast_root;

    /* functions */
/**
 * Initializes an abstract syntax tree instance
 * with primitive type identifiers and empty declaration list
 * 
 * @param[out] ast Pointer to the AST
 */
void ast_init(ast_root* ast);

/**
 * Creates a new AST declaration from
 * given parameters and adds it to the
 * declaration list
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] kind  Kind of the declaration
 * @param[in] value Value of the declaration
 */
void ast_add_declaration(ast_root* ast, int kind, void* value);

/**
 * Creates a new global identifier entry
 * and adds it to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] token Token kind of the identifier
 * @param[in] name  Name of the identifier
 * @param[in] value Value of the identifier
 */
void ast_add_identifier(ast_root* ast, char* name, int token, void* value);

/**
 * Alias for creating an AST declaration
 * and a lookup table entry
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] kind  Kind of the declaration
 * @param[in] token Token kind of the identifier
 * @param[in] name  Name of the identifier
 * @param[in] value Value of the identifier
 */
static inline void ast_declare(ast_root* ast, int kind, int token, char* name, void* value) {
    ast_add_declaration(ast, kind, value);
    ast_add_identifier(ast, name, token, value);
}


#endif /* CARBONSTEEL_AST_ROOT_H */