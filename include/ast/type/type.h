/**
 * @file type.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Lexical type structure declaration
 *  and basic functions
 */
    /* header guard */
#ifndef AST_TYPE_TYPE_H
#define AST_TYPE_TYPE_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */

    /* typedefs */
/**
 * The lexical type which contains a
 * value of specified kind and a level list
 * 
 * Type level is an array "[]" or a pointer "*"
 * 
 * Type value is a reference to 
 * a declaration in the AST.
 */
enum ast_type_level_kind {
    AT_LEVEL_POINTER, AT_LEVEL_ARRAY
};
struct ast_type_level {
    ast_type_level_kind kind;
    expression_data* u_array_size;
};
arraylist_declare_functions(ast_type_level);

enum ast_type_kind {
    AST_TYPE_PRIMITIVE, AST_TYPE_STRUCTURE,
    AST_TYPE_ENUM, AST_TYPE_FUNCTION, AST_TYPE_GENERIC
};

struct ast_type {
    ast_type_kind kind;
    arraylist(ast_type_level) level_list;
    index_t _generic_impl_index; /* the index of the generic implementation used */
    union {
        /* warning: do not add any non-pointer types */
        ast_type_primitive* u_primitive;
        dc_structure* u_structure;
        dc_enum* u_enum;
        dc_alias* u_alias;
        dc_function* u_function;
        dc_generic* u_generic;

        /**
         * Todo: Support for function types, Support for Carbonsteel Unions, Support for C-native Unions,
         * Generic Types and Methods. Fix type checking system.
         */

        void* u__any;
    };
};
arraylist_declare_functions(ast_type);

    /* functions */
/**
 * Initializes a new lexical type 
 * with specified kind and value
 * 
 * @param[out] type  Pointer to the initialized type
 * @param[in]  kind  Kind of the type
 * @param[in]  value Value of the type
 */
void ast_type_init(ast_type* type, ast_type_kind kind, void* value);

/**
 * Allocates and initializes a new lexical type 
 * with specified kind and value
 * 
 * @param[in] kind  Kind of the type
 * @param[in] value Value of the type
 * 
 * @return Pointer to the created type
 */
ast_type* ast_type_new(ast_type_kind kind, void* value);

/**
 * Deeply clones the source type to the destination type
 * 
 * @param[out] dest Pointer to the destination type
 * @param[in]  src  The source type
 */
void ast_type_clone_to(ast_type* dest, ast_type src);

/**
 * Creates a deep copy of the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return The cloned type
 */
ast_type ast_type_copy(ast_type value);

/**
 * Creates a deep copy of the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return Pointer to the cloned type
 */
ast_type* ast_type_clone(ast_type value);

/**
 * Appends a new type level
 * to the specified type
 * 
 * @param[out] value Pointer to the type
 */
void ast_type_array_wrap(ast_type* value);
void ast_type_constant_array_wrap(ast_type* value, expression_data* size);
void ast_type_pointer_wrap(ast_type* value);

/**
 * Returns the size if the type is a constant array,
 * or throws an internal error otherwise
 * 
 * @param[out] size Size expression of the constant array
 */
expression_data* ast_type_constant_array_size(ast_type* value);

/**
 * Converts a lexical type to a string,
 * showing its display name
 * 
 * @param[in] value Pointer to the type
 * 
 * @return String representation of the type
 */
char* ast_type_display_name(ast_type* value);

/**
 * Mangles the name of a type in case it is anonymous
 * or has generics, ignoring the type's level list
 * 
 * @param[in] value Pointer to the type
 * 
 * @return Identifier-valid plain mangled name of the type
 */
char* ast_type_mangled_name_plain(ast_type* value);

/**
 * Mangles the name of a type in case it is anonymous
 * or has generics
 * 
 * @param[in] value Pointer to the type
 * 
 * @return Identifier-valid mangled name of the type
 */
char* ast_type_mangled_name(ast_type* value);


#endif /* AST_TYPE_TYPE_H */