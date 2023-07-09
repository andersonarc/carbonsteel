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
    arl_init(declaration_ptr, ast->declaration_list);
    ast->hash_table = calloc(1, sizeof(struct hsearch_data));
    hcreate_r(32768, ast->hash_table);
    /**
     * @todo
     * Arraylist and HashTable addAll functions
     * Resizable HashTable
     * Use ast_declaration instead of void*
     */
    iterate_array(i, primitive_list.size) {
        declaration* dc = allocate(declaration);
        dc->is_full = true;
        dc->name = primitive_list.data[i].name;
        dc->token = TOKEN_PRIMITIVE_NAME;
        dc->kind = DC_PRIMITIVE;
        dc->u_primitive = &primitive_list.data[i];
        ast_add_identifier(ast, TOKEN_PRIMITIVE_NAME, dc);
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
 * 
 * @return Pointer to the created declaration or NULL if it has been merged
 */
declaration* ast_add_declaration(ast_root* ast, int kind, void* value) {
    declaration* dc = allocate(declaration);
    dc->kind = kind;
    dc->u__any = value;
    switch (dc->kind) {
        case DC_IMPORT:
            dc->name = NULL;
            dc->is_full = true;
            break;

        case DC_STRUCTURE:
            dc->name = dc->u_structure->name;
            dc->is_full = dc->u_structure->is_full;
            break;

        case DC_ALIAS:
            /* todo also skip alias definitions on pass 1. only include the identifier names */
            dc->name = dc->u_alias->name;
            dc->is_full = dc->u_alias->is_full;
            break;

        case DC_ENUM:
            dc->name = dc->u_enum->name;
            dc->is_full = dc->u_enum->is_full;
            break;

        case DC_FUNCTION:
            dc->name = dc->u_function->name;
            dc->is_full = dc->u_function->is_full;
            break;
        
        case DC_ST_VARIABLE:
            dc->name = dc->u_variable->name;
            dc->is_full = dc->u_variable->is_full;
            break;

        otherwise_error
    }

    if (dc->name != NULL) {
        if (!ast_declaration_merge(ast, dc)) {
            arl_add(declaration_ptr, ast->declaration_list, dc);
        } else {
            return NULL; /* merged */
        }
    } else {
        arl_add(declaration_ptr, ast->declaration_list, dc);
    }

    return dc;
}


/**
 * Creates a new global identifier entry
 * and adds it to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] token Token kind of the identifier
 * @param[in] dc    Pointer to the declaration
 */
void ast_add_identifier(ast_root* ast, int token, declaration* dc) {
    dc->token = token;

    ENTRY entry = {
        .data = (void*) dc,
        .key = dc->name
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


/**
 * Looks up a declaration by its name and expected kind
 * 
 * @param[in] ast  Pointer to the AST
 * @param[in] kind Kind of the declaration
 * @param[in] name Name of the declaration
 * 
 * @return Pointer to the declaration structure or NULL
 */
declaration* ast_declaration_lookup(ast_root* ast, declaration_kind kind, char* name) {
    size_t length = strlen(name);

    iterate_array(i, ast->declaration_list.size) {
        declaration* dc = ast->declaration_list.data[i];
        if (dc->kind != kind) continue;
        
        char* current_name = dc->name;
        if (strncmp(name, current_name, length) == 0) return dc;
    }

    return NULL;
}


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
bool ast_declaration_merge(ast_root* ast, declaration* dc) {
    declaration* dc_parent = ast_declaration_lookup(ast, dc->kind, dc->name);
    if (dc_parent == NULL && dc->name != NULL) {
        logd("adding new declaration %s", dc->name);
        return false;
    }

    if (dc_parent->is_full) {
        #ifndef NDEBUG
            logw("attempt to merge a new declaration onto an already full declaration %s", dc->name);
        #else
            logfe("attempt to merge a new declaration onto an already full declaration %s", dc->name);
        #endif
        return true;
    }

    logd("merging %s and %s", dc->name, dc_parent->name)
    switch (dc->kind) {
        case DC_STRUCTURE:
            dc_structure* sparent = dc_parent->u_structure;
            dc_structure* sthis = dc->u_structure;

            list_free(dc_structure_member)(&sparent->member_list);
            
            memcpy(sparent, sthis, sizeof(dc_structure));
            dc_parent->is_full = sparent->is_full;
            break;

        case DC_ALIAS:
            dc_alias* aparent = dc_parent->u_alias;
            dc_alias* athis = dc->u_alias;

            memcpy(aparent, athis, sizeof(dc_alias));
            dc_parent->is_full = aparent->is_full;
            break;

        case DC_ENUM:
            dc_enum* eparent = dc_parent->u_enum;
            dc_enum* ethis = dc->u_enum;

            list_free(dc_enum_member)(&eparent->member_list);

            memcpy(eparent, ethis, sizeof(dc_enum));
            dc_parent->is_full = eparent->is_full;
            break;

        case DC_FUNCTION:
            dc_function* fparent = dc_parent->u_function;
            dc_function* fthis = dc->u_function;

            list_free(dc_function_parameter)(&fparent->parameters.value);
            
            memcpy(fparent, fthis, sizeof(dc_function));
            dc_parent->is_full = fparent->is_full;
            break;
        
        case DC_ST_VARIABLE:
            dc_st_variable* vparent = dc_parent->u_variable;
            dc_st_variable* vthis = dc->u_variable;
            
            memcpy(vparent, vthis, sizeof(dc_st_variable));
            dc_parent->is_full = vparent->is_full;
            break;

        otherwise_error
    }
    
    //dc_parent->u__any = dc->u__any;
    //free(dc->name);
    return true;
}