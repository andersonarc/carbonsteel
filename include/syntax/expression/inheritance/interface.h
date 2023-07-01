/**
 * @file interface.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-18
 * 
 *  Expression inheritance interface definitions
 * 
 *  Inheritance interface (API) is a layer of abstraction
 *  which allows to eliminate code duplication and make
 *  the code more readable.
 * 
 *  Inheritance API macro:
 * @todo think about naming. need a convention.
 *      iapi_init_union_from_expression = expression_inheritance
 *      iapi_init_union_from_extern     = extern_inheritance
 *          - initializes data from a parent and a union kind
 * 
 *      iapi_init_from_parent = expression_inheritance
 *          - initializes data from a parent
 * 
 *      iapi_init_with_kind = self_inheritance
 *          - initializes data from a parent of same type and a kind value
 * 
 *      iapi_append_level = self_inheritance_with
 *      iapi_append_level_from_expression = self_inheritance_with_ex
 *          - initializes data from a parent of same type and appends a level value
 * 
 *      iapi_append_op = self_inheritance
 *          - initializes data from a parent of same type and appends a constant (operator) value
 * 
 *      iapi_binary_* = self_inheritance_with_ex_and
 *          - initializes a binary expression from a parent of same type,
 *              another expression and an operator value
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_INTERFACE_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_INTERFACE_H

    /* includes */
#include "syntax/expression/inheritance/core.h" /* inheritance core */
#include "syntax/expression/inheritance/advanced.h" /* advanced inheritance */

    /* defines */
/**
 * @param[in] ex_type Type of the initialized expression without prefix
 * @param[in] action_name The inheritance action name which must be unique for given ex_type
 * @param[in] parent_ex_type Type of the parent expression without prefix
 */

/**
 * Expression structure declaration
 * with a generic data field and
 * heap-allocated expression properties
 */
#define declare_expression(ex_type)          \
    struct ex_##ex_type {                    \
        expression_properties* properties;   \
        ex_##ex_type##_data data;            \
    };


/**
 * Union inheritance patterns that initializes expression data
 * and its kind from an extern parameter or a parent expression
 * 
 * @param[in] u_prefix Expression kind prefix (without EX_)
 * @param[in] u_name_upper Expression kind in upper case (without prefix)
 * @param[in] u_name_lower Expression kind in upper case (without prefix)
 * @param[in] value_type Type of the value parameter
 */
#define iapi_init_union_from_expression(u_prefix, ex_type, parent_ex_type, u_name_upper, u_name_lower) \
    data_expression_inheritance(ex_type, parent_ex_type) {                                      \
        this->kind = EX_##u_prefix##_##u_name_upper;                                            \
        this->u_##u_name_lower = parent;                                                        \
    }                                                                                           \
    properties_expression_inheritance(ex_type, parent_ex_type) {                                \
        /* no-op because properties are copied in expression inheritance */                     \
    }

#define iapi_init_union_from_dereferenced_expression(u_prefix, ex_type, parent_ex_type, u_name_upper, u_name_lower) \
    data_expression_inheritance(ex_type, parent_ex_type) {                                                          \
        this->kind = EX_##u_prefix##_##u_name_upper;                                                                \
        this->u_##u_name_lower = *parent;                                                                           \
    }                                                                                                               \
    properties_expression_inheritance(ex_type, parent_ex_type) {                                                    \
        /* no-op because properties are copied in expression inheritance */                                         \
    }

#define iapi_init_union_from_extern(u_prefix, ex_type, value_type, u_name_upper, u_name_lower) \
    data_extern_inheritance(ex_type, u_name_lower, value_type) {                        \
        this->kind = EX_##u_prefix##_##u_name_upper;                                    \
        this->u_##u_name_lower = value;                                                 \
    }                                                                                   \
    properties_extern_inheritance(ex_type, u_name_lower, value_type)


/**
 * Expression inheritance pattern for wrapping
 * the value of a parent expression without
 * making any modifications to properties
 */
#define iapi_init_from_parent(ex_type, parent_ex_type)                           \
    properties_expression_inheritance(ex_type, parent_ex_type) {            \
        /* no-op because properties are copied in expression inheritance */ \
    }                                                                       \
    data_expression_inheritance(ex_type, parent_ex_type)


/**
 * Kind inheritance macro that
 * adds a kind to a parent expression
 * of the same type as this expression
 * 
 * @param[in] k_prefix Prefix of the kind (without EX_)
 * @param[in] k_upper Kind in upper case (without prefix and EX_)
 */
#define iapi_init_with_kind(k_prefix, ex_type, action_name, k_upper) \
    data_self_inheritance(ex_type, action_name) {                 \
        *this = *parent;                                          \
        this->kind = EX_##k_prefix##_##k_upper;                   \
    }                                                             \
    properties_self_inheritance(ex_type, action_name)


/**
 * Level inheritance pattern that appends
 * a new value wrapped in a level structure
 * to a field of the expression named "level_list"
 * 
 * @param[in] l_prefix Prefix of the level kind (without EX_)
 * @param[in] l_kind_lower Level kind in lower case (without prefix and EX_) and name of the parameter
 * @param[in] l_kind_upper Level kind in upper case (without prefix and EX_)
 * @param[in] l_type Type of the level and name of the level structure field to assign the value to
 */
#define iapi_append_level(l_prefix, ex_type, l_kind_lower, l_kind_upper, l_type) \
    data_self_inheritance_with(ex_type, l_kind_lower, l_type l_kind_lower) {     \
        *this = *parent;                                                         \
        ex_##ex_type##_level level = {                                           \
            .kind = EX_##l_prefix##_##l_kind_upper,                              \
            .u_##l_kind_lower = l_kind_lower                                     \
        };                                                                       \
        arl_add(ex_##ex_type##_level, this->level_list, level);                  \
    }                                                                            \
    properties_self_inheritance_with(ex_type, l_kind_lower, l_type l_kind_lower)

#define iapi_append_level_from_expression(l_prefix, ex_type, l_kind_lower, l_kind_upper, l_type) \
    data_self_inheritance_with_ex(ex_type, l_kind_lower, l_type, l_kind_lower) {                 \
        *this = *parent;                                                                         \
        ex_##ex_type##_level level = {                                                           \
            .kind = EX_##l_prefix##_##l_kind_upper,                                              \
            .u_##l_kind_lower = l_kind_lower                                                     \
        };                                                                                       \
        arl_add(ex_##ex_type##_level, this->level_list, level);                                  \
    }                                                                                            \
    properties_self_inheritance_with_ex(ex_type, l_kind_lower, l_kind_lower)


/**
 * Operator inheritance pattern that appends
 * a new constant (operator) value to a field 
 * of the expression named "op_list"
 * 
 * @param[in] op_prefix Prefix 
 */
#define iapi_append_operator(op_prefix, ex_type, action_name, op_upper)    \
    data_self_inheritance(ex_type, action_name) {                          \
        *this = *parent;                                                   \
        arl_add(op_##ex_type, this->op_list, OP_##op_prefix##_##op_upper); \
    }                                                                      \
    properties_self_inheritance(ex_type, action_name)


/**
 * Binary expression properties inheritance interface
 * which utilizes common inheritance functions to
 * avoid code duplication for similar kinds
 * of binary expressions
 * 
 * @param[in] common_name Name of the common inheritance function
 * @param[in] operator Binary operator name (without EX_ and prefix)
 * @param[in] ex The expression value
 * @param[in] parent_ex The parent expression value
 * @param[in] other Expression parameter value
 */
#define iapi_binary_properties(action_name)                                                \
    properties_self_inheritance_with_ex_and(binary, action_name, other, op_binary operator)

#define iapi_binary_data()                                                                 \
    data_self_inheritance_with_ex_and(binary, operation, binary, other, op_binary operator)

#define iapi_binary_properties_from_common(action_name, common_name, operation_range) \
    iapi_binary_properties(action_name) {                                             \
        ex_binary_inherit_properties_on_##common_name(this, parent, other, operator); \
                                                                                      \
        iset_constant(binary_##action_name) {                                         \
            ex_constant_inherit_binary_##operation_range(this->constant, parent->constant, lookup_op_binary(action_name), other->constant); \
        }                                                                             \
    }

#define iapi_inherit_binary(action_name, operator, ex, parent_ex, other)                                                 \
    ex_binary_inherit_data_on_operation(&ex.data, &parent_ex.data, &other.data, OP_B_##operator);                        \
    ex_binary_inherit_properties_on_##action_name(ex.properties, parent_ex.properties, other.properties, OP_B_##operator);


    /* inheritance annotations */
/**
 * Assignment annotations that are used
 * to indicate mutation of type or a value
 * of expression properties
 */
#define iset_type(_)
#define iset_constant(_)
#define iset_constant_origin(value, _) if ((value)->constant.origin != NULL)
#define iset_type_and_constant(_)

/**
 * Required annotations for conditional
 * constant expression evaluation
 * 
 * If the constant expression is unable to be evaluated,
 * this->constant is initialized as dynamic.
 */
#define irequire_constant(mode) _irequire_constant_impl_##mode
#define _irequire_impl(condition)                  \
    if (!(condition)) {                            \
        ex_constant_dynamic(&this->constant); \
    } else

#define _irequire_constant_impl_(value)         _irequire_impl((value)->constant.kind != EX_C_DYNAMIC)
#define _irequire_constant_impl_integer(value)  _irequire_impl(ex_constant_is_integer(&(value)->constant))
#define _irequire_constant_impl_signed(value)   _irequire_impl(ex_constant_is_signed(&(value)->constant))
#define _irequire_constant_impl_unsigned(value) _irequire_impl(ex_constant_is_unsigned(&(value)->constant))

/**
 * Expection annotations that are used
 * to indicate a type check for either 
 * parameter, parent, or both
 */
#define iexpect_parameter(_)
#define iexpect_parent(_)
#define iexpect_parent_and_parameter(_)


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_INTERFACE_H */