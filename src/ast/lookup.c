/**
 * @file lookup.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-18
 * 
 *  Identifier lookup functions based
 *  on global identifier table
 *  and local context
 */
    /* includes */
#include "ast/lookup.h" /* this */

#include <string.h> /* string functions */

#include "misc/memory.h" /* memory copying */

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
myytoken_kind_t ast_lex_token(se_context* context, MYYSTYPE* yylval, char* token) {
    /**
     * Handle flag context
     */
    flag_context flags;
    if (context_is(context, SCTX_FLAG)) {
        flags = context_current(context)->u_flag_context;
        context_exit(context);
    } else {
        flag_context_init(flags);
    }

    /**
     * Context-aware lookup
     */
    int context_result = context_lex_token(context, yylval, token);
    if (context_result > 0) {
        return context_result;
    }

    /**
     * Global declarations lookup
     */
    ENTRY* result;
    ENTRY request = { .key = token };
    if (hsearch_r(request, FIND, &result, context->ast.hash_table) != 0) {
        declaration* dc = (declaration*) result->data;
        yylval->TOKEN_ANY_NAME = dc->u__any;

        /*
            I don't remember why this is here, but let it stay.
        if (!dc->is_full) {
            yylval->TOKEN_IDENTIFIER = copy_string(token);
            return TOKEN_IDENTIFIER;
        }*/

        /**
         * Unsigned integer workaround
         */
        flag_context_if_unsigned(flags) {
            //todo workaround for aliases required?
            if (dc->token == TOKEN_PRIMITIVE_NAME && ast_type_primitive_is_signed(dc->u_primitive)) {
                dc->u_primitive = primitive_signed_to_unsigned(dc->u_primitive);
            } else {
                error_syntax("only primitive integers can be unsigned");
            }
        }

        return dc->token;
    }

    /**
     * If the token does not match, return IDENTIFIER
     */
    yylval->TOKEN_IDENTIFIER = copy_string(token);
    return TOKEN_IDENTIFIER;
}


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
myytoken_kind_t context_lex_token(se_context* context, MYYSTYPE* yylval, char* token) {
    for (int i = context->stack.size - 1; i >= 0; i--) {
        se_context_level* current = &context->stack.data[i];
        switch (current->kind) {
            case SCTX_FUNCTION:
                /* check local declarations */
                iterate_array(i, current->u_locals.size) {
                    ast_local_declaration dc = current->u_locals.data[i];
                    if (strcmp(token, dc.name) == 0) {
                        yylval->TOKEN_ANY_NAME = dc.u__any;
                        return dc.kind;
                    }
                }
                break;

            case SCTX_IMPORT:
                /* imports are identifiers */
                yylval->TOKEN_IDENTIFIER = copy_string(token);
                return TOKEN_IDENTIFIER;

            case SCTX_GLOBAL:
                return -1;

            case SCTX_ENUM:
            case SCTX_EXPRESSION:
                break;

            otherwise_error
        }
    }
    return -1;
}