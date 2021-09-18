/**
 * @file type.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.3
 * @date 2021-07-26
 * 
 *  Lexical type interaction and
 *  conversion functions
 */
    /* includes */
#include "ast/type.h" /* this */

#include <string.h> /* string utilities */

#include "misc/convert.h" /* conversion */
#include "misc/memory.h" /* memory allocation */
#include "ast/root.h" /* primitive types */
#include "syntax/declaration/declaration.h" /* declarations */
#include "syntax/expression/unary.h" /* unary expressions */
#include "mparser.h" /* parser */

    /* functions */
/**
 * Returns a token kind for a lexical type
 *
 * @param[in] value Pointer to the type
 * 
 * @return The token kind
 */
int ast_type_to_token_kind(ast_type* value) {
    switch (value->kind) {
        case AST_TYPE_ENUM:
            return TOKEN_ENUM_NAME;

        case AST_TYPE_FUNCTION:
            return TOKEN_FUNCTION_NAME;

        case AST_TYPE_PRIMITIVE:
            return TOKEN_PRIMITIVE_NAME;

        case AST_TYPE_STRUCTURE:
            return TOKEN_STRUCTURE_NAME;

        otherwise_error
    }
}


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
ast_type* ast_type_merge_prioritized(ast_type* a, ast_type* b) {
    if (ast_type_is_equal(a, b)) {
        return a;
    }

    /* number implicit casting */
    if (ast_type_is_number(a) && ast_type_is_number(b)) {
        return a;
    }

    return NULL;
}


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
ast_type* ast_type_merge_extend(ast_type* a, ast_type* b) {
    if (ast_type_is_equal(a, b)) {
        return a;
    }

    /* number implicit casting */
    if (ast_type_is_number(a) && ast_type_is_number(b)) {
        int a_index = a->u_primitive - primitive_list.data;
        int b_index = b->u_primitive - primitive_list.data;

        /* find the bigger number */
        if (a_index > 1 && b_index > 1) {
            if (a->u_primitive->size > b->u_primitive->size) {
                return a;
            } else if (a->u_primitive->size < b->u_primitive->size) {
                return b;
            } 
            
            /* equal sizes, find more precise number */
            else {
                if (a_index > b_index) {
                    return a;
                } else {
                    return b;
                }
            }
        }
    }

    return NULL;
}


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
bool ast_type_can_merge(ast_type* a, ast_type* b) {
    if (ast_type_is_equal(a, b)) {
        return true;
    }

    if (ast_type_is_number(a) && ast_type_is_number(b)) {
        return true;
    }

    return false;
}


/**
 * Compares two lexical types
 * and returns the comparison result
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return true if types are equal
 */
bool ast_type_is_equal(ast_type* a, ast_type* b) {
    /* compare values */
    if (a->kind != b->kind) return false;

    /*
     * Because types store pointers to global
     * declarations, pointer comparison is
     * acceptable in this case
     */
    if (a->u__any != b->u__any) return false;

    /* compare levels */
    if (a->level_list.size != b->level_list.size) return false;
    iterate_array(i, a->level_list.size) {
        if (a->level_list.data[i].kind != b->level_list.data[i].kind) return false;
    }

    return true;
}


/**
 * Checks the kind of the last type level
 * and returns true if the level list is 
 * not empty and the last level kind is equal 
 * to the specified kind parameter 
 * 
 * @param value Pointer to the type
 * @param kind  The expected last level kind
 * 
 * @return true if the level list is not empty and
 *          the last level kind is equal to specified kind
 */
bool ast_type_last_level_is(ast_type* value, ast_type_level_kind kind) {
    /* check the last level */
    if (arraylist_is_empty(value->level_list)) return false;
    return arraylist_last(value->level_list).kind == kind;
}


/**
 * Checks if a type is of primitive kind
 * and is not an array or a pointer
 * 
 * @param value Pointer to the type
 * 
 * @return true if the type is primitive
 */
bool ast_type_is_primitive(ast_type* value) {
    /* check kind */
    if (value->kind != AST_TYPE_PRIMITIVE) return false;

    /* check levels */
    if (!arraylist_is_empty(value->level_list)) return false;

    return true;
}


/**
 * Checks if type is a boolean
 * 
 * @param value Pointer to the type
 * 
 * @return true if the type is a boolean
 */
bool ast_type_is_boolean(ast_type* value) {
    if (!ast_type_is_primitive(value)) return false;

    /* check primitive type index */
    int index = value->u_primitive - primitive_list.data;
    if (index != 1) return false;

    return true;
}


/**
 * Checks if numerical operations 
 * can be applied to a type
 * 
 * @param value Pointer to the type
 * 
 * @return true if the type is a primitive number
 */
bool ast_type_is_number(ast_type* value) {
    if (!ast_type_is_primitive(value)) return false;

    /* check primitive type index */
    int index = value->u_primitive - primitive_list.data;
    if (index < 2 /* void or bool */) return false;

    return true;
}


/**
 * Initializes a type with specified kind and value
 * 
 * @param[in] kind Kind of the type
 * @param[in] value Value of the type
 */
void ast_type_init(ast_type* type, ast_type_kind kind, void* value) {
    /* initialize a new type */
    type->kind = kind;
    type->u__any = value;
    arl_init_empty(ast_type_level, type->level_list);
}


/**
 * Creates a new type with specified kind
 * and unitialized value
 * 
 * @param[in] kind Kind of the type
 * @param[in] value Value of the type
 */
ast_type* ast_type_new(ast_type_kind kind, void* value) {
    /* create a new type without value */
    ast_type* type = allocate(ast_type);
    ast_type_init(type, kind, value);

    return type;
}


/**
 * Returns the default numerical expression
 * type for the specified kind
 * 
 * @param[in] kind Kind of the numerical expression
 * 
 * @return Primitive type (float or int)
 */
ast_type* ast_type_of_ex_number(ex_number_kind kind) {
    /**
     * @todo
     * Signed and unsigned integers
     * Integer type conversion
     * Check for max integer value when assigning a constant expression
     */

    /* create a primitive value */
    ast_type_primitive* primitive;
    
    /* check number kind */
    switch (kind) {
        case EX_N_FLOAT:
            primitive = &primitive_list.data[7];
            break;

        case EX_N_INT:
            primitive = &primitive_list.data[4];
            break;

        otherwise_error
    }

    /* return a new type */
    return ast_type_new(AST_TYPE_PRIMITIVE, primitive);;
}


/**
 * Returns the default boolean expression type
 * 
 * @return Primitive bool type
 */
ast_type* ast_type_of_boolean() {
    /* return a new boolean type */
    return ast_type_new(AST_TYPE_PRIMITIVE, &primitive_list.data[1]);
}

/**
 * Returns the default character expression type
 * 
 * @return Primitive byte type
 */
ast_type* ast_type_of_character() {
    /* return a new byte type */
    return ast_type_new(AST_TYPE_PRIMITIVE, &primitive_list.data[2]);
}

/**
 * Returns the default string literal type
 * 
 * @return byte[] type
 */
ast_type* ast_type_of_string() {
    /* create a new byte type */
    ast_type* type = ast_type_of_character();

    /* add array level */
    ast_type_level level;
    level.kind = AT_LEVEL_ARRAY;
    arl_add(ast_type_level, type->level_list, level);

    return type;
}


/**
 * Returns the type of the specified
 * constructor expression
 * 
 * @param[in] ex The constructor expression
 * 
 * @return Type of the constructor expression
 */
ast_type* ast_type_of_ex_constructor(ex_constructor* ex) {
    /* clone the base type */
    ast_type* type = ast_type_clone(*ex->type);

    /* add array or pointer level */
    ast_type_level level;
    if (ex->is_array) {
        level.kind = AT_LEVEL_ARRAY;
        arl_add(ast_type_level, type->level_list, level);
    } else if (ex->is_new) {
        level.kind = AT_LEVEL_POINTER;
        arl_add(ast_type_level, type->level_list, level);
    }

    return type;
}


/**
 * Clones a type (src)
 * to another type (dest)
 * 
 * @param[in] dest Pointer to the destination type
 * @param[in] src  The source type
 */
void ast_type_clone_to(ast_type* dest, ast_type src) {
    /* initialize the destination type */
    dest->kind = src.kind;
    dest->u__any = src.u__any;

    /* copy levels */
    arl_init_empty(ast_type_level, dest->level_list);
    iterate_array(i, src.level_list.size) {
        arl_add(ast_type_level, dest->level_list, src.level_list.data[i]);
    }
}

/**
 * Clones the type
 * 
 * @param[in] value The type to be cloned
 * 
 * @return Copy of the type
 */
ast_type* ast_type_clone(ast_type value) {
    /* create a new type */
    ast_type* result = allocate(ast_type);

    /* clone to it */
    ast_type_clone_to(result, value);

    return result;
}


/**
 * Returns a string representation of a type
 * 
 * @param[in] value Pointer to the type
 * 
 * @return The string representation of the type
 */
char* ast_type_to_string(ast_type* value) {
    /* create a buffer */
    char* name;
    char* buffer;
    size_t buffer_size = 1;

    /* assign the name */
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

    /* calculate size for name */
    size_t name_size = strlen(name);
    buffer_size += name_size;

    /* calculate size for levels */
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

    /* allocate buffer */
    buffer = allocate_array(char, buffer_size);
    buffer[buffer_size - 1] = 0;
    strcpy(buffer, name);

    /* add levels */
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