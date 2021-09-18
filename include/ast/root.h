/**
 * @file ast.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree type definition
 *  and lookup functions
 * 
 *  Global identifiers are stored in a
 *  hash search table and in a declaration
 *  list in the AST. 
 * 
 *  Local identifiers are looked up 
 *  using parser context.
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_ROOT_H
#define CARBONSTEEL_AST_ROOT_H

    /* includes */
#define _GNU_SOURCE
#include <search.h> /* hash table search */
#undef  _GNU_SOURCE

#include "misc/list.h" /* list utilities */
#include "syntax/predeclaration.h" /* predeclarations */

    /* typedefs */
/**
 * Abstract syntax tree root
 * that contains the list of all 
 * declarations
 */
typedef struct ast_root {
    arraylist(declaration) declaration_list;
    struct hsearch_data* hash_table;
} ast_root;


    /* global variables */
/**
 * Primitive type list
 * 
 * Primitive types are ordered by
 * size/precision and their order is constant,
 * so pointer comparisons are acceptable
 */
extern list(ast_type_primitive) primitive_list;


    /* functions */
/**
 * Initializes the primitive type list
 */
void primitive_list_init();

/**
 * Initializes an abstract syntax tree instance
 * 
 * @param[out] ast Pointer to the AST
 */
void ast_init(ast_root* ast);


/**
 * Adds a new entry to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] entry The entry
 */
void ast_add_entry(ast_root* ast, ENTRY entry);


/**
 * Creates and adds a new value entry to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] name  Name of the entry
 * @param[in] kind  Kind of the token
 * @param[in] value The value
 */
void ast_add_new_entry(ast_root* ast, char* name, int kind, void* value);


#endif /* CARBONSTEEL_AST_ROOT_H */