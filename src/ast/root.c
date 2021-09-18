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

#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h" /* allocation */
#include "mparser.h" /* parser */


    /* global variables */
list(ast_type_primitive) primitive_list;

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
    primitive_list.data[index].name = _name;                                \
    primitive_list.data[index].code_name = _code_name;                      \
    primitive_list.data[index].size = _size;


    /* functions */
/**
 * Initializes the primitive type list
 */
void primitive_list_init() {
    li_init(ast_type_primitive, primitive_list, 8);
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
 * Initializes an abstract syntax tree instance
 * 
 * @param[out] ast Pointer to the AST
 */
void ast_init(ast_root* ast) {
    arl_init(declaration, ast->declaration_list);
    ast->hash_table = calloc(1, sizeof(struct hsearch_data));
    hcreate_r(32768, ast->hash_table);
    iterate_array(i, primitive_list.size) {
        ast_add_new_entry(ast, primitive_list.data[i].name, TOKEN_PRIMITIVE_NAME, &primitive_list.data[i]);
    }
}

/**
 * Adds a new entry to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] entry The entry
 */
void ast_add_entry(ast_root* ast, ENTRY entry) {
    /* check if the entry already exists */
    ENTRY* existing;
    if (hsearch_r(entry, FIND, &existing, ast->hash_table) != 0) {
        error_syntax("identifier \"%s\" already exists", entry.key)
    }

    /* add the entry */
    if (hsearch_r(entry, ENTER, &existing, ast->hash_table) == 0) {
        error_internal("unable to add new symbol to the symbol table");
    }
}

/**
 * Creates and adds a new value entry to the AST lookup table
 * 
 * @param[in] ast   Pointer to the AST
 * @param[in] name  Name of the entry
 * @param[in] kind  Kind of the token
 * @param[in] value The value
 */
void ast_add_new_entry(ast_root* ast, char* name, int kind, void* value) {
    ast_declaration* dc = allocate(ast_declaration);
    dc->kind = kind;
    dc->u__any = value;

    /* create a new entry */
    ENTRY entry;
    entry.data = (void*) dc;
    entry.key = name;

    /* add it to the symbol table */
    ast_add_entry(ast, entry);
}