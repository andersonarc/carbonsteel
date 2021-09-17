/**
 * @file declaration.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-28
 * 
 *  Declaration parser functions
 *  implementation file
 */
    /* includes */
#include "syntax/declaration/declaration.h" /* this */

#include "ast/root.h" /* syntax tree */
#include "misc/convert.h" /* conversion */

    /* functions */

void new_type_declaration(declaration* this, declaration_kind kind, int token_kind, void* value, char* name) {
    ast_add_new_entry(name, token_kind, value);
    new_declaration(this, kind, value);
}

void new_declaration(declaration* this, declaration_kind kind, void* value) {
    declaration dc = {
        .kind = kind,
        .u__any = value
    };
    arl_add(declaration, ast.declaration_list, dc);
}