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
    bool is_unsigned = false;
    if (context_is(context, SCTX_FLAG)) {
        flag_context_if_unsigned(context_current(context)->u_flag_context) {
            is_unsigned = true;
            context_exit(context);
        }
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

        /**
         * Unsigned integer workaround
         */
        if (is_unsigned) {
            //todo workaround for aliases required?
            if (dc->kind == DC_PRIMITIVE && ast_type_primitive_is_signed(dc->u_primitive)) {
                yylval->TOKEN_ANY_NAME = primitive_signed_to_unsigned(dc->u_primitive);
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
cyytoken_kind_t ast_lex_token_native(se_context* context, CYYSTYPE* yylval, char* token) {
    /**
     * Global declarations lookup
     */
    ENTRY* result;
    ENTRY request = { .key = token };
    if (hsearch_r(request, FIND, &result, context->ast.hash_table) != 0) {
        declaration* dc = (declaration*) result->data;
        yylval->CTOKEN_ANY_NAME = dc->u__any;

        /**
         * Some primitives are not primitives in C
         */
        if (dc->kind == DC_PRIMITIVE && !dc->u_primitive->is_allowed_in_native) {
            yylval->CTOKEN_IDENTIFIER = copy_string(token);
            return CTOKEN_IDENTIFIER;
        }

        /**
         * If it has already been defined, but in another file - return identifier
         */
        if (dc->is_native) {
            bool declared_locally = false;
            char* this_filename = arraylist_last(context->file_list)->filename;
            size_t this_filename_length = strlen(this_filename);
            for (int i = 0; i < dc->native_filename_list.size; i++) {
                if (strncmp(dc->native_filename_list.data[i], this_filename, this_filename_length) == 0) {
                    declared_locally = true;
                }
            }
            if (!declared_locally) {
                logd("ImportGuard: %s has not been declared in %s yet, setting as identifier",
                        token, this_filename);
                yylval->CTOKEN_IDENTIFIER = copy_string(token);
                return CTOKEN_IDENTIFIER;
            }
        }
        
        return dc->ctoken;
    }

    /**
     * If the token does not match, return IDENTIFIER
     */
    yylval->CTOKEN_IDENTIFIER = copy_string(token);
    return CTOKEN_IDENTIFIER;
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