/**
 * @file type.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.4
 * @date 2021-07-25
 * 
 *  Lexical type interaction and
 *  conversion functions
 */
    /* header guard */
#ifndef CARBONSTEEL_AST_TYPE_H
#define CARBONSTEEL_AST_TYPE_H

    /* includes */
#include "syntax/predeclaration.h" /* predeclarations */

    /* typedefs */
/**
 * Primitive lexical type, either a
 * number, a logic value or a void type
 * 
 * Primitive type list is initialized
 * in ast.c and is constant for all
 * program execution.
 */
struct ast_type_primitive {
    char* name;
    char* code_name;
    size_t size;
};


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
};
enum ast_type_kind {
    AST_TYPE_PRIMITIVE, AST_TYPE_STRUCTURE,
    AST_TYPE_ENUM, AST_TYPE_FUNCTION
};
struct ast_type {
    ast_type_kind kind;
    arraylist(ast_type_level) level_list;
    union {
        /* warning: do not add any non-pointer types */
        ast_type_primitive* u_primitive;
        dc_structure* u_structure;
        dc_enum* u_enum;
        dc_alias* u_alias;
        dc_function* u_function;
        void* u__any;
    };
};


    /* functions */
/**
 * Returns a token kind for a lexical type
 *
 * @param[in] value Pointer to the type
 * 
 * @return The token kind
 */
int ast_type_to_token_kind(ast_type* value);


/**
 * More advanced type comparison
 * function which supports implicit
 * integer casts
 * 
 * If both types are numbers, they
 * are casted to the first type (a)
 * 
 * @param[in] a Pointer to the first type, which is prioritized when casting
 * @param[in] b Pointer to the second type
 * 
 * @return NULL if types are not equal,
 *          resulting type if types are the same
 */
ast_type* ast_type_merge_prioritized(ast_type* a, ast_type* b);


/**
 * More advanced type comparison
 * function which supports implicit
 * integer casts
 * 
 * If both types are numbers, they
 * are casted to the larger number type
 * 
 * If sizes are equal, the type with most
 * precision is returned (up to double)
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return NULL if types are not equal,
 *          resulting type if types are the same
 */
ast_type* ast_type_merge_extend(ast_type* a, ast_type* b);


/**
 * More advanced type comparison
 * function which supports implicit
 * integer casts
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return true if these types can be merged
 */
bool ast_type_can_merge(ast_type* a, ast_type* b);


/**
 * Compares two lexical types
 * and returns the comparison result
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return true if types are equal
 */
bool ast_type_is_equal(ast_type* a, ast_type* b);


/**
 * Checks the kind of the last type level
 * and returns true if the level list is 
 * not empty and the last level kind is equal 
 * to the specified kind parameter 
 * 
 * @param[in] value Pointer to the type
 * @param[in] kind  The expected last level kind
 * 
 * @return true if the level list is not empty and
 *          the last level kind is equal to specified kind
 */
bool ast_type_last_level_is(ast_type* value, ast_type_level_kind kind);


/**
 * Checks if numerical operations 
 * can be applied to a type
 * 
 * @param[in] value Pointer to the type
 * 
 * @return true if the type is a primitive number
 */
bool ast_type_is_number(ast_type* value);


/**
 * Checks if type is a boolean
 * 
 * @param[in] value Pointer to the type
 * 
 * @return true if the type is a boolean
 */
bool ast_type_is_boolean(ast_type* value);


/**
 * Returns the default numerical expression
 * type for the specified kind
 * 
 * @param[in] kind Kind of the numerical expression
 * 
 * @return Primitive type (float or int)
 */
ast_type* ast_type_of_ex_number(ex_number_kind kind);


/**
 * Returns the default boolean type
 * 
 * @return Primitive bool type
 */
ast_type* ast_type_of_boolean();


/**
 * Returns the default character expression type
 * 
 * @return Primitive byte type
 */
ast_type* ast_type_of_character();


/**
 * Returns the type of the specified
 * constructor expression
 * 
 * @param[in] ex The constructor expression
 * 
 * @return Type of the constructor expression
 */
ast_type* ast_type_of_ex_constructor(ex_constructor* ex);


/**
 * Returns the default string literal type
 * 
 * @return byte[] type
 */
ast_type* ast_type_of_string();


/**
 * Initializes a type with specified kind and value
 * 
 * @param[in] type  Pointer to the initialized type
 * @param[in] kind  Kind of the type
 * @param[in] value Value of the type
 */
void ast_type_init(ast_type* type, ast_type_kind kind, void* value);


/**
 * Creates a new type with specified kind and value
 * 
 * @param[in] kind Kind of the type
 * @param[in] value Value of the type
 */
ast_type* ast_type_new(ast_type_kind kind, void* value);


/**
 * Clones a type (src)
 * to another type (dest)
 * 
 * @param[in] dest Pointer to the destination type
 * @param[in] src  The source type
 */
void ast_type_clone_to(ast_type* dest, ast_type src);


/**
 * Clones the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return Copy of the type
 */
ast_type* ast_type_clone(ast_type value);


/**
 * Returns a string representation of a type
 * 
 * @param[in] value Pointer to the type
 * 
 * @return The string representation of the type
 */
char* ast_type_to_string(ast_type* value);


#endif /* CARBONSTEEL_AST_TYPE_H */