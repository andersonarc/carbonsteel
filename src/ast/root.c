/**
 * @file root.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree
 *  modification function implementations
 */
    /* includes */
#include "ast/root.h" /* this */

#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h"     /* memory allocation */
#include "language/parser.h" /* parser */

/**
 * @todo
 * "Precompiled headers" - Serialize processed files' AST's into cache files
 * (De)serialize pointers first (use a "pointer table"), then allocated and resolve them
 */

    /* functions */
/**
 * Initializes an abstract syntax tree instance
 * with primitive type identifiers and empty declaration list
 * 
 * @param[out] ast Pointer to the AST
 */
void ast_init(ast_root* ast) {
    arl_init(declaration, ast->declaration_list);
    ast->hash_table = calloc(1, sizeof(struct hsearch_data));
    hcreate_r(32768, ast->hash_table);
    /**
     * @todo
     * Arraylist and HashTable addAll functions
     * Resizable HashTable
     * Use ast_declaration instead of void*
     */
    iterate_array(i, primitive_list.size) {
        ast_add_identifier(ast, primitive_list.data[i].name, TOKEN_PRIMITIVE_NAME, &primitive_list.data[i]);
    }
}

/**
 * Creates a new AST declaration from
 * given parameters and adds it to the
 * declaration list
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] kind  Kind of the declaration
 * @param[in] value Value of the declaration
 */
void ast_add_declaration(ast_root* ast, int kind, void* value) {
    declaration dc = {
        .kind = kind,
        .u__any = value
    };
    arl_add(declaration, ast->declaration_list, dc);
}

/**
 * Creates a new global identifier entry
 * and adds it to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] token Token kind of the identifier
 * @param[in] name  Name of the identifier
 * @param[in] value Value of the identifier
 */
void ast_add_identifier(ast_root* ast, char* name, int token, void* value) {
    ast_declaration* dc = allocate(ast_declaration);
    dc->kind = token;
    dc->u__any = value;

    ENTRY entry = {
        .data = (void*) dc,
        .key = name
    };

    /* check if the identifier is already declared */
    ENTRY* existing;
    if (hsearch_r(entry, FIND, &existing, ast->hash_table) != 0) {
        error_syntax("identifier \"%s\" already exists", entry.key)
    }
    
    /* if it is not declared, add the entry */
    if (hsearch_r(entry, ENTER, &existing, ast->hash_table) == 0) {
        error_internal("unable to add new symbol to the symbol table");
    }
}