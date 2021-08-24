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
#include "copy.h" /* copying */

    /* global variables */
ast_root ast;

    /* defines */
/**
 * Helper macro for looking up types
 */
#define lookup_arraylist(arraylist, name_lower, name_upper) \
    iterate_array(i, arraylist.size) { \
        if (strcmp(token, arraylist.data[i].name) == 0) { \
            stack_push_new_value(arraylist.data[i], name_upper##_REFERENCE, name_lower); \
            return name_upper##_NAME; \
        } \
    }

    /* functions */
/**
 * Initializes the abstract syntax tree
 */
void ast_init() {
    assert_arraylist_init(declaration, ast.declaration_list);
    assert_list_init(dc_type_primitive, ast.primitive_list, 8);
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
    dc_type type;
    assert_arraylist_init_empty(dc_type_level, type.level_list);

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
        lookup_arraylist(node->u_function._variable_stack, st_variable, ST_VARIABLE);
        if (node->u_function.parameter_list.data != NULL) {
            lookup_arraylist(node->u_function.parameter_list, function_parameter, FUNCTION_PARAMETER);
        }
    }

    stack_node node_new;
    dc_type value;
    assert_arraylist_init_empty(dc_type_level, value.level_list);

    /* check primitive types */
    iterate_array(i, ast.primitive_list.size) {
        if (strcmp(ast.primitive_list.data[i].name, token) == 0) {
            value.kind = DC_TK_PRIMITIVE;
            value.u_primitive = &ast.primitive_list.data[i];
            stack_push_new_value_predeclared(value, TYPE, type);
            return PRIMITIVE_NAME;
        }
    }

    /* check global definitions */
    iterate_array(i, ast.declaration_list.size) {
        declaration dc = ast.declaration_list.data[i];
        if (dc.name != NULL) {
            if (strcmp(dc.name, token) != 0) {
                continue;
            }
        } else {
            continue;
        }

        int result;
        switch (dc.kind) {
            case DC_STRUCTURE:
                result = STRUCTURE_NAME;
                value.kind = DC_TK_STRUCTURE;
                value.u_structure = copy_structure(dc.u_structure);
                break;

            case DC_ENUM:
                result = ENUM_NAME;
                value.kind = DC_TK_ENUM;
                value.u_enum = copy_structure(dc.u_enum);
                break;

            case DC_ALIAS:
                result = ALIAS_NAME;
                value.kind = DC_TK_ALIAS;
                value.u_alias = copy_structure(dc.u_alias);
                break;

            case DC_FUNCTION:
                stack_push_new_value_predeclared(dc.u_function, FUNCTION_REFERENCE, function);
                return FUNCTION_NAME;
            
            case DC_ST_VARIABLE:
                stack_push_new_value_predeclared(*dc.u_st_variable, ST_VARIABLE_REFERENCE, st_variable);
                return ST_VARIABLE_NAME;

            case DC_IMPORT:
            otherwise_error
        }

        stack_push_new_value_predeclared(value, TYPE, type);
        return result;
    }

    /* push new identifier */
    stack_push_name(token);
    return IDENTIFIER;
}