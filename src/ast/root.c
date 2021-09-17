/**
 * @file root.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree storage variable
 *  and lookup function implementations
 */
    /* includes */
#include "ast/root.h" /* this */

#include <string.h> /* string functions */

#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h" /* copying */

    /* global variables */
ast_root ast;

    /* defines */
/**
 * Helper macro for initializing primitive types
 * 
 * @param[in] index      Index in the primitive type list
 * @param[in] _name      Primitive type name
 * @param[in] _code_name Primitive type actual (code) name
 * @param[in] _size      Primitive type size
 */
#define ast_add_primitive(index, _name, _code_name, _size)                  \
    ast.primitive_list.data[index].name = _name;                            \
    ast.primitive_list.data[index].code_name = _code_name;                  \
    ast.primitive_list.data[index].size = _size;                            \
    ast_add_new_entry(_name, TOKEN_PRIMITIVE_NAME, &ast.primitive_list.data[index]);


    /* functions */
/**
 * Initializes the abstract syntax tree
 * and primitive type list
 */
void ast_init() {
    arl_init(declaration, ast.declaration_list);
    li_init(ast_type_primitive, ast.primitive_list, 8);
    hcreate(25565);

    /* initialize primitive types */
    ast_add_primitive(0, "void",   "void",    0);
    ast_add_primitive(1, "bool",   "int8_t",  1);
    ast_add_primitive(2, "byte",   "int8_t",  1);
    ast_add_primitive(3, "short",  "int16_t", 2);
    ast_add_primitive(4, "int",    "int32_t", 4);
    ast_add_primitive(5, "long",   "int64_t", 8);
    ast_add_primitive(6, "float",  "float",   4);
    ast_add_primitive(7, "double", "double",  8);
}


/**
 * Adds a new entry to the lookup table
 * 
 * @param[in] entry The entry
 */
void ast_add_entry(ENTRY entry) {
    /* check if the entry already exists */
    ENTRY* existing = hsearch(entry, FIND);
    if (existing != NULL) {
        error_syntax("identifier \"%s\" already exists", entry.key)
    }

    /* add the entry */
    if (hsearch(entry, ENTER) == NULL) {
        error_internal("unable to add new symbol to the symbol table");
    }
}


/**
 * Creates and adds a new entry to the symbol table
 * 
 * @param[in] name Name of the entry
 * @param[in] kind Kind of the token
 * @param[in] value The value
 */
void ast_add_new_entry(char* name, int kind, void* value) {
    ast_declaration* dc = allocate(ast_declaration);
    dc->kind = kind;
    dc->u__any = value;

    /* create a new entry */
    ENTRY entry;
    entry.data = (void*) dc;
    entry.key = name;

    /* add it to the symbol table */
    ast_add_entry(entry);
}


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
int ast_lex_token(YYSTYPE* yylval, char* token, se_context* context) {
    /**
     * Context-aware lookup
     */
    int context_result = context_lex_token(yylval, token, context);
    if (context_result > 0) {
        return context_result;
    }

    /**
     * Global declarations lookup
     */
    ENTRY entry = { .key = token };
    ENTRY* result = hsearch(entry, FIND);
    if (result != NULL) {
        ast_declaration* dc = (ast_declaration*) result->data;
        yylval->TOKEN_ANY_NAME = dc->u__any;
        return dc->kind;
    }

    /**
     * If the token does not match, return IDENTIFIER
     */
    yylval->TOKEN_IDENTIFIER = copy_string(token);
    return TOKEN_IDENTIFIER;
}


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
int context_lex_token(YYSTYPE* yylval, char* token, se_context* context) {
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