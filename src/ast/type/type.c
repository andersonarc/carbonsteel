/**
 * @file type.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Lexical type basic function
 *  implementations
 */
    /* includes */
#include "ast/type/type.h" /* this */

#include "syntax/declaration/declaration.h" /* declarations */
#include "ast/type/primitive.h" /* primitive type */
#include "ast/type/check.h"  /* type checks */
#include "misc/memory.h"   /* memory allocation */

    /* functions */
/**
 * Initializes a new lexical type 
 * with specified kind and value
 * 
 * @param[out] type  Pointer to the initialized type
 * @param[in]  kind  Kind of the type
 * @param[in]  value Value of the type
 */
void ast_type_init(ast_type* type, ast_type_kind kind, void* value) {
    /* initialize a new type */
    type->kind = kind;
    type->u__any = value;
    arraylist_init_empty(ast_type_level)(&type->level_list);
}

/**
 * Allocates and initializes a new lexical type 
 * with specified kind and value
 * 
 * @param[in] kind  Kind of the type
 * @param[in] value Value of the type
 * 
 * @return Pointer to the created type
 */
ast_type* ast_type_new(ast_type_kind kind, void* value) {
    /* create a new type without value */
    ast_type* type = allocate(ast_type);
    ast_type_init(type, kind, value);

    return type;
}

/**
 * Deeply clones the source type to the destination type
 * 
 * @param[out] dest Pointer to the destination type
 * @param[in]  src  The source type
 */
void ast_type_clone_to(ast_type* dest, ast_type src) {
    dest->kind = src.kind;
    dest->u__any = src.u__any;
    dest->_generic_impl_index = src._generic_impl_index;
    arraylist_init_empty(ast_type_level)(&dest->level_list);
    iterate_array(i, src.level_list.size) {
        arl_add(ast_type_level, dest->level_list, src.level_list.data[i]);
    }
}

/**
 * Creates a deep copy of the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return The cloned type
 */
ast_type ast_type_copy(ast_type value) {
    ast_type result;
    ast_type_clone_to(&result, value);
    return result;
}

/**
 * Creates a deep heap-allocated copy of the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return Pointer to the cloned type
 */
ast_type* ast_type_clone(ast_type value) {
    ast_type* result = allocate(ast_type);
    ast_type_clone_to(result, value);
    return result;
}

/**
 * Appends a new type level
 * to the specified type
 * 
 * @param[out] value Pointer to the type
 */
void ast_type_array_wrap(ast_type* value) {
    ast_type_level level = { .kind = AT_LEVEL_ARRAY, .u_array_size = NULL };
    arl_add(ast_type_level, value->level_list, level);
}

void ast_type_constant_array_wrap(ast_type* value, expression_data* size) {
    if (size == 0) {
        error_syntax("array size cannot be equal to 0");
    }

    ast_type_level level = { .kind = AT_LEVEL_ARRAY, .u_array_size = size };
    arl_add(ast_type_level, value->level_list, level);
}

void ast_type_pointer_wrap(ast_type* value) {
    ast_type_level level = { .kind = AT_LEVEL_POINTER, .u_array_size = NULL };
    arl_add(ast_type_level, value->level_list, level);
}

/**
 * Returns the size if the type is a constant array,
 * or throws an internal error otherwise
 * 
 * @param[out] size Size of the constant array
 */
expression_data* ast_type_constant_array_size(ast_type* value) {
    if (!ast_type_is_constant_array(value)) {
        error_internal("expected a constant array");
    }

    return arraylist_last(value->level_list).u_array_size;
}

/**
 * Converts a lexical type to a string,
 * showing its display name
 * 
 * @param[in] value Pointer to the type
 * 
 * @return String representation of the type
 */
char* ast_type_display_name(ast_type* value) {
    char* name;

    /* find the typename */    
    switch (value->kind) {
        case AST_TYPE_PRIMITIVE:
            name = value->u_primitive->name;
            break;

        case AST_TYPE_GENERIC:
            name = value->u_generic->name;
            break;

        case AST_TYPE_STRUCTURE:
            name = dc_structure_display_name(value->u_structure, value->_generic_impl_index);
            break;

        case AST_TYPE_ENUM:
            name = dc_enum_display_name(value->u_enum, value->_generic_impl_index);
            break;

        case AST_TYPE_FUNCTION:
            name = dc_function_display_name(value->u_function, value->_generic_impl_index);
            break;

        otherwise_error
    }

    /* calculate size of the typename */
    size_t name_size = strlen(name);
    size_t buffer_size = name_size;

    /* calculate size of the level list */
    iterate_array(i, value->level_list.size) {
        switch (value->level_list.data[i].kind) {
            case AT_LEVEL_POINTER:
                buffer_size++;
            break;

            case AT_LEVEL_ARRAY:
                buffer_size += 2;
            break;
            
            otherwise_error
        }
    }

    /* allocate the buffer and append the typename */
    char* buffer = allocate_array(char, buffer_size + 1);
    buffer[buffer_size] = 0;
    strcpy(buffer, name);

    /* append the level list after the typename */
    index_t pos = name_size;
    iterate_array(i, value->level_list.size) {
        switch (value->level_list.data[i].kind) {
            case AT_LEVEL_POINTER:
                buffer[pos] = '*';
            break;

            case AT_LEVEL_ARRAY:
                buffer[pos] = '[';
                buffer[pos + 1] = ']';
                pos++;
            break;

            otherwise_error
        }
        pos++;
    }

    return buffer;
}

/**
 * Mangles the name of a type in case it is anonymous
 * or has generics, ignoring the type's level list
 * 
 * @param[in] value Pointer to the type
 * 
 * @return Identifier-valid plain mangled name of the type
 */
char* ast_type_mangled_name_plain(ast_type* value) {
    /* a trick to the ignore type levels */
    size_t old_size = value->level_list.size;
    value->level_list.size = 0;

    char* name = ast_type_mangled_name(value);

    value->level_list.size = old_size;
    return name;
}

/**
 * Mangles the name of a type in case it is anonymous
 * or has generics
 * 
 * @param[in] value Pointer to the type
 * 
 * @return Identifier-valid mangled name of the type
 */
char* ast_type_mangled_name(ast_type* value) {
    char* name;

    /* mangle the typename */    
    switch (value->kind) {
        case AST_TYPE_PRIMITIVE:
            name = value->u_primitive->name;
            break;

        case AST_TYPE_GENERIC:
            name = ast_type_mangled_name(&value->u_generic->_impl);
            break;

        case AST_TYPE_STRUCTURE:
            name = dc_structure_mangled_name(value->u_structure, value->_generic_impl_index);
            break;

        case AST_TYPE_ENUM:
            name = dc_enum_mangled_name(value->u_enum, value->_generic_impl_index);
            break;

        case AST_TYPE_FUNCTION:
            name = dc_function_mangled_name(value->u_function, value->_generic_impl_index);
            break;

        otherwise_error
    }

    /* calculate size of the typename */
    size_t name_size = strlen(name);
    size_t buffer_size = name_size;

    /* calculate size of the level list */
    iterate_array(i, value->level_list.size) {
        switch (value->level_list.data[i].kind) {
            case AT_LEVEL_POINTER:
                buffer_size += strlen("__cst_pointer");
            break;

            case AT_LEVEL_ARRAY:
                buffer_size += strlen("__cst_array");
            break;
            
            otherwise_error
        }
    }

    /* allocate the buffer and append the typename */
    char* buffer = allocate_array(char, buffer_size + 1);
    buffer[buffer_size ] = 0;
    strcpy(buffer, name);

    /* append the level list after the typename */
    index_t pos = name_size;
    iterate_array(i, value->level_list.size) {
        switch (value->level_list.data[i].kind) {
            case AT_LEVEL_POINTER:
                strcpy(buffer + pos, "__cst_pointer");
                pos += strlen("__cst_pointer");
                break;

            case AT_LEVEL_ARRAY:
                strcpy(buffer + pos, "__cst_array");
                pos += strlen("__cst_array");
                break;

            otherwise_error
        }
    }

    return buffer;
}