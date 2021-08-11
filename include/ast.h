/**
 * @file ast.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree implementation
 *  with type and symbol lookup functions
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_H
#define CARBONSTEEL_AST_H

    /* includes */
#include "type.h" /* statements */
#include "list.h" /* list utilities */

    /* typedefs */
typedef struct ast_root {
    arraylist(ast_enum) enum_list;
    arraylist(ast_structure) structure_list;
    arraylist(ast_alias) alias_list;
    arraylist(ast_function) function_list;
    arraylist(ast_import) import_list;
    arraylist(st_variable) st_variable_list;
    list(ast_primitive) primitive_list;
} ast_root;

    /* global variables */
extern ast_root ast;

    /* functions */
/**
 * Initializes the abstract syntax tree
 */
void ast_init();

/**
 * Looks up the lexical type of a token,
 * and pushes it on the lexical stack
 * 
 * @param[in] token The token
 * 
 * @return Type of the token, or IDENTIFIER
 */
int ast_lex_token(const char* token);

#endif /* CARBONSTEEL_AST_H */