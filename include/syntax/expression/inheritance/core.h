/**
 * @file core.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-18
 * 
 *  Expression inheritance core macro definitions
 * 
 *  Generally, there are three origins of inheritance
 *  (self, extern and expression) and two types of
 *  parameters (extern and expression)
 * 
 *  Inheritance core consists of the following combinations:
 *      << TYPE         (extern inheritance)
 *      <-< EXPRESSION  (expression inheritance)
 *      < ACTION        (self inheritance)
 * 
 *  For each of those combination, several macro are defined:
 *      [data/properties]_XXX_inheritance - definition (implementation) of the inheritance functions
 *      XXX_inheritance - declaration of the inheritance functions
 *      inherit_XXX - invocation of the inheritance functions
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_CORE_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_CORE_H

    /* includes */
#include "syntax/expression/properties.h" /* expression properties */
#include "ctool/macro.h" /* macro concatenation */
#include "misc/memory.h" /* memory allocation */

    /* defines */
/**
 * @param[in] ex The expression value
 * @param[in] ex_type Type of the initialized expression without prefix
 * @param[in] action_name The inheritance action name which must be unique for given ex_type
 */

/**
 * Extern inheritance macro to initialize
 * an expression from any value
 * 
 * Used in Basic expression initialiation
 * to convert wrap lexical primitives
 * into an expression.
 * 
 * @param[in] value The extern value
 * @param[in] value_type Type of the extern value
 */
    /* invocation */
#define inherit_extern(ex_type, action_name, ex, value)                     \
    ex.properties = allocate(expression_properties);                        \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, value);          \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, value);

    /* declaration */
#define extern_inheritance(ex_type, action_name, value_type)        \
    data_extern_inheritance(ex_type, action_name, value_type);      \
    properties_extern_inheritance(ex_type, action_name, value_type);

    /* definition */
#define data_extern_inheritance(ex_type, action_name, value_type) \
    void ex_##ex_type##_inherit_data_on_##action_name(            \
        ex_##ex_type##_data* this,                                \
        value_type value)

    /* definition */
#define properties_extern_inheritance(ex_type, action_name, value_type) \
    void ex_##ex_type##_inherit_properties_on_##action_name(            \
        expression_properties* this,                                    \
        value_type value)


/**
 * Expression inheritance macro to iniitalize
 * from an expression of different type
 * 
 * Used as first initialization for most of
 * the expressions.
 * 
 * @param[in] parent_ex_type Type of the parent expression without prefix
 * @param[in] parent_ex The parent expression value
 */
    /* invocation */
#define inherit_expression(ex_type, parent_ex_type, ex, parent_ex)                 \
    ex.properties = parent_ex.properties;                                          \
    ex_##ex_type##_inherit_data_on_ex_##parent_ex_type(&ex.data, &parent_ex.data); \
    ex_##ex_type##_inherit_properties_on_ex_##parent_ex_type(ex.properties);

    /* declaration */
#define expression_inheritance(ex_type, parent_ex_type)        \
    data_expression_inheritance(ex_type, parent_ex_type);      \
    properties_expression_inheritance(ex_type, parent_ex_type);

    /* definition */
#define data_expression_inheritance(ex_type, parent_ex_type) \
    void ex_##ex_type##_inherit_data_on_ex_##parent_ex_type( \
        ex_##ex_type##_data* this,                           \
        ex_##parent_ex_type##_data* parent)

    /* definition */
#define properties_expression_inheritance(ex_type, parent_ex_type) \
    void ex_##ex_type##_inherit_properties_on_ex_##parent_ex_type( \
        expression_properties* this)


/**
 * Self-inheritance macro for initializing
 * an expression from itself
 * 
 * Although it may seem useless at first glance,
 * self-inheritance is used for unary operations
 * such as increment or dereference.
 * 
 * @param[in] parent_ex The parent expression value
 */
#define inherit_self(ex_type, action_name, ex, parent_ex)                                   \
    ex.properties = parent_ex.properties; /* todo hotfix, could be invalid */               \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent_ex.data);                \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent_ex.properties);

    /* declaration */
#define self_inheritance(ex_type, action_name)        \
    data_self_inheritance(ex_type, action_name);      \
    properties_self_inheritance(ex_type, action_name);

    /* definition */
#define data_self_inheritance(ex_type, action_name)    \
    void ex_##ex_type##_inherit_data_on_##action_name( \
        ex_##ex_type##_data* this,                     \
        ex_##ex_type##_data* parent)

    /* definition */
#define properties_self_inheritance(ex_type, action_name)    \
    void ex_##ex_type##_inherit_properties_on_##action_name( \
        expression_properties* this,                         \
        expression_properties* parent)
        

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_CORE_H */