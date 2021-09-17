/**
 * @file union.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-04
 * 
 *  Union utilities
 */
    /* header guard */
#ifndef CTOOL_MISC_UNION_H
#define CTOOL_MISC_UNION_H

    /* includes */
#include "misc/memory.h" /* memory allocation */

    /* defines */
/**
 * Simple union initialization expression
 * which must be followed by a property initialization
 * 
 * @param[in] value_ The structure with kind and a union
 * @param[in] kind_  Kind of the union
 */
#define u_init(value_, kind_) value_.kind = kind_; value_

/**
 * Heap-allocated union initialization expression
 * which must be followed by a property initialization
 * 
 * @param[in] value_ The structure with kind and a union
 * @param[in] kind_  Kind of the union
 */
#define u_init_ptr(value_, kind_) value_ = allocate((*value_)); value_->kind = kind_; value_


#endif /* CTOOL_MISC_UNION_H */