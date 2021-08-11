/**
 * @file ast.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-22
 * 
 *  Abstract syntax tree storage variable
 *  and lookup function implementations
 */
    /* includes */
#include "ast.h" /* this */

#include <string.h> /* string functions */
#include "parser.h" /* parser */
#include "stack.h" /* stack */

    /* global variables */
ast_root ast;

    /* defines */
/**
 * Helper macro for looking up types in the AST
 */
#define _lex_macro_not_type(arraylist, name_lower, name_upper) \
    iterate_array(i, arraylist.size) { \
        if (strcmp(token, arraylist.data[i].name) == 0) { \
            stack_push_new_value(arraylist.data[i], name_upper##_REFERENCE, name_lower); \
            return name_upper##_NAME; \
        } \
    }

#define _ast_lex_token_not_type(name_lower, name_upper) \
_lex_macro_not_type(ast.name_lower##_list, name_lower, name_upper)

#define _ast_lex_token_type(name_lower, name_upper) \
    iterate_array(i, ast.name_lower##_list.size) { \
        if (strcmp(token, ast.name_lower##_list.data[i].name) == 0) { \
            type.kind = AST_TK_##name_upper; \
            type.u_##name_lower = &ast.name_lower##_list.data[i]; \
            stack_push_new_value(type, TYPE, type); \
            return name_upper##_NAME; \
        } \
    }

    /* functions */
/**
 * Initializes the abstract syntax tree
 */
void ast_init() {
    assert_arraylist_init(ast_import, ast.import_list);
    assert_arraylist_init(ast_structure, ast.structure_list);
    assert_arraylist_init(ast_enum, ast.enum_list);
    assert_arraylist_init(ast_alias, ast.alias_list);
    assert_arraylist_init(ast_function, ast.function_list);
    assert_arraylist_init(st_variable, ast.st_variable_list);
    assert_list_init(ast_primitive, ast.primitive_list, 8);
    ast.primitive_list.data[0].name = "bool";   ast.primitive_list.data[0].code_name = "int8_t";  ast.primitive_list.data[0].size = 1;
    ast.primitive_list.data[1].name = "byte";   ast.primitive_list.data[1].code_name = "int8_t";  ast.primitive_list.data[1].size = 1;
    ast.primitive_list.data[2].name = "short";  ast.primitive_list.data[2].code_name = "int16_t"; ast.primitive_list.data[2].size = 2;
    ast.primitive_list.data[3].name = "int";    ast.primitive_list.data[3].code_name = "int32_t"; ast.primitive_list.data[3].size = 4;
    ast.primitive_list.data[4].name = "long";   ast.primitive_list.data[4].code_name = "int64_t"; ast.primitive_list.data[4].size = 8;
    ast.primitive_list.data[5].name = "void";   ast.primitive_list.data[5].code_name = "void";    ast.primitive_list.data[5].size = 0;
    ast.primitive_list.data[6].name = "float";  ast.primitive_list.data[6].code_name = "float";   ast.primitive_list.data[6].size = 4;
    ast.primitive_list.data[7].name = "double"; ast.primitive_list.data[7].code_name = "double";  ast.primitive_list.data[7].size = 8;
}

/**
 * Looks up the lexical type of a token,
 * and pushes it on the lexical stack
 * 
 * @param[in] token The token
 * 
 * @return Type of the token, or IDENTIFIER
 */
int ast_lex_token(const char* token) {
    ast_type type;
    assert_arraylist_init_empty(ast_type_level, type.level_list);

    /* context-aware lookup */
    if (!arraylist_is_empty(stack)) {
        if (stack.data[0].kind == STACK_IMPORT) {
            /* push an import node */
            stack_push_name(token);
            return IDENTIFIER;
        }
    }

    /* check local variables */
    stack_node* node = stack_find_or_null(STACK_FUNCTION);
    if (node != NULL) {
        _lex_macro_not_type(node->u_function._variable_stack, st_variable, ST_VARIABLE);
        if (node->u_function.parameter_list.data != NULL) {
            _lex_macro_not_type(node->u_function.parameter_list, function_parameter, FUNCTION_PARAMETER);
        }
    }

    /* check global definitions */
    _ast_lex_token_type(primitive, PRIMITIVE);
    _ast_lex_token_not_type(st_variable, ST_VARIABLE);
    _ast_lex_token_not_type(function, FUNCTION);
    _ast_lex_token_type(structure, STRUCTURE);
    _ast_lex_token_type(enum, ENUM);
    _ast_lex_token_type(alias, ALIAS);

    /* push new identifier */
    stack_push_name(token);
    return IDENTIFIER;
}