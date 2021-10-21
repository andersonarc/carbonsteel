/**
 * @file check.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-20
 * 
 *  Lexical type comparison and check
 *  function implementations
 */
    /* includes */
#include "ast/type/check.h" /* this */

#include "ast/type/primitive.h" /* primitive types */

    /* internal functions */
/**
 * Checks whether the last type level
 * is of specified kind or not
 * 
 * @param value Pointer to the type
 * @param level Expected kind of the last type level
 * 
 * @return true if the last type level 
 *          is of specified kind
 */
bool ast_type_last_level_is(ast_type* value, ast_type_level level) {
    if (arraylist_is_empty(value->level_list)) return false;
    return arraylist_last(value->level_list) == level;
}

//todo @return With Capital Letter

/**
 * Performs operations with the so-called
 * primitive index of the primitive type
 * 
 * [0] is void
 * [1] is bool
 * [2..5] are integers
 * [6..7] are floats
 * 
 * @param value Pointer to the primitive type
 * 
 * @return(get) The primitive index
 * 
 * @return(is)  True if the primitive index 
 *                  matches the expected range
 */
int ast_type_get_primitive_index(ast_type_primitive* value) { 
    return value - primitive_list.data; 
}

bool ast_type_primitive_is_void(ast_type_primitive* value) { 
    return ast_type_get_primitive_index(value) == PRIMITIVE_INDEX_VOID;
}

bool ast_type_primitive_is_boolean(ast_type_primitive* value) {
    return ast_type_get_primitive_index(value) == PRIMITIVE_INDEX_BOOL;
}

bool ast_type_primitive_is_number(ast_type_primitive* value) { 
    return ast_type_get_primitive_index(value) > PRIMITIVE_INDEX_BOOL;
}


    /* functions */
/**
 * Type check functions
 * 
 *  ast_type_is_array   - type is an array
 *  ast_type_is_pointer - type is a pointer
 *  ast_type_is_plain   - type is plain (not array or pointer)
 * 
 *  Plain and primitive types are referred as [pp]
 * 
 *  ast_type_is_pp        - type is [p]lain and [p]rimitive
 *  ast_type_is_pp_number - type is [pp] and a number
 *  ast_type_is_pp_bool   - type is [pp] and a boolean
 * 
 * @param[in] value Pointer to the type
 * 
 * @return true if the type matches the specified condition
 */
bool ast_type_is_array(ast_type* value) { 
    return ast_type_last_level_is(value, AT_LEVEL_ARRAY);
}

bool ast_type_is_pointer(ast_type* value) {
    return ast_type_last_level_is(value, AT_LEVEL_POINTER);
}

bool ast_type_is_single_pointer(ast_type* value) {
    return value->level_list.size == 1 && ast_type_is_pointer(value);
}

bool ast_type_is_plain(ast_type* value) {
    return arraylist_is_empty(value->level_list);
}

bool ast_type_is_pp(ast_type* value) { 
    return value->kind == AST_TYPE_PRIMITIVE 
        && ast_type_is_plain(value); 
}

bool ast_type_is_pp_number(ast_type* value) { 
    if (!ast_type_is_pp(value)) return false;
    return ast_type_primitive_is_number(value->u_primitive); 
}

bool ast_type_is_pp_boolean(ast_type* value) { 
    if (!ast_type_is_pp(value)) return false; 
    return ast_type_primitive_is_boolean(value->u_primitive); 
}


/**
 * Advanced type comparison functions
 * which support implicit number casts
 * 
 * Implicit number casts are performed
 * as described below:
 * 
 * When merge is [prioritized], the second type is possibly
 * !truncated! and implicitly casted to the first type
 * 
 * When merge is [extend], the smaller type is casted 
 * to the bigger type, so truncation is impossible
 * 
 * @todo Remove [prioritized] merges, use a workaround for constant expressions
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return(merge) Merge result, or NULL if types are incompatible
 * 
 * @return(can_merge) true if the types are compatible
 */
ast_type* ast_type_merge_prioritized(ast_type* a, ast_type* b) {
    return ast_type_can_merge(a, b) ? a : NULL;
}

ast_type* ast_type_merge_extend(ast_type* a, ast_type* b) {
    if (ast_type_is_equal(a, b)) {
        return a;
    }

    /* number implicit casting */
    if (ast_type_is_pp_number(a) && ast_type_is_pp_number(b)) {
        int a_index = ast_type_get_primitive_index(a->u_primitive);
        int b_index = ast_type_get_primitive_index(b->u_primitive);

         /** @todo ast_type_is_integer() */

        /* if both types are integer/float, find the bigger one */
        if ((a_index <  PRIMITIVE_INDEX_FLOAT && b_index <  PRIMITIVE_INDEX_FLOAT) ||
            (a_index >= PRIMITIVE_INDEX_FLOAT && b_index >= PRIMITIVE_INDEX_FLOAT)) {
            if (a->u_primitive->size > b->u_primitive->size) {
                return a;
            } else if (a->u_primitive->size < b->u_primitive->size) {
                return b;
            } 
        } else {
            if (a_index > PRIMITIVE_INDEX_FLOAT) {
                /* a is a float */

                /* halven the size to simplify computations */
                int f_capacity = a->u_primitive->size / 2;

                if (b->u_primitive->size > f_capacity) {
                        /* integer is too big, explicit cast required */
                    /* case INT(4)/LONG(8) > FLOAT(2)  */
                    /* case LONG(8)        > DOUBLE(4) */
                    return NULL;
                } else {
                        /* float can contain the integer */
                    /* case BYTE(1)/SHORT(2) <= FLOAT(2)/DOUBLE(4) */
                    /* case INT(4)           <= DOUBLE(4)          */
                    return a;
                }
            } else {
                /* b is a float */

                /* do the same */
                int f_capacity = b->u_primitive->size / 2;
                if (a->u_primitive->size > f_capacity) {
                    return NULL;
                } else {
                    return b;
                }
            }
        }
    }

    return NULL;
}

bool ast_type_can_merge(ast_type* a, ast_type* b) {
    /** @todo advanced can_merge check based on extend merge */
    return ast_type_is_equal(a, b) || (ast_type_is_pp_number(a) && ast_type_is_pp_number(b));
}


/**
 * Simple deep type comparison
 * without any implicit casting
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return true if the types are equal
 */
bool ast_type_is_equal(ast_type* a, ast_type* b) {
    if (a->kind != b->kind) return false;
    if (a->u__any != b->u__any) return false;
    if (a->level_list.size != b->level_list.size) return false;

    iterate_array(i, a->level_list.size) {
        if (a->level_list.data[i] != b->level_list.data[i]) return false;
    }

    return true;
}