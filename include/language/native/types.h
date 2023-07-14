/**
 * @file types.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-13
 * 
 *  Type definitions for interoperability with C
 */
#ifndef CARBONSTEEL_LANGUAGE_NATIVE_TYPES_H
#define CARBONSTEEL_LANGUAGE_NATIVE_TYPES_H

    /* includes */
#include "syntax/predeclaration.h"
#include "syntax/declaration/declaration.h"
#include "ast/type/type.h"
#include "language/context.h"

    /* definitions */
#define CST_NATIVE_ENUM_PREFIX "enum_"
#define CST_NATIVE_ENUM_PREFIX_STRLEN (strlen(CST_NATIVE_ENUM_PREFIX))
#define CST_NATIVE_STRUCT_PREFIX "struct_"
#define CST_NATIVE_STRUCT_PREFIX_STRLEN (strlen(CST_NATIVE_STRUCT_PREFIX))

    /* declaration specifiers and qualifiers */
enum c_storage_class_specifier {
    C_SCLS_TYPEDEF, C_SCLS_EXTERN, C_SCLS_STATIC,
    C_SCLS_THREAD_LOCAL, C_SCLS_AUTO, C_SCLS_REGISTER
};
arraylist_declare_functions(c_storage_class_specifier);

enum c_function_specifier {
    C_FS_INLINE, C_FS_NORETURN
};
arraylist_declare_functions(c_function_specifier);

enum c_type_qualifier {
    C_TQ_CONST, C_TQ_RESTRICT, C_TQ_VOLATILE, C_TQ_ATOMIC
};
arraylist_declare_functions(c_type_qualifier);

enum c_type_specifier_kind {
    C_TS_SIGNED, C_TS_UNSIGNED, C_TS_COMPLEX,
    C_TS_IMAGINARY, C_TS_ATOMIC, C_TS_TYPE
};

struct c_type_specifier {
    c_type_specifier_kind kind;
    union {
        ast_type u_type;
    };
};
arraylist_declare_functions(c_type_specifier);

typedef struct c_declaration_specifiers {
    arraylist(c_storage_class_specifier) storage_specs;
    arraylist(c_function_specifier) function_specs;
    arraylist(c_type_qualifier) type_quals;
    arraylist(c_type_specifier) type_specs;
} c_declaration_specifiers;

static void c_declaration_specifier_init(c_declaration_specifiers* this) {
    arraylist_init_empty(c_storage_class_specifier)(&this->storage_specs);
    arraylist_init_empty(c_function_specifier)(&this->function_specs);
    arraylist_init_empty(c_type_qualifier)(&this->type_quals);
    arraylist_init_empty(c_type_specifier)(&this->type_specs);
}



    /* declarators */
struct c_declarator {
    char* name;
    arraylist(ast_type_level) level_list;
    bool is_function;
    dc_function_parameters u_parameters;
};
arraylist_declare_functions(c_declarator);



    /* declarations */
struct c_declaration {
    c_declaration_specifiers specs;
    arraylist(c_declarator) declarators;
    arraylist(c_declaration) declarations;
};
arraylist_declare_functions(c_declaration);

#endif /* CARBONSTEEL_LANGUAGE_NATIVE_TYPES_H */