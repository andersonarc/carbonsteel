/**
 * @file type.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Stack manipulation function for
 *  structures, enums, aliases, imports,
 *  types and other lexical types
 */
    /* includes */
#include "type.h" /* this */
#include "ctool/iteration.h" /* iteration */

#include "ast.h" /* syntax tree */
#include "stack.h" /* stack */
#include "lexer.h" /* lexer */
#include "copy.h" /* duplication */
#include "convert.h" /* conversion utilities */
    
/**
 * @todo
 * No duplicates ---
 * Global lookup for Structure, Enum, Variable, Alias declarations
 * Local (member) lookup for Structure, Enum
 * Local (function) lookup for Variable
 * (arraylist_add_unique)
 */

    /* generic implementations */
list_define(ast_structure_member);
arraylist_define(ast_alias);
arraylist_define(string);
list_define(ast_enum_member);
arraylist_define(ast_enum);
arraylist_define(ast_structure);
arraylist_define(ast_type_level);
arraylist_define(ast_type);
list_define(ast_function_parameter);
arraylist_define(ast_function);
list_define(ast_primitive);
arraylist_define(ast_import);
list_define(ast_import_level);

        /* stack functions */

    /** STRUCTURE START */

void start_structure() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_STRUCTURE;

    /* initialize name */
    node.u_structure.name = stack_find_pop(STACK_NAME).u_name;
    logd("structure '%s'", node.u_structure.name);

    /* push new node */
    stack_push(node);
}

//todo stack_pop_get(UPPER, lower)

    /** STRUCTURE END **/

void end_structure() {
    stack_walk_prefix(ast_structure_member, STRUCTURE, structure, .member_list);

    /* initialize members */
    stack_walk {
        ast_structure_member value;
        value.type = stack_get_current(STACK_TYPE).u_type;
        value.name = stack_get_next   (STACK_NAME).u_name;
        logd("member '%s'", value.name);

        node->u_structure.member_list.data[i] = value;
    }
    stack_walk_end(ast_structure_member, node->u_structure.member_list);

    /* declare and remove from stack */
    assert_arraylist_add(ast_structure, ast.structure_list, node->u_structure);
    stack_pop();
}


    /** TYPE LEVEL ADD **/

void add_type_level() {
    /* check whether the last stack node is a type */
    stack_check_kind(stack_top, STACK_TYPE);

    /* add new level */
    assert_arraylist_add(ast_type_level, stack_top.u_type.level_list, AST_TL_POINTER);
}


    /* ENUM START **/

void start_enum() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_ENUM;

    /* initialize name */
    node.u_enum.name = stack_find_pop(STACK_NAME).u_name;
    logd("enum '%s'", node.u_enum.name);

    /* push new node */
    stack_push(node);
}

    /** ENUM END **/

void end_enum() {
    stack_walk_prefix(ast_enum_member, ENUM, enum, .member_list);

    /* initialize members */
    stack_walk {
        /* create an enum member with a name */
        ast_enum_member value;
        value.name = stack_get_current(STACK_NAME).u_name;
        logd("member '%s'", value.name);

        /* get optional member value */
        stack_node* next = stack_get_next_or_null(STACK_NAME);
        if (next != NULL) {
            value.value = copy_structure(stack_top.u_number);
        } else {
            ex_number number;
            number.kind = EX_N_INT;
            number.value = size2string(i);
            value.value = copy_structure(number);
        }
        
        /* add the member */
        node->u_enum.member_list.data[i] = value;
    }
    stack_walk_end(ast_enum_member, node->u_enum.member_list);

    /* declare and remove from stack */
    assert_arraylist_add(ast_enum, ast.enum_list, node->u_enum);
    stack_pop();
}


    /** FUNCTION START **/

void start_function() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_FUNCTION;

    /* initialize name and return type */
    node.u_function.name = stack_find_pop(STACK_NAME).u_name;
    logd("function '%s'", node.u_function.name);

    node.u_function.return_type = stack_find_pop(STACK_TYPE).u_type;

    /* init lists and push new node */
    node.u_function.parameter_list.data = NULL;
    assert_arraylist_init(st_variable, node.u_function._variable_stack);
    stack_push(node);
}


    /** FUNCTION PARAMETERS ADD **/

void add_function_parameters() {
    /* initialize the function parameters */
    stack_walk_prefix(ast_function_parameter, FUNCTION, function, .parameter_list);
    stack_walk {
        ast_function_parameter value;
        value.type = stack_get_current(STACK_TYPE).u_type;
        value.name = stack_get_next   (STACK_NAME).u_name;
        logd("parameter '%s'", value.name);

        node->u_function.parameter_list.data[i] = value;
    }
    stack_walk_end(ast_function_parameter, node->u_function.parameter_list);
}


    /** FUNCTION END **/

void end_function() {
    /* find the function */
    ast_function* function = &stack_find(STACK_FUNCTION)->u_function;

    /* initialize the function body */
    function->body = stack_find_pop(STACK_ST_COMPOUND).u_st_compound;
    
    /* declare and remove from stack */
    assert_arraylist_add(ast_function, ast.function_list, *function);
    stack_pop();
}


    /** IMPORT START **/

void start_import() {
    /* create and push new node */
    stack_node node;
    node.kind = STACK_IMPORT;
    stack_push(node);
    logd("start import");
}


    /** IMPORT END **/

void end_import() {
    /* add import nodes */
    stack_walk_prefix(ast_import_level, IMPORT, import, );
    stack_walk {
        node->u_import.data[i] = stack_current.u_name;
        logd("import %s", stack_current.u_name);
    }
    stack_walk_end(ast_import_level, node->u_import);

    /* check for empty import */
    if (node->u_import.size == 0) {
        error_syntax("empty import statement");
    }

    /* declare import and remove from stack */
    assert_arraylist_add(ast_import, ast.import_list, node->u_import);
    stack_pop();
}


    /** ALIAS ADD **/

void add_alias() {
    /* create new alias */
    ast_alias value;
    ast_type type = stack_find_pop(STACK_TYPE).u_type;
    value.target = copy_structure(type);
    value.name = stack_find_pop(STACK_NAME).u_name;
    logd("alias %s", value.name);

    /* declare the alias */
    assert_arraylist_add(ast_alias, ast.alias_list, value);
}