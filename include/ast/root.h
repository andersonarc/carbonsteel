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
#include <search.h> /* hash table search */

#include "parser/context.h" /* parser context */
#include "misc/list.h"      /* list utilities */
#include "parser.h"         /* parser */


    /* typedefs */
/**
 * Abstract syntax tree root
 * that contains the list of all 
 * declarations and primitive types
 */
typedef struct ast_root {
    arraylist(declaration) declaration_list;
    list(ast_type_primitive) primitive_list;
} ast_root;


    /* global variables */
/**
 * Storage variable for the abstract syntax tree
 */
extern ast_root ast;


    /* functions */
/**
 * Initializes the abstract syntax tree
 * and its primitive type list
 */
void ast_init();


/**
 * Adds a new entry to the lookup table
 * 
 * @param[in] entry The entry
 */
void ast_add_entry(ENTRY entry);


/**
 * Creates and adds a new value entry to the symbol table
 * 
 * @param[in] name  Name of the entry
 * @param[in] kind  Kind of the token
 * @param[in] value The value
 */
void ast_add_new_entry(char* name, int kind, void* value);


/**
 * Looks up the lexical type of a token
 * and assigns its semantic value from
 * the declaration table
 * 
 * @param[out] yylval  Sematic value of the token
 * @param[in]  token   The token string
 * @param[in]  context The parser context
 * 
 * @return Type of the token, or IDENTIFIER
 */
int ast_lex_token(YYSTYPE* yylval, char* token, se_context* context);


/**
 * Recursively checks the context stack to
 * determine the kind of an identifier
 * and assigns 
 * 
 * @param[out] yylval  Sematic value of the token
 * @param[in] token The identifier
 * @param[in] context Pointer to the parser context
 * 
 * @return Token kind, or -1 if not found
 */
int context_lex_token(YYSTYPE* yylval, char* token, se_context* context);


#endif /* CARBONSTEEL_AST_ROOT_H */