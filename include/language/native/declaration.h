/**
 * @file declaration.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-13
 * 
 *  Functions for working with C-native declarations
 */
    /* header guard */
#ifndef CARBONSTEEL_LANGUAGE_NATIVE_DECLARATION_H
#define CARBONSTEEL_LANGUAGE_NATIVE_DECLARATION_H

    /* includes */
#include "language/native/types.h"
#include "syntax/predeclaration.h"
#include "syntax/declaration/declaration.h"

    /* functions */
/**
 * Translates a C enum name into a Carbonsteel-compatible one
 * 
 * @param name The enum name
 * 
 * @return Carbonsteel-compatible enum name, allocated with malloc
 */
static inline char* cst_native_enum_name(char* name) {
    size_t name_length = strlen(name);
    char* concat = allocate_array(char, CST_NATIVE_ENUM_PREFIX_STRLEN + name_length + 1);
    strcpy(concat, CST_NATIVE_ENUM_PREFIX);
    strncpy(concat + CST_NATIVE_ENUM_PREFIX_STRLEN, name, name_length);
    concat[CST_NATIVE_STRUCT_PREFIX_STRLEN + name_length] = 0;
    return concat;
}

/**
 * Translates a C struct name into a Carbonsteel-compatible one
 * 
 * @param name The struct name
 * 
 * @return Carbonsteel-compatible struct name, allocated with malloc
 */
static inline char* cst_native_struct_name(char* name) {
    size_t name_length = strlen(name);
    char* concat = allocate_array(char, CST_NATIVE_STRUCT_PREFIX_STRLEN + name_length + 1);
    strcpy(concat, CST_NATIVE_STRUCT_PREFIX);
    strncpy(concat + CST_NATIVE_STRUCT_PREFIX_STRLEN, name, name_length);
    concat[CST_NATIVE_STRUCT_PREFIX_STRLEN + name_length] = 0;
    return concat;
}

/**
 * Translates a C-native declaration specifier list
 * into a Carbonsteel type, listing all
 * non-anonymous declarations into the provided arraylist
 * 
 * @param context The parser context
 * @param in The C-native declaration specifiers
 * @param declarations The arraylist to append any inner declarations into
 * 
 * @return The type
 */
ast_type cst_native_declspecs_translate(se_context* context, c_declaration_specifiers in, arraylist(declaration)* declarations);

/**
 * Translates a C-native declaration into a list of
 * Carbonsteel declarations
 * 
 * @param context The parser context
 * @param in The C-native declaration
 * 
 * @return A list of processed declarations 
 */
arraylist(declaration) cst_native_declaration_translate(se_context* context, c_declaration in);

/**
 * Translates a declaration into a type
 * 
 * @param in The declaration
 * 
 * @return A Carbonsteel type
 */
static inline ast_type cst_declaration_to_type(declaration dc) {
    ast_type type;
    switch (dc.kind) {
        case DC_ENUM:
            ast_type_init(&type, AST_TYPE_ENUM, dc.u_enum);
            break;
        
        case DC_STRUCTURE:
            ast_type_init(&type, AST_TYPE_STRUCTURE, dc.u_structure);
            break;

        case DC_ALIAS:
            ast_type_clone_to(&type, dc.u_alias->target);
            break;

        case DC_PRIMITIVE:
            ast_type_init(&type, AST_TYPE_PRIMITIVE, dc.u_primitive);
            break;

        case DC_FUNCTION:
            ast_type_init(&type, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_VOID]);
            ast_type_pointer_wrap(&type);
            logw("function types are not supported yet!");
            break;

        case DC_ST_VARIABLE:
        case DC_IMPORT:
        otherwise_error
    }

    return type;
}

/**
 * Translates a C-native declaration into a function parameter
 * 
 * @param context The parser context
 * @param in The C-native declaration
 * 
 * @return A function parameter
 */
static inline dc_function_parameter cst_native_declaration_to_function_parameter(se_context* context, c_declaration in) {
    arraylist(declaration) decls = cst_native_declaration_translate(context, in);

    if (decls.size != 1) {
        logfe("expected 1 declaration inside a function parameter, got %zu", decls.size);
    }

    dc_function_parameter p;
    p.name = decls.data[0].name;
    p.type = cst_declaration_to_type(decls.data[0]);

    return p;
}

/**
 * Translates a C-native declaration into a structure member
 * 
 * @param context The parser context
 * @param in The C-native declaration
 * 
 * @return A structure member
 */
static inline dc_structure_member cst_native_declaration_to_structure_member(se_context* context, c_declaration in) {
    arraylist(declaration) decls = cst_native_declaration_translate(context, in);

    if (decls.size != 1) {
        logfe("expected 1 declaration inside a structure member, got %zu", decls.size);
    }

    dc_structure_member m;
    m.name = decls.data[0].name;
    m.type = cst_declaration_to_type(decls.data[0]);

    return m;
}

#endif /* CARBONSTEEL_LANGUAGE_NATIVE_DECLARATION_H */