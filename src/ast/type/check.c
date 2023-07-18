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
#include "syntax/declaration/declaration.h"

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
bool ast_type_last_level_is(ast_type* value, ast_type_level_kind level) {
    if (arraylist_is_empty(value->level_list)) return false;
    return arraylist_last(value->level_list).kind == level;
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

bool ast_type_is_constant_array(ast_type* value) { 
    if (ast_type_is_array(value)) {
        return arraylist_last(value->level_list).u_array_size != NULL;
    } else return false;
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

bool ast_type_is_pp_integer(ast_type* value) { 
    if (!ast_type_is_pp(value)) return false;
    return ast_type_primitive_is_integer(value->u_primitive); 
}

bool ast_type_is_pp_boolean(ast_type* value) { 
    if (!ast_type_is_pp(value)) return false; 
    return ast_type_primitive_is_boolean(value->u_primitive); 
}


/**
 * Advanced type comparison function
 * which ensures that a value of given type
 * can be assigned to a variable of specified type
 * without truncation or precision loss
 * 
 * @param[in] assignee The variable
 * @param[in] value The value to be checked for being suitable
 */
#define merged_assignee_is(range) (primitive_index_in_range(i_assignee, range))
#define merged_value_is(range)    (primitive_index_in_range(i_value, range))

    bool ast_type_can_assign(ast_type* assignee, ast_type* value) {
        if (ast_type_is_equal(assignee, value)) {
            return true;
        }

        if (ast_type_is_pp_number(assignee) && ast_type_is_pp_number(value)) {
            index_t i_assignee = ast_type_primitive_get_index(assignee->u_primitive);
            index_t i_value = ast_type_primitive_get_index(value->u_primitive);

            if (merged_assignee_is(INTEGER)  && merged_value_is(FLOATING)) {
                return false;
            }
            if (merged_assignee_is(UNSIGNED) && merged_value_is(SIGNED)) {
                return false;
            }
            if (assignee->u_primitive->capacity <= value->u_primitive->capacity) {
                return false;
            }

            return true;
        }

        return false;
    }

#undef merged_assignee_is
#undef merged_value_is


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
 * @todo make constant expression type inference compliant with this function
 * 
 * @param[in] a Pointer to the first type
 * @param[in] b Pointer to the second type
 * 
 * @return(merge) Merge result, or NULL if types are incompatible
 * 
 * @return(can_merge) true if the types are compatible
 */
#define merged_a_is(range) (primitive_index_in_range(i_a, range))
#define merged_b_is(range) (primitive_index_in_range(i_b, range))
#define merge_swap()       { ast_type* tmp = a; a = b; b = tmp; }

    bool ast_type_can_merge(ast_type* a, ast_type* b) {
        return ast_type_merge_extend(a, b) != NULL;
    }

    ast_type* ast_type_merge_extend(ast_type* a, ast_type* b) {
        /* handle generics */
        while (a->kind == AST_TYPE_GENERIC) {
            a = dc_generic_get_impl(a);
        }
        while (b->kind == AST_TYPE_GENERIC) {
            b = dc_generic_get_impl(b);
        }

        if (ast_type_is_equal(a, b)) {
            return a;
        }

        if (ast_type_is_pp_number(a) && ast_type_is_pp_number(b)) {
            index_t i_a = ast_type_primitive_get_index(a->u_primitive);
            index_t i_b = ast_type_primitive_get_index(b->u_primitive);

            /* A is always promoted to B */
            {
                /* swap cases */
                if (merged_a_is(FLOATING)  && merged_b_is(INTEGER))  merge_swap();
                if (merged_a_is(SIGNED)    && merged_b_is(UNSIGNED)) merge_swap();
            }
            
            /* re-cast A to match the type of B if needed */
            int recast_range_start = 0;
            int recast_range_end   = 0;
            if (merged_a_is(UNSIGNED) && merged_b_is(SIGNED)) {
                recast_range_start = primitive_index_min_range(SIGNED);
                recast_range_end   = primitive_index_min_range(SIGNED);
            }
            if (merged_a_is(INTEGER)  && merged_b_is(FLOATING)) {
                recast_range_start = primitive_index_min_range(FLOATING);
                recast_range_end   = primitive_index_min_range(FLOATING);
            }
            iterate_range_single(i, recast_range_start, recast_range_end) {
                if (primitive_list.data[i].capacity >= a->u_primitive->capacity) {
                    a = ast_type_new(AST_TYPE_PRIMITIVE, &primitive_list.data[i]);
                }
            }

            /* straightforward upcasting by capacity */
            if (a->u_primitive->capacity > b->u_primitive->capacity) {
                return a;
            } else {
                return b;
            }
        }

        return NULL;
    }

#undef merged_a_is
#undef merged_b_is
#undef merge_swap

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
    /* handle generics */
    if (a->kind == AST_TYPE_GENERIC) {
        a = dc_generic_get_impl(a);
        bool result = ast_type_is_equal(a, b);

        arraylist_free(ast_type_level)(&a->level_list);
        free(a);
        return result;
    }
    if (b->kind == AST_TYPE_GENERIC) {
        b = dc_generic_get_impl(b);
        bool result = ast_type_is_equal(a, b);

        arraylist_free(ast_type_level)(&b->level_list);
        free(b);
        return result;
    }

    if (a->kind != b->kind) return false;
    if (a->level_list.size != b->level_list.size) return false;
    iterate_array(i, a->level_list.size) {
        if (a->level_list.data[i].kind != b->level_list.data[i].kind) return false;
    }

    /* special case - char and byte are interchangeable */
    if (a->kind == AST_TYPE_PRIMITIVE && b->kind == AST_TYPE_PRIMITIVE) {
        index_t index_a = ast_type_primitive_get_index(a->u_primitive);
        index_t index_b = ast_type_primitive_get_index(b->u_primitive);
        
        //todo better workaround: declare char as alias to byte, and uchar as alias to ubyte
        if ((index_a == PRIMITIVE_INDEX_BYTE && index_b == PRIMITIVE_INDEX_CHAR)
        || (index_a == PRIMITIVE_INDEX_CHAR && index_b == PRIMITIVE_INDEX_BYTE)
        || (index_a == PRIMITIVE_INDEX_UBYTE && index_b == PRIMITIVE_INDEX_UCHAR)
        || (index_a == PRIMITIVE_INDEX_UCHAR && index_b == PRIMITIVE_INDEX_UBYTE)) {
            return true;
        }
    }

    if (a->u__any != b->u__any) return false;

    return true;
}

/**
 * TODO:
 * 
 * Full refactor of (at least) the primitive type and constant expression modules
 * 
 * Make use of Kotlin script code generation for Primitive type system (make several lists, sort them by capacity and then sort them by SPECIAL-ORDER)
 */