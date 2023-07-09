/**
 * @file predeclaration.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-09
 * 
 *  All type declarations are moved to this file
 *  to avoid circular dependency conflicts
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_PREDECLARATION_H
#define CARBONSTEEL_SYNTAX_PREDECLARATION_H

    /* includes */
#include <stdbool.h> /* boolean type */

#include "ctool/macro.h" /* macro concatenation */
#include "misc/list.h" /* list utilities */

    /* defines */
/**
 * Structure declaration macro
 * 
 * @param[in] name Name of the structure
 */
#define d_struct(name)   typedef struct name name
#define da_struct(name)  d_struct(name); arraylist_declare_type(name)
#define dl_struct(name)  d_struct(name); list_declare(name)

/**
 * Enum declaration macro
 * 
 * @param[in] name Name of the enum
 */
#define d_enum(name)  typedef enum name name
#define da_enum(name) d_enum(name); arraylist_declare_type(name)
#define dl_enum(name) d_enum(name); list_declare(name)

/**
 * Pointer declaration macro
 * 
 * @param[in] name Name of the pointer value
 */
#define d_pointer(name)   typedef name* name##_ptr
#define da_pointer(name)  d_pointer(name); arraylist_declare(name##_ptr)
#define dl_pointer(name)  d_pointer(name); list_declare(name##_ptr)

/**
 * Alias declaration macro
 * 
 * @param[in] name Name of the alias
 * @param[in] target Target of the alias
 */
#define d_alias(name, target)   typedef target name
#define da_alias(name, target)  d_alias(name, target); arraylist_declare_type(name)
#define dl_alias(name, target)  d_alias(name, target); list_declare(name)

            /* typedefs */
d_struct(se_context_import_file);
    da_pointer(se_context_import_file);

        /** LEXICAL TYPES **/

    /* type declaration */
d_struct(ast_type_declaration);

    /* lexical type */
da_struct(ast_type);
    d_enum(ast_type_kind);
    da_struct(ast_type_level);
    d_enum(ast_type_level_kind);

    /* primitive type */
dl_struct(ast_type_primitive);


        /** DECLARATIONS **/

    /* alias */
d_struct(dc_alias);

    /* enum */
d_struct(dc_enum);
    da_struct(dc_enum_member);
    d_struct(enum_context);

    /* structure */
d_struct(dc_structure);
    da_struct(dc_structure_member);

    /* function */
d_struct(dc_function);
    d_struct(dc_function_parameters);
    da_struct(dc_function_parameter);

    /* import */
da_alias(dc_import_node, char*);
arraylist_declare_functions(dc_import_node);
    d_struct(dc_import);

    /* declaration */
d_struct(declaration);
da_pointer(declaration);
    d_enum(declaration_kind);


        /** EXPRESSIONS **/

    /* properties */
d_struct(expression_properties);

    /* constant expression */
dl_struct(ex_constant);
    d_enum(ex_constant_kind);

    /* primitive */
d_alias(ex_character, char);

    /* number */
d_struct(ex_number);
    d_struct(ex_number_data);
    d_enum(ex_number_kind);

    /* enum member */
d_alias(ex_enum_member, dc_enum_member);

    /* constructor */
d_struct(ex_constructor);
    da_pointer(ex_constructor);

    /* basic */
d_struct(ex_basic);
    d_struct(ex_basic_data);
    d_enum(ex_basic_kind);

    /* postfix */
d_struct(ex_postfix);
    d_struct(ex_postfix_data);
    d_enum(ex_postfix_kind);
    da_struct(ex_postfix_level);
    d_struct(ex_postfix_level_raw);
    da_enum(ex_postfix_level_kind);

    /* unary */
d_struct(ex_unary);
    d_struct(ex_unary_data);
    d_enum(ex_unary_kind);
    da_enum(op_unary);

    /* cast */
da_struct(ex_cast);
    d_struct(ex_cast_data);

    /* binary */
d_struct(ex_binary);
    d_struct(ex_binary_data);

    /* condition */
d_struct(ex_condition);
    d_struct(ex_condition_data);

    /* expression */
d_struct(ex_expression);
    d_alias(expression, ex_expression);
    d_struct(expression_data);
    d_alias(ex_expression_data, expression_data);
    da_pointer(expression);

    /* block */
d_struct(ex_block);


        /** STATEMENTS **/
        
    /* if */
d_struct(st_if);

    /* while */
d_struct(st_while);

    /* jump */
d_struct(st_jump);
    d_enum(st_jump_kind);

    /* statement */
da_struct(statement);
    d_enum(statement_kind);

d_struct(dc_st_variable);
    da_pointer(dc_st_variable);

d_alias(st_expression, ex_block);

da_struct(st_compound_item);
    d_alias(st_compound, list(st_compound_item));

#endif /* CARBONSTEEL_SYNTAX_PREDECLARATION_H */