/**
 * @file lookup.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-18
 * 
 *  Identifier lookup functions based
 *  on global identifier table
 *  and local context
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_LOOKUP_H
#define CARBONSTEEL_AST_LOOKUP_H

    /* includes */
#include "language/context.h" /* parser context */
#include "language/parser.h"  /* parser */
#include "language/native/parser.h" /* native parser */

    /* functions */
/**
 * Looks up the lexical type of a token
 * and assigns its semantic value from
 * the declaration table
 * 
 * @param[in]  context The parser context
 * @param[out] yylval  Sematic value of the token
 * @param[in]  token   The token string
 * 
 * @return Type of the token, or IDENTIFIER
 */
myytoken_kind_t ast_lex_token(se_context* context, MYYSTYPE* yylval, char* token);
cyytoken_kind_t ast_lex_token_native(se_context* context, CYYSTYPE* yylval, char* token);

/**
 * Iterates the context stack to
 * determine the kind of an identifier
 * and assign its value to the yylval parameter
 * 
 * @param[in]  context Pointer to the parser context
 * @param[out] yylval  Sematic value of the token
 * @param[in]  token   The identifier
 * 
 * @return Token kind, or -1 if not found
 */
myytoken_kind_t context_lex_token(se_context* context, MYYSTYPE* yylval, char* token);


#endif /* CARBONSTEEL_AST_LOOKUP_H */