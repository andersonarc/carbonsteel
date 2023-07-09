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
    arraylist(declaration_ptr) declaration_list;
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
 * 
 * @return Pointer to the created declaration or NULL if it has been merged
 */
declaration* ast_add_declaration(ast_root* ast, int kind, void* value);

/**
 * Creates a new global identifier entry
 * and adds it to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] token Token kind of the identifier
 * @param[in] dc    Pointer to the declaration
 */
void ast_add_identifier(ast_root* ast, int token, declaration* dc);

/**
 * Looks up a declaration by its name and expected kind
 * 
 * @param[in] ast  Pointer to the AST
 * @param[in] kind Kind of the declaration
 * @param[in] name Name of the declaration
 * 
 * @return Pointer to the declaration structure or NULL
 */
declaration* ast_declaration_lookup(ast_root* ast, declaration_kind kind, char* name);

/**
 * Attemps to merge two declarations, either adding extra information
 * to an existing declaration or throwing an error in case the existing declaration
 * is flagged as full
 * 
 * @param[in] ast Pointer to the AST
 * @param[in] dc  Declaration to append on the AST
 * 
 * @return false if a new declaration needs to be created,
 *          true if the declaration has been merged successfully
 */
bool ast_declaration_merge(ast_root* ast, declaration* dc);

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
    declaration* dc = ast_add_declaration(ast, kind, value);
    if (dc != NULL) {
        ast_add_identifier(ast, token, dc);
    }
}


#endif /* CARBONSTEEL_AST_ROOT_H */