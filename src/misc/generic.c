/**
 * @file generic.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-12
 * 
 *  Generic list and arraylist implementations
 *  for all files
 */
    /* includes */
#include "syntax/predeclaration.h"  /* predeclarations */
#include "syntax/declaration/declaration.h"  /* declarations */
#include "syntax/statement/statement.h"  /* statement */

    /* generic implementations */
list_define(st_compound_item);
arraylist_define(dc_st_variable_ptr);
arraylist_define(statement);
arraylist_define(ast_local_declaration);
list_define(ast_type_primitive);
arraylist_define(ast_type_level);
arraylist_define(ast_type);
arraylist_define(se_context_level);
list_define(dc_structure_member);
list_define(dc_enum_member);
list_define(dc_function_parameter);
list_define(dc_import_node);
arraylist_define(ex_postfix_level);
arraylist_define(ex_constructor_ptr);
list_define(expression_ptr);
arraylist_define(expression_ptr);
arraylist_define(st_compound_item);
arraylist_define(dc_structure_member);
arraylist_define(dc_enum_member);
arraylist_define(dc_function_parameter);
arraylist_define(dc_import_node);
arraylist_define(declaration);
arraylist_define(op_unary);