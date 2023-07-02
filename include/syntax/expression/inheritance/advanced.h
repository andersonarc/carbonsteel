/**
 * @file advanced.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-18
 * 
 *  Advanced expression inheritance macro definitions
 * 
 *  Unlike the core inheritance, advanced inheritance
 *  has more complex schemes (specifiers in [] can be omitted):
 *      < [ACTION] TYPE       (self inheritance + parameter)
 *      < [ACTION] EXPRESSION (self inheritance + expression)
 *      < [ACTION] EXPRESSION + TYPE (self inheritance + expression + parameter)
 *      < [ACTION] EXPRESSION + EXPRESSION (self inheritance + expression + expression)
 *      < [ACTION] {SOURCE_TYPE -> TYPE} (self inheritance + computed parameter)
 * 
 *  For each of those combination, several macro are defined:
 *      [data/properties]_XXX_inheritance - definition (implementation) of the inheritance functions
 *      XXX_inheritance - declaration of the inheritance functions
 *      inherit_XXX - invocation of the inheritance functions
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_ADVANCED_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_ADVANCED_H

    /* includes */
#include "syntax/expression/properties.h" /* expression properties */
#include "ctool/macro.h" /* macro concatenation */
#include "misc/memory.h" /* memory allocation */

    /* defines */
/**
 * @param[in] ex The expression value
 * @param[in] ex_type Type of the initialized expression without prefix
 * @param[in] action_name The inheritance action name which must be unique for given ex_type
 * @param[in] parent_ex The parent expression value
 * @param[in] parameter Parameter declaration (with or without name)
 * @param[in] parameter_ex_type Parameter expression type (without prefix and name)
 * @param[in] parameter_ex_name Parameter expression name
 * @param[in] argument Parameter value
 * @param[in] argument_ex Parameter expression value
 */

/**
 * Self-inheritance macro for initializing
 * an expression from itself while accepting
 * an additional parameter
 * 
 * It is used for postfix operations.
 */
    /* invocation */
#define inherit_self_with(ex_type, action_name, ex, parent_ex, argument)                              \
    ex.properties = parent_ex.properties; /* todo hotfix, could be invalid */                         \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent_ex.data, argument);                \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent_ex.properties, argument);

    /* declaration */
#define self_inheritance_with(ex_type, action_name, parameter)        \
    data_self_inheritance_with(ex_type, action_name, parameter);      \
    properties_self_inheritance_with(ex_type, action_name, parameter);

    /* definition */
#define data_self_inheritance_with(ex_type, action_name, parameter) \
    void ex_##ex_type##_inherit_data_on_##action_name(              \
        ex_##ex_type##_data* this,                                  \
        ex_##ex_type##_data* parent,                                \
        parameter)

    /* definition */
#define properties_self_inheritance_with(ex_type, action_name, parameter) \
    void ex_##ex_type##_inherit_properties_on_##action_name(              \
        expression_properties* this,                                      \
        expression_properties* parent,                                    \
        parameter)


/**
 * Self-inheritance macro for initializing
 * an expression from itself while accepting
 * an additional *expression* parameter
 * 
 * It is used for certain postfix operations.
 */
    /* invocation */
#define inherit_self_with_ex(ex_type, action_name, ex, parent_ex, argument_ex)                                     \
    ex.properties = parent_ex.properties; /* todo hotfix, could be invalid */                                      \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent_ex.data, &argument_ex.data);                    \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent_ex.properties, argument_ex.properties);

    /* declaration */
#define self_inheritance_with_ex(ex_type, action_name, parameter_ex_type, parameter_ex_name)        \
    data_self_inheritance_with_ex(ex_type, action_name, parameter_ex_type, parameter_ex_name);      \
    properties_self_inheritance_with_ex(ex_type, action_name, parameter_ex_name);

    /* definition */
#define data_self_inheritance_with_ex(ex_type, action_name, parameter_ex_type, parameter_ex_name) \
    void ex_##ex_type##_inherit_data_on_##action_name(                                            \
        ex_##ex_type##_data* this,                                                                \
        ex_##ex_type##_data* parent,                                                              \
        ex_##parameter_ex_type##_data* parameter_ex_name)

    /* definition */
#define properties_self_inheritance_with_ex(ex_type, action_name, parameter_ex_name) \
    void ex_##ex_type##_inherit_properties_on_##action_name(                         \
        expression_properties* this,                                                 \
        expression_properties* parent,                                               \
        expression_properties* parameter_ex_name)


/**
 * Self-inheritance macro for initializing
 * an expression from itself while accepting
 * an additional *expression* parameter and
 * a regular additional parameter
 * 
 * It is used for complex ternary operations.
 */
    /* invocation */
#define inherit_self_with_ex_and(ex_type, action_name, ex, parent_ex, argument_ex, argument)                                  \
    ex.properties = parent_ex.properties; /* todo hotfix, could be invalid */                                                 \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent_ex.data, &argument_ex.data, argument);                     \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent_ex.properties, argument_ex.properties, argument);

    /* declaration */
#define self_inheritance_with_ex_and(ex_type, action_name, parameter_ex_type, parameter_ex_name, parameter)        \
    data_self_inheritance_with_ex_and(ex_type, action_name, parameter_ex_type, parameter_ex_name, parameter);      \
    properties_self_inheritance_with_ex_and(ex_type, action_name, parameter_ex_name, parameter);

    /* definition */
#define data_self_inheritance_with_ex_and(ex_type, action_name, parameter_ex_type, parameter_ex_name, parameter) \
    void ex_##ex_type##_inherit_data_on_##action_name(                                                           \
        ex_##ex_type##_data* this,                                                                               \
        ex_##ex_type##_data* parent,                                                                             \
        ex_##parameter_ex_type##_data* parameter_ex_name,                                                        \
        parameter)

    /* definition */
#define properties_self_inheritance_with_ex_and(ex_type, action_name, parameter_ex_name, parameter) \
    void ex_##ex_type##_inherit_properties_on_##action_name(                                        \
        expression_properties* this,                                                                \
        expression_properties* parent,                                                              \
        expression_properties* parameter_ex_name,                                                   \
        parameter)


/**
 * Self-inheritance macro for initializing
 * an expression from itself while accepting
 * two additional *expression* parametera
 * 
 * It is used for complex ternary operations.
 */
    /* invocation */
#define inherit_self_with_ex_and_ex(ex_type, action_name, ex, parent_ex, argument_ex_1, argument_ex_2)                                         \
    ex.properties = parent_ex.properties; /* todo hotfix, could be invalid */                                                                  \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent_ex.data, &argument_ex_1.data, &argument_ex_2.data);                         \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent_ex.properties, argument_ex_1.properties, argument_ex_2.properties);

    /* declaration */
#define self_inheritance_with_ex_and_ex(ex_type, action_name, parameter_ex_type_1, parameter_ex_name_1, parameter_ex_type_2, parameter_ex_name_2)        \
    data_self_inheritance_with_ex_and_ex(ex_type, action_name, parameter_ex_type_1, parameter_ex_name_1, parameter_ex_type_2, parameter_ex_name_2);      \
    properties_self_inheritance_with_ex_and_ex(ex_type, action_name, parameter_ex_name_1, parameter_ex_name_2);

    /* definition */
#define data_self_inheritance_with_ex_and_ex(ex_type, action_name, parameter_ex_type_1, parameter_ex_name_1, parameter_ex_type_2, parameter_ex_name_2) \
    void ex_##ex_type##_inherit_data_on_##action_name(                                                                                                 \
        ex_##ex_type##_data* this,                                                                                                                     \
        ex_##ex_type##_data* parent,                                                                                                                   \
        ex_##parameter_ex_type_1##_data* parameter_ex_name_1,                                                                                          \
        ex_##parameter_ex_type_2##_data* parameter_ex_name_2)

    /* definition */
#define properties_self_inheritance_with_ex_and_ex(ex_type, action_name, parameter_ex_name_1, parameter_ex_name_2) \
    void ex_##ex_type##_inherit_properties_on_##action_name(                                                       \
        expression_properties* this,                                                                               \
        expression_properties* parent,                                                                             \
        expression_properties* parameter_ex_name_1,                                                                \
        expression_properties* parameter_ex_name_2)


/**
 * Self-inheritance macro for initializing
 * an expression from itself while accepting
 * an additional *precomputed* parameter
 * 
 * It is used for binary or postfix operations
 * where the parameter needs to be pre-processed
 * before passing it to the inheritance functions.
 * 
 * @param[in] parameter_source Parameter *source* declaration (with or without name)
 * @param[in] parameter_type Parameter declaration *without name*
 */
    /* invocation */
#define inherit_self_with_before(ex_type, action_name, ex, parent, parameter_type, argument)                 \
    ex.properties = parent.properties; /* todo hotfix, could be invalid */                                   \
    parameter_type _computed_argument;                                                                       \
    ex_##ex_type##_inherit_before_on_##action_name(parent.properties, &_computed_argument, argument);        \
    ex_##ex_type##_inherit_data_on_##action_name(&ex.data, &parent.data, _computed_argument);                \
    ex_##ex_type##_inherit_properties_on_##action_name(ex.properties, parent.properties, _computed_argument)

    /* declaration */
#define self_inheritance_with_before(ex_type, action_name, parameter, parameter_source) \
    self_inheritance_before(ex_type, action_name, parameter, parameter_source);         \
    data_self_inheritance_with(ex_type, action_name, parameter);                        \
    properties_self_inheritance_with(ex_type, action_name, parameter);

    /* definition */
#define self_inheritance_before(ex_type, action_name, parameter_type, parameter_source) \
    void ex_##ex_type##_inherit_before_on_##action_name(                                \
        expression_properties* parent,                                                  \
        parameter_type* value,                                                          \
        parameter_source)


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_INHERITANCE_ADVANCED_H */