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
 * Converts a lexical type to a string
 * 
 * @param[in] value Pointer to the type
 * 
 * @return String representation of the type
 */
char* ast_type_to_string(ast_type* value) {
    char* name;
    char* buffer;
    size_t buffer_size = 1 /* null terminator */;

    /* find typename of the type */
    switch (value->kind) {
        case AST_TYPE_PRIMITIVE:
            name = value->u_primitive->name;
            break;

        case AST_TYPE_STRUCTURE:
            if (value->u_structure->name != NULL) {
                name = value->u_structure->name;
            } else {
                name = "<anonymous structure>";
            }
            break;

        case AST_TYPE_ENUM:
            if (value->u_enum->name != NULL) {
                name = value->u_enum->name;
            } else {
                name = "<anonymous enum>";
            }
            break;

        case AST_TYPE_FUNCTION:
            name = "<function>";
            break;

        otherwise_error
    }

    /* calculate size of the typename */
    size_t name_size = strlen(name);
    buffer_size += name_size;

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
    buffer = allocate_array(char, buffer_size);
    buffer[buffer_size - 1] = 0;
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